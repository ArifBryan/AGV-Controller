#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  Sample();
  delay(100);
}

#define SENS1A_ADDRESS  0b0100110
#define SENS1B_ADDRESS  0b0100101
#define SENS2A_ADDRESS  0b0100100
#define SENS2B_ADDRESS  0b0100011

byte sensLUT[15] = {8, 9, 10, 12, 13, 14, 15, 11, 0, 1, 2, 4, 5, 6, 7};

bool sens1data[15];
bool sens2data[15];
bool sens_st = 1;

void Sample(){
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
    sens1data[i] = (a >> sensLUT[i]) & 1;
    sens2data[i] = (b >> sensLUT[i]) & 1;
  }

  for(byte i = 0; i < 15; i ++){
    Serial.print(sens1data[i]);
  }
  Serial.print(" ");
  for(byte i = 0; i < 15; i ++){
    Serial.print(sens2data[i]);
  }
  Serial.println();
}
