//https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/

/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN Pode ser qualquer porta digital
//definindo os pipes
const byte endereco[][10] = {"2arduino", "1arduino"};

void setup() {
  //define o pipe de escrita
  radio.openWritingPipe(endereco[0]);
  //define o pipe de leitura
  radio.openReadingPipe(1, endereco[1]);

  radio.startListening();
}
void loop() {
  //precisa parar de ouvir para poder enviar
  radio.stopListening();
  const char text[32] = "Hello World";
  radio.write(&text, sizeof(text));
  radio.startListening();

  delay(1000);
}