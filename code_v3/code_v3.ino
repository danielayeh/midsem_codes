#include <LiquidCrystal_I2C.h>
#define TRIG_PIN 26 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 25 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h> 
#include "html.h"

float durationUs, distanceCm;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

int redLed = 23;
int relayPin = 19;

const int maxTankHeight = 100;

// Set automatic mode to true initially
boolean autoMode = true;

boolean motorLedOn = false;
boolean emptyTank = false;

const char* ssid = "Laye";
const char* password = "kppu3807";

WebServer server(80);

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin (115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(relayPin, OUTPUT);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", base);
  server.on("/getDistance", getDistance);
  server.on("/auto", changeToAuto);
  server.on("/manual", changeToManual);
  server.on("/on", turnMotorOn);
  server.on("/off", turnMotorOff);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void base(){
  server.send(200, "text/html", page);
}

void getDistance() {
  sendDistance();
}

float measureDistance() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  durationUs = pulseIn(ECHO_PIN, HIGH);

  distanceCm = 0.017 * durationUs;
  displayDistance(distanceCm);

  if(autoMode == false){
    
  }
  else{
    updatePump(distanceCm);
  }
  return distanceCm;
}

void displayDistance(float distance) {
  lcd.setCursor(0, 0);
  lcd.print("distance in cm:");

  lcd.setCursor(0, 1);
  lcd.print(distance);
}

void sendDistance() {
  float distance = measureDistance();
  delay(1000);
   postToDb(distance);
  server.send(200, "text/plain", String(int(distance)));
}

void updatePump(float distance) {
  if(distance <= 15 && !emptyTank){
    emptyTank = true;
  }
  if(distance >= maxTankHeight && emptyTank) {
    emptyTank = false;
  }
  if(autoMode && emptyTank && distance < maxTankHeight){
    digitalWrite(redLed, HIGH);
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(redLed, LOW);
    digitalWrite(relayPin, LOW);
  }
}

String serverName = "http://192.168.118.167:8080/new-reading";

void postToDb(float distance){
  if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      http.addHeader("Content-Type", "application/json");
      String json = "{\"ownerName\":\"Isaac_and_Daniel\",\"waterLevel\":\" ";
      json += String(int(distance));
      json += "\"}";
      int httpResponseCode = http.POST(json);
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void turnMotorOn() {
  Serial.println("on");
  // take out redLED (only for debugging purposes only)
  if(!autoMode){
    digitalWrite(redLed, HIGH);
    digitalWrite(relayPin, HIGH);
    server.send(200, "text/plain", "Motor on");
  }
}

void turnMotorOff() {
  Serial.println("off");
  // take out redLED (only for debugging purposes only)
  if(!autoMode) {
    digitalWrite(redLed, LOW);
    digitalWrite(relayPin, LOW);
    server.send(200, "text/plain", "Motor off");
  }
}

void changeToManual() {
  Serial.println("manual");
  autoMode = false;
  server.send(200, "text/plain", "Manual mode");
}

void changeToAuto() {
  Serial.println("auto");
  autoMode = true;
  server.send(200, "text/plain", "Automatic mode");
}
