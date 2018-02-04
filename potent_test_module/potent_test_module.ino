/*
Potentiometer testing module
The program is designed to increase the resistance of the potentiometer every 10 seconds
allowing enough time to be measured confirming the proper functioning
Assuming A1 and A0 at 5v -> In different cases change both the addr and the identification byte
in the following form  
addr: 10100(A1)(A0)
identification: 10100(A1)(A0)0
*/

#include <Wire.h>
#include <math.h> 

const int addr = 0x53; //Addr
byte level = 0x100;
const byte ident = 0xA6
int aux_level=level;
int resistance;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(addr); // transmit to device #addr (0x2c)
  // device address is specified in datasheet
  Wire.write(ident);   //identification byte
  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(level);             // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
} 
void loop(){
  
  resistance = (10000/256)*aux_level;
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
  Wire.beginTransmission(addr); 
  // device address is specified in datasheet
  Wire.write(ident);   //identification byte
  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(level);            // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
}




