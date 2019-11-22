#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA
#include <LiquidCrystal.h> //INCLUSÃO DE BIBLIOTECA LCD
#include <nRF24L01.h> //INCLUSÃO DE BIBLIOTECA
#include <RF24.h> //INCLUSÃO DE BIBLIOTECA
#include <string.h>

#define SS_PIN 20 //PINO SDA
#define RST_PIN 24 //PINO DE RESET

RF24 radio(30, 31); //CRIA UMA INSTÂNCIA UTILIZANDO OS PINOS (CE, CSN)
const byte address[6] = "00002"; //CRIA UM ENDEREÇO PARA ENVIO DOS
const byte address2[6] = "00001";

String modo, strID;
 
MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

//LCD Constructor
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //PORTAS DO ARDUINO QUE SÃO UTILIZADAS PELO SHIELD KEYPAD DISPLAY LCD

enum tipoEstados {Consulta, Remocao};
tipoEstados ESTADO = Consulta;

	
void setup() {
	Serial.begin(9600); //INICIALIZA A SERIAL
//Conexao Serial
	SPI.begin(); //INICIALIZA O BARRAMENTO SPI
	rfid.PCD_Init(); //INICIALIZA MFRC522

//LCD
	lcd.begin(16,2); //SETA A QUANTIDADE DE COLUNAS(16) E O NÚMERO DE LINHAS(2) DO DISPLAY. EM SUMA: UMA MATRIZ DE 16 COLUNAS E 2 LINHAS
	lcd.setCursor(0,0); //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1) 
	lcd.print("Modo: Consulta");  //ESCREVE NO LCD O TEXTO
//RF24
	radio.begin(); //INICIALIZA A COMUNICAÇÃO SEM FIO
	radio.openWritingPipe(address); //DEFINE O ENDEREÇO PARA ENVIO DE DADOS AO RECEPTOR
	radio.openReadingPipe(0, address2);
	radio.setPALevel(RF24_PA_HIGH); //DEFINE O NÍVEL DO AMPLIFICADOR DE POTÊNCIA
	//radio.stopListening(); //DEFINE O MÓDULO COMO TRANSMISSOR (NÃO RECEBE DADOS)
}


void loop() {
	if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()){ //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ

		switch (ESTADO)
		{
//estado consulta
		case Consulta:
			lcd.setCursor(0,1);
			lcd.print(strID);
			//Serial.print("Envia: ");
			//Serial.println(strID);


			if (analogRead(0) < 400 && analogRead(0) > 300){ //baixo
				lcd.setCursor(0,0); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)
				lcd.print("Modo: Remocao "); //ESCREVE NO LCD O TEXTO
				ESTADO = Remocao;
			}
			break;
//estado remocao
		case Remocao:
			lcd.setCursor(0,1);
			lcd.print(strID);
			//Serial.print("Remove: ");
			//Serial.println(strID);

			if (analogRead(0) < 200 && analogRead(0) > 100){ // cima
				lcd.setCursor(0,0); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)
				lcd.print("Modo: Consulta");  //ESCREVE NO LCD O TEXTO
				ESTADO = Consulta;
			}
			break;
		}
		return;
	}
//leitura
	//strID.clear();
	if(ESTADO == Consulta) modo = "S";
	else modo = "R";
	strID = modo;
	for (byte i = 0; i < 4; i++) {
		strID +=
		(rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
		String(rfid.uid.uidByte[i], HEX);
    }
    strID.toUpperCase();
    /***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/

	//Serial.print("Leu: ");
	//Serial.println(strID);
 
    rfid.PICC_HaltA(); //PARADA DA LEITURA DO CARTÃO
    rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD
	radio.stopListening();
	char *text = strID.c_str(); //VARIÁVEL RECEBE A MENSAGEM A SER TRANSMITIDA
	Serial.print(text);
	radio.write(text, strID.length()); //ENVIA AO RECEPTOR A MENSAGEM
	radio.startListening();

	while (!radio.available()){Serial.println(radio.available());}
	char textConfirma[32] = "";
	radio.read(&textConfirma, sizeof(textConfirma));
	String mensagem = String(textConfirma);
	lcd.setCursor(0,1);
	lcd.print("                ");
	lcd.print(mensagem);
	Serial.println(mensagem);
	radio.stopListening();
	
}
