#include <LiquidCrystal_I2C.h>
#define TRIG_PIN 26 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 25 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin
#include <WiFi.h>

#include <HTTPClient.h> 


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in 60 seconds) */

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

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setup() {
  lcd.init();
  lcd.noBacklight();
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
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

  for(int i=0;i<10;i++){
    float distance = measureDistance();
    postToDb(distance);
    delay(12000);
  }

  Serial.println("Going to sleep now");
  Serial.flush(); 
  esp_deep_sleep_start();

}

void loop() {
    
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
//  delay(1000);
  postToDb(distance);
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
