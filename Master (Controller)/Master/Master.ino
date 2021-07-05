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

uint8_t nav_dest1;
uint8_t nav_dest2;

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
}

void SerialNodeMCU_CallbackHandler(char data[22]){
  Serial.println(data);
  sscanf(data, ">R%ddD%d", &nav_dest1, &nav_dest2);
  if(Motion_GetDrive() == DRIVE_STOP)
    Mapping_Destination(nav_dest1);
    UserInterface_Beep(100);
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
