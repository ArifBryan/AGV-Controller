#define BEEPER_PIN  13
#define LED_RED_PIN 12
#define LED_GRN_PIN 11

#define LED_FLASH_INTERVAL  250

#define LED_OFF   0
#define LED_ON    1
#define LED_FLASH 2

const uint8_t _keypad_pins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};

const char headingString[][8] = {
  "UNKNOWN",
  "NORTH  ",
  "WEST   ",
  "SOUTH  ",
  "EAST   "
};

uint32_t beeperTmr;
uint32_t beepTime;
uint32_t ledTmr;
uint32_t dispTmr;
uint8_t _ledRedState;
uint8_t _ledGrnState;
char dispBuffer[21];
bool dispUpdate = true;

void UserInterface_Init(){
  lcd.begin(20, 4);
  lcd.backlight();
  for(uint8_t i = 0; i < 4; i++){
    pinMode(_keypad_pins[i], OUTPUT);
    pinMode(_keypad_pins[i + 4], INPUT_PULLUP);
  }
  pinMode(BEEPER_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GRN_PIN, OUTPUT);
  digitalWrite(LED_RED_PIN, HIGH);
  digitalWrite(LED_GRN_PIN, HIGH);

  lcd.setCursor(0, 0);
  lcd.print("AGV");
  delay(1000);
  UserInterface_Beep(100);
}

void UserInterface_Beep(uint32_t t){
  beepTime = t;
  beeperTmr = millis();
  digitalWrite(BEEPER_PIN, HIGH);
}

void UserInterface_LED(uint8_t ledR, uint8_t ledG){
  _ledRedState = ledR;
  _ledGrnState = ledG;
}

void UserInterface_Handler(){
  // Keypad
  char key = Keypad_GetKey();

  if(key != 0 && Motion_GetDrive() != DRIVE_STOP){
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
//    else if(key == '4'){
//      Motion_ManualDrive(100, -70, -90);
//    }
//    else if(key == '3'){
//      Motion_ManualDrive(-200, -50, 50);
//    }
//    else if(key == '2'){
//      Motion_ManualDrive(-200, 0, 0);
//    }
//    else if(key == '1'){
//      Motion_ManualDrive(-200, 50, -50);
//    }
//    else{
//      Motion_ManualDrive(0, 0, 0);
//    }
  }
  /////
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
    /////
  
  // Display
  if(dispUpdate || millis() - dispTmr >= 500){
    dispTmr = millis();
    dispUpdate = false;

    lcd.setCursor(0, 0);
    sprintf(dispBuffer, "Heading : %s ", headingString[Motion_GetHeading()]);
    lcd.print(dispBuffer);
    lcd.setCursor(0, 1);
    sprintf(dispBuffer, "Position : %d", Mapping_GetPosition());
    lcd.print(dispBuffer);
    lcd.setCursor(0, 2);
    sprintf(dispBuffer, "Destination : %d", Mapping_GetDestination());
    lcd.print(dispBuffer);
    lcd.setCursor(0, 3);
    sprintf(dispBuffer, "Battery : %d.%02dV  ", PowerMonitor_GetVoltage(), (int)(PowerMonitor_GetVoltage() * 100) % 100);
    lcd.print(dispBuffer);
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

  // Status indicator
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
  
  // LED
  if(millis() - ledTmr >= LED_FLASH_INTERVAL){
    ledTmr = millis();
    digitalWrite(LED_RED_PIN, (_ledRedState == LED_FLASH ? !digitalRead(LED_RED_PIN) : (_ledRedState == LED_ON ? 1 : 0)));
    digitalWrite(LED_GRN_PIN, (_ledGrnState == LED_FLASH ? !digitalRead(LED_GRN_PIN) : (_ledGrnState == LED_ON ? 1 : 0)));
  }

  // Beeper
  if(beepTime > 0){
    if(millis() - beeperTmr >= beepTime){
      digitalWrite(BEEPER_PIN, LOW);
      beepTime = 0;
    }
    else if(millis() - beeperTmr >= beepTime * 2){
      beepTime = 0;
    }
  }
}

char Keypad_GetKey(){
  char t = 0;

  // Row 1
  digitalWrite(_keypad_pins[0], LOW);
  digitalWrite(_keypad_pins[1], HIGH);
  digitalWrite(_keypad_pins[2], HIGH);
  digitalWrite(_keypad_pins[3], HIGH);
  if(!digitalRead(_keypad_pins[4]))     {t = 'D';}
  else if(!digitalRead(_keypad_pins[5])){t = 'C';}
  else if(!digitalRead(_keypad_pins[6])){t = 'B';}
  else if(!digitalRead(_keypad_pins[7])){t = 'A';}

  // Row 2
  digitalWrite(_keypad_pins[0], HIGH);
  digitalWrite(_keypad_pins[1], LOW);
  digitalWrite(_keypad_pins[2], HIGH);
  digitalWrite(_keypad_pins[3], HIGH);
  if(!digitalRead(_keypad_pins[4]))     {t = '#';}
  else if(!digitalRead(_keypad_pins[5])){t = '9';}
  else if(!digitalRead(_keypad_pins[6])){t = '6';}
  else if(!digitalRead(_keypad_pins[7])){t = '3';}

  // Row 3
  digitalWrite(_keypad_pins[0], HIGH);
  digitalWrite(_keypad_pins[1], HIGH);
  digitalWrite(_keypad_pins[2], LOW);
  digitalWrite(_keypad_pins[3], HIGH);
  if(!digitalRead(_keypad_pins[4]))     {t = '0';}
  else if(!digitalRead(_keypad_pins[5])){t = '8';}
  else if(!digitalRead(_keypad_pins[6])){t = '5';}
  else if(!digitalRead(_keypad_pins[7])){t = '2';}

  // Row 4
  digitalWrite(_keypad_pins[0], HIGH);
  digitalWrite(_keypad_pins[1], HIGH);
  digitalWrite(_keypad_pins[2], HIGH);
  digitalWrite(_keypad_pins[3], LOW);
  if(!digitalRead(_keypad_pins[4]))     {t = '*';}
  else if(!digitalRead(_keypad_pins[5])){t = '7';}
  else if(!digitalRead(_keypad_pins[6])){t = '4';}
  else if(!digitalRead(_keypad_pins[7])){t = '1';}

  return t;
}
