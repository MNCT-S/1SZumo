#include "pitches.h"

#define SIZEOF(a) (sizeof(a)/sizeof(a[0]))

const int BT_PIN = 2;
const int LEFT_PIN = A0;
const int RIGHT_PIN = A2;

const int sound1[] = {
  NOTE_D4, NOTE_E4, 0, NOTE_D4, NOTE_E4, 0, NOTE_C5, NOTE_B4, 0, NOTE_G4
};
const int sound2[] = {
  NOTE_E5, NOTE_G5, NOTE_E6, NOTE_C6, NOTE_D6, NOTE_G6
};
const int duration1[] = {
  12, 12, 12, 12, 12, 12, 12, 12, 6, 12
};
const int duration2[] = {
  14, 14, 14, 14, 14, 14
};

void playTone(const int* t, const int* v, int S)
{
  // iterate over the notes of the melody:
  for (int i=0; i<S; i++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / v[i];
    tone(8, t[i], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}

void setup(){
    Serial.begin( 19200 );
    pinMode(BT_PIN, INPUT_PULLUP);
    playTone(sound1, duration1, SIZEOF(sound1));
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

  // debug
  if ( digitalRead(BT_PIN)==0 ) {
    playTone(sound2, duration2, SIZEOF(sound2));
  }
}
