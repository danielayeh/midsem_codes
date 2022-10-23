#include <LiquidCrystal_I2C.h>
#define TRIG_PIN 26 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 25 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin
#include <WiFi.h>
#include <WebServer.h>

float duration_us, distance_cm;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

const char* ssid = "DUFIE-HOSTEL";  // Enter SSID here
const char* password = "Duf1e@9723";  //Enter Password here

WebServer server(80);

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
   // begin serial port
  Serial.begin (115200);

  // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);
  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", sendDistance);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(){ 
  server.handleClient();
}

float measureDistance() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration_us = pulseIn(ECHO_PIN, HIGH);

  distance_cm = 0.017 * duration_us;
 
  return distance_cm;
}

void displayDistance(float distance) {
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print distancep'
  lcd.print("distance in cm:");


   // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print(distance_cm);
  delay(1000);
  lcd.clear(); 
}

void sendDistance(){
  float distance = measureDistance();
  server.send(200, "text/html", SendHTML(distance));
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float distance){
  String ptr = "<!DOCTYPE html> <html>\n";
  
  ptr +="<script>\n";
  ptr +="setInterval(loadDoc,200);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.getElementById(\"webpage\").innerHTML =this.responseText}\n";
  ptr +="};\n";
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
  ptr +="</script>\n";

  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP32 Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>MIDSEM WATER LEVEL PROJECT - Daniel and Isaac</h1>\n";
  
  ptr +="<p>Distance: ";
  ptr +=(int)distance;
  ptr +="cm</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
