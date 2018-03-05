/*Adc testing module 10/01
Test the adc by measuring the voltage and current provided by the dummy cell and one 
Current drain is expected to be around 14,38 due to the potentiometer's limitation
*/

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 

/*Fill in the desired adc addr!*/
Adafruit_ADS1115 ads1115_1(0x48);
const int rele_bat = 22;
const int addr = 0x50;
const int level=0xC0;
const int termistor=A0;
unsigned long tempo;

void setup(){
  ads1115_1.begin();
  Serial.begin(9600);
  Wire.begin();
  pinMode(rele_bat,OUTPUT);
  digitalWrite(rele_bat,HIGH);
  potentiometer(level);
} 
void loop(){
  float volts,curr,temp;
  String data;

  potentiometer(level);
  curr = current();
  volts = voltage();
  //temp=temperatures(termistor);
  temp=0;
  tempo=millis();
  data = String(volts)+"V \t"+String(curr)+"A \t"+String(temp)+"C \t"+String(tempo)+"ms";
  Serial.println(data);
  digitalWrite(rele_bat,HIGH);
  delay(1000);
}

float voltage (){
    return ads1115_1.readADC_Differential_2_3()*0.000188;
}

float current (){
    return (ads1115_1.readADC_Differential_0_1()*0.000188)/0.0003;
}
void potentiometer(int lvl) 
{
  Wire.beginTransmission(addr); // transmit to device #addr (0x2c)
  // device address is specified in datasheet

  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(lvl);             // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
}

float temperatures(int THERMISTORPIN){          
  double reading;
  reading = analogRead(THERMISTORPIN); 
  float voltage = reading * (5.0 / 1023.0);
  float current = voltage/10000;
  float Rthermistor = (5-(1+125/10000)*voltage)/current;
  float temperature = 3435/(log(Rthermistor/(10000*exp(-3435/298.15) ) )) - 273.15;
  return  temperature;
}

