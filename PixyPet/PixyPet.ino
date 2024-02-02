#include <PixyI2C.h>
#include <Zumo32U4.h>

//#define _DEBUG
#define NO_BUTTON

PixyI2C pixy;
Zumo32U4Motors motors;
#ifndef NO_BUTTON
Zumo32U4ButtonC button;
#endif

#define X_CENTER        160L
#define Y_CENTER        100L

class ServoLoop
{
public:
  int32_t   m_pos,
            m_prevError,
            m_kP, m_kD;

  ServoLoop(int32_t kP, int32_t kD) {
    m_pos = PIXY_RCS_CENTER_POS;
    m_prevError = 0x80000000L;
    m_kP = kP;
    m_kD = kD;
  }

  void  update(int32_t error) {
    long int velocity;
    if ( m_prevError != 0x8000000 ) {
      velocity = (error*m_kP + (error - m_prevError) * m_kD) >> 10;
      m_pos += velocity;
      if ( m_pos > PIXY_RCS_MAX_POS )
        m_pos = PIXY_RCS_MAX_POS;
      else if ( m_pos < PIXY_RCS_MIN_POS )
        m_pos = PIXY_RCS_MIN_POS;
    }
    m_prevError = error;
  }
};

ServoLoop   panLoop(200, 200);
ServoLoop   tiltLoop(150, 200);
//ServoLoop   panLoop(350, 600);
//ServoLoop   tiltLoop(500, 700);
int         oldX, oldY, oldSignature,
            scanIncrement = (PIXY_RCS_MAX_POS - PIXY_RCS_MIN_POS) / 150;
int32_t     sz = 400;
uint32_t    lastBlockTime = 0, lastMove = 0;

int TrackBlock(int blockCount)
{
  int   trackedBlock = 0;
  long  maxSize = 0;
#ifdef _DEBUG
  Serial.print("blocks =");
  Serial.println(blockCount);
#endif
  for ( int i=0; i<blockCount; i++ ) {
    if ( oldSignature==0 || pixy.blocks[i].signature==oldSignature ) {
      long  newSize = pixy.blocks[i].height * pixy.blocks[i].width;
      if ( newSize > maxSize ) {
        trackedBlock = i;
        maxSize = newSize;
      }
    }
  }

  int32_t   panError  = X_CENTER - pixy.blocks[trackedBlock].x;
  int32_t   tiltError = pixy.blocks[trackedBlock].y - Y_CENTER;
  panLoop.update(panError);
  tiltLoop.update(tiltError);
  pixy.setServos(panLoop.m_pos, tiltLoop.m_pos);

  oldX = pixy.blocks[trackedBlock].x;
  oldY = pixy.blocks[trackedBlock].y;
  oldSignature = pixy.blocks[trackedBlock].signature;

  return trackedBlock;
}

void FollowBlock(int trackedBlock)
{
  int32_t followError = PIXY_RCS_CENTER_POS - panLoop.m_pos;

  sz += pixy.blocks[trackedBlock].width * pixy.blocks[trackedBlock].height;
  sz -= sz >> 3;

  int     forwardSpeed = constrain(400 - (sz/256), -100, 400);
  int32_t diff = (followError + (followError*forwardSpeed)) >> 8;
  int     l = constrain(forwardSpeed + diff, -400, 400),
          r = constrain(forwardSpeed - diff, -400, 400);
  motors.setLeftSpeed(l);
  motors.setRightSpeed(r);
}

void ScanForBlocks()
{
  if ( millis() - lastMove > 20 ) {
    lastMove = millis();
    panLoop.m_pos += scanIncrement;
    if ( panLoop.m_pos>=PIXY_RCS_MAX_POS || panLoop.m_pos<=PIXY_RCS_MIN_POS ) {
      tiltLoop.m_pos = random(PIXY_RCS_MAX_POS*0.6, PIXY_RCS_MAX_POS);
      scanIncrement = -scanIncrement;
      if ( scanIncrement < 0 ) {
        motors.setLeftSpeed(-180);
        motors.setRightSpeed(+180);
      }
      else {
        motors.setLeftSpeed(+180);
        motors.setRightSpeed(-180);
      }
      delay(random(250, 500));
    }
    pixy.setServos(panLoop.m_pos, tiltLoop.m_pos);
  }
}

void setup()
{
#ifdef _DEBUG  
  Serial.begin(9600);
  Serial.print("Starting...\n");
#endif
#ifndef NO_BUTTON
  button.waitForButton();
#endif
  pixy.init();
}

void loop() 
{ 
  uint16_t  blocks = pixy.getBlocks();

  if ( blocks ) {
    FollowBlock( TrackBlock(blocks) );
    lastBlockTime = millis();
  }
  else if ( millis() - lastBlockTime > 100 ) {
    motors.setSpeeds(0, 0);
    ScanForBlocks();
  }
}


