/**
 * author : Kenya Ukai
 * license: MIT
 *
 */

#include<Wire.h>
#include<Adafruit_PWMServoDriver.h>
#include <SerialCommand.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

SerialCommand SCmd;
volatile uint32_t timer_cnt;

unsigned long prev_1;

void setup() {
    Serial.begin(115200);

    pinMode(STAT_LED1, OUTPUT);
    pinMode(STAT_LED2, OUTPUT);
    pinMode(SERVO_OFF, OUTPUT);
    digitalWrite(SERVO_OFF, LOW);

    pwm.begin();
    pwm.setPWMFreq(50); // 20ms
    
    Serial.println("Arm Servo Program");

    Serial.println("set all servo to center (1.5ms pulse)");

    for (int i = 0; i < 10; i++) {
        setServoPulse(i, 1.5);
    }

    SCmd.addCommand("s", servoCallback);
    
    Serial.println(". . . Ready !");
    Serial.println("s <no> <pulse (us)>");
    
}

static void servoCallback() {
    const char *n_str = SCmd.next();
    const char *p_str = SCmd.next();
    if(n_str == NULL || p_str == NULL) {
        parseError();
        return;
    }
    int n = atoi(n_str);
    double p = atoi(p_str) / 1000.0;
    setServoPulse(n, p);
}

void parseError() { Serial.println("parse error"); }

void loop() {
    SCmd.readSerial();

    unsigned long now = millis();
    if(now - prev_1 >= 1000) {
        digitalWrite(STAT_LED1, !digitalRead(STAT_LED1));
        prev_1 = now;
    }
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

