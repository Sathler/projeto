#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define CE_PIN 5
#define CSN_PIN 6

MFRC522 rfid(SS_PIN, RST_PIN);
RF24 radio(CE_PIN, CSN_PIN);

const byte reading_address[6] = "00005";
const byte writing_address[6] = "00004";

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  radio.begin();
  radio.openReadingPipe(0, reading_address);
  radio.openWritingPipe(writing_address);
  radio.setPALevel(RF24_PA_HIGH); //DEFINE O NÍVEL DO AMPLIFICADOR DE POTÊNCIA
  radio.startListening();
}

void loop()
{
  if (radio.available()) //RECEBENDO UMA MENSAGEM VIA RADIO 
  {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    String tag = String(text);
    if (!tag.equals(""))
    {
      Serial.print(tag); //ENVIA A MENSAGEM PARA A APLICAÇÃO VIA PORTA SERIAL
      while (Serial.available() == 0) //ENQUANTO A APLICAÇÃO NÃO RESPONDER O PROGRAMA
      {                               //FICARA EM ESTADO DE ESPERA NESTE BLOCO
      }
      String ret = "";
      char c;
      //BLOCO QUE REALIZA A LEITURA DA PORTA SERIAL
      while (Serial.available() > 0) 
      {
        c = Serial.read();
        if (c != '\n')
        {
          ret += c;
        }
        delay(10);
      }

      //ENVIANDO O RETORNO DA APLICAÇÃO PARA O ARDUINO REMOTO
      radio.stopListening();
      char *retCStr = ret.c_str();
      radio.write(retCStr, ret.length());
      radio.startListening();
    }
  }

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //RETORNA PARA O INICIO DO LOOP ENQUANTO NÃO LER UM CARTAO
    return;                                                         
  
  String strID = "I";
  for (byte i = 0; i < 4; i++)
  {
    strID +=
        (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
        String(rfid.uid.uidByte[i], HEX);
  }
  strID.toUpperCase();
  
  rfid.PICC_HaltA();      
  rfid.PCD_StopCrypto1(); 
  Serial.print(strID); //ENVIANDO TAG PARA CADASTRO NA APLICACÃO   
}

//TODO COMANDO ENVIADO POR ESTA UNIDADE PARA A APLCIAÇÃO ESTARA NO FORMATO
// C + TAG, ONDE C É UM CARACTERE QUE REPRESENTA A INSTRUÇÃO SQL A SER REALIZADA
//PELA APLICAÇÃO, 'I' PARA CADASTRO, 'S' PARA CONSULTA e 'R' PARA REMOÇÃO
//E A TAG SENDO FORMADA POR OITO CARACTERES ALFA-NUMÉRICOS, TODOS EM UPPER CASE
