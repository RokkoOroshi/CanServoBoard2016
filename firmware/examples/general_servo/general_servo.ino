/**
 * author : Kenya Ukai
 * license: MIT
 *
 */

#include<Wire.h>
#include<Adafruit_PWMServoDriver.h>

#include <avr/wdt.h>
#include <SPI.h>
#include <mcp_can.h>
#include <SerialCommand.h>
#include <can_communication.h>
#include <can_communication_def.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const double SERVO_MAX_PULSE_WIDTH[] = {2.0, 1.96, 1.96, 2.0, 1.75, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
const double SERVO_MIN_PULSE_WIDTH[] = {0.5, 0.5, 0.58, 0.5, 0.75, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
const double SERVO_DEFAULT_PULSE_WIDTH[] = {1.25, 1.96, 0.58, 1.25, 0.75, 1.25, 1.25, 1.25, 1.25, 1.25, 1.25};


enum CAN_COMMAND {
    CAN_COMMAND_RESET = 0,
    CAN_COMMAND_SERVO = 1
};

static const uint8_t CAN_SPEED     = CAN_250KBPS;
static const uint8_t TPIP_CAN_ADDR = 1;
static const uint8_t self_can_addr = 5;

static void canOnReceive(uint16_t std_id, const byte* data, uint8_t len);

SerialCommand SCmd;
volatile uint32_t timer_cnt;
volatile bool connection_alive;

unsigned long prev_01;
unsigned long prev_1;

void setup() {
    Serial.begin(115200);

    pinMode(STAT_LED1, OUTPUT);
    pinMode(STAT_LED2, OUTPUT);
    pinMode(SERVO_OFF, OUTPUT);
    digitalWrite(SERVO_OFF, LOW);
    CanCom.begin(self_can_addr, CAN_SPEED);
    CanCom.onReceive(canOnReceive);

    pwm.begin();
    pwm.setPWMFreq(50); // 20ms
    
    Serial.println("Arm Servo Program");
    Serial.print("addr : ");
    Serial.println(self_can_addr);

    Serial.println("* Commands : ");
    Serial.println("* <s 0 ~ 10> <-90 ~ 90>");

    
    SCmd.addCommand("s", servoCallback);
    
    Serial.println(". . . Ready !");

    for (int i = 0; i < 11; i++) {
        setServoPulse(i, SERVO_DEFAULT_PULSE_WIDTH[i]);
    }
    
    wdt_enable(WDTO_120MS);
}

//
// CAN通信
//

static void canOnReceive(uint16_t std_id, const byte* data, uint8_t len) {
    uint8_t type = CanCommunication::getDataTypeFromStdId(std_id);
    uint8_t src  = CanCommunication::getSrcFromStdId(std_id);
    uint8_t dest = CanCommunication::getDestFromStdId(std_id);

    digitalWrite(STAT_LED2, !digitalRead(STAT_LED2));
    if(dest != self_can_addr) return;
    if (type == CAN_DATA_TYPE_COMMAND) {
        connection_alive = true;

        uint8_t command = CanCommunication::get<uint8_t>(&data[0]);

        if(command == CAN_COMMAND_RESET) {
            uint8_t reset = data[1];
            digitalWrite(SERVO_OFF, reset);
        } else if(command == CAN_COMMAND_SERVO) {
            uint8_t num = CanCommunication::get<uint8_t>(&data[1]);
            int8_t angle = CanCommunication::get<int8_t>(&data[2]);
            setServoAngle(num, angle);
        }
    }
}

static void servoCallback() {
    const char *arg = SCmd.next();
    if(arg == NULL) {
        parseError();
        return;
    }
    int num = atoi(arg);
    if(num < 0 || num >= 12) return;
    arg = SCmd.next();
    if(arg == NULL) {
        parseError();
        return;
    }
    int angle = atoi(arg);    
    if(abs(angle) > 90) angle = 90 * (angle > 0 ? 1 : -1);
    setServoAngle(num, angle);
}

void parseError() { Serial.println("parse error"); }

//
// その他
//

void loop() {
    SCmd.readSerial();
    CanCom.tasks();

    unsigned long now = millis();
    if(now - prev_01 >= 100) {
        wdt_reset();
        if (!connection_alive) {
        } else {
            connection_alive = false;
        }
        prev_01 = now;
    }
    if(now - prev_1 >= 1000) {
        digitalWrite(STAT_LED1, !digitalRead(STAT_LED1));
        CanCom.sendHeartBeat();
        prev_1 = now;
    }
}

void setServoAngle(uint8_t n, double deg) {
    if(deg < -90) deg = -90;
    if(deg > 90) deg = 90;
    double width = SERVO_MAX_PULSE_WIDTH[n] - SERVO_MIN_PULSE_WIDTH[n];
    double pulse = (deg + 90) / 180 * width + SERVO_MIN_PULSE_WIDTH[n];
    Serial.print("Pulse is (ms)");
    Serial.println(pulse);
    setServoPulse(n, pulse);
}

// pulse : ms
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  //Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  //Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000;
  pulse /= pulselength;
  //Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

//ウォッチドッグタイマー用おまじない
uint8_t mcusr_mirror __attribute__((section(".noinit")));
void get_mcusr(void) __attribute__((naked)) __attribute__((section(".init3")));
void get_mcusr(void) {
    mcusr_mirror = MCUSR;
    MCUSR = 0;
    wdt_disable();
}
