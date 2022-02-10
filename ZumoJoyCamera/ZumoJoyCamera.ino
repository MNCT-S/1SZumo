#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4Motors  motors;
#define I2C_ADDRESS 0x10

static int motorL = 0, motorR = 0;

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

void loop()
{
  motors.setSpeeds(motorL, motorR);
  ledRed(0);
}

void dataRecv(int)
{
  byte i = 0;
  int c[2];
  while(Wire.available())
  { 
    c[i] = (signed char)Wire.read() * 4;
#ifdef _DEBUG
    Serial.println(c[i]);
#endif
    i++;
    ledRed(1);
  }
  motorL = c[0];
  motorR = c[1];
}
