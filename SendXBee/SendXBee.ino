const int F1_PIN = 4;
const int F2_PIN = 5;
const int F3_PIN = 6;
const int UP_PIN = 9;
const int DW_PIN = 11;
const int LT_PIN = 10;
const int RT_PIN = 12;

#define SEND_UP   0x19
#define SEND_DW   0x1c
#define SEND_LT   0x0c
#define SEND_RT   0x5e
#define SEND_A    0x44
#define SEND_B    0x40
#define SEND_C    0x43

void setup() {
  pinMode(F1_PIN, INPUT_PULLUP);
  pinMode(F2_PIN, INPUT_PULLUP);
  pinMode(F3_PIN, INPUT_PULLUP);
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DW_PIN, INPUT_PULLUP);
  pinMode(LT_PIN, INPUT_PULLUP);
  pinMode(RT_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  if ( digitalRead(F1_PIN)==0 ) {
    Serial.write(SEND_A);
  }
  else if ( digitalRead(F2_PIN)==0 ) {
    Serial.write(SEND_B);
  }
  else if ( digitalRead(F3_PIN)==0 ) {
    Serial.write(SEND_C);
  }
  else if ( digitalRead(UP_PIN)==0 ) {
    Serial.write(SEND_UP);
  }
  else if ( digitalRead(DW_PIN)==0 ) {
    Serial.write(SEND_DW);
  }
  else if ( digitalRead(LT_PIN)==0 ) {
    Serial.write(SEND_LT);
  }
  else if ( digitalRead(RT_PIN)==0 ) {
    Serial.write(SEND_RT);
  }
  else {
    Serial.write(0);
  }
  
//  delay(10);
}
