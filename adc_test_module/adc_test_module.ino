/*Adc testing module 10/01
Test the adc by measuring a dummy voltage provided by the PSU
*/

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 

/*Fill in the desired adc addr!*/
Adafruit_ADS1115 ads1115(0x49); //Mudar isso no codigo principal

double volts;
void setup(){
  ads1115.begin();
  Serial.begin(9600);
  
} 
void loop(){
  
  
  Serial.print("Voltage: ");
  Serial.print(volts);
  Serial.println(" V");
  volts = ads1115.readADC_Differential_2_3()*0.000188;
  delay(100);
}

