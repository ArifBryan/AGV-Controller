#include <Wire.h>

#define SENS1A_ADDRESS  0b0100110
#define SENS1B_ADDRESS  0b0100101
#define SENS2A_ADDRESS  0b0100100
#define SENS2B_ADDRESS  0b0100011

byte sensLUT[15] = {8, 9, 10, 12, 13, 14, 15, 11, 0, 1, 2, 4, 5, 6, 7};

bool sens1Data[15];
bool sens2Data[15];
bool sens_st = 1;

void LineSensor_Init(){
  //Wire.begin();
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

//  for(byte i = 0; i < 15; i ++){
//    Serial.print(sens1Data[i]);
//  }
//  Serial.print(" ");
//  for(byte i = 0; i < 15; i ++){
//    Serial.print(sens2Data[i]);
//  }
//  Serial.println();
}

void LineSensor1_GetBits(bool bits[15]){
  memcpy(bits, sens1Data, 15);
}

int8_t pos1 = 0;

int8_t LineSensor1_Position(uint8_t mode){  
  uint8_t l = 0;
  uint8_t r = 0;

  switch(mode){
    case 0:
      for(uint8_t i = 0; i < 15; i++){
        if(sens1Data[i] && r == 0){r = i + 1;}
        if(sens1Data[14 - i] && l == 0){l = i + 1;}
      }
      if(l != 0 && r != 0){
        pos1 = l - r;
      }
      else{
        if(pos1 < 0){pos1 = -15;}
        else if(pos1 > 0){pos1 = 15;}
      }
    break;
    case 1:
      for(uint8_t i = 0; i < 15; i++){
        if(sens1Data[i] && r == 0){r = i + 1;}
        if(sens1Data[i] && r != 0 ){l = i + 1;}
        if(l > r && !sens1Data[i]){break;}
      }
      if(l != 0 && r != 0){
        l = 16 - l;
        //pos1 = 4 - r;
        if(r == 1 && l == 1){
          pos1 = 8 - r;
          pos1 *= 2;
        }
        else if(l > 1){
          pos1 = 8 - r;
          pos1 *= 2;
        }
        else{
          pos1 = l - r;
        }
      }
      else{
        if(pos1 < 0){pos1 = -15;}
        else if(pos1 > 0){pos1 = 15;}
      }
    break;

  }

  //if(pos1 < 0){pos1 = -pow(pos1, 2);}
  //else{pos1 = pow(pos1, 2);}

  return -pos1;
}

void LineSensor2_GetBits(bool bits[15]){
  memcpy(bits, sens2Data, 15);
}

int8_t pos2;

int8_t LineSensor2_Position(uint8_t mode){  
  uint8_t l = 0;
  uint8_t r = 0;

  switch(mode){
    case 0:
      for(uint8_t i = 0; i < 15; i++){
        if(sens2Data[i] && r == 0){r = i + 1;}
        if(sens2Data[14 - i] && l == 0){l = i + 1;}
      }
      if(l != 0 && r != 0){
        pos2 = l - r;
      }
      else{
        if(pos2 < 0){pos2 = -15;}
        else if(pos2 > 0){pos2 = 15;}
      }
    break;

  }

  //if(pos2 < 0){pos2 = -pow(pos2, 2);}
  //else{pos2 = pow(pos2, 2);}

  return pos2;
}
