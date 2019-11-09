//https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA
#include <string.h>

#define SS_PIN 20 //PINO SDA
#define RST_PIN 9 //PINO DE RESET

bool radioNumber = 1;
MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS
RF24 radio(7, 8); // CE, CSN


byte addresses[][6] = {"1Hard","2Izi"};


void setup() {
  radio.begin();
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
    radio.stopListening();
  }
  else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }

  SPI.begin(); //INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522

}
void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
    return; //RETORNA PARA LER NOVAMENTE

  /***INICIO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  strID.toUpperCase();
  /***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/

  //Serial.print("Identificador (UID) da tag: "); //IMPRIME O TEXTO NA SERIAL
  Serial.println(strID); //IMPRIME NA SERIAL O UID DA TAG RFID

  rfid.PICC_HaltA(); //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD
  char *text = strID.c_str();
  radio.write(text, strID.length());
  delay(1000);
}

 /*
  * //https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/

#include <SPI.h>
#include <RF24.h>
bool teste;
RF24 radio(7, 8); // CE, CSN Pode ser qualquer porta digital
//definindo os pipes
//const byte endereco[][6] = {"2ino", "1uno"};

void setup() {
  Serial.begin(9600);
  radio.begin();
  //define o pipe de escrita
  radio.openWritingPipe(0xF0F0F0F066);
  //define o pipe de leitura
  radio.openReadingPipe(2, 0xF0F0F0F0AA);

  radio.startListening();
}
void loop() {
  //precisa parar de ouvir para poder enviar
  //Serial.println("ola");
  radio.stopListening();
  const char text[32] = "Hello World";
  //Serial.println(text);
  teste = radio.write(&text, sizeof(&text));
  Serial.println(teste);
  radio.startListening();

  delay(1000);
}
  * 
  */
