#include <FirebaseESP8266.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "DHT.h"
#include <time.h>
#include <NTPClient.h>

#define DHTPIN D5
#define DHTTYPE DHT11
#define LEDWIFI D8
#define BWRESET D7                          
#define FIREBASE_HOST "testled-17b94-default-rtdb.firebaseio.com"                     //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "yW3se8F3fnaBELRgEvuq7Bme8OzZdnoIGEUCwE08" //Your Firebase Database Secret goes here

// Declare the Firebase Data object in the global scope
FirebaseData firebaseData;

#define Relay1 D1 //D1
String val1;

#define Relay2 D2 //D2
String val2;

#define Relay3 D3 //D3
String val3;

#define Relay4 D4 //D4
String val4;

#define Sensor D5 //D
String val5;

String val6; //for time

String zero = "0" , one = "1";

DHT dht(DHTPIN, DHTTYPE);

void(* reset) (void) = 0;
int reset_counter = 0;
int max_to_reset = 1000;
 //perintah reset setelah berhasil berapa eksekusi

int buttonresetwifi = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"id.pool.ntp.org");

void setup()
{ 
  timeClient.begin();
  
  WiFiManager wm;
    wm.resetSettings();
    bool res;
    res = wm.autoConnect("SmartGarden");
    if(!res) {
     Serial.println("Failed to connect");
     // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }

    pinMode (LEDWIFI, OUTPUT);
    pinMode (BWRESET, INPUT);
     
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);

  // Arduino OTA Program
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
   });
   ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
   });
   ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
   });
   ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
   });
   ArduinoOTA.begin();
   Serial.println("Ready");
   Serial.print("IP address: ");
   Serial.println(WiFi.localIP());
    
 pinMode(Relay1,OUTPUT);
 pinMode(Relay2,OUTPUT);
 pinMode(Relay3,OUTPUT);
 pinMode(Relay4,OUTPUT);
 pinMode(Sensor,OUTPUT);

 digitalWrite(Relay1,LOW);
 digitalWrite(Relay2,LOW);
 digitalWrite(Relay3,LOW);
 digitalWrite(Relay4,LOW);
 digitalWrite(Sensor,LOW);

 Serial.print('\n');
 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // connect to firebase
 Firebase.setString(firebaseData, "/Suhu", one);
 dht.begin();
 Firebase.reconnectWiFi(true);
 delay(100);

}

void loop()
{
  timeClient.update();
  //Handling Arduino OTA
  ArduinoOTA.handle();

 buttonresetwifi = digitalRead(BWRESET);

 if (buttonresetwifi == HIGH)
 {
   digitalWrite(LEDWIFI, HIGH);
 }
  else { digitalWrite(LEDWIFI, LOW);}

    {reset_counter++;
    if(reset_counter >= max_to_reset){ //* kode ini untuk mereset nilai variable jadi 0 lagi. 
        reset_counter = 0; //* kode ini memanggil fungsi reset. perangkat akan tereset langsung. 
        reset();
        }}

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    
    if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
    
    float hif = dht.computeHeatIndex(f, h);
    
    float hic = dht.computeHeatIndex(t, h, false);
    
 Firebase.getString(firebaseData, "S1");
 val1 = firebaseData.stringData(); Serial.print('\n');

 Serial.print("Value 1- ");Serial.println(val1);

 if(val1== one) // If, the Status is 1, turn on the Relay1
     { digitalWrite(Relay1,LOW); Serial.println("light 1 OFF"); 
      Firebase.setString(firebaseData, "/Suhu", zero);
        Firebase.setString(firebaseData, "/Kelembapan", zero);
          } else if(val1==zero) // If, the Status is 0, turn Off the Relay1
     { digitalWrite(Relay1,HIGH); Serial.println("light 1 ON");
        Firebase.setString(firebaseData, "/Suhu", t);
        Firebase.setString(firebaseData, "/Kelembapan", h);
        Serial.print(t);
  Serial.print(F("°C "));
        Serial.print(h);
  Serial.print(F("% "));
         }
    
    Firebase.getString(firebaseData, "S2");
 val2 = firebaseData.stringData(); Serial.print('\n');

 Serial.print("Value 2- ");Serial.println(val2);

    if(val2==one) // If, the Status is 1, turn on the Relay1
     { digitalWrite(Relay2,LOW); Serial.println("light 2 OFF"); } else if(val2==zero) // If, the Status is 0, turn Off the Relay1
     { digitalWrite(Relay2,HIGH); Serial.println("light 2 ON"); }
    
    Firebase.getString(firebaseData, "S3");
 val3 = firebaseData.stringData(); Serial.print('\n');

    Serial.print("Value 3- ");Serial.println(val3); if(val3==one) // If, the Status is 1, turn on the Relay1
     { digitalWrite(Relay3,LOW); Serial.println("light 3 OFF");
      } else if(val3==zero) // If, the Status is 0, turn Off the Relay1
    { digitalWrite(Relay3,HIGH); Serial.println("light 3 ON"); }
    
    Firebase.getString(firebaseData, "S4");
 val4 = firebaseData.stringData(); Serial.print('\n');
    
    Serial.print("Value 4- ");Serial.println(val4); if(val4==one) // If, the Status is 1, turn on the Relay1
     { digitalWrite(Relay4,LOW); Serial.println("light 4 OFF"); 
     Firebase.setString(firebaseData, "/Time/sec",timeClient.getSeconds());
     Firebase.setString(firebaseData, "/Time/min",timeClient.getMinutes());
     Firebase.setString(firebaseData, "/Time/hour",timeClient.getHours());
     } else if(val4==zero) // If, the Status is 0, turn Off the Relay1
     { digitalWrite(Relay4,HIGH); Serial.println("light 4 ON"); } Serial.print('\n');    
     Serial.print("------------------------------------------"); Serial.print('\n'); 
}