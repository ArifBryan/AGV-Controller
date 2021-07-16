#define BEEPER_PIN  13
#define LED_RED_PIN 12
#define LED_GRN_PIN 11

#define LED_FLASH_INTERVAL  250

#define LED_OFF   0
#define LED_ON    1
#define LED_FLASH 2

const uint8_t _keypad_pins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};

uint32_t beeperTmr;
uint32_t beepTime;
uint32_t ledTmr;
uint8_t _ledRedState;
uint8_t _ledGrnState;

void UserInterface_Init(){
  lcd.begin();
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
  if(millis() - ledTmr >= LED_FLASH_INTERVAL){
    ledTmr = millis();
    digitalWrite(LED_RED_PIN, (_ledRedState == LED_FLASH ? !digitalRead(LED_RED_PIN) : (_ledRedState == LED_ON ? 1 : 0)));
    digitalWrite(LED_GRN_PIN, (_ledGrnState == LED_FLASH ? !digitalRead(LED_GRN_PIN) : (_ledGrnState == LED_ON ? 1 : 0)));
  }
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
