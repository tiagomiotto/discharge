/*
Potentiometer testing module
The program is designed to increase the resistance of the potentiometer every 10 seconds
allowing enough time to be measured confirming the proper functioning
*/

#include <Wire.h>
#include <math.h> 

const int addr = 0x50;  // device address is specified in datasheet IMPORTANT      
const int level=0xFF;
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
  float diferential;
  String data; 
 
 
  potentiometer(aux_level);
  delay(250);
  diferential = read_diferential();
  data = String(aux_level) + "\t"+ String(diferential) +"\t";
  if(aux_level<1){
    aux_level=0xFF;
  }
  else{
    aux_level=aux_level-1; 
  }
 Serial.println(data);
}


//Function to communicate with POT and define lvl
void potentiometer(int lvl) 
{
  Wire.beginTransmission(addr); // transmit to device #addr (0x2c)
  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(lvl);             // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
}


//Arduino pins, read outuput voltage from POT
float read_diferential(){
  float positive,negative;
  positive=analogRead(A2)*(5.0/1024.0);
  negative=analogRead(A3)*(5.0/1024.0);
  return positive-negative;
}