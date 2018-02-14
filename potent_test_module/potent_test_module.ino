/*
Potentiometer testing module
The program is designed to increase the resistance of the potentiometer every 10 seconds
allowing enough time to be measured confirming the proper functioning
*/

#include <Wire.h>
#include <math.h> 

const int addr = 0xA0;
const int level = 0xFF;
int aux_level=level;
const int rele_bat=22;
int X0;
void setup(){
  
  Serial.begin(9600);
  Wire.begin();
  pinMode(rele_bat,OUTPUT);
  digitalWrite(rele_bat,HIGH);
}
void loop(){
  float resistance;
  
  delay(1000);
  if(aux_level>=10) {
    aux_level=aux_level-10;
    potentiometer(aux_level);
  }
  if(aux_level<10){
    aux_level=0xFF;
    potentiometer(aux_level);
  }
 resistance = (10000/256)*readfrom();
  Serial.print("Resistance: ");
  Serial.print(resistance);
  Serial.println(" V");
}

void potentiometer(int lvl) 
{
  Wire.beginTransmission(addr); // transmit to device #addr (0x2c)
  // device address is specified in datasheet

  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(lvl);             // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
}
int readfrom(){
  Wire.beginTransmission(addr); // Begin transmission to the Sensor 
  //Ask the particular registers for data
  Wire.write(0x00);
  
  Wire.endTransmission(); // Ends the transmission and transmits the data from the two registers
  
  Wire.requestFrom(addr,2); // Request the transmitted two bytes from the two registers
  
  if(Wire.available()<=1) {  // 
    X0 = Wire.read(); // Reads the data from the register
    
  }
  
 return X0;

}




