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


enum CAN_COMMAND {
    CAN_COMMAND_RESET = 0,
    CAN_COMMAND_ARM = 1,
    CAN_COMMAND_CAMERA = 2
};

const uint8_t ARM_TOP = 0;
const uint8_t ARM_MIDDLE = 2;
const uint8_t ARM_BOTTOM = 3;
const uint8_t CAM_TOP = 4;
const uint8_t CAM_BOTTOM = 5;

const double SERVO_MAX_PULSE_WIDTH[] = {2.0, 2.0, 1.8, 2.0, 2.0, 2.0};
const double SERVO_MIN_PULSE_WIDTH[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};

const uint8_t CAN_SPEED     = CAN_250KBPS;
const uint8_t TPIP_CAN_ADDR = 1;
const uint8_t self_can_addr = 8;

SerialCommand SCmd;
uint32_t timer_cnt;
bool connection_alive;

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
    Serial.println("* <top | middle | bottom> <-90 ~ 90>");
    Serial.println("* <s 0 ~ 10> <-90 ~ 90>");
    
    SCmd.addCommand("t", topCallback);
    SCmd.addCommand("m", middleCallback);
    SCmd.addCommand("b", bottomCallback);
    SCmd.addCommand("s", servoCallback);
    
    Serial.println(". . . Ready !");

    for (int i = 0; i < 10; i++) {
        setServoAngle(i, 1.5);
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
        
        switch (data[0]) {
            case CAN_COMMAND_RESET:
                if(len == 2) {
                    uint8_t reset = data[1];
                    digitalWrite(SERVO_OFF, reset);
                }
            case CAN_COMMAND_ARM:
                if(len == 1 + 3) {
                    int8_t top = CanCommunication::get<int8_t>(&data[1]);
                    int8_t middle = CanCommunication::get<int8_t>(&data[2]);
                    int8_t bottom = CanCommunication::get<int8_t>(&data[3]);
                    setServoAngle(ARM_TOP, top);
                    setServoAngle(ARM_MIDDLE, middle);
                    setServoAngle(ARM_BOTTOM, bottom);
                }
                break;
            case CAN_COMMAND_CAMERA:
                if(len == 1 + 2) {
                    int8_t top = CanCommunication::get<int8_t>(&data[1]);
                    int8_t bottom = CanCommunication::get<int8_t>(&data[2]);
                    setServoAngle(CAM_TOP, top);
                    setServoAngle(CAM_BOTTOM, bottom);
                }
            default:
                break;
        }
    }
}

static void topCallback() {
    const char *arg = SCmd.next();
    if(arg == NULL) {
        parseError();
        return;
    }
    int16_t pos = atoi(arg);
    if(abs(pos) > 90) pos = 90 * (pos > 0 ? 1 : -1);
    setServoAngle(ARM_TOP, pos);
    /*
    uint8_t data[1] = { CAN_COMMAND_ARM_MOTOR };
    CanCommunication::set<int8_t>(&data[1], (int8_t)speed);
    canOnReceive(
        CanCommunication::generateStdId(CAN_DATA_TYPE_COMMAND,
                                        self_can_addr,
                                        self_can_addr),
        data, 2);
    */
        
}
static void middleCallback() {
    const char *arg = SCmd.next();
    if(arg == NULL) {
        parseError();
        return;
    }
    int16_t pos = atoi(arg);
    if(abs(pos) > 90) pos = 90 * (pos > 0 ? 1 : -1);
    setServoAngle(ARM_MIDDLE, pos);
}
static void bottomCallback() {
    const char *arg = SCmd.next();
    if(arg == NULL) {
        parseError();
        return;
    }
    int16_t pos = atoi(arg);
    if(abs(pos) > 90) pos = 90 * (pos > 0 ? 1 : -1);
    setServoAngle(ARM_BOTTOM, pos);
}

void servoCallback() {
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
    setServoPulse(n, pulse);
}

// pulse : ms
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000;
  pulse /= pulselength;
  Serial.println(pulse);
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

