#include <Zumo32U4.h>

Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonC buttonC;

void setup() {
}

void loop() {

  if ( buttonA.isPressed() ) {
    ledRed(1);
    buzzer.play("L16 >e>e>r>er>c>er>grrrg");
    while(buzzer.isPlaying());
    ledRed(0);
  }

  if ( buttonC.isPressed() ) {
    ledYellow(1);
    buzzer.play("L16 b>fr>f>f>er>d>c");
    while(buzzer.isPlaying());
    ledYellow(0);
  }
}
