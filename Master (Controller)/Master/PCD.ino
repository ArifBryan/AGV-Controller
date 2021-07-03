#include <SPI.h>
#include <MFRC522.h>

#define PCD1_INT_PIN  A11
#define PCD2_INT_PIN  A12

MFRC522 pcd1(53, 49);
MFRC522 pcd2(10, 48);

uint32_t pcdTmr;

bool lastPcd1;
bool lastPcd2;

void (*_PCD1Handler)(uint8_t[]);
void (*_PCD2Handler)(uint8_t[]);

void PCD_Init(void (*PCD1Callback_Handler)(uint8_t[]), void (*PCD2Callback_Handler)(uint8_t[])){
  SPI.begin();
  pcd1.PCD_Init();
  pcd2.PCD_Init();

  pcd1.PCD_WriteRegister(pcd1.ComIEnReg, 0xA0);
  pcd2.PCD_WriteRegister(pcd2.ComIEnReg, 0xA0);

  pinMode(PCD1_INT_PIN, INPUT_PULLUP);
  pinMode(PCD2_INT_PIN, INPUT_PULLUP);

  pcd1.PCD_WriteRegister(pcd1.ComIrqReg, 0x7F);
  pcd2.PCD_WriteRegister(pcd2.ComIrqReg, 0x7F);
  
  _PCD1Handler = PCD1Callback_Handler;
  _PCD2Handler = PCD2Callback_Handler;
}

void activateRec(MFRC522 mfrc522);

void PCD_Handler(){
  if(digitalRead(PCD1_INT_PIN) == LOW && lastPcd1 == HIGH){
    lastPcd1 = LOW;
    pcd1.PICC_ReadCardSerial();
    uint8_t uid[6];
    memcpy(uid, pcd1.uid.uidByte, 4);
    _PCD1Handler(uid);
    pcd1.PCD_WriteRegister(pcd1.ComIrqReg, 0x7F);
    pcd1.PICC_HaltA();
  }
  else if(digitalRead(PCD1_INT_PIN) == HIGH){
    lastPcd1 = HIGH;
  }
  if(digitalRead(PCD2_INT_PIN) == LOW && lastPcd2 == HIGH){
    lastPcd2 = LOW;
    pcd2.PICC_ReadCardSerial();
    uint8_t uid[6];
    memcpy(uid, pcd2.uid.uidByte, 4);
    _PCD2Handler(uid);
    pcd2.PCD_WriteRegister(pcd2.ComIrqReg, 0x7F);
    pcd2.PICC_HaltA();
  }
  else if(digitalRead(PCD2_INT_PIN) == HIGH){
    lastPcd2 = HIGH;
  }
  if(millis() - pcdTmr >= 80){
    pcdTmr = millis();
    activateRec(pcd1);
    activateRec(pcd2);
  }
}

void activateRec(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);
  mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
  mfrc522.PCD_WriteRegister(mfrc522.BitFramingReg, 0x87);
}
