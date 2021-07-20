#include <SPI.h>
#include "ESP8266WiFi.h"

const char* ssid     = "NETGEAR45";
const char* password = "curlybird210";

const char* server = "api.carriots.com";
const String APIKEY = "1e2f6920a92e3ca079bf60329d7871d69b3881df88df9b2c44e6d71f17610d8f"; // Replace with your Carriots apikey
const String DEVICE = "light@junaid.junaid"; // Replace with the id_developer of your device

int ledpin = 4;
int pirpin = 12;
int pirstate = LOW;
int val = 0;

void setup(){
  Serial.begin(115200);
  delay(10);
  pinMode(ledpin,OUTPUT);
  pinMode(pirpin,INPUT);
   Serial.println("calibrating");
   for(int i = 0; i < 20; i++){
      Serial.print(".");
      delay(1000);
      }
  
  // start wifi

    Serial.println();
    Serial.println();
    Serial.print("Connecting to "); 
   Serial.println(ssid);
   WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// function to talk to the Carriot Platform

void sendStream()
{
  
   String txt = "";          // Text to send
  if ( pirstate == HIGH ) {   // Alarm OFF
     txt = "Motion Detected";
  } else {                  // Alarm ON
   txt = "Somethig wrong";
  }
  Serial.println(txt); 
   //Serial.println(val);// For debugging purpose only
  WiFiClient client;
  //const int httpPort = 80;
  if (client.connect(server, 80)) {   // If there's a successful connection
    Serial.println(F("connected"));
    // Build the data field
    String json = "{\"protocol\":\"v2\",\"device\":\""+DEVICE+"\",\"at\":\"now\",\"data\":{\"light\":\""+txt+"\"}}";

    // Make a HTTP request
    client.println("POST /streams HTTP/1.1");
    client.println("Host: api.carriots.com");
    client.println("Accept: application/json");
    client.println("User-Agent: Arduino-Carriots");
    client.println("Content-Type: application/json");
    client.print("carriots.apikey: ");
    client.println(APIKEY);
    client.print("Content-Length: ");
    int thisLength = json.length();
    client.println(thisLength);
    client.println("Connection: close");
    client.println();

    client.println(json);
  }
  else {
    // If you didn't get a connection to the server:
    Serial.println(F("connection failed"));
  }

}


void loop() {
  val = digitalRead(pirpin);
  Serial.println(val);
  if(val == HIGH){

    digitalWrite(ledpin,HIGH);
    if (pirstate == LOW){
      Serial.println("Motion Detected");
      pirstate =HIGH;
      Serial.println(F("Send Stream"));
      sendStream();
      delay(30000);
      }
   /*   while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
    delay(30000);
  } */
  }
else{
  digitalWrite(ledpin,LOW);
  if(pirstate == HIGH){
    Serial.println("motion Detected email sent");
    pirstate = LOW;
  }
}
}
