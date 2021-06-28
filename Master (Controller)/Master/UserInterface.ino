const uint8_t _keypad_pins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};

void UserInterface_Init(){
  lcd.begin(20, 4);
  lcd.backlight();
  for(uint8_t i = 0; i < 4; i++){
    pinMode(_keypad_pins[i], OUTPUT);
    pinMode(_keypad_pins[i + 4], INPUT_PULLUP);
  }
  pinMode(13, OUTPUT);
}

void Beeper_Enable(bool en){
  digitalWrite(13, en);
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
