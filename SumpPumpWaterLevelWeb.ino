#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Pushsafer.h>
#include <neotimer.h>

#include "index.h"


#define LED D0
#define SOUND_VELOCITY 0.034

const char* ssid = "TreeRat";
const char* password = "deadsquirrel";

const int trigPin = 12;
const int echoPin = 14;

long duration;
float distanceCm;
float distanceInch;

ESP8266WebServer server(80);
Neotimer pushTimer = Neotimer(60000); // 60 second timer

// Pushsafer private or alias key
#define PushsaferKey "pjaEn9pAdlxK7nUNjCoG"

/*WiFiClientSecure client;*/
WiFiClient client;
Pushsafer pushsafer(PushsaferKey, client);

void handleRoot() 

{
 String s = webpage;
 server.send(200, "text/html", s);
}

void sensor_data() 
{
 // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  
  server.send(200, "text/plane", String(distanceCm));
 
}

void sensor_dataInch() 
{
  // Convert to inches
  distanceInch = distanceCm * 0.393701;
  
  server.send(200, "text/plane", String(distanceInch));

  if(pushTimer.done() && distanceInch <= 5)
  {
    pushPumpNotification();
    pushTimer.start();
  }

  else if (pushTimer.waiting())
  {
    Serial.println("Timer until next possible push.");
  }

}


void sensor_pumpHealth() 
{
  String pumpHealth = "No issues detected.";

  if (distanceInch <= 5)
  {
    pumpHealth = "PUMP FAILURE! WATER LEVEL TOO HIGH!";
  }
  
  server.send(200, "text/plane", String(pumpHealth));
}

void led_control() 
{
 String state = "OFF";
 String act_state = server.arg("state");
 if(act_state == "1")
 {
  digitalWrite(LED,HIGH); //LED OFF
  state = "OFF";
 }

 else
 {
  digitalWrite(LED,LOW); //LED ON (Active Low)
  state = "ON";
 }
 
 server.send(200, "text/plane", state);
}


void setup(void)
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  
  pinMode(LED,OUTPUT); 
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".../n");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led_set", led_control);
  server.on("/readcm", sensor_data);
  server.on("/readin", sensor_dataInch);
  server.on("/pumphealth", sensor_pumpHealth);
  server.begin();

  pushTimer.start();  //first go at timer

}


void loop(void)
{
  server.handleClient();
}

void pushPumpNotification()
{
  pushsafer.debug = true;

  // Take a look at the Pushsafer.com API at
  // https://www.pushsafer.com/en/pushapi
  
  struct PushSaferInput input;
  input.message = "Possible sump pump failure!!!!";
  input.title = "WARNING";
  input.sound = "8";
  input.vibration = "1";
  input.icon = "73";   //Flooded basment icon.
  input.iconcolor = "#FFCCCC";
  input.priority = "1";
  input.device = "a";
  input.url = "https://www.pushsafer.com";
  input.urlTitle = "Open Pushsafer.com";
  input.picture = "";
  input.picture2 = "";
  input.picture3 = "";
  input.time2live = "";
  input.retry = "";
  input.expire = "";
  input.answer = "";

  Serial.println(pushsafer.sendEvent(input));
  Serial.println("Sent");
  
  // client.stop();
}
