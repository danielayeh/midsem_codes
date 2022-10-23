#include <LiquidCrystal_I2C.h>
#define TRIG_PIN 26 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 25 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin
#include <WiFi.h>
#include <WebServer.h>

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

const char* ssid = "DUFIE-HOSTEL";
const char* password = "Duf1e@9723";

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
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

float measureDistance() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  durationUs = pulseIn(ECHO_PIN, HIGH);

  distanceCm = 0.017 * durationUs;
  displayDistance(distanceCm);

  updatePump(distanceCm);

  return distanceCm;
}

void displayDistance(float distance) {
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print distancep'
  lcd.print("distance in cm:");

  // set cursor to first column, second row
  lcd.setCursor(0, 1);
  lcd.print(distance);
}

void sendDistance() {
  float distance = measureDistance();
  server.send(200, "text/html", sendHTML(distance));
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

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

String sendHTML(float distance) {
  String ptr = "<!DOCTYPE html> <html>\n";

  ptr += "<script>\n";
  ptr += "setInterval(loadDoc,200);\n";
  ptr += "function loadDoc() {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "document.getElementById(\"webpage\").innerHTML =this.responseText}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";

  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Midsem project</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>MIDSEM WATER LEVEL PROJECT - Daniel and Isaac</h1>\n";

  ptr += "<p>Water level: ";
  ptr += (int)distance;
  ptr += "cm</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
