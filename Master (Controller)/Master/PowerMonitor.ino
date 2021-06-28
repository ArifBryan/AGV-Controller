#include <Wire.h>
#include <INA226.h>

INA226 ina;

void PowerMonitor_Init(){
  ina.begin();
  ina.configure(INA226_AVERAGES_4, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
  ina.calibrate(0.1, 4);
}

float PowerMonitor_GetVoltage(){
  return ina.readBusVoltage();
}

float PowerMonitor_GetCurrent(){
  return ina.readShuntCurrent();
}

float PowerMonitor_GetPower(){
  return ina.readBusPower();
}