#include <Wire.h>

void _I2CSlave_ReceiveHandler(int n);
void (*ReceiveCallbackHandler)(uint8_t, uint8_t*, uint8_t);

void I2CSlave_Init(uint8_t Address, void (*ReceiveCallback)(uint8_t, uint8_t*, uint8_t)){
  Wire.begin(Address);
  Wire.onReceive(&_I2CSlave_ReceiveHandler);
  ReceiveCallbackHandler = ReceiveCallback;
}

void _I2CSlave_ReceiveHandler(int n){
  uint8_t cmd;
  uint8_t data[20];
  memset(data, 0, 20);
  cmd = Wire.read();
  for(uint8_t i = 0; Wire.available() > 0; i++){
    data[i] = Wire.read();
  }
  ReceiveCallbackHandler(cmd, data, n - 1);
}