#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DRIVE_STOP        0
#define DRIVE_NORTH       1
#define DRIVE_NORTHWEST   2
#define DRIVE_NORTHEAST   3
#define DRIVE_WEST        4
#define DRIVE_WESTNORTH   5
#define DRIVE_WESTSOUTH   6
#define DRIVE_SOUTH       7
#define DRIVE_SOUTHWEST   8
#define DRIVE_SOUTHEAST   9
#define DRIVE_EAST        10
#define DRIVE_EASTNORTH   11
#define DRIVE_EASTSOUTH   12

#define PCD_RIGHT 0
#define PCD_LEFT  1

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

  Motion_Init();
  PowerMonitor_Init();
  PCD_Init(&PCD1_CallbackHandler, &PCD2_CallbackHandler);
  UserInterface_Init();
  lcd.setCursor(0, 0);
  lcd.print("AGV");
  Beeper_Enable(1);
  delay(100);
  Beeper_Enable(0);
  Motion_Drive(DRIVE_STOP, 400);
}

uint32_t filterTmr;
uint32_t loopTmr;
uint32_t dispTmr;

char dispBuff[21];

void loop() {
  LineSensor_Handler();
  PCD_Handler();  
  Mapping_Handler();
  Motion_Handler();

  if(PCD1_Detect){
    PCD1_Detect = false;
    if(Motion_GetDrive() == DRIVE_NORTH){
      //Motion_Drive(DRIVE_NORTHWEST, 200);
    }
    Beeper_Enable(1);
  }  
  if(PCD2_Detect){
    PCD2_Detect = false;
    if(Motion_GetDrive() == DRIVE_SOUTH){
      //Motion_Drive(DRIVE_SOUTH, 400);
    }
    Beeper_Enable(1);
  }    
  
  if(millis() - dispTmr >= 250){
    dispTmr = millis();

    char key = Keypad_GetKey();

    if(key != 0 && Motion_GetDrive() != DRIVE_STOP)   {
      Motion_Drive(DRIVE_STOP);
    }
    if(Motion_GetDrive() == DRIVE_STOP){
      if(key == '0'){
        Mapping_Destination(1);
      }
      else if(key == '1'){
        Mapping_Destination(2);
      }
      else if(key == '2'){
        Mapping_Destination(3);
      }
      else if(key == '3'){
        Mapping_Destination(4);
      }
//      else if(key == '4'){
//        Motion_ManualDrive(100, -70, -90);
//      }
//      else if(key == '3'){
//        Motion_ManualDrive(-200, -50, 50);
//      }
//      else if(key == '2'){
//        Motion_ManualDrive(-200, 0, 0);
//      }
//      else if(key == '1'){
//        Motion_ManualDrive(-200, 50, -50);
//      }
//      else{
//        Motion_ManualDrive(0, 0, 0);
//      }
    }
    
    if(Motion_GetDrive() == DRIVE_STOP && 0){
      if(key == '9'){
        Motion_ManualDrive(200, 50, 50);
      }
      else if(key == '8'){
        Motion_ManualDrive(200, 0, 0);
      }
      else if(key == '7'){
        Motion_ManualDrive(200, -50, -50);
      }
      else if(key == '6'){
        Motion_ManualDrive(100, 70, 90);
      }
      else if(key == '4'){
        Motion_ManualDrive(100, -70, -90);
      }
      else if(key == '3'){
        Motion_ManualDrive(-200, -50, 50);
      }
      else if(key == '2'){
        Motion_ManualDrive(-200, 0, 0);
      }
      else if(key == '1'){
        Motion_ManualDrive(-200, 50, -50);
      }
      else{
        Motion_ManualDrive(0, 0, 0);
      }
    }
//    
//    lcd.setCursor(0, 0);
//    if(driveMode == DRIVE_STOP)        {lcd.print("STOP   ");}
//    else if(driveMode == DRIVE_FORWARD){lcd.print("FORWARD");}
//    else if(driveMode == DRIVE_REVERSE){lcd.print("REVERSE");}
//    lcd.setCursor(0, 1);
//    lcd.print("Line : ");
//    lcd.print(linePos);
//    lcd.print("   ");
//    lcd.print(dispBuff);
//    lcd.setCursor(0, 2);
//    lcd.print("PID : ");
//    lcd.print(PID_GetU());
//    lcd.print("   ");
//    lcd.setCursor(0, 3);
//    lcd.print("Batt. : ");
//    lcd.print(PowerMonitor_GetVoltage());
//    lcd.print("V   ");
//
    if((!PCD1_Detect) && (!PCD2_Detect)){
      Beeper_Enable(0);
    }   
//  
    Serial.print("Heading: ");  
    Serial.println(Motion_GetHeading());
    Serial.print("Drive  : ");  
    Serial.println(Motion_GetDrive());
    Serial.print("Head   : ");  
    Serial.println(Motion_GetHead());
//    Serial.print(',');
//    Serial.print(PowerMonitor_GetVoltage());
//    Serial.print(',');
//    Serial.print(PowerMonitor_GetCurrent());
//    Serial.println();
  }
}


void PCD1_CallbackHandler(uint8_t uid[4]){
  Serial.print("PCD1: ");
  Serial.println(Tag_Lookup(uid));
  Mapping_RFIDHandler(PCD_RIGHT, uid);
  PCD1_Detect = true;
}

void PCD2_CallbackHandler(uint8_t uid[4]){
  Serial.print("PCD2: ");
  Serial.println(Tag_Lookup(uid));
  Mapping_RFIDHandler(PCD_LEFT, uid);
  PCD2_Detect = true;
}
