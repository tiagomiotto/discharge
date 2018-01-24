

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <math.h> 
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Time.h>


#define level_1 0xD6
#define level_2 0x31
#define level_3 0x1C

File dis_log[3];
File chg_log[3];

boolean first_3C,first_4C;
const int led_chg[3] = {24,38,50};
const int leds[3][3] = {{25,26,27},{39,40,41},{51,52,53}}; //Leds bancada 1
const int transistorPin_rele_bat[3] = {22,36,48}; //Mudar alguns pinos por causa do cartão sd
const int transistorPin_rele_chg[3] = {23,37,49};
const int pot_addr[3] = {0x50,0x51,0x52};
const int termistorPin[3]= {0,0,0};
int level;
Adafruit_ADS1015 ads1115_1(0x48);
Adafruit_ADS1015 ads1115_2(0x49);
Adafruit_ADS1015 ads1115_3(0x4A);
RTC_DS1307 RTC; //Real time clock
String file_name;
String file_name_2;

int hour,min,sec;
float voltage_1; 
float current_1;
double temperature[3];
int fail_s[3];
int um_c[3];
boolean charge[3];
String data;
const int chipSelect = 10;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  ads1115_1.begin();
  ads1115_2.begin();
  ads1115_3.begin();
  pinMode(termistorPin[0],INPUT);
  pinMode(termistorPin[1],INPUT);
  pinMode(termistorPin[2],INPUT);
  pinMode(transistorPin_rele_bat[0], OUTPUT);
  pinMode(transistorPin_rele_bat[1], OUTPUT);
  pinMode(transistorPin_rele_bat[2], OUTPUT);
  pinMode(transistorPin_rele_chg[0], OUTPUT);
  pinMode(transistorPin_rele_chg[1], OUTPUT);
  pinMode(transistorPin_rele_chg[2], OUTPUT);
  pinMode(led_chg[0], OUTPUT);
  pinMode(led_chg[1], OUTPUT);
  pinMode(led_chg[2], OUTPUT);
  pinMode(leds[0][2], OUTPUT);
  pinMode(leds[0][1], OUTPUT);
  pinMode(leds[0][0], OUTPUT);
  pinMode(leds[1][0], OUTPUT);
  pinMode(leds[1][1], OUTPUT);
  pinMode(leds[1][2], OUTPUT);
  pinMode(leds[2][0], OUTPUT);
  pinMode(leds[2][1], OUTPUT);
  pinMode(leds[2][2], OUTPUT);
  pinMode(SS,OUTPUT);
 // pinMode(13,OUTPUT);
  level=level_1; // Mudar conforme a descarga, pra começar perto do que é suposto
  for (int i = 0; i < 3; ++i) // Open files to log processes
  { 
     fail_s[i]= 0;
     um_c[i]= 0;
     charge[i]= false;
  }  
 
  pinMode(SS,OUTPUT);
  /*Só pro primeiro teste sem a bateria*/
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  /**/

  DateTime now=RTC.now();
 
  if (!SD.begin(chipSelect)) { //É assim pro mega
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) ;
  }
  if(!RTC.isrunning()){
    Serial.println("RTC error\n");
  }
  //Colocar nome com no maximo 8 digitos.
  //Colocar \t depois de qualquer leitura
  //Parece que assim da 
  for (int i = 0; i < 3; ++i) // Open files to log processes
  {
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
    dis_log[i]=SD.open(file_name.c_str(),FILE_WRITE);
    chg_log[i]=SD.open(file_name_2.c_str(),FILE_WRITE);
    // O nome fica d+numero da bench+hora que começa o teste
    
    if(!dis_log[i] || !chg_log[i]){
      Serial.println("Error opening file");
      while(1);
    }
  }
  first_3C=false;
  first_4C=false;
  for (int i = 0; i < 3; ++i) if(voltage(i+1)<4.2) charge[i]=true; //Checks if battery is fully charged when the arduino is powered on, before starting the test
} 

void loop() {

  
  //Set time no RTC

  //Marcar as baterias pela ordem de teste, pra poder saber qual se relaciona com cada ficheiro de texto
  //que é marcado pelo código dado pelo unix time de inicio do teste

 
  DateTime now=RTC.now(); 
  //Use this or unix time
  hour=now.hour();
  min=now.minute();
  sec=now.second();

 


  for (int i = 0; i < 3; ++i)
  {
  //Get temperatures
  temperature[i]=temperatures(termistorPin[i]);

  //Fail-safe in case the battery gets too hot
  fail_s[i] = fail_safe(fail_s[i], temperature[i],um_c[i],i);

  //DIscharge at 1c
  if (um_c[i] < 1 && !charge[i])
  {
    voltage_1 = voltage(i);
    current_1 = current(i);
    turn_on(fail_s[i], i);
    um_c[i] = um_c[i] + discharge(current_1, voltage_1, level, charge[i],i,um_c[i]);
      if(um_c[i]>0){ //Turn on LED to indicate n discharges complete
        digitalWrite(leds[i][um_c[i]-1],HIGH); //Minus one is due to the value having already been incremented;
      }
    data = String(voltage_1) + "\t," + String(current_1) + "\t," +String(temperature[i]) + "\t," + String(hour) + ":" +String(min) + ":"+String(sec) + "\n" ;
    Serial.println(data);
    dis_log[i].println(data);
    dis_log[i].flush();
  }

  //Charge
  if (charge[i])
  {
    digitalWrite(led_chg[i],HIGH);
    voltage_1 = voltage(i);
    charge[i] = charge_bat(voltage_1,i);
    if(!charge[i]){
      digitalWrite(led_chg[i],LOW);
    }
    data = "Charge " + String(voltage_1) + "\t," + String(current_1) + "\t," +String(temperature[i]) + "\t," + String(hour) + ":" +String(min) + ":"+String(sec) + "\n" ;
    Serial.println(data);
    chg_log[i].println(data);
    chg_log[i].flush();
  }

      //All finished
  if(um_c[i]>3  && !charge[i]) {
    turn_off(i);  
  }
  
  }

  delay(500);

}

int fail_safe(int fail_s, int temperature, int n,int bench_r) //Fail-safe in case the battery gets too hot
{
  
  if ( temperature > 60)
  {
    digitalWrite(transistorPin_rele_bat[bench_r], LOW);
    digitalWrite(transistorPin_rele_bat[bench_r], LOW);
    return 1;
  }
  if (fail_s == 1 )//Blink leds to indicate in which of the tests it failed
  {

      digitalWrite(leds[bench_r][n],HIGH);
      delay(1000);
      digitalWrite(leds[bench_r][n],LOW);
  }

}

int turn_on(int fail_safe, int bench_r)
{
  if (fail_safe == 0)
  {
    digitalWrite(transistorPin_rele_bat[bench_r], HIGH);
    digitalWrite(transistorPin_rele_bat[bench_r], HIGH);
  }
  return 1;
}

void turn_off(int bench_r)
{
  digitalWrite(transistorPin_rele_bat[bench_r], LOW);
  digitalWrite(transistorPin_rele_bat[bench_r], LOW);
}
int discharge (int16_t current, int16_t voltage, int level, boolean &charge, int n, int ce) //N defines the battery being tested
{
  switch (ce){
    case 0: 
            return discharge_1C(current,voltage,level,charge,n);
            break;
    case 1: if(!first_3C)
            {
              level=level_2;
              first_3C=true; //So it can correct itself on the upcoming iterations of the code.
            }
            return discharge_3C(current,voltage,level,charge,n);
            break;
    case 2:  if(!first_4C)
            {
              level=level_3;
              first_4C=true; //So it can correct itself on the upcoming iterations of the code.
             }
            return discharge_4C(current,voltage,level,charge,n);
            break;
    default: return 0;
  }
}
int discharge_1C(int16_t current, int16_t voltage, int level, boolean &charge, int n)
{
  //Define the addr in regard with the chosen bench
  int addr;
  addr=pot_addr[n-1]; 
  

  if (current > 16.8) { //Keep the precision on the discharge current
    potentiometer(level + 1,addr);
  }
  if (current < 16.8) {
    potentiometer(level - 1,addr);
  }
  if (voltage <= 3) { // Discharge cycle ended
    charge = true;
    return 1;
  }
  return 0;
}

int discharge_3C(int16_t current, int16_t voltage, int level, boolean &charge, int n) { //Tirar?
  int addr;
  addr = pot_addr[n-1]; //Select the correct addres for the given bench


  if (current > 51.4) { //Keep the precision on the discharge current
    potentiometer(level + 1,addr);
  }
  if (current < 49.4) {
    potentiometer(level - 1,addr);
  }

  if (voltage <= 3) {
    charge = true;
    return 1;
  }
}

int discharge_4C(int16_t current, int16_t voltage, int level, boolean &charge, int n) { //Tirar?
  int addr;
  addr = pot_addr[n-1]; //Select the correct addres for the given bench


  if (current > 69.2) { //Keep the precision on the discharge current
    potentiometer(level + 1,addr);
  }
  if (current < 65.2) {
    potentiometer(level - 1,addr);
  }

  if (voltage <= 3) {
    charge = true;
    return 1;
  }
}

boolean charge_bat(int16_t voltage, int bench) { //Corrigir aqui
  //Light up charging led
  digitalWrite(led_chg[bench], HIGH);
  digitalWrite(transistorPin_rele_chg[bench], HIGH);
  digitalWrite(transistorPin_rele_bat[bench], LOW);
  if (voltage >= 4.2) {
    return  false; //To indicate that charging has finished
  }
  else return true;
}

void potentiometer(int level,int addr) // Testar isso com a célula grande
{
  Wire.beginTransmission(addr); // transmit to device #addr (0x2c)
  // device address is specified in datasheet
  Wire.write(byte(0x00));        // sends instruction byte // Essa instrução é pra escrever no Wiper Registry
  Wire.write(level);             // sends potentiometer value byte 
  Wire.endTransmission();     // stop transmitting
}


float temperatures(int THERMISTORPIN){ //Se não funcionar com esta fómula, usar a interpolação do Jonathan
// Colocar calculos da temperatura aqui
  double reading;
  reading = analogRead(THERMISTORPIN); 
  float voltage = reading * (5.0 / 1023.0);
  float current = voltage/10000;
  float Rthermistor = (5-(1+125/10000)*voltage)/current;
  float temperature = 3435/(log(Rthermistor/(10000*exp(-3435/298.15) ) )) - 273.15;
  return  temperature;
}


float voltage (int n){

  if (n==0)
  {
    return ads1115_1.readADC_Differential_0_1()/0.0000762939453125;
  }
  if (n==1)
  {
    return ads1115_2.readADC_Differential_0_1()/0.0000762939453125;
  }
  if (n==2)
  {
    return ads1115_3.readADC_Differential_0_1()/0.0000762939453125;
  } // Return the voltage according to the chosen bench
}

float current (int n){
  if (n==0)
  {
    return (ads1115_1.readADC_Differential_2_3()/0.0000762939453125)/0.0003;
  }
  if (n==1)
  {
    return (ads1115_2.readADC_Differential_2_3()/0.0000762939453125)/0.0003;
  }
  if (n==2)
  {
    return (ads1115_3.readADC_Differential_2_3()/0.0000762939453125)/0.0003;
  } // Return the current according to the chosen bench
}


