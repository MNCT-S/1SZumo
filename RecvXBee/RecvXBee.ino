#include <Zumo32U4.h>

#define SEND_UP   0x19
#define SEND_DW   0x1c
#define SEND_LT   0x0c
#define SEND_RT   0x5e
#define SEND_A    0x44
#define SEND_B    0x40
#define SEND_C    0x43

Zumo32U4Motors  motors;

void setup()
{
  ledRed(1);
  ledYellow(1);
  ledGreen(1);
  Serial1.begin(115200);
  while(!Serial1);
  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}

void loop()
{
  static int  speed = 100;
  
  if ( Serial1.available() <= 0 ) {
    delay(10);
    return;
  }
  byte  var = Serial1.read();
//  if ( var )
//    ledRed(1);

  switch ( var ) {
  case 0:
    motors.setSpeeds(0,0);
    break;
  case SEND_A:
    speed = 100;
    break;
  case SEND_B:
    speed = 200;
    break;
  case SEND_C:
    speed = 400;
    break;
  case SEND_UP:
    motors.setSpeeds(speed, speed);
    break;
  case SEND_DW:
    motors.setSpeeds(-speed, -speed);
    break;
  case SEND_LT:
    motors.setSpeeds(-speed, speed);
    break;
  case SEND_RT:
    motors.setSpeeds(speed, -speed);
    break;
  }
//  delay(10);
//  ledRed(0);
}
