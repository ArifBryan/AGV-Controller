float Resolver_GetPosition(){
  return ((analogRead(A1) - 394) / 1.165 - 100) * -1;
}