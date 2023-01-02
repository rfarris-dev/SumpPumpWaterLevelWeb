#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Pushsafer.h>
#include <neotimer.h>

#include "index.h"

#define LED D0
#define SOUND_VELOCITY 0.034

#define EEPROM_SIZE 12

const char* ssid = "TreeRat";
const char* password = "deadsquirrel";

const int trigPin = 12;
const int echoPin = 14;

long duration;
float distanceCm;
float distanceInch;
int percentFilled;

float warningWaterLevelDistanceInches;
float emptyWaterLevelDistanceInches;

int boardId;

ESP8266WebServer server(80);
Neotimer pushTimer = Neotimer(60000); // 60 second timer

// Pushsafer private or alias key
#define PushsaferKey "pjaEn9pAdlxK7nUNjCoG"

/*WiFiClientSecure client;*/
WiFiClient client;
Pushsafer pushsafer(PushsaferKey, client);

void readEepromBoardId() 
{
  //Read data from eeprom
  Serial.println("----------EEPROM READ----------");
  unsigned int address = 0;
  int readBoardId;
  EEPROM.get(address, readBoardId);
  Serial.print("Read Board Id = ");
  Serial.println(readBoardId);
  
  address += sizeof(readBoardId); //update address value

  server.send(200, "text/plane", String(readBoardId));
  
  boardId = readBoardId;

}

void readEepromWarningWaterLevelDistanceInches()
{
  //Read data from eeprom
  Serial.println("----------EEPROM READ----------");
  unsigned int address = 0;
  int readBoardId;
  EEPROM.get(address, readBoardId);

  address += sizeof(readBoardId); //update address value

  float readEmptyWaterLevelDistanceInches;
  
  EEPROM.get(address, readEmptyWaterLevelDistanceInches);

  address += sizeof(readEmptyWaterLevelDistanceInches); //update address value

  float readWarningWaterLevelDistanceInches;
  
  EEPROM.get(address, readWarningWaterLevelDistanceInches);
  Serial.print("Sensor to Max Water Level Distance (in): ");
  Serial.println(readWarningWaterLevelDistanceInches);
  Serial.println("");

  server.send(200, "text/plane", String(readWarningWaterLevelDistanceInches));
  
  warningWaterLevelDistanceInches = readWarningWaterLevelDistanceInches;
}


void readEepromEmptyWaterLevelDistanceInches()
{
  //Read data from eeprom
  Serial.println("----------EEPROM READ----------");
  unsigned int address = 0;
  int readBoardId;
  EEPROM.get(address, readBoardId);
  
  address += sizeof(readBoardId); //update address value

  float readEmptyWaterLevelDistanceInches;
  
  EEPROM.get(address, readEmptyWaterLevelDistanceInches);
  Serial.print("Sensor to Min Water Level Distance (in): ");
  Serial.println(readEmptyWaterLevelDistanceInches);
  Serial.println("");

  server.send(200, "text/plane", String(readEmptyWaterLevelDistanceInches));
  
  emptyWaterLevelDistanceInches = readEmptyWaterLevelDistanceInches;
  
}

void writeEeprom()
{
  //Write data into eeprom
  unsigned int address = 0;
  boardId = 1000;
  EEPROM.put(address, boardId);

  address += sizeof(boardId); //update address value
  EEPROM.put(address, emptyWaterLevelDistanceInches);

  address += sizeof(warningWaterLevelDistanceInches); //update address value
  EEPROM.put(address, warningWaterLevelDistanceInches);
  
  EEPROM.commit();
  
}

void handleRoot() 
{
 String s = webpage;
 server.send(200, "text/html", s);
}

void handleForm()
{
  String s = "<script>window.location = '/'</script>";  // Back to main page after writing.
  server.send(200, "text/html", s); //Send web page 
  
  readEepromBoardId();
  readEepromEmptyWaterLevelDistanceInches();
  readEepromWarningWaterLevelDistanceInches();
  
  boardId = (server.arg("boardid")).toFloat(); 
  warningWaterLevelDistanceInches = (server.arg("warningwaterleveldistanceinches")).toFloat(); 
  emptyWaterLevelDistanceInches  = (server.arg("emptywaterleveldistanceinches")).toFloat();
  
  Serial.println("----------WEB FORM HANDLER----------");
  
  Serial.print("Sensor to Min Water Level Distance (in): ");
  Serial.println(emptyWaterLevelDistanceInches);
  
  Serial.print("Sensor to Max Water Level Distance (in): ");
  Serial.println(warningWaterLevelDistanceInches);
  Serial.println("");
  
  writeEeprom();
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

  if(distanceInch > emptyWaterLevelDistanceInches)
  {
    distanceInch = emptyWaterLevelDistanceInches; // Measurements above the maximum are erroneous.
  }
  
  server.send(200, "text/plane", String(distanceInch));

  if(pushTimer.done() && distanceInch <= warningWaterLevelDistanceInches)
  {
    //pushPumpNotification();   // Disable pushing for now.
    pushTimer.start();
  }

  else if (pushTimer.waiting())
  {
    Serial.println("Timer until next possible push.");
  }
}

void calcPercentFilled() 
{
  percentFilled = (1 - ((distanceInch - warningWaterLevelDistanceInches) / (emptyWaterLevelDistanceInches - warningWaterLevelDistanceInches))) * 100;
  
  server.send(200, "text/plane", String(percentFilled));
}


void sensor_pumpHealth() 
{
  String pumpHealth = "No issues detected.";

  if (distanceInch <= warningWaterLevelDistanceInches)
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

void getIPAddress() 
{
  String ipAddress = WiFi.localIP().toString().c_str();
  Serial.print("IP address: ");
  Serial.println(ipAddress);
  server.send(200, "text/plane", ipAddress);

  Serial.println();
  Serial.print("MAC: ");
  String macAddress = WiFi.macAddress();
  Serial.println(macAddress);
  server.send(200, "text/plane", macAddress); 
}

void getMACAddress() 
{
  Serial.println();
  Serial.print("MAC: ");
  String macAddress = WiFi.macAddress();
  Serial.println(macAddress);
  server.send(200, "text/plane", macAddress);
}

void getSsid() 
{
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  server.send(200, "text/plane", ssid);
}


void setup(void)
{
  Serial.begin(115200);

  Serial.println(F("Initialize EEPROM"));
  //Init EEPROM
  EEPROM.begin(EEPROM_SIZE);
  
  WiFi.begin(ssid, password);
  Serial.println("");
  
  pinMode(LED,OUTPUT); 
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("---\n");
  }
  
  getSsid();
  getIPAddress();
  getMACAddress();

  readEepromBoardId();
  readEepromEmptyWaterLevelDistanceInches();
  readEepromWarningWaterLevelDistanceInches();

  server.on("/", handleRoot);
  server.on("/action_page", handleForm); 
  server.on("/led_set", led_control);
  server.on("/readcm", sensor_data);
  server.on("/readin", sensor_dataInch);
  server.on("/percentfilled", calcPercentFilled);
  server.on("/pumphealth", sensor_pumpHealth);
  server.on("/ipaddress", getIPAddress);
  server.on("/macaddress", getMACAddress);
  server.on("/ssid", getSsid);
  server.on("/boardid", readEepromBoardId);
  server.on("/emptywaterleveldistanceinches", readEepromEmptyWaterLevelDistanceInches);
  server.on("/warningwaterleveldistanceinches", readEepromWarningWaterLevelDistanceInches);

 
  server.begin();
  Serial.println("HTTP server started");
  
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

  // Note on TCL 10 Pro: For reliable pushing, you need to use ADBAppControl for Windows to remove TCL bloatware Smart Manager and Smart Push.

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
