/*Adc testing module 10/01
Test the adc by measuring a dummy voltage provided by the PSU
*/

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 

/*Fill in the desired adc addr!*/
Adafruit_ADS1015 ads1115_1(0x48); 


void setup(){
    ads1115_1.begin();
  Serial.begin(9600);
} 
void loop(){
  float volts;
  volts = voltage();
  Serial.print("Voltage: ");
  Serial.print(volts);
  Serial.print(" V");
  delay(1000);
}

float voltage (int n){
    return ads1115_1.readADC_Differential_0_1()/0.0000762939453125;
}
