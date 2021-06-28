#include <Wire.h>

#define I2C_SLV_ADDR  0x02
#define I2C_LED_STA_Pin LED_BUILTIN

extern volatile uint32_t timer0_millis;

uint32_t I2C_LED_STA_Timer;
volatile float var_SetPoint;
volatile bool I2C_ACT_Flag;

uint32_t tmr;

void setup() {
  pinMode(I2C_LED_STA_Pin, OUTPUT);
  Motor_Init();
  Encoder_Init(24, 9);
  I2CSlave_Init(I2C_SLV_ADDR, &I2CReceive_Handler);
  Serial.begin(115200);

  PID_SetRange(-700, 700, -100, 100);
}

void loop() {
  if(I2C_ACT_Flag && millis() - I2C_LED_STA_Timer >= 100){
    I2C_LED_STA_Timer = millis();
    digitalWrite(I2C_LED_STA_Pin, 1);
    I2C_ACT_Flag = false;
  }
  else if(millis() - I2C_LED_STA_Timer >= 50){
    digitalWrite(I2C_LED_STA_Pin, 0);
  }
  else if(millis() - I2C_LED_STA_Timer > 100){
    I2C_LED_STA_Timer = millis() - 100;
  }

  if(millis() - tmr >= 50){
    tmr = millis();
    float speedActual = Encoder_GetSpeed();
    if(var_SetPoint < 0){speedActual *= -1;}
    if(var_SetPoint != 0){
      if(var_SetPoint < 0){
        PID_Calculate(var_SetPoint, speedActual);
        Motor_Drive(PID_GetU());
      }
      else{
        PID_Calculate(var_SetPoint, speedActual);
        Motor_Drive(PID_GetU());
      }
    }
    else{
      Motor_Drive(0);
      PID_Reset();
    }
    Serial.println(speedActual);
  }

  Encoder_Handler();
  
}

void I2CReceive_Handler(uint8_t Command, uint8_t *Data, uint8_t Len){
  I2C_ACT_Flag = true;
  switch(Command){
    case(0):
      noInterrupts();
      timer0_millis = 0;
      PID_Reset();
      interrupts();
    break;
    case(1):
      float kp, ki, kd;
      memcpy(&kp, Data, 4);
      memcpy(&ki, Data + 4, 4);
      memcpy(&kd, Data + 8, 4);
      PID_SetConstants(kp, ki, kd);
    break;
    case(10):
      memcpy((float*)&var_SetPoint, Data, 4);
    break;
    default:
    break;
  }
}
