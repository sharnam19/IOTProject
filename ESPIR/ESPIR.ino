#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#include <IRremoteESP8266.h>

#define ssid "JioFi2_4FBA47"    //  your network SSID (name) 
#define pass "lpv0kqdnzv"   // your network password

#define FIREBASE_HOST "arduinotemp-4ca39.firebaseio.com"
#define FIREBASE_AUTH "4EPI0CHbwLLQA3zpAw2pr4olo7YOuwmbkIelah2B"

#define RECV_PIN 2

#define TEMPHUM 1
#define KHZ 38

#define OFF "OFF"
#define ON "ON"

IRrecv irrecv(RECV_PIN);
IRsend irsend(0);

int status = WL_IDLE_STATUS;
WiFiClient  client;
unsigned int* a;
decode_results results;

int set=0;
int execute=0;
String ACstatus;
int temperature;
int humidity;
int offThreshold=20;
int onThreshold=30;
bool autoMode=false;


void readIR(){
  if(digitalRead(RECV_PIN)==HIGH && set!=0){
    if(irrecv.decode(&results)){
      irrecv.resume();
      Firebase.setInt("set",0);
      String path=String("command"+String(set));
      Firebase.remove(path);
      Firebase.setInt(String(path+"/length"),results.rawlen);
      for(int i=0;i<results.rawlen;i++){
        Firebase.setInt(String(path+String("/")+String(i)),results.rawbuf[i]);
      }
      set=0;
    }
  }
  delay(3000);
}

void transmit(){
  if(execute==1){
    Firebase.setString("status",ON);
    ACstatus=ON;
  }else if(execute==2){
    Firebase.setString("status",OFF);
    ACstatus=OFF;
  }
  execute=0;
  Firebase.setInt("execute",0);
}

void setup() {  
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  irrecv.enableIRIn();
  irsend.begin();
  ACstatus=Firebase.getString("status");
}


void loop() {
  // Read the latest value from field 1 of channel 1417
  set=Firebase.getInt("set");
  delay(400);
  execute=Firebase.getInt("execute");
  delay(400);
  offThreshold=Firebase.getInt("offthreshold");
  onThreshold=Firebase.getInt("onthreshold");
  delay(400);
  autoMode=Firebase.getBool("automode");
  delay(400);
  if(Serial.available()){
    Firebase.setString("Updated","1");
    int code=Serial.read();
    if(code==TEMPHUM){
      Firebase.setString("Updated","2");
      temperature=Serial.read();
      Firebase.setString("Updated","3");
      humidity=Serial.read();
      Firebase.setString("Updated","4");
      Firebase.setInt("temperature",temperature);
      Firebase.setInt("humidity",humidity);
      if(autoMode){
        if(temperature<offThreshold && ACstatus!=OFF){
          execute=2;
          transmit();
        }else if(temperature>=onThreshold && ACstatus!=ON){
          execute=1;
          transmit();
        }  
      }
    }
  }
  if(execute!=0){
    transmit();
  }
  if(set!=0){
    readIR();
  }
  //sendIR();
  delay(400);
}
