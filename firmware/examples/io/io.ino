/*
  2015/03/11
  CanServoDriver 動作確認プログラム

  全拡張用ポートで入力値読み取り

  ポート名はArduino完全互換
  （
  A0 - A3 はアナログ入力対応
  D9, D10はディジタルのみ
  ）
  
 */


void setup() {
    pinMode(STAT_LED1, OUTPUT);
    pinMode(STAT_LED2, OUTPUT);
    Serial.begin(115200);
    Serial.println("IO test");

}


void read_all_input_port(){
    Serial.println("- - - Read all input port - - -");
    Serial.print("- Input0(AI) : ");
    Serial.println(analogRead(INPUT0));
    Serial.print("* Input1(AI) : ");
    Serial.println(analogRead(INPUT1));
    Serial.print("* Input2(AI) : ");
    Serial.println(analogRead(INPUT2));
    Serial.print("* Input3(AI) : ");
    Serial.println(analogRead(INPUT3));
    Serial.print("* Input9(DI) : ");
    Serial.println(digitalRead(INPUT9));
    Serial.print("* Input10(DI) : ");
    Serial.println(digitalRead(INPUT10));
    
    Serial.println("- - - - - - - - - - - - - - - -");
}

int cnt = 0;

void loop() {
    digitalWrite(STAT_LED1,   cnt%2 );
    digitalWrite(STAT_LED2, !(cnt%2));
    cnt++;
    
    read_all_input_port();
    
    delay(500);
}
