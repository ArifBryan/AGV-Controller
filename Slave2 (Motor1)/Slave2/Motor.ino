
void Motor_Init(){
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void Motor_Drive(float Velocity){
  Velocity = (Velocity > 100 ? 100 : (Velocity < -100 ? -100 : Velocity));
  digitalWrite(8, 1);
  if(Velocity < 0){
    digitalWrite(10, 0);
    analogWrite(9, abs(Velocity) * 2.55);
  }
  else if(Velocity > 0){
    digitalWrite(9, 0);
    analogWrite(10, abs(Velocity) * 2.55);
  }
  else{
    digitalWrite(9, 0);
    digitalWrite(10, 0);
  }
}

void Motor_Release(){
  digitalWrite(8, 0);
  digitalWrite(9, 0);
  digitalWrite(10, 0);
}