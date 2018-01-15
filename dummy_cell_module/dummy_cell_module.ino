/*Adc testing module 10/01
Test the adc by measuring the voltage and current provided by the dummy cell and one 
Current drain is expected to be around 14,38 due to the potentiometer's limitation
*/

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 

/*Fill in the desired adc addr!*/
Adafruit_ADS1115 ads1115_1(0x48);
const int rele_bat = 23;

void setup(){
    ads1115_1.begin();
  Serial.begin(9600);
  pinMode(rele_bat,OUTPUT);
  digitalWrite(rele_bat,HIGH);
  Wire.beginTransmission(0x53); // transmit to device #addr (0x2c)
  // device address is specified in datasheet
  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(0x00);             // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
} 
void loop(){
  float volts;
  float curr;
  curr = current();
  volts = voltage();
  Serial.print("Voltage: ");
  Serial.print(volts);
  Serial.println(" V");
  Serial.print("Current: ");
  Serial.print(curr);
  Serial.println(" A");
  delay(1000);
}

float voltage (){
    return ads1115_1.readADC_Differential_2_3()*0.000188;
}

float current (){
    return (ads1115_1.readADC_Differential_0_1()*0.000188)/0.0003;
 }

