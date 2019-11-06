#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
bool radioNumber = 0;
byte addresses[][6] = {"1Hard","2Izi"}; //o endereco pelo qual os dois se comunicam
void setup() {
  Serial.begin(9600);
  radio.begin();
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }
  else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.startListening();
  }
}
void loop() {
  if (radio.available()) {//acho que tem a ver com esse radio available
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}
/*
 * #include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN Pode ser qualquer porta digital
//const byte endereco[][6] = {0xF0F0F0F0AA, 0xF0F0F0F066};
void setup() {
  Serial.begin(9600);
  radio.begin();
  //define o pipe de escrita
  radio.openWritingPipe(0xF0F0F0F0AA);
  //define o pipe de leitura
  radio.openReadingPipe(1, 0xF0F0F0F066);

  radio.startListening();
}
void loop() {
 // Serial.println(radio.available());
  if (radio.available()) {
    char text[20] = "";
    radio.read(&text, sizeof(&text));
    Serial.println(text);
  }
}

 */
