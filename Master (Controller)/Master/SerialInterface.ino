volatile char serial1Buffer[22];
volatile uint8_t serial1BufferPtr;
volatile uint8_t serial1BufferLen;

void (*_SerialInterfaceCallback_Handler)(char[]);

void SerialInterface_Init(void (*SerialInterfaceCallback_Handler)(char[])){
  _SerialInterfaceCallback_Handler = SerialInterfaceCallback_Handler;
  Serial1.begin(9600);
}

void SerialInterface_Handler(){
  if(serial1BufferLen > 0){
    _SerialInterfaceCallback_Handler((char*)serial1Buffer);
    _serial1BufferFlush();
  }
}

void _serial1BufferFlush(){
  serial1BufferLen = 0;
  memset(serial1Buffer, 0, 22);
}

void serialEvent1(){
  while(Serial1.available()){
    char data = Serial1.read();
    if(data == '>'){
      serial1Buffer[0] = data;
      serial1BufferPtr = 1;
    }
    else if(data == '\r' || data == '\n'){
      serial1BufferLen = serial1BufferPtr;
      serial1BufferPtr = 0;
    }
    else if(serial1BufferPtr > 0 && serial1BufferPtr < 20){
      serial1Buffer[serial1BufferPtr++] = data;      
    }
  }
}
