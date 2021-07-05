#include <Wire.h>

#define LINE_SAMPLE_INTERVAL  50

#define SENS1A_ADDRESS  0b0100110
#define SENS1B_ADDRESS  0b0100101
#define SENS2A_ADDRESS  0b0100100
#define SENS2B_ADDRESS  0b0100011

#define HEAD_FRONT  0
#define HEAD_REAR   1

#define LINE_MODE_CENTER  0
#define LINE_MODE_LEFT    1
#define LINE_MODE_RIGHT   2

byte sensLUT[15] = {8, 9, 10, 12, 13, 14, 15, 11, 0, 1, 2, 4, 5, 6, 7};

bool sens1Data[15];
bool sens2Data[15];
bool sens_st = 1;

uint32_t _lineSampleTmr;

void LineSensor_Handler(){
  if(millis() - _lineSampleTmr >= LINE_SAMPLE_INTERVAL){
    _lineSampleTmr = millis();
    LineSensor_Scan();
  }
}

void LineSensor_Scan(){
  sens_st = !sens_st;
  
  Wire.beginTransmission(SENS1A_ADDRESS);
  Wire.write(~(sens_st << 3));
  Wire.endTransmission();

  Wire.requestFrom(SENS1A_ADDRESS, 1);
  uint16_t a = Wire.read();

  Wire.requestFrom(SENS1B_ADDRESS, 1);
  a |= Wire.read() << 8;

  Wire.beginTransmission(SENS2A_ADDRESS);
  Wire.write(~(sens_st << 3));
  Wire.endTransmission();

  Wire.requestFrom(SENS2A_ADDRESS, 1);
  uint16_t b = Wire.read();

  Wire.requestFrom(SENS2B_ADDRESS, 1);
  b |= Wire.read() << 8;

  for(byte i = 0; i < 15; i ++){
    sens1Data[i] = !((a >> sensLUT[i]) & 1);
    sens2Data[i] = !((b >> sensLUT[i]) & 1);
  }
}

int8_t LineSensor_Position(uint8_t head, uint8_t mode){
  int8_t pos;
  
  if(head == HEAD_FRONT){
    pos = LineSensor1_Position(mode);
  }
  else if(head == HEAD_REAR){
    pos = LineSensor2_Position(mode);    
  }
  
  return pos;
}

uint8_t LineSensor_NumDetected(uint8_t head){
  uint8_t num = 0;
  
  if(head == HEAD_FRONT){
    for(uint8_t i = 0; i < 15; i ++){
      num += sens1Data[i] == 1;
    }
  }
  else if(head == HEAD_REAR){
    for(uint8_t i = 0; i < 15; i ++){
      num += sens2Data[i] == 1;
    }   
  }
  
  return num;
}

void LineSensor1_GetBits(bool bits[15]){
  memcpy(bits, sens1Data, 15);
}

int8_t pos1 = 0;

int8_t LineSensor1_Position(uint8_t mode){  
  uint8_t l = 0;
  uint8_t r = 0;

  switch(mode){
    case LINE_MODE_CENTER:
      for(uint8_t i = 0; i < 15; i++){
        if(sens1Data[i] && r == 0){r = i + 1;}
        if(sens1Data[14 - i] && l == 0){l = i + 1;}
      }
      if(r != 0 && l != 0){
        pos1 = r - l;
      }
      else{
        if(pos1 < 0){pos1 = -15;}
        else if(pos1 > 0){pos1 = 15;}
      }
    break;
    case LINE_MODE_RIGHT:
      for(uint8_t i = 0; i < 15; i++){
        if(sens1Data[14 - i] && l == 0){l = i + 1;}
        if(sens1Data[14 - i] && l != 0 ){r = i + 1;}
        if(r > l && !sens1Data[i]){break;}
      }
      if(l != 0 && r != 0){
        r = 16 - r;
        //pos1 = 4 - l;
        if(l == 1 && r == 1){
          pos1 = 7 - l;
          pos1 *= 2;
        }
        else if(r > 1){
          pos1 = 7 - l;
          pos1 *= 2;
        }
        else{
          pos1 = r - l;
        }
      }
      else{
        if(pos1 < 0){pos1 = -15;}
        else if(pos1 > 0){pos1 = 15;}
      }
    break;
    case LINE_MODE_LEFT:
      for(uint8_t i = 0; i < 15; i++){
        if(sens1Data[i] && r == 0){r = i + 1;}
        if(sens1Data[i] && r != 0 ){l = i + 1;}
        if(r < l && !sens1Data[i]){break;}
      }
      if(l != 0 && r != 0){
        l = l - 16;
        //pos1 = 4 - l;
        if(l == 1 && r == 1){
          pos1 = r - 7;
          pos1 *= 2;
        }
        else if(l > 1){
          pos1 = r - 7;
          pos1 *= 2;
        }
        else{
          pos1 = r - l;
        }
      }
      else{
        if(pos1 < 0){pos1 = -15;}
        else if(pos1 > 0){pos1 = 15;}
      }
    break;

  }

  return pos1;
}

void LineSensor2_GetBits(bool bits[15]){
  memcpy(bits, sens2Data, 15);
}

int8_t pos2;

int8_t LineSensor2_Position(uint8_t mode){  
  uint8_t l = 0;
  uint8_t r = 0;

  switch(mode){
    case LINE_MODE_CENTER:
      for(uint8_t i = 0; i < 15; i++){
        if(sens2Data[i] && r == 0){r = i + 1;}
        if(sens2Data[14 - i] && l == 0){l = i + 1;}
      }
      if(r != 0 && l != 0){
        pos2 = r - l;
      }
      else{
        if(pos2 < 0){pos2 = -15;}
        else if(pos2 > 0){pos2 = 15;}
      }
    break;
    case LINE_MODE_RIGHT:
      for(uint8_t i = 0; i < 15; i++){
        if(sens2Data[14 - i] && l == 0){l = i + 1;}
        if(sens2Data[14 - i] && l != 0 ){r = i + 1;}
        if(r > l && !sens1Data[i]){break;}
      }
      if(l != 0 && r != 0){
        r = 16 - r;
        //pos2 = 4 - l;
        if(l == 1 && r == 1){
          pos2 = 7 - l;
          pos2 *= 2;
        }
        else if(r > 1){
          pos2 = 7 - l;
          pos2 *= 2;
        }
        else{
          pos2 = r - l;
        }
      }
      else{
        if(pos2 < 0){pos2 = -15;}
        else if(pos2 > 0){pos2 = 15;}
      }
    break;
    case LINE_MODE_LEFT:
      for(uint8_t i = 0; i < 15; i++){
        if(sens2Data[i] && r == 0){r = i + 1;}
        if(sens2Data[i] && r != 0 ){l = i + 1;}
        if(r < l && !sens2Data[i]){break;}
      }
      if(l != 0 && r != 0){
        l = l - 16;
        //pos2 = 4 - l;
        if(l == 1 && r == 1){
          pos2 = r - 7;
          pos2 *= 2;
        }
        else if(l > 1){
          pos2 = r - 7;
          pos2 *= 2;
        }
        else{
          pos2 = r - l;
        }
      }
      else{
        if(pos2 < 0){pos2 = -15;}
        else if(pos2 > 0){pos2 = 15;}
      }
    break;

  }

  return pos2;
}
