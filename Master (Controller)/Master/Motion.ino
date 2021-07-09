#define SLAVE1ID 0x01
#define SLAVE2ID 0x02
#define SLAVE3ID 0x03

#define DRIVE_STOP        0
#define DRIVE_NORTH       1
#define DRIVE_NORTHWEST   2
#define DRIVE_NORTHEAST   3
#define DRIVE_WEST        4
#define DRIVE_SOUTH       5
#define DRIVE_SOUTHWEST   6
#define DRIVE_SOUTHEAST   7
#define DRIVE_EAST        8

#define HEAD_FRONT  0
#define HEAD_REAR   1

#define LINE_MODE_CENTER  0
#define LINE_MODE_LEFT    1
#define LINE_MODE_RIGHT   2

#define HEADING_UNKNOWN 0
#define HEADING_NORTH   1
#define HEADING_WEST    2
#define HEADING_SOUTH   3
#define HEADING_EAST    4

#define MSF  0  // Straight-Forward
#define MSL  1  // Straight-Left
#define MSR  2  // Straight-Right
#define MFF  3  // Flip-Forward
#define MFL  4  // Flip-Left
#define MFR  5  // Flip-Right

uint8_t _driveLUT[5][9] = {
// N    NW   NE   W    S    SW   SE   E     //
  {MSF, MSL, MSR, MSL, MFF, MFL, MFR, MSR}, // 0
  {MSF, MSL, MSR, MSL, MFF, MFR, MFL, MSR}, // N
  {MSR, MSR, MFL, MSF, MSL, MSL, MFR, MFF}, // W
  {MFF, MFL, MFR, MSR, MSF, MSR, MSL, MSL}, // S
  {MSL, MFR, MSL, MFF, MSR, MFL, MSR, MSF}  // E
};

uint32_t loopTmr;
uint8_t _drive;
uint8_t _head;
uint8_t _heading;
uint8_t _lineMode;
int16_t _xVel;
float linePos;

void Motion_Init(){
  Slave_Init(SLAVE1ID);
  Slave_Init(SLAVE2ID);
  Slave_Init(SLAVE3ID);
  Slave_PID_SetConstants(SLAVE1ID, 1.2, 0.1, 2.0);  // Konstanta PID steering.
  Slave_PID_SetConstants(SLAVE2ID, 0.7, 0.3, 0.01); // Konstanta PID motor kiri.
  Slave_PID_SetConstants(SLAVE3ID, 0.7, 0.3, 0.01); // Konstanta PID motor kanan.
  PID_SetRange(-15, 15, -100, 100);
  //PID_SetConstants(0.75, 0.06, 0.4);  // Konstanta PID (kP, kI, kD).
  Drive(0, 0, 0);
}

void Motion_SetHeading(uint8_t heading){
  _heading = heading;
}

void Motion_ManualDrive(float xVel,float w1Vel,float w2){
  Drive(xVel, w1Vel, w2);
}

uint8_t Motion_GetHeading(){
  return _heading;
}

uint8_t Motion_GetDrive(){
  return _drive;
}

uint8_t Motion_GetHead(){
  return _head;
}

void Motion_Drive(uint8_t drive){
  Motion_Drive(drive, _xVel);
}

void Motion_Drive(uint8_t drive, int16_t vel){
  _xVel = vel;
  if(drive != _drive || 1){
    _drive = drive;
    if(_drive != DRIVE_STOP){
      switch(_driveLUT[_heading][_drive - 1]){
        case MSF:
          if(_heading == HEADING_UNKNOWN) _head = 0;
          _lineMode = LINE_MODE_CENTER;
        break;
        case MSL:
          if(_heading == HEADING_UNKNOWN) _head = 0;
          _lineMode = LINE_MODE_LEFT;
        break;
        case MSR:
          if(_heading == HEADING_UNKNOWN) _head = 0;
          _lineMode = LINE_MODE_RIGHT;
        break;
        case MFF:
          if(_heading == HEADING_UNKNOWN) _head = 1;
          else{
            _HeadFlip();
            PID_Reset();
            Slave_Init(SLAVE2ID);
            Slave_Init(SLAVE3ID);
          }
          _lineMode = LINE_MODE_CENTER;
        break;
        case MFL:
          if(_heading == HEADING_UNKNOWN) _head = 1;
          else{
            _HeadFlip();
            PID_Reset();
            Slave_Init(SLAVE2ID);
            Slave_Init(SLAVE3ID);
          }
          _lineMode = LINE_MODE_LEFT;
        break;
        case MFR:
          if(_heading == HEADING_UNKNOWN) _head = 1;
          else{
            _HeadFlip();
            PID_Reset();
            Slave_Init(SLAVE2ID);
            Slave_Init(SLAVE3ID);
          }
          _lineMode = LINE_MODE_RIGHT;
        break;
      }
    }
    else{
      Drive(0, 0, 0);
      Slave_Init(SLAVE1ID);
      Slave_Init(SLAVE2ID);
      Slave_Init(SLAVE3ID);
    }
    //Serial.print("head:");
    //Serial.println(_head);
  }
}

void Motion_Handler(){
  uint8_t kF = 2;

  int pos = LineSensor_Position(_head, _lineMode);
  linePos = (linePos * kF) + pos;
  linePos /= kF + 1;

  if(millis() - loopTmr >= 50){
    loopTmr = millis();
    
    //Serial.println(linePos);
    
    int16_t xspeed = 0; 
    if(_lineMode != LINE_MODE_CENTER){
      xspeed = _xVel / 1.7 / (abs(0 - linePos) / 20 + 1);       
    }
    else{
      xspeed = _xVel / (abs(0 - linePos) / 20 + 1); 
    }
    if(abs(linePos) > 14){
      PID_SetConstants(0.6, 0.1, 1.2);  // Konstanta PID (kP, kI, kD).
    }
    else if(abs(linePos) > 8){
      PID_SetConstants(0.26, 0.07, 1.2);  // Konstanta PID (kP, kI, kD).
    }
    else{
      PID_SetConstants(0.15, 0.005, 0.78);  // Konstanta PID (kP, kI, kD).
    }
    if(_drive != DRIVE_STOP){      
      if(_head == HEAD_FRONT){
        PID_Calculate(0, linePos);
        Drive(xspeed, PID_GetU() * 1.0, PID_GetU()); // Drive robot (X, w0, w1);  
        //Drive(0, 0, PID_GetU() * 0.20); // Drive robot (X, w0, w1);    
      }
      else if(_head == HEAD_REAR){
        PID_Calculate(0, linePos);
        Drive(-xspeed, PID_GetU() * 1.0, -PID_GetU()); // Drive robot (X, w0, w1);   
        //Drive(0, PID_GetU(), PID_GetU()); // Drive robot (X, w0, w1);
      }
    }
    else{
      PID_Reset();
    }  
  }
}

void _HeadFlip(){
  _head = (_head == HEAD_FRONT ? HEAD_REAR : HEAD_FRONT);
}

void Drive(float x, float w0, float w1){
  Slave_PID_SetPoint(SLAVE1ID, -w1);          // Steering
  Slave_PID_SetPoint(SLAVE2ID, (x - w0));     // Left
  Slave_PID_SetPoint(SLAVE3ID, (-x - w0));    // Right
}
