#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <string.h>

#define LEDLINE 16
#define SS_PIN 20
#define RST_PIN 24
#define CE_PIN 30
#define CSN_PIN 31

MFRC522 rfid(SS_PIN, RST_PIN);
RF24 radio(CE_PIN, CSN_PIN);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //PORTAS DO ARDUINO QUE SÃO UTILIZADAS PELO SHIELD KEYPAD DISPLAY //lcd

const byte writing_address[6] = "00005";
const byte reading_address[6] = "00004";

String strID;
String mensagem("Bom dia"); //MENSAGEM INICIAL A SER EXIBIDA NO LCD

enum tipoEstados
{
	Consulta,
	Remocao
};
tipoEstados ESTADO = Consulta;

const int ledScrollSpeed[2] = {225, 450};	 //DEFINE A VELOCIDADE DE ROLAGEM DAS DUAS LINHAS DO LED
const boolean ledScrollDir[2] = {true, true}; //DEFINE A DIREÇÃO DA ROLAGEM DAS DUAS LINHAS DO LED
											  //TRUE SIGINIFICA DA DIREITA PRA ESQUERDA

char charAt(char *text, int pos)
{
	if (pos < LEDLINE)
		return ' ';
	else if (pos >= LEDLINE && pos < LEDLINE + strlen(text))
		return text[pos - LEDLINE];
	else
		return ' ';
}

//FUNÇÃO RESPONSAVEL PELA ROLAGEM DO LED
void task_text(char *text, byte line)
{
	char currenttext[LEDLINE + 1];
	static unsigned long nextscroll[2];
	static int positionCounter[2];
	int i;
	if (millis() > nextscroll[line])
	{
		nextscroll[line] = millis() + ledScrollSpeed[line];

		for (i = 0; i < LEDLINE; i++)
			currenttext[i] = charAt(text, positionCounter[line] + i);
		currenttext[LEDLINE] = 0;

		lcd.setCursor(0, line);
		lcd.print(currenttext);
		if (ledScrollDir[line])
		{
			positionCounter[line]++;
			if (positionCounter[line] == strlen(text) + LEDLINE)
				positionCounter[line] = 0;
		}
		else
		{
			positionCounter[line]--;
			if (positionCounter[line] < 0)
				positionCounter[line] = strlen(text) + LEDLINE - 1;
		}
	}
}

void setup()
{
	SPI.begin();
	rfid.PCD_Init();
	//LCD
	lcd.begin(16, 2);
	lcd.setCursor(0, 1);
	lcd.print("Modo: Consulta"); //ESTADO INICIAL DO SISTEMA
	//RF24
	radio.begin();
	radio.openWritingPipe(writing_address);
	radio.openReadingPipe(0, reading_address);
	radio.setPALevel(RF24_PA_HIGH); //DEFINE O NÍVEL DO AMPLIFICADOR DE POTÊNCIA
									//radio.stopListening();
}

void loop()
{
	if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //ENQUANTO NÃO HOUVER UM NOVO CARTÃO O SISTEMA FICARA PRESO NESTE BLOCO
	{
		task_text(mensagem.c_str(), 0); //ROLAGEM DA MESNAGEM DO LCD

		switch (ESTADO) //SELEÇÃO DO MODO DE OPERAÇÃO DO SISTEMA
		{
		case Consulta:

			if (analogRead(0) < 400 && analogRead(0) > 300)
			{
				lcd.setCursor(0, 1);
				lcd.print("Modo: Remocao ");
				ESTADO = Remocao;
			}
			break;
			//estado remocao
		case Remocao:
			if (analogRead(0) < 200 && analogRead(0) > 100)
			{
				lcd.setCursor(0, 1);
				lcd.print("Modo: Consulta");
				ESTADO = Consulta;
			}
			break;
		}
		return;
	}
	//SELEÇÃO DA INSTRUÇÃO A SER ENVIADA
	if (ESTADO == Consulta)
		strID = "S";
	else
		strID = "R";

	//GERAÇÃO DA TAG
	for (byte i = 0; i < 4; i++)
	{
		strID +=
			(rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
			String(rfid.uid.uidByte[i], HEX);
	}
	strID.toUpperCase();
	rfid.PICC_HaltA();
	rfid.PCD_StopCrypto1();

	//ENVIO DA MENSAGEM PARA O MICROCONTROLADOR DA APLICAÇÃO
	radio.stopListening();
	char *text = strID.c_str();
	radio.write(text, strID.length());
	radio.startListening();

	while (!radio.available()) //SISTEMA FICA EM ESTADO DE ESPERA ENQUANTO NÃO RECEBE RESPOSTA
	{
	}

	//LEITURA DA RESPOSTA RECEBIDA
	char textConfirma[32] = "";
	radio.read(&textConfirma, sizeof(textConfirma));
	mensagem = String(textConfirma);

	//INTERPRETAÇÃO DA MENSAGEM RECEBIDA E GERAÇÃO DA STRING DE RESPOSTA A SER EXIBIDA NO LED
	switch (ESTADO)
	{
	case Consulta:
		if (mensagem[0] == '0')
			mensagem = String("Produto nao cadastrado");
		else if (mensagem[0] == '1')
		{
			int pos = mensagem.indexOf(":");
			String nome = mensagem.substring(pos + 1);
			String valor = mensagem.substring(1, pos);
			mensagem = String("Nome: " + nome + " Valor: " + valor);
		}
	case Remocao:
		if (mensagem[0] == '0')
			mensagem = String("Produto nao cadastrado");
		else if (mensagem[0] == '1')
			mensagem = String("Produto removido com sucesso");
	}

	radio.stopListening();
}