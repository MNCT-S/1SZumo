#include <Zumo32U4.h>
#include <Wire.h>

//#define _DEBUG

Zumo32U4Motors  motors;
#define I2C_ADDRESS   0x10
#define MOTOR_RUNUP   20    // ボタン時の助走（徐々にスピードアップ）
#define TURN_SPEED    200   // ボタン時の旋回スピード

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

volatile byte var = 0, varbak = 0;

void loop()
{
  static int   motorL = 0, motorR = 0;
  
  if ( var == 0 || var & 0x80 ) {
    // ボタン系
    switch ( var & 0x07 ) {
    case 0:   // 停止
      motorL = motorR = 0;
      break;
    case 1:   // 前進
      if ( var == varbak ) {
        if ( motorL < 400 ) {
          motorL += MOTOR_RUNUP;
          motorR += MOTOR_RUNUP;
        }
      }
      else {
        motorL = motorR = MOTOR_RUNUP;
      }
      break;
    case 2:   // 後退
      if ( var == varbak ) {
        if ( motorL > -400 ) {
          motorL -= MOTOR_RUNUP;
          motorR -= MOTOR_RUNUP;
        }
      }
      else {
        motorL = motorR = -MOTOR_RUNUP;
      }
      break;
    case 3:   // 左
      motorL = 0;
      motorR = TURN_SPEED;
      break;
    case 4:   // 右
      motorL = TURN_SPEED;
      motorR = 0;
      break;
    }
  }
  else {
    // アナログスティック系
    int p = var >> 4,   // モータパワー（0-6 たまに7）
        a = var & 0x0f; // 進行方向（30度ずつ）
    motorL = motorR = constrain(400*p/6, 0, 400); // パワー設定
    switch ( a ) {
    case 0:   // 0
      motorR = 0;
      break;
    case 1:   // 30
      motorR *= 0.33;
      break;
    case 2:   // 60
      motorR *= 0.66;
      break;
    case 3:   // 90
      break;
    case 4:   // 120
      motorL *= 0.66;
      break;
    case 5:   // 150
      motorL *= 0.33;
      break;
    case 6:   // 180
      motorL = 0;
      break;
    case 7:   // 210
      motorL = -motorL * 0.33;
      motorR = -motorR;
      break;
    case 8:   // 240
      motorL = -motorL * 0.66;
      motorR = -motorR;
      break;
    case 9:   // 270
      motorL = -motorL;
      motorR = -motorR;
      break;
    case 10:  // 300
      motorL = -motorL;
      motorR = -motorR * 0.66;
      break;
    case 11:  // 330
      motorL = -motorL;
      motorR = -motorR * 0.33;
      break;
    }
  }

  motors.setSpeeds(motorL, motorR);
  varbak = var;
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
