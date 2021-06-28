volatile float _pidKp;
volatile float _pidKi;
volatile float _pidKd;
float _pidInRangeMin;
float _pidInRangeMax;
float _pidOutRangeMin;
float _pidOutRangeMax;
float _pidLastError;
float _pidSumError;
float _pidU;

void PID_SetConstants(float Kp, float Ki, float Kd){
  _pidKp = Kp;
  _pidKi = Ki;
  _pidKd = Kd;
}

void PID_SetRange(float InMin, float InMax, float OutMin, float OutMax){
  _pidInRangeMin = InMin;
  _pidInRangeMax = InMax;
  _pidOutRangeMin = OutMin;
  _pidOutRangeMax = OutMax;
}

float PID_Calculate(float Sp, float Av){
  float e = (Sp - Av) / (_pidInRangeMax - _pidInRangeMin);

  float de = e - _pidLastError;

  float u = (e * _pidKp) + (_pidSumError * _pidKi) + (de * _pidKd);
  u = u * (_pidOutRangeMax - _pidOutRangeMin);
  u = (u > _pidOutRangeMax ? _pidOutRangeMax : (u < _pidOutRangeMin ? _pidOutRangeMin : u));

  _pidSumError += e;
  _pidSumError = (_pidSumError > 1.0 ? 1.0 : (_pidSumError < -1.0 ? -1.0 : _pidSumError));

  _pidLastError = e;

  _pidU = u;

  return u;
}

void PID_Reset(){
  _pidLastError = 0;
  _pidSumError = 0;
  _pidU = 0;
}

float PID_GetU(){
  return _pidU;
}
