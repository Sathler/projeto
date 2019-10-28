/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN Pode ser qualquer porta digital
const byte endereco[][10] = {"1arduino", "2arduino"};
void setup() {
  Serial.begin(9600);


  //define o pipe de escrita
  radio.openWritingPipe(endereco[0]);
  //define o pipe de leitura
  radio.openReadingPipe(1, endereco[1]);

  radio.startListening();
}
void loop() {
  if (radio.available()) {
    const char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}