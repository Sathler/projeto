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