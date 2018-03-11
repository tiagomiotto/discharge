/*
This is a draft in progress of what will be the final main
*/

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

/*Fill in the desired adc addr!*/
Adafruit_ADS1115 ads1115_1(0x48);
const int rele_bat = 22;
const int rele_charger = 22;
const int addr = 0x50;
const int level=0xFF;
const int termistor=A0;


void setup(){
  ads1115_1.begin();
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  DateTime now=RTC.now();
  setup_SD();
  potentiometer(level);
} 



void loop(){
  float volts,curr,temp;
  String data;
  curr = current();
  volts = voltage();
  temp=temperatures(termistor);
  tempo=millis();

}





//Get voltage from battery
float voltage (){
    return ads1115_1.readADC_Differential_2_3()*0.000188;
}

//Get Current from shunt
float current (){
    return (ads1115_1.readADC_Differential_0_1()*0.000188)/0.0003;
}

//Write new step in potentiometer
void potentiometer(int lvl) 
{
  Wire.beginTransmission(addr); // transmit to device #addr (0x2c)
  // device address is specified in datasheet

  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(lvl);             // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
}

//Get temperature
float temperatures(int THERMISTORPIN){          
  double reading;
  reading = analogRead(THERMISTORPIN); 
  float voltage = reading * (5.0 / 1023.0);
  float current = voltage/10000;
  float Rthermistor = (5-(1+125/10000)*voltage)/current;
  float temperature = 3435/(log(Rthermistor/(10000*exp(-3435/298.15) ) )) - 273.15;
  return  temperature;
}

//Write data in SD
void write_SD(float Voltage,float Amps,float Temp,int Time){
	data = String(Voltage) + "\t" + String(Amps) + "\t" + String(Temp) + "\t" + String(Time) + "\n";
	dis_log.printIn(data);
}

//If Voltage, Current and Temperature are too hugh or low shutdown the bench
void check_safety(float Voltage, float Amps, float Temp){

	if(Voltage > 4.25 || Voltage < 2.8)
		shutdown();

}

//
void shutdown(int Time){

	digitalWrite(rele_bat,LOW);
	digitalWrite(rele_charger,LOW);
	write_SD(0,0,0,Time);
	while(1)
}



//Create new file and open to write TO DO->checkar erros
void setup_SD(){
	if (!SD.begin(10,11,12,13)) { //É assim pro mega
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) ;
  }
  if(!RTC.isrunning()){
    Serial.println("RTC error\n");
  }

    file_name =  "discharge_battery_bench_"; 
    file_name += String(i+1) ;
    file_name += "_" ;
    file_name += String(now.day());
    file_name += "_" ;
    file_name += String(now.hour());
    file_name +=".txt"; 

    dis_log=SD.open(file_name.c_str(),FILE_WRITE);
    
    
    if(dis_log[i]<=0 || !chg_log[i]<=0){
      Serial.println("Error opening file");
      while(1)
    }
}