#include <LiquidCrystal_I2C.h>
#define TRIG_PIN 26 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 25 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin

float duration_us, distance_cm;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
   // begin serial port
  Serial.begin (9600);

  // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);
  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);
}

void loop(){

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

   duration_us = pulseIn(ECHO_PIN, HIGH);

   distance_cm = 0.017 * duration_us;
 
 
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
