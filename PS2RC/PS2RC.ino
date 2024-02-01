#include <PS2X_lib.h>
#include <Zumo32U4.h>
#include <ZumoServo.h>  // Change to match Zumo32U4

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        2
#define PS2_CMD        4
#define PS2_SEL        5
#define PS2_CLK        3

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false

PS2X ps2x;
Zumo32U4Motors motors;
Servo sv;
byte  vibrate=0, nRx, nLy;
int   nPosX, nPosY;

void setup() {
//  ps2x.config_gamepad(3,4,5,2, true, true);
  ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  sv.attach(0);
}

void loop() {
  ps2x.read_gamepad(false, vibrate);

  if ( ps2x.ButtonPressed(PSB_RED) ) ledRed(true);
  else ledRed(false);
  if ( ps2x.ButtonPressed(PSB_GREEN) ) ledGreen(true);
  else ledGreen(false);
  if ( ps2x.ButtonPressed(PSB_PINK) ) ledYellow(true);
  else ledYellow(false);
   
  vibrate = ps2x.Analog(PSAB_CROSS);

  nRx = ps2x.Analog(PSS_RX);
  nLy = ps2x.Analog(PSS_LY);
  nPosY = (int)nLy - 128;
  nPosX = (int)nRx - 128;

  sv.write( constrain(180-nRx*0.7, 0, 180) ); // *180/255
  
  int motorL=0, motorR=0;
  if ( abs(nPosY) > 10 ) {
    motorL = motorR = -nPosY * 3.125;    // *400/128;
    if ( nPosX > 10 )
      motorR -= nPosX * 3.125;
    else if ( nPosX < -10 )
      motorL += nPosX * 3.125;
  }
  motors.setSpeeds(motorL, motorR);

  delay(50);
}
