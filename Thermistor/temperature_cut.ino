/*Adc testing module 10/01
Test the adc by measuring a dummy voltage provided by the PSU


Gain:
    #define ADS1015_REG_CONFIG_PGA_MASK     (0x0E00)
    #define ADS1015_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
    #define ADS1015_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
    #define ADS1015_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
    #define ADS1015_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
    #define ADS1015_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
    #define ADS1015_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

*/

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 

/*Fill in the desired adc addr!*/
Adafruit_ADS1115 ads1115(0x48); //ADRRS = GND
byte rele_bat = 30;
const int term = A0;
double volts;
float temperature;
float aux=35;
void setup(){
  Wire.begin();
  ads1115.begin();
  ads1115.setGain(ADS1015_REG_CONFIG_PGA_6_144V);    // GAIN_TWOTHIRDS
  Serial.begin(9600);
  pinMode(rele_bat,OUTPUT);
  pinMode(term,INPUT);
  digitalWrite(rele_bat,HIGH);
} 
void loop(){
  float volts;
  float curr;

  volts = voltage();
  temperature=temperatures(term);
  Serial.print("Voltage: ");
  Serial.print(volts);
  Serial.println(" V");
  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  if(temperature>=aux){
    digitalWrite(rele_bat,LOW);
  }
  delay(1000);
}

float voltage (){
    return ads1115.readADC_Differential_2_3()*0.000188;
    
}

/*READ THERMISTOR VALUES*/

float temperatures(int THERMISTORPIN){          
  double reading;
  reading = analogRead(THERMISTORPIN); 
  float voltage = reading * (5.0 / 1023.0);
  float current = voltage/10000;
  float Rthermistor = (5-(1+125/10000)*voltage)/current;
  float temperature = 3435/(log(Rthermistor/(10000*exp(-3435/298.15) ) )) - 273.15;
  return  temperature;
}

float current (){
    return (ads1115.readADC_Differential_0_1()*0.000188)/0.0003;
}