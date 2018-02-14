/*
Potentiometer testing module
The program is designed to increase the resistance of the potentiometer every 10 seconds
allowing enough time to be measured confirming the proper functioning
*/

#include <Wire.h>
#include <math.h> 

const int addr = 0x50;
const int level = 0x100;

void setup(){
  int aux_level=level;
  int resistance;
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(addr); // transmit to device #addr (0x2c)
  // device address is specified in datasheet
  Wire.write(byte(0xA1)); //Mudar aqui conforme o addr 10100(A1)(A0)0
  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(level);             // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
} 
void loop(){
  float resistance;
  resistance = (10000/256)*level;
  Serial.print("Resistance: ");
  Serial.print(resistance);
  Serial.print(" V");
  delay(10000);
  if(aux_level>=0x0A) {
    aux_level=aux_level-10;
    potentiometer(aux_level);
  }
  else{
    Serial.println("Can't go any lower !!");
    while(1); //Lock the arduino
  }
}

void potentiometer(int level) 
{
  Wire.beginTransmission(addr); // transmit to device #addr (0x2c)
  // device address is specified in datasheet
  Wire.write(byte(0xA1)); //Mudar aqui conforme o addr 10100(A1)(A0)0
  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(level);             // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
}




