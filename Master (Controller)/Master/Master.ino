#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Watchdog.h>

#define HEADING_UNKNOWN 0
#define HEADING_NORTH   1
#define HEADING_WEST    2
#define HEADING_SOUTH   3
#define HEADING_EAST    4

#define DRIVE_STOP        0
#define DRIVE_NORTH       1
#define DRIVE_NORTHWEST   2
#define DRIVE_NORTHEAST   3
#define DRIVE_WEST        4
#define DRIVE_SOUTH       5
#define DRIVE_SOUTHWEST   6
#define DRIVE_SOUTHEAST   7
#define DRIVE_EAST        8

#define LED_OFF   0
#define LED_ON    1
#define LED_FLASH 2

#define PCD_RIGHT 0
#define PCD_LEFT  1

uint32_t dispTmr;
uint8_t nav_dest1;
uint8_t nav_dest2;
char dispBuff[21];

LiquidCrystal_I2C lcd(0x27, 20, 4);
Watchdog watchdog;

void setup() {
  watchdog.enable(Watchdog::TIMEOUT_4S);
  Wire.setClock(400000);
  Wire.begin();
  Serial.begin(115200);

  delay(100);

  Motion_Init();
  PowerMonitor_Init();
  SerialInterface_Init(&SerialNodeMCU_CallbackHandler);
  PCD_Init(&PCD1_CallbackHandler, &PCD2_CallbackHandler);
  UserInterface_Init();
  
  lcd.setCursor(0, 0);
  lcd.print("AGV");
  UserInterface_Beep(100);
  Motion_Drive(DRIVE_STOP, 400);
}

void loop() {
  UserInterface_Handler();
  SerialInterface_Handler();
  LineSensor_Handler();
  PCD_Handler();  
  Mapping_Handler();
  Motion_Handler();
  watchdog.reset();
  
  if(millis() - dispTmr >= 250){
    dispTmr = millis();

    if(Motion_GetDrive() == DRIVE_STOP){
      if(Motion_GetHeading() == HEADING_UNKNOWN){
        UserInterface_LED(LED_FLASH, LED_OFF);      
      }
      else{
        UserInterface_LED(LED_ON, LED_OFF);          
      }
    }
    else{
      if(Motion_GetHeading() == HEADING_UNKNOWN){
        UserInterface_LED(LED_FLASH, LED_FLASH);      
      }
      else{
        UserInterface_LED(LED_OFF, LED_FLASH);          
      }
    }

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
//    Serial.print("Heading: ");  
//    Serial.println(Motion_GetHeading());
//    Serial.print("Drive  : ");  
//    Serial.println(Motion_GetDrive());
//    Serial.print("Pos    : ");  
//    Serial.println(Mapping_ArrivedPosition());
//    Serial.print("Dest   : ");  
//    Serial.println(Mapping_GetDestination());
//    Serial.print(',');
//    Serial.print(PowerMonitor_GetVoltage());
//    Serial.print(',');
//    Serial.print(PowerMonitor_GetCurrent());
//    Serial.println();
  }
}

void SerialNodeMCU_CallbackHandler(char data[22]){
  Serial.println(data);
  sscanf(data, ">R%ddD%d", &nav_dest1, &nav_dest2);
  if(Motion_GetDrive() == DRIVE_STOP)
    Mapping_Destination(nav_dest1);
//    Serial.println(nav_dest1);
}

void PCD1_CallbackHandler(uint8_t uid[4]){
  Serial.print("PCD1: ");
  Serial.println(Tag_Lookup(uid));
  Mapping_RFIDHandler(PCD_RIGHT, uid);
  UserInterface_Beep(50);
}

void PCD2_CallbackHandler(uint8_t uid[4]){
  Serial.print("PCD2: ");
  Serial.println(Tag_Lookup(uid));
  Mapping_RFIDHandler(PCD_LEFT, uid);
  UserInterface_Beep(50);
}
