#include "DHT.h"
#include <SoftwareSerial.h> 

#define DHTPIN 2
#define DHTTYPE DHT11   // DHT 11

#define TEMPHUM 1

SoftwareSerial esp8266(7,8);

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  esp8266.begin(115200);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  dht.begin();
}

int prevTemp=0;
int prevHum=0;
int temp=1;
int hum=1;
void loop() {  
  hum=(int)dht.readHumidity();
  temp=(int)dht.readTemperature();
  if(prevTemp!=temp || prevHum!=hum){
      prevTemp=temp;
      prevHum=hum;
      Serial.println(temp);
      Serial.println(hum);
      esp8266.write(TEMPHUM);
      esp8266.write(temp);
      esp8266.write(hum);
  }
  delay(2000);
}
