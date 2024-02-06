#include <Zumo32U4.h>

Zumo32U4Motors  motors;

void setup()
{
  ledRed(1);
  ledYellow(1);
  ledGreen(1);
  Serial1.begin(9600);
  while(!Serial1);
  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}

void loop()
{
  if ( Serial1.available() <= 0 ) {
    ledRed(0);
    delay(100);
    return;
  }
  ledRed(1);
  byte  var = Serial1.read();
  int   ly = var >> 4,
        rx = var & 0x0f;

  int   motorL, motorR, turn;

  motorL = motorR = (ly&0x07)/7.0 * 400.0;
  turn = (rx&0x07)/7.0 * 400.0;
  
  if ( !(ly & 0x08) ) {
    motorL = -motorL;
    motorR = -motorR;
  }
  if ( rx & 0x08 )
    motorL -= turn;
  else
    motorR -= turn;
  
  motors.setSpeeds(motorL, motorR);

  delay(100);
}
