const int LEFT_PIN = A0;
const int RIGHT_PIN = A2;

void setup(){
    Serial.begin( 19200 );
}

void loop(){
    int    valL, valR;
    byte   snd;
    
    valL = analogRead( LEFT_PIN );
    valR = analogRead( RIGHT_PIN );

    snd = valL >> 3;
    Serial.write(snd);
//    Serial.println(snd, HEX);
    
    snd = 0x80 | (valR >> 3);
    Serial.write(snd);
//    Serial.println(snd, HEX);
}
