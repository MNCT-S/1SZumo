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
byte  vibrate=0, nLY, nRY;
int nPosLY, nPosRY;
int motorL=0, motorR=0;

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

  nLY = ps2x.Analog(PSS_LY);
  nRY = ps2x.Analog(PSS_RY);
  nPosLY = (int)nLY - 128;
  nPosRY = (int)nRY - 128;
  motorL = -nPosLY * 3.125;    // *400/128;
  motorR = -nPosRY * 3.125;
  motors.setSpeeds(motorL, motorR);

  sv.write( constrain(nPosLY-nPosRY+90, 0, 180) );
  
  delay(50);
}
