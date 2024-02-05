#include <Zumo32U4.h>

Zumo32U4Motors  motors;

void setup()
{
  ledRed(1);
  ledYellow(1);
  ledGreen(1);
  Serial1.begin(19200);
  while(!Serial1);
  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}

int   speedL = 0, speedR = 0;

void loop()
{
  if ( Serial1.available() <= 0 ) {
//    delay(10);
    return;
  }
  ledRed(1);
  byte  var = Serial1.read();

  if ( var & 0x80 ) {
    speedR = var & 0x7f;
    speedR -= 64;
    if ( abs(speedR) <= 10 ) speedR = 0;
    speedR *= 6.25;
  }
  else {
    speedL = var;
    speedL -= 64;
    if ( abs(speedL) <= 10 ) speedL = 0;
    speedL *= 6.25;   // 400/64
  }

  motors.setSpeeds(speedL, speedR);

//  delay(10);
  ledRed(0);
}
