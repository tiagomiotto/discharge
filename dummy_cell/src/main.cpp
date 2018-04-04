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
#define CHG_INA 1
#define DSC_INA 2
#define POT_LEVEL 0x00
#define POT_ADDR 0x53
#define ADC_ADDR 0x48
#define GPIO_ADDR 0x3B
#define GPIO_BAT 7 //rele bat
#define GPIO_CHG 6 //rele chg

Adafruit_ADS1115 ads1115_1(0x4B);
INA226 ina; 
INA226 ina_chg;
TCA9534 gpio(0x38);
const int addr = 0x50;
const int level= 0x00;
const int termistor=A0;
unsigned long tempo;


void potentiometer(int);
float temperatures(int);
float current ();
float voltage ();

void setup(){
  
  
  Wire.begin();
  ads1115_1.begin();
  Serial.begin(9600);
 
  gpio.Begin();
  gpio.PinMode(GPIO_BAT, OUTPUT);
  gpio.PinMode(GPIO_CHG, OUTPUT); //colocar sempre essa merda
  gpio.PinMode(0, OUTPUT);
  gpio.PinMode(1, OUTPUT);
  gpio.PinMode(2, OUTPUT);
  gpio.PinMode(3, OUTPUT);
  gpio.PinMode(4, OUTPUT);
  gpio.PinMode(5, OUTPUT);


  // o primeiro parametro é a corrente máxima do shunt da datasheet
  // Default INA226 address is 0x40
  ina.begin(0x46);
  ina_chg.begin(0x40);
  // Configure INA226
  ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);

  // Calibrate INA226. Rshunt = 0.0003 ohm, Corrente maxima do shunt 141.3
  ina.calibrate(0.0003, 141.3);
  ina_chg.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);

  // Calibrate INA226. Rshunt = 0.001 ohm, Max accepted current = 71.2A
  ina_chg.calibrate(0.001, 71.2);
} 

void loop(){

  String data;
  potentiometer(level);
  //Serial.println(data);
  gpio.DigitalWrite(GPIO_BAT,LOW);
  gpio.DigitalWrite(0,HIGH);
  gpio.DigitalWrite(1,HIGH);
  gpio.DigitalWrite(2,HIGH);
  gpio.DigitalWrite(3,HIGH);
  /*
  gpio.DigitalWrite(4,HIGH);
  gpio.DigitalWrite(5,HIGH);
  */
  gpio.DigitalWrite(GPIO_CHG,HIGH);
  
  delay(500);
  gpio.DigitalWrite(0,LOW);
  gpio.DigitalWrite(1,LOW);
  gpio.DigitalWrite(2,LOW);
  gpio.DigitalWrite(3,LOW);
  /*
  gpio.DigitalWrite(4,LOW);
  gpio.DigitalWrite(5,LOW); //Alert pins for inas
  */
  Serial.print("ADC Voltage ");
  Serial.print(voltage(), 5);
  Serial.println(" V");

  Serial.print("Shunt current: ");
  Serial.print(ina.readShuntCurrent(), 5);
  Serial.println(" A");
  Serial.print("CHG Shunt current: ");
  Serial.print(ina_chg.readShuntCurrent(), 5);
  Serial.println(" A");

  delay(500);

}

float voltage (){
    return ads1115_1.readADC_Differential_0_1()*0.000188*2;
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