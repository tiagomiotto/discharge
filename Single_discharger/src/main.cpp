/*Adc testing module 30/03
Test the adc by measuring the voltage and current provided by the dummy cell and one 
Current drain is expected to be around 14,38 due to the potentiometer's limitation
*/
#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 
#include <INA226.h>
#include <TCA9534.h>
#include <math.h> 
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Time.h>



//INA
#define CHG_INA 0x40
#define DSC_INA 0x47
INA226 ina; 
INA226 ina_chg;

//adc
#define ADC_ADDR 0x48
Adafruit_ADS1115 ads1115_1(ADC_ADDR);

//POTENTIOMETER
#define POT_LEVEL 0x00
#define POT_ADDR 0x53
const int level= POT_LEVEL;
const int addr = POT_ADDR;

//GPIO ADDDRS
#define GPIO_ADDR 0x3B
#define GPIO_BAT 7 //rele bat
#define GPIO_CHG 6 //rele chg
#define GPIO_CHG_LED 0
#define GPIO_LED_1 1
#define GPIO_LED_2 2
#define GPIO_LED_3 3
TCA9534 gpio(GPIO_ADDR);

//SD CARD AND FILES
File dis_log;
File chg_log;
String file_name;
String file_name_2;
String data;
const int chipSelect = 10;

//Real time clock
RTC_DS1307 RTC; 

//ENVIROMENT VARIABLES
int hour,min,sec;
float voltage_1; 
float current_1;
double temperature;
int fail_s;
int um_C;
boolean charge;



const int termistor=A0;
unsigned long tempo;


void potentiometer(int);
float temperatures(int);
float current ();
float voltage ();

void setup()
{
  Wire.begin();
  ads1115_1.begin();
  Serial.begin(9600);
  RTC.begin();
  gpio.Begin();
  ina.begin(CHG_INA);
  ina_chg.begin(DSC_INA);

  //GPIO SETUP
  gpio.PinMode(GPIO_BAT, OUTPUT);
  gpio.PinMode(GPIO_CHG, OUTPUT); //colocar sempre essa merda
  gpio.PinMode(GPIO_CHG_LED, OUTPUT);
  gpio.PinMode(GPIO_LED_1, OUTPUT);
  gpio.PinMode(GPIO_LED_2, OUTPUT);
  gpio.PinMode(GPIO_LED_3, OUTPUT);

  fail_s= 0;
  charge= false;
  //SD AND RTC SETUP
  pinMode(SS,OUTPUT);
  if (!SD.begin(chipSelect)) { //É assim pro mega
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) ;
  }
  if(!RTC.isrunning()){
    Serial.println("RTC error\n");
  }
  //SETUP FILES
    file_name= "d";
    file_name += String(i+1) ;
    file_name += "_" ;
    file_name += String(now.hour());
    file_name += String(now.minute());
    file_name +=".txt"; 
    file_name_2 =  "c"; 
    file_name_2 += String(i+1) ;
    file_name_2 += "_" ;
    file_name_2 += String(now.hour()) ;
    file_name_2 += String(now.minute());
    file_name_2 +=".txt";
    dis_log=SD.open(file_name.c_str(),FILE_WRITE);
    chg_log=SD.open(file_name_2.c_str(),FILE_WRITE);

    if(!dis_log || !chg_log){
      Serial.println("Error opening file");
      while(1);
    }


  //CONFIGURE INA
  ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
  ina_chg.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
  // Calibrate INA226. Rshunt = 0.0003 ohm, Corrente maxima do shunt 141.3
  ina.calibrate(0.0003, 141.3);
  // Calibrate INA226. Rshunt = 0.001 ohm, Max accepted current = 71.2A
  ina_chg.calibrate(0.001, 71.2);

  //IN CASE THE BATTERY IS DISCHARGED
  if(voltage()<4.2) charge=true;

} 

void loop(){

  //Marcar as baterias pela ordem de teste, pra poder saber qual se relaciona com cada ficheiro de texto
  //que é marcado pelo código dado pelo unix time de inicio do teste
  //GET CURRENT TIME
  DateTime now=RTC.now(); 
  hour=now.hour();
  min=now.minute();
  sec=now.second();


  delay(500);

}

float voltage (){
    return ads1115_1.readADC_Differential_0_1()*0.000188*2;
}

float current(bool chg){
  if(chg){
    return ina_chg.readShuntCurrent();
  }
  return ina.readShuntCurrent();
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
boolean charge_bat(int16_t voltage) { //Corrigir aqui
  //Light up charging led
  gpio.DigitalWrite(GPIO_CHG_LED, HIGH);
  gpio.DigitalWrite(GPIO_CHG,HIGH);
  gpio.DigitalWrite(GPIO_BAT, LOW);
  if (voltage >= 4.2) {
    return  false; //To indicate that charging has finished
  }
  else return true;
}

int discharge(int16_t current, int16_t voltage, int level, boolean &charge)
{
  //Define the addr in regard with the chosen bench
 
  if (current > 16.8) { //Keep the precision on the discharge current
    potentiometer(level + 1);
  }
  if (current < 16.8) {
    potentiometer(level - 1);
  }
  if (voltage <= 3) { // Discharge cycle ended
    charge = true;
    return 1;
  }
  return 0;
}

int turn_on(int fail_safe)
{
  if (fail_safe == 0)
  {
   gpio.DigitalWrite(GPIO_CHG,LOW);
   delay(500);
   gpio.DigitalWrite(GPIO_BAT, HIGH);
  }
  return 1;
}

void turn_off(int bench_r)
{
  digitalWrite(GPIO_CHG, LOW);
  delay(500);
  digitalWrite(GPIO_BAT, LOW);
}

int fail_safe(int fail_s, int temperature,int n) //Fail-safe in case the battery gets too hot
{
  
  if ( temperature > 60)
  {
    gpio.DigitalWrite(GPIO_CHG, LOW);
    gpio.DigitalWrite(GPIO_BAT, LOW);
    return 1;
  }
  if (fail_s == 1 )//Blink leds to indicate in which of the tests it failed
  {

      gpio.DigitalWrite(GPIO_LED_1,HIGH);
      gpio.DigitalWrite(GPIO_LED_1,HIGH);
      gpio.DigitalWrite(GPIO_LED_1,HIGH);
      delay(1000);
      gpio.DigitalWrite(GPIO_LED_1,LOW);
      gpio.DigitalWrite(GPIO_LED_2,LOW);
      gpio.DigitalWrite(GPIO_LED_3,LOW);
  }

}