//https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
#include <SPI.h>
#include <RF24.h>
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA
#include <string.h>
#include <LiquidCrystal.h> //INCLUSÃO DE BIBLIOTECA

#define SS_PIN 20  //PINO SDA
#define RST_PIN 32 //PINO DE RESET

//RFID
MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

//RF24
bool radioNumber = 1;
RF24 radio(30, 31); // CE, CSN
byte addresses[][6] = {"1Hard", "2Izi"};

//LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //PORTAS DO ARDUINO QUE SÃO UTILIZADAS PELO SHIELD KEYPAD DISPLAY LCD

void setup()
{
  //RF24
  radio.begin();
  if (radioNumber)
  {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
    radio.stopListening();
  }
  else
  {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }
  //RFID
  SPI.begin();                  //INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init();              //INICIALIZA MFRC522
                                //lcd
  lcd.begin(16, 2);             //SETA A QUANTIDADE DE COLUNAS(16) E O NÚMERO DE LINHAS(2) DO DISPLAY. EM SUMA: UMA MATRIZ DE 16 COLUNAS E 2 LINHAS
  lcd.setCursor(0, 0);          //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1)
  lcd.print("APERTOU A TECLA"); //ESCREVE A FRASE "APERTOU A TECLA" NA PRIMEIRA LINHA DO DISPLAY LCD
}
void loop()
{
  //RFID
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
    return;                                                         //RETORNA PARA LER NOVAMENTE

  /***INICIO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  String strID = "";
  for (byte i = 0; i < 4; i++)
  {
    strID +=
        (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
        String(rfid.uid.uidByte[i], HEX) +
        (i != 3 ? ":" : "");
  }
  strID.toUpperCase();
  /***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/

  //Serial.print("Identificador (UID) da tag: "); //IMPRIME O TEXTO NA SERIAL
  Serial.println(strID); //IMPRIME NA SERIAL O UID DA TAG RFID

  rfid.PICC_HaltA();      //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD

  //lcd
  lcd.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)
  if ((analogRead(0)) < 80)
  {                               //SE A LEITURA DO PINO FOR MENOR QUE 80, FAZ
    lcd.print("    DIREITA    "); //ESCREVE NO LCD O TEXTO
  }
  else if ((analogRead(0)) < 200)
  {                               //SE A LEITURA DO PINO FOR MENOR QUE 200, FAZ
    lcd.print("      CIMA     "); //ESCREVE NO LCD O TEXTO
  }
  else if ((analogRead(0)) < 400)
  {                               //SE A LEITURA DO PINO FOR MENOR QUE 400, FAZ
    lcd.print("     BAIXO     "); //ESCREVE NO LCD O TEXTO
  }
  else if ((analogRead(0)) < 600)
  {                               //SE A LEITURA DO PINO FOR MENOR QUE 600, FAZ
    lcd.print("    ESQUERDA   "); //ESCREVE NO LCD O TEXTO
  }
  else if ((analogRead(0)) < 800)
  {                               //SE A LEITURA DO PINO FOR MENOR QUE 800, FAZ
    lcd.print("     SELECT    "); //ESCREVE NO LCD O TEXTO
  }

  //RF24
  char *text = strID.c_str();
  radio.write(text, strID.length());
  delay(1000);
}