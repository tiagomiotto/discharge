/*
Sd card testing module 17/01
Ps. We have to set the time in the rtc beforehand.
*/

#include <Wire.h>
#include <math.h> 
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>


RTC_DS1307 RTC;
File dis_log;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  DateTime now=RTC.now();
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
    file_name += String(now.day()) ;
    file_name += "_" ;
    file_name += String(now.hour());
    file_name +=".txt"; 

    dis_log=SD.open(file_name.c_str(),FILE_WRITE);
    
    
    if(dis_log[i]<=0 || !chg_log[i]<=0){
      Serial.println("Error opening file");
      //while(1);
    }
  }

  void loop(){
      DateTime now=RTC.now();

  hour=now.hour();
  min=now.minute();
  sec=now.second(); 
    data = String(hour) + ":" +String(min) + ":"+String(sec) + "\n" ;    //Prin in serial
    Serial.println(data);
    dis_log.println(data);                                              //Print in SD card
  }
