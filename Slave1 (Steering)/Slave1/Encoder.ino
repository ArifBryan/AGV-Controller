
volatile uint32_t _encoderCounter1;
volatile uint32_t _encoderCounter2;
uint32_t _encoderMillis;
float _encoderSpeed;
float _encoderRatio;
uint8_t _encoderCPR;

void Encoder_Init(uint8_t CPR, float Ratio){
  _encoderCPR = CPR;
  _encoderRatio = Ratio;
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(2), _Encoder_Counter1_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), _Encoder_Counter2_ISR, CHANGE);
}

void _Encoder_Counter1_ISR(){
  _encoderCounter1++;  
}

void _Encoder_Counter2_ISR(){
  _encoderCounter2++;
}

void Encoder_Handler(){
  if(millis() - _encoderMillis >= 100){
    noInterrupts();
    _encoderSpeed = (float)_encoderCounter1 / _encoderCPR * 600 / _encoderRatio;
    _encoderCounter1 = 0;
    interrupts();
    _encoderMillis = millis();
  }
}

float Encoder_GetSpeed(){
  return _encoderSpeed;
}