#include <Wire.h>

void I2CMaster_Init(){
  //Wire.begin();
}

void I2CMaster_Transmit(uint8_t Address, uint8_t Command, uint8_t Data[], uint8_t Len){
  Wire.beginTransmission(Address);
  Wire.write(Command);
  for(uint8_t i = 0; i < Len; i++){
    Wire.write(Data[i]);
  }
  Wire.endTransmission();
}

void I2CMaster_Transmit(uint8_t Address, uint8_t Command){
  I2CMaster_Transmit(Address, Command, (uint8_t*){0}, 0);
}

void I2CMaster_Transmit(uint8_t Address, uint8_t Command, uint8_t Data){
  I2CMaster_Transmit(Address, Command, &Data, 1);
}

void I2CMaster_Transmit(uint8_t Address, uint8_t Command, uint32_t Data){
  uint8_t tmp[4] = {(uint8_t)(Data >> 24), (uint8_t)(Data >> 16), (uint8_t)(Data >> 8), (uint8_t)(Data)};
  I2CMaster_Transmit(Address, Command, tmp, 4);
}

void I2CMaster_Transmit(uint8_t Address, uint8_t Command, float Data){
  uint8_t *tmp = (byte*)&Data;
  I2CMaster_Transmit(Address, Command, tmp, 4);
}
