#include <Zumo32U4.h>
#include <Wire.h>

//#define _DEBUG

Zumo32U4Motors  motors;
#define I2C_ADDRESS   0x10

void setup()
{
  ledRed(1);
  ledYellow(1);
  ledGreen(1);
#ifdef _DEBUG
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Zumo32U4 I2C setup()");
#endif

  Wire.begin(I2C_ADDRESS);  // slave
  Wire.onReceive(dataRecv); // ISR

  delay(1000);
  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}

volatile byte var = 0;

void loop()
{
  int   ly, rx,
        motorL, motorR, turn;
  
  ly = var >> 4;
  rx = var & 0x0f;
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

  ledRed(0);
  delay(100);
}

void dataRecv(int)
{
  if ( Wire.available() ) {
    var = Wire.read();
    ledRed(1);
  }
}
