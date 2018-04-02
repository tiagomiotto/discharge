/*Adc testing module 30/03
Test the adc by measuring the voltage and current provided by the dummy cell and one 
Current drain is expected to be around 14,38 due to the potentiometer's limitation
*/
#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 
#include <INA226.h>
#include <TCA9534.h>


/*Fill in the desired adc addr!*/
#define INA_ADDR 0x44
#define POT_LEVEL 0x00
#define POT_ADDR 0x53
#define ADC_ADDR 0x48
#define GPIO_ADDR 0x3B
#define GPIO_PIN 7
#define GPIO_PIN_2 6

Adafruit_ADS1115 ads1115_1(0x4B);
INA226_Class INA226; 
TCA9534 gpio(0x38);
const int addr = 0x50;
const int level= 0x80;
const int termistor=A0;
unsigned long tempo;

void potentiometer(int);
float temperatures(int);
float current ();
float voltage ();

void setup(){
  
  
  Wire.begin();
  //ads1115_1.begin();
  Serial.begin(9600);
 
  gpio.Begin();
  gpio.PinMode(GPIO_PIN_2, OUTPUT);
  gpio.PinMode(GPIO_PIN, OUTPUT); //colocar sempre essa merda
  gpio.PinMode(1, OUTPUT);
  
  INA226.begin(141,300,71); // o primeiro parametro é a corrente máxima do shunt da datasheet

  INA226.setAveraging(4);                                                     // Average each reading n-times     //
  INA226.setBusConversion(7);                                                 // Maximum conversion time 8.244ms  //
  INA226.setShuntConversion(7);                                               // Maximum conversion time 8.244ms  //
  INA226.setMode(INA_MODE_CONTINUOUS_BOTH); 
  
  //gpio.DigitalWrite(7,HIGH);

} 
void loop(){
  float volts=1,curr=1;
  String data;
  potentiometer(level);
  curr = current()*-1;
  //volts = voltage();
  //temp=temperatures(termistor);
  tempo=millis();
  //data = String(volts)+"V \t"+String(curr)+"mA \t"+String(temp)+"C \t"+String(tempo)+"ms";
  data = String(volts)+"V \t"+String(curr)+"mA \t"+String(tempo)+"ms";
  Serial.println(data);
  gpio.DigitalWrite(GPIO_PIN_2,LOW);
  gpio.DigitalWrite(1,HIGH);

  gpio.DigitalWrite(GPIO_PIN,HIGH);
  delay(500);

}
/*
float voltage (){
    return ads1115_1.readADC_Differential_2_3()*0.000188;
}*/

float current (){
    return (float)(INA226.getBusMicroAmps()/1000.0);
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