/*
  2015/03/11
  CanServoDriver 動作確認プログラム

  PWM 全ポートにPWMを出力
  
 */
#include<Wire.h>
#include<Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setup() {
    pinMode(STAT_LED1, OUTPUT);
    pinMode(STAT_LED2, OUTPUT);
    Serial.begin(115200);
    Serial.println("PWM test");
    pwm.begin();
    pwm.setPWMFreq(1000); // 1kHz ?
}


int cnt = 0;
int duty[16] = {};

void loop() {
    digitalWrite(STAT_LED1,   cnt%2 );
    digitalWrite(STAT_LED2, !(cnt%2));

    pwm.setPin(cnt % 16, duty[cnt % 16], false);
    duty[ cnt % 16 ] += 500;
    if( duty[ cnt % 16 ] > 4000) duty[ cnt % 16] = 0;
    delay(100);
}
