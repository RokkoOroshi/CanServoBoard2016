/*
  2015/02/12 
  CanMotorDriver 動作確認プログラム
  LED、モーター制御の確認
  状態LEDが交互に点滅、モーター出力が５秒ごとに逆転（LEDだけで確認可能）
 */


void setup() {
    pinMode(STAT_LED1, OUTPUT);
    pinMode(STAT_LED2, OUTPUT);
}


int cnt = 0;
void loop() {
    digitalWrite(STAT_LED1,   cnt%2 );
    digitalWrite(STAT_LED2, !(cnt%2));
    if(++cnt > 100) cnt = 0;
    delay(100);
}
