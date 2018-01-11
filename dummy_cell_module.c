/*Adc testing module 10/01
Test the adc by measuring the voltage and current provided by the dummy cell and one 
Current drain is expected to be around 14,38 due to the potentiometer's limitation
*/

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 

/*Fill in the desired adc addr!*/
Adafruit_ADS1015 ads1115_1(0x48); 
const int addr = 0x50;
const int level = 0x100;

void setup(){
  	ads1115_1.begin();
	Serial.begin(9600);
	Wire.beginTransmission(addr); // transmit to device #addr (0x2c)
	// device address is specified in datasheet
	Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
	Wire.write(level);             // sends potentiometer value byte 
	Wire.endTransmission();     // stop transmitting
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

float current (int n){
    return (ads1115_1.readADC_Differential_2_3()/0.0000762939453125)/0.0003;
 }
