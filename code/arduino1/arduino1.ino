#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <nRF24L01.h> //INCLUSÃO DE BIBLIOTECA
#include <RF24.h> //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA

#define SS_PIN 10 //PINO SDA
#define RST_PIN 9 //PINO DE RESET

MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

RF24 radio(5, 6); //CRIA UMA INSTÂNCIA UTILIZANDO OS PINOS (CE, CSN)

const byte address[6] = "00005"; //CRIA UM ENDEREÇO PARA ENVIO DOS
const byte address2[6] = "00004";
//DADOS (O TRANSMISSOR E O RECEPTOR DEVEM SER CONFIGURADOS COM O MESMO ENDEREÇO)

void setup() {
  Serial.begin(9600); //INICIALIZA A SERIAL
  SPI.begin(); //INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522
  radio.begin(); //INICIALIZA A COMUNICAÇÃO SEM FIO
  radio.openReadingPipe(0, address); //DEFINE O ENDEREÇO PARA RECEBIMENTO DE DADOS VINDOS DO TRANSMISSOR
  radio.openWritingPipe(address2);
  radio.setPALevel(RF24_PA_HIGH); //DEFINE O NÍVEL DO AMPLIFICADOR DE POTÊNCIA
  radio.startListening(); //DEFINE O MÓDULO COMO RECEPTOR (NÃO ENVIA DADOS)
}

void loop() {
  if (radio.available()) { //SE A COMUNICAÇÃO ESTIVER HABILITADA, FAZ
    char text[32] = ""; //VARIÁVEL RESPONSÁVEL POR ARMAZENAR OS DADOS RECEBIDOS
    radio.read(&text, sizeof(text)); //LÊ OS DADOS RECEBIDOS
    String tag = String(text);
    if(!tag.equals("")){
      Serial.print(tag); //IMPRIME NA SERIAL OS DADOS RECEBIDOS
      while(Serial.available() == 0){}
      String ret = "";
      char c;
      while(Serial.available() > 0){
        c = Serial.read();
        if(c != '\n'){
          ret += c;
        }
        delay(10);
      }
      radio.stopListening();
      char *retCStr = ret.c_str();
      radio.write(retCStr, ret.length());
      
      radio.startListening();
    }
  }
  
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
  return; //RETORNA PARA LER NOVAMENTE
  /***INICIO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  String strID = "I";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX);
  }
  strID.toUpperCase();
  /***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  rfid.PICC_HaltA(); //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD
  Serial.print(strID); //IMPRIME NA SERIAL O UID DA TAG RFID
}
