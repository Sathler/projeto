#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA
#include <LiquidCrystal.h> //INCLUSÃO DE BIBLIOTECA //lcd
#include <nRF24L01.h> //INCLUSÃO DE BIBLIOTECA
#include <RF24.h> //INCLUSÃO DE BIBLIOTECA
#include <string.h>
#define LEDLINE 16 

#define SS_PIN 20 //PINO SDA
#define RST_PIN 24 //PINO DE RESET

RF24 radio(30, 31); //CRIA UMA INSTÂNCIA UTILIZANDO OS PINOS (CE, CSN)
const byte address[6] = "00005"; //CRIA UM ENDEREÇO PARA ENVIO DOS
const byte address2[6] = "00004";

String modo, strID;
	String mensagem("Bom dia");

MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

////lcd Constructor
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //PORTAS DO ARDUINO QUE SÃO UTILIZADAS PELO SHIELD KEYPAD DISPLAY //lcd

enum tipoEstados {Consulta, Remocao};
tipoEstados ESTADO = Consulta;

// define scrolling speed for two LED lines
const int ledScrollSpeed[2]={225,450};
// define scrolling direction for two LED lines (true means right-to-left scrolling)
const boolean ledScrollDir[2]={true,true};


char charAt(char *text, int pos)
// scrolling-logic coded here
{
	if (pos<LEDLINE) return ' '; // scroll in
	else if (pos>=LEDLINE && pos<LEDLINE+strlen(text))
		return text[pos-LEDLINE]; // scroll text
	else return ' ';  // scroll out

}


void task_text(char *text, byte line)
// scroll the LED lines
{
	char currenttext[LEDLINE+1];
	static unsigned long nextscroll[2];
	static int positionCounter[2];
	int i;
	if (millis()>nextscroll[line])
	{
		nextscroll[line]=millis()+ledScrollSpeed[line];

		for (i=0;i<LEDLINE;i++)
			currenttext[i]=charAt(text,positionCounter[line]+i);
		currenttext[LEDLINE]=0;   

		lcd.setCursor(0,line);
		lcd.print(currenttext);
		if (ledScrollDir[line])
		{
			positionCounter[line]++;
			if (positionCounter[line]==strlen(text)+LEDLINE) positionCounter[line]=0;
		} 
		else 
		{
			positionCounter[line]--;
			if (positionCounter[line]<0) positionCounter[line]=strlen(text)+LEDLINE-1;
		} 
	}
}

void setup() {
	Serial.begin(9600); //INICIALIZA A SERIAL
//Conexao Serial
	SPI.begin(); //INICIALIZA O BARRAMENTO SPI
	rfid.PCD_Init(); //INICIALIZA MFRC522

////lcd
	lcd.begin(16,2); //SETA A QUANTIDADE DE COLUNAS(16) E O NÚMERO DE LINHAS(2) DO DISPLAY. EM SUMA: UMA MATRIZ DE 16 COLUNAS E 2 LINHAS
	lcd.setCursor(0,1); //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1) 
	lcd.print("Modo: Consulta");  //ESCREVE NO //lcd O TEXTO
//RF24
	radio.begin(); //INICIALIZA A COMUNICAÇÃO SEM FIO
	radio.openWritingPipe(address); //DEFINE O ENDEREÇO PARA ENVIO DE DADOS AO RECEPTOR
	radio.openReadingPipe(0, address2);
	radio.setPALevel(RF24_PA_HIGH); //DEFINE O NÍVEL DO AMPLIFICADOR DE POTÊNCIA
	//radio.stopListening(); //DEFINE O MÓDULO COMO TRANSMISSOR (NÃO RECEBE DADOS)
}


void loop() {
	if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()){ //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
		task_text(mensagem.c_str(), 0);

		switch (ESTADO)
		{
//estado consulta
		case Consulta:
			//lcd.setCursor(0,1);
			//lcd.print(strID);
			//Serial.print("Envia: ");
			//Serial.println(strID);


			if (analogRead(0) < 400 && analogRead(0) > 300){ //baixo
				lcd.setCursor(0,1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)
				lcd.print("Modo: Remocao "); //ESCREVE NO //lcd O TEXTO
				ESTADO = Remocao;
			}
			break;
//estado remocao
		case Remocao:
			//lcd.setCursor(0,1);
			//lcd.print(strID);
			//Serial.print("Remove: ");
			//Serial.println(strID);

			if (analogRead(0) < 200 && analogRead(0) > 100){ // cima
				lcd.setCursor(0,1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)
				lcd.print("Modo: Consulta");  //ESCREVE NO //lcd O TEXTO
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
	char *texto2 = strID.c_str(); //VARIÁVEL RECEBE A MENSAGEM A SER TRANSMITIDA
	Serial.print(texto2);
	radio.write(texto2, strID.length()); //ENVIA AO RECEPTOR A MENSAGEM
	radio.startListening();

	while (!radio.available()){Serial.println(radio.available());}
	char textConfirma[32] = "";
	radio.read(&textConfirma, sizeof(textConfirma));
	mensagem = String(textConfirma);

	switch (ESTADO)
	{
		case Consulta:
			if(mensagem[0]=='0')
				mensagem = String("Produto nao cadastrado");
			else if (mensagem[0]=='1'){
				int pos = mensagem.indexOf(":");
				String nome = mensagem.substring(pos+1);
				String valor = mensagem.substring(1, pos);
				mensagem = String("Nome: " + nome + " Valor: " + valor);
			}
		case Remocao:
			if (mensagem[0]=='0')
				mensagem = String("Produto nao cadastrado");
			else if (mensagem[0]=='1')
				mensagem = String("Produto removido com sucesso");
	}

	Serial.println(mensagem);
	radio.stopListening();
	
}