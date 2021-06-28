void Slave_Init(uint8_t SlaveID){
  I2CMaster_Transmit(SlaveID, 0);
}

void Slave_PID_SetConstants(uint8_t SlaveID, float Kp, float Ki, float Kd){
  uint8_t data[12];

  memcpy(data, &Kp, 4);
  memcpy(data + 4, &Ki, 4);
  memcpy(data + 8, &Kd, 4);
  I2CMaster_Transmit(SlaveID, 1, data, 12);
}

void Slave_PID_SetPoint(uint8_t SlaveID, float Sp){
  I2CMaster_Transmit(SlaveID, 10, Sp);
}