#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SLAVE1ID 0x01
#define SLAVE2ID 0x02
#define SLAVE3ID 0x03

enum steering_t{
  STEER_NORMAL,
  STEER_AGGRESIVE
};
enum drive_t{
  DRIVE_STOP,
  DRIVE_FORWARD,
  DRIVE_REVERSE
};
enum heading_t{
  HEAD_STRAIGHT,
  HEAD_LEFT,
  HEAD_RIGHT
};

steering_t steeringMode;
drive_t driveMode;
heading_t heading;

volatile bool PCD1_Detect;
volatile bool PCD2_Detect;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  Wire.setClock(400000);
  Wire.begin();
  Serial.begin(115200);

  delay(100);

  LineSensor_Init();
  PowerMonitor_Init();
  PCD_Init(&PCD1_CallbackHandler, &PCD2_CallbackHandler);
  Slave_Init(SLAVE1ID);
  Slave_Init(SLAVE2ID);
  Slave_Init(SLAVE3ID);
  UserInterface_Init();
  lcd.setCursor(0, 0);
  lcd.print("AGV");
  Beeper_Enable(1);
  delay(100);
  Beeper_Enable(0);

  Slave_PID_SetConstants(SLAVE1ID, 1.2, 0.1, 2.0);  // Konstanta PID steering.
  Slave_PID_SetConstants(SLAVE2ID, 0.7, 0.3, 0.01); // Konstanta PID motor kiri.
  Slave_PID_SetConstants(SLAVE3ID, 0.7, 0.3, 0.01); // Konstanta PID motor kanan.
  PID_SetRange(-15, 15, -100, 100);
  //PID_SetConstants(0.75, 0.06, 0.4);  // Konstanta PID (kP, kI, kD).
  Drive(0, 0, 0);
}

uint32_t filterTmr;
uint32_t loopTmr;
uint32_t dispTmr;

float linePos;

char dispBuff[21];

void loop() {
  int16_t xspeed = 400;
  float kF = 2;
  steeringMode = STEER_NORMAL;

  PCD_Handler();  

  if(heading != HEAD_STRAIGHT){
    steeringMode = STEER_AGGRESIVE;
  }
  else{
    steeringMode = STEER_NORMAL;
  }

  if(driveMode == DRIVE_FORWARD){
    int pos;
    if(heading == HEAD_STRAIGHT){
      pos = LineSensor1_Position(0);
    }
    else if(heading == HEAD_LEFT){
      pos = LineSensor1_Position(1);
    }
    else if(heading == HEAD_RIGHT){
      pos = LineSensor1_Position(2);
    }
    linePos = (linePos * kF) + pos;
    linePos /= kF + 1;
  }
  else if(driveMode == DRIVE_REVERSE){
    int pos;
    if(heading == HEAD_STRAIGHT){
      pos = LineSensor2_Position(0);
    }
    else if(heading == HEAD_LEFT){
      pos = LineSensor2_Position(1);
    }
    else if(heading == HEAD_RIGHT){
      pos = LineSensor2_Position(2);
    }
    linePos = (linePos * kF) + pos;
    linePos /= kF + 1;
  }

  if(millis() - loopTmr >= 50){
    loopTmr = millis();
    LineSensor_Scan();
    xspeed /= abs(0 - linePos) / 20 + 1; 
    if(abs(linePos) > 11){
      PID_SetConstants(0.57, 0.1, 1.2);  // Konstanta PID (kP, kI, kD).
    }
    else{
      PID_SetConstants(0.15, 0.005, 0.78);  // Konstanta PID (kP, kI, kD).
    }
    if(driveMode != DRIVE_STOP){      if(steeringMode == STEER_NORMAL){
        if(driveMode == DRIVE_FORWARD){
          //PID_SetConstants(0.7, 0.0, 0.0);  // Konstanta PID (kP, kI, kD).
          //PID_SetConstants(0.14, 0.001, 0.75);  // Konstanta PID (kP, kI, kD).
          PID_Calculate(0, linePos);
          Drive(xspeed, PID_GetU() * 1.5, PID_GetU()); // Drive robot (X, w0, w1);  
          //Drive(0, 0, PID_GetU() * 0.20); // Drive robot (X, w0, w1);    
        }
        else if(driveMode == DRIVE_REVERSE){
          //PID_SetConstants(0.15, 0.001, 0.75);  // Konstanta PID (kP, kI, kD).
          PID_Calculate(0, linePos);
          Drive(-xspeed, -PID_GetU() * 1.5, PID_GetU()); // Drive robot (X, w0, w1);   
          //Drive(0, 0, PID_GetU() * 0.20); // Drive robot (X, w0, w1);     
        }
      }
      else if(steeringMode == STEER_AGGRESIVE){
        if(driveMode == DRIVE_FORWARD){
          //PID_SetConstants(0.8, 0.1, 0.7);  // Konstanta PID (kP, kI, kD).
          //PID_SetConstants(0.13, 0.1, 0.5);  // Konstanta PID (kP, kI, kD).
          PID_Calculate(0, linePos);
          Drive(xspeed / 2, PID_GetU() * 1.5, PID_GetU()); // Drive robot (X, w0, w1);  
          //Drive(0, 0, PID_GetU() * 0.20); // Drive robot (X, w0, w1);    
        }
        else if(driveMode == DRIVE_REVERSE){
          //PID_SetConstants(0.13, 0.00, 0.6);  // Konstanta PID (kP, kI, kD).
          PID_Calculate(0, linePos);
          Drive(-xspeed / 2, -PID_GetU() * 1.5, PID_GetU()); // Drive robot (X, w0, w1);   
          //Drive(0, 0, PID_GetU() * 0.20); // Drive robot (X, w0, w1);        
        }
      }
    }
    else{
      PID_Reset();
    }  

    bool sensdata[10];
    if(driveMode == DRIVE_FORWARD){
      LineSensor1_GetBits(sensdata);
    }
    else if(driveMode == DRIVE_REVERSE){
      LineSensor2_GetBits(sensdata);
    }
    bool sta = 0;
    bool blk = 1;
    for(uint8_t i = 0; i < 15; i ++){
      sta |= sensdata[i];
      blk &= sensdata[i];
    }
    if(driveMode != DRIVE_STOP){
      if(sta == 0){
        if(driveMode == DRIVE_REVERSE){
          //driveMode = DRIVE_FORWARD;
        }
        else{
          //driveMode = DRIVE_STOP;
        }
      }
      if(blk == 1){
        //driveMode = DRIVE_STOP;
      }
    }
  }

  if(PCD1_Detect){
    PCD1_Detect = false;
    if(driveMode == DRIVE_FORWARD){
      heading = HEAD_LEFT;
    }
    Beeper_Enable(1);
  }  
  if(PCD2_Detect){
    PCD2_Detect = false;
    if(driveMode == DRIVE_REVERSE){
      driveMode = DRIVE_STOP;
    }
    Beeper_Enable(1);
  }    
  
  if(millis() - dispTmr >= 250){
    dispTmr = millis();

    char key = Keypad_GetKey();

    if(key == 'A'){
      driveMode = DRIVE_FORWARD;
      heading = HEAD_STRAIGHT;
    }
    else if(key == 'B'){
      driveMode = DRIVE_REVERSE;
      heading = HEAD_STRAIGHT;
    }
    else if(key != 0 && driveMode != DRIVE_STOP)   {
      Drive(0, 0, 0);
      driveMode = DRIVE_STOP;
      Slave_Init(SLAVE1ID);
      Slave_Init(SLAVE2ID);
      Slave_Init(SLAVE3ID);
    }
    if(driveMode == DRIVE_STOP){
      if(key == '9'){
        Drive(200, 50, 50);
      }
      else if(key == '8'){
        Drive(200, 0, 0);
      }
      else if(key == '7'){
        Drive(200, -50, -50);
      }
      else if(key == '6'){
        Drive(100, 70, 90);
      }
      else if(key == '4'){
        Drive(100, -70, -90);
      }
      else if(key == '3'){
        Drive(-200, -50, 50);
      }
      else if(key == '2'){
        Drive(-200, 0, 0);
      }
      else if(key == '1'){
        Drive(-200, 50, -50);
      }
      else{
        Drive(0, 0, 0);
      }
    }
    
    lcd.setCursor(0, 0);
    if(driveMode == DRIVE_STOP)        {lcd.print("STOP   ");}
    else if(driveMode == DRIVE_FORWARD){lcd.print("FORWARD");}
    else if(driveMode == DRIVE_REVERSE){lcd.print("REVERSE");}
    lcd.setCursor(0, 1);
    lcd.print("Line : ");
    lcd.print(linePos);
    lcd.print("   ");
    lcd.print(dispBuff);
    lcd.setCursor(0, 2);
    lcd.print("PID : ");
    lcd.print(PID_GetU());
    lcd.print("   ");
    lcd.setCursor(0, 3);
    lcd.print("Batt. : ");
    lcd.print(PowerMonitor_GetVoltage());
    lcd.print("V   ");

    if((!PCD1_Detect) && (!PCD2_Detect)){
      Beeper_Enable(0);
    }   
    
//    Serial.print(linePos);
//    Serial.print(',');
//    Serial.print(PowerMonitor_GetVoltage());
//    Serial.print(',');
//    Serial.print(PowerMonitor_GetCurrent());
//    Serial.println();
  }
}

void Drive(float x, float w0, float w1){
  Slave_PID_SetPoint(SLAVE1ID, -w1);          // Steering
  Slave_PID_SetPoint(SLAVE2ID, (x - w0));     // Left
  Slave_PID_SetPoint(SLAVE3ID, (-x - w0));    // Right
}

void PCD1_CallbackHandler(uint8_t uid[4]){
  Serial.print("PCD1: ");
  Serial.println(Tag_Lookup(uid));
  PCD1_Detect = true;
}

void PCD2_CallbackHandler(uint8_t uid[4]){
  Serial.print("PCD2: ");
  Serial.println(Tag_Lookup(uid));
  PCD2_Detect = true;
}
