#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Pushsafer.h>
#include <neotimer.h>

#include "index.h"

#define LED D0
#define SOUND_VELOCITY 0.034

#define EEPROM_SIZE 64

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
int boardLedOn;

ESP8266WebServer server(80);
Neotimer pushTimer = Neotimer(60000); // 60 second timer

// Pushsafer private or alias key
#define PushsaferKey "pjaEn9pAdlxK7nUNjCoG"

WiFiClient client;
Pushsafer pushsafer(PushsaferKey, client);

void readEepromBoardId() 
{
  int readBoardId;
  EEPROM.get(0, readBoardId);
  server.send(200, "text/plane", String(readBoardId));
  boardId = readBoardId;
}

void readEepromEmptyWaterLevelDistanceInches()
{
  float readEmptyWaterLevelDistanceInches;
  EEPROM.get(2, readEmptyWaterLevelDistanceInches);
  server.send(200, "text/plane", String(readEmptyWaterLevelDistanceInches));
  emptyWaterLevelDistanceInches = readEmptyWaterLevelDistanceInches;
}

void readEepromWarningWaterLevelDistanceInches()
{
  float readWarningWaterLevelDistanceInches;
  EEPROM.get(6, readWarningWaterLevelDistanceInches);
  server.send(200, "text/plane", String(readWarningWaterLevelDistanceInches));
  warningWaterLevelDistanceInches = readWarningWaterLevelDistanceInches;
}

void readEepromBoardLedOn()
{
  int readBoardLedOn;
  EEPROM.get(10, readBoardLedOn);
  server.send(200, "text/plane", String(readBoardLedOn));
  boardLedOn = readBoardLedOn;
}

void writeEeprom()
{
  boardId = 1001;
  EEPROM.put(0, boardId);
  EEPROM.put(2, emptyWaterLevelDistanceInches);
  EEPROM.put(6, warningWaterLevelDistanceInches);
  EEPROM.put(10, boardLedOn);
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
  readEepromBoardLedOn();
  
  boardId = (server.arg("boardid")).toInt(); 
  warningWaterLevelDistanceInches = (server.arg("warningwaterleveldistanceinches")).toFloat(); 
  emptyWaterLevelDistanceInches  = (server.arg("emptywaterleveldistanceinches")).toFloat();
  boardLedOn  = (server.arg("boardledon")).toInt();
  
  writeEeprom();
}

void getSensorDataCm() 
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

  if(distanceCm > (emptyWaterLevelDistanceInches / 0.393701))
  {
    distanceCm = emptyWaterLevelDistanceInches / 0.393701; // Measurements above the maximum are erroneous.
  }
  
  server.send(200, "text/plane", String(distanceCm));
}

void getSensorDataInch() 
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


void checkPumpHealth()
{
  String pumpHealth = "No issues detected.";

  if (distanceInch <= warningWaterLevelDistanceInches)
  {
    pumpHealth = "PUMP FAILURE!";
  }
  server.send(200, "text/plane", String(pumpHealth));
}

void setLed()
{
  String state = "(OFF)";
  String act_state = server.arg("state");
  
  if(act_state == "1")
  {
    digitalWrite(LED,HIGH); //LED OFF
    state = "(OFF)";
  }
  else
  {
    digitalWrite(LED,LOW); //LED ON (Active Low)
    state = "(ON)";
  }
  server.send(200, "text/plane", state);
}

void getIPAddress() 
{
  String ipAddress = WiFi.localIP().toString().c_str();
  Serial.print("IP address: ");
  Serial.println(ipAddress);
  server.send(200, "text/plane", ipAddress);
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
    Serial.write(27); //Print "esc"
    Serial.print("[2J");
  }
  
  getSsid();
  getIPAddress();
  getMACAddress();

  readEepromBoardId();
  readEepromEmptyWaterLevelDistanceInches();
  readEepromWarningWaterLevelDistanceInches();
  readEepromBoardLedOn();

  server.on("/", handleRoot);
  server.on("/action_page", handleForm); 
  server.on("/set_led", setLed);
  server.on("/distance_cm", getSensorDataCm);
  server.on("/distance_in", getSensorDataInch);
  server.on("/percent_filled", calcPercentFilled);
  server.on("/pump_health", checkPumpHealth);
  server.on("/ip_address", getIPAddress);
  server.on("/mac_address", getMACAddress);
  server.on("/ssid", getSsid);
  server.on("/board_id", readEepromBoardId);
  server.on("/empty_water_level_distance_inches", readEepromEmptyWaterLevelDistanceInches);
  server.on("/warning_water_level_distance_inches", readEepromWarningWaterLevelDistanceInches);
  server.on("/board_led_on", readEepromBoardLedOn);

  // Set Onboard LED on/off depending on user EEPROM setting
  if (boardLedOn == 1)
  {
    digitalWrite(LED,HIGH); //LED OFF
  }
  else
  {
    digitalWrite(LED,LOW); //LED ON
  }
 
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
