#include <ESP8266WiFi.h>              // Library for using the NodeMCU 
#include <PubSubClient.h>            // Library for using the Network protocol MQTT in this case


/********* WiFi Access Point ***********/
const char* ssid = "yashgarg";        // Add your SSID(The Name of the Wifi Router you connect to)
const char* password = "1235yash12"; // The PassWord (Wireless Key)
          /********* LinuxMqtt Setup ***********/
const char* mqtt_server = "test.mosquitto.org";
WiFiClient espClient;
PubSubClient client(espClient);

/********* PIN DECLARATION ***********/


// defines pins numbers
const int trigPin = 5;
const int echoPin = 4;
const int ledPin = 0;
const int ldrPin = A0;
// defines variables
long duration;
int distance;
int ldPin = 16;                // LED 
int pirPin = 13;                 // PIR Out pin 
int pirStat = 0;                   // PIR status

/******CONNECTION TO WIFI*******/
void setup_wifi() {
  delay(100);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");

    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200); // Starts the serial monitor at 115200
  setup_wifi(); // add Method for the Wifi setup
  client.setServer(mqtt_server, 1883); // connects to the MQTT broker

  /********* ULTRASONIC Setup *************/
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
Serial.begin(9600); // Starts the serial communication

  /**********PIR SETUP ************/
 pinMode(ldPin, OUTPUT);     
 pinMode(pirPin, INPUT);     
 Serial.begin(9600);
  /**********LDR SETUP*************/

pinMode(ledPin, OUTPUT);

pinMode(ldrPin, INPUT);
Serial.begin(9600);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  } // MQTT
  /********* WORKING IN NODE RED *************/
    // start working...
  Serial.println("=================================");
  Serial.println("Sample Ultrasonic...");
  Serial.println("Sample Motionsensor...");

  // Wait a few seconds between measurements.
  delay(2000);
  // Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
//motion sensor code
 pirStat = digitalRead(pirPin); 
  delay(1000);
  String motion;
    if (pirStat == HIGH) {            // if motion detected
      digitalWrite(ldPin, HIGH);  // turn LED ON
      Serial.println("Motion detected");
      motion="Motion detected!";
      client.publish("wifiMotion", String(motion).c_str());
      delay(30000);
    }
    else {
      digitalWrite(ldPin, LOW); // turn LED OFF if we have no motion
      Serial.println("Motion absent");
      motion="Motion absent!";
      client.publish("wifiMotion", String(motion).c_str());
      delay(2000);
      }
  //ldr coding
  int ldrStatus = analogRead(ldrPin);
  String value;
  if (ldrStatus <=300) {

  digitalWrite(ledPin, HIGH);
  Serial.print(ldrStatus);
  value="ON";
  Serial.println("LDR is DARK, LED is ON");

  }

  else {

  digitalWrite(ledPin, LOW);
  value="OFF";
  Serial.println("LED is OFF");
  
  }
  /********* Publish String MQTT ********/
  client.publish("wifiDist", String(distance).c_str());
  client.publish("wifildr", String(value).c_str());
  delay(1000);
}
