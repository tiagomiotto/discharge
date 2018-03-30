

/*Adc testing module 30/03
Test the adc by measuring the voltage and current provided by the dummy cell and one 
Current drain is expected to be around 14,38 due to the potentiometer's limitation
*/

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 
#include <INA226.h>
#include <tiPortExpander.h>

/*Fill in the desired adc addr!*/
#define INA_ADDR 0x20
#define POT_LEVEL 0xFF
#define POT_ADDR 0x50
#define R_B_ADDR 22
#define ADC_ADDR 0x48
#define GPIO_ADDR 0x00

Adafruit_ADS1115 ads1115_1(ADC_ADDR);
INA226_Class INA226; 
TiPortExpander gpio(GPIO_ADDR);
const int rele_bat = R_B_ADDR;
const int addr = POT_ADDR;
const int level= POT_LEVEL;
const int termistor=A0;
unsigned long tempo;

void potentiometer(int);
float temperatures(int);
float current ();
float voltage ();

void setup(){
  ads1115_1.begin();
  Serial.begin(9600);
  Wire.begin();
  gpio.configurePins(0,0,0,0,0,0,0,0);
  INA226.begin(141.2142,300,INA_ADDR);
  INA226.setAveraging(4);                                                     // Average each reading n-times     //
  INA226.setBusConversion(7);                                                 // Maximum conversion time 8.244ms  //
  INA226.setShuntConversion(7);                                               // Maximum conversion time 8.244ms  //
  INA226.setMode(INA_MODE_CONTINUOUS_BOTH); 

  potentiometer(level);
} 
void loop(){
  float volts,curr,temp;
  String data;
  digitalWrite(rele_bat,LOW);
  potentiometer(level);
  curr = current();
  volts = voltage();
  temp=temperatures(termistor);
  tempo=millis();
  data = String(volts)+"V \t"+String(curr)+"mA \t"+String(temp)+"C \t"+String(tempo)+"ms";
  Serial.println(data);
  gpio.writePin(1,true);
  delay(1000);
}

float voltage (){
    return ads1115_1.readADC_Differential_2_3()*0.000188;
}

float current (){
    return (float)INA226.getBusMicroAmps()/1000.0,3;
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
