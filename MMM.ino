
// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLggkXKEaZ"
#define BLYNK_DEVICE_NAME "GassOf"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7
//#define USE_ESP32C3_DEV_MODULE
//#define USE_ESP32S2_DEV_KIT

#include "BlynkEdgent.h"
#include <NewPing.h>
#include <DHT.h>

#define TRIGGER_PIN 22
#define ECHO_PIN 23
#define MAX_DISTANCE 200
#define RELAY 15
#define MOVE 2
#define DHTPIN 32
#define DHTTYPE DHT11
#define FLUID 5
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long historyMills;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
DHT dht(DHTPIN, DHTTYPE);

void IRAM_ATTR pulseCounter()
{
pulseCount++;
}

void setup()
{
   pinMode(RELAY, OUTPUT);
   pinMode(MOVE , INPUT);
   pinMode(FLUID, INPUT_PULLUP);
   digitalWrite(RELAY, HIGH);
   Serial.begin(115200);
   dht.begin();
   pulseCount = 0;
   flowRate = 0.0;
   flowMilliLitres = 0;
   totalMilliLitres = 0;
   previousMillis = 0;
   historyMills=0;

   attachInterrupt(digitalPinToInterrupt(FLUID), pulseCounter, FALLING);
    
   BlynkEdgent.begin();
}

void loop() {
  BlynkEdgent.run();
  delay(1000);
   currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    historyMills += flowMilliLitres;
  }
   int movement = digitalRead(MOVE);
   float t = dht.readTemperature();
   currentMillis = millis();
   if (isnan(t)){
    Serial.println("Failed to read from DHT sensor!");
   }
   Serial.print("Temperature: ");
   Serial.print(t);
   Serial.println("°C ");
   Blynk.virtualWrite(V3, t);
   Serial.print("Movimiento: ");
   Serial.println(movement);
   Serial.print("GAS: ");
   Serial.print(totalMilliLitres );
   Serial.print(" ml/");
   Serial.print(totalMilliLitres / 1000);
   Serial.println(" L");
   Blynk.virtualWrite(V2, totalMilliLitres/ 1000);
   Serial.print("GAS history: ");
   Serial.print(historyMills / 1000);
   Serial.println(" L");
   Blynk.virtualWrite(V4, historyMills / 1000);
   if(t>29){
    digitalWrite(RELAY, LOW);
    Blynk.virtualWrite(V0, 0);
   }
   if(movement==HIGH){
    digitalWrite(RELAY, LOW);
    Blynk.virtualWrite(V5, 1);
    Blynk.virtualWrite(V0, 0);
   }
   if((totalMilliLitres) >  5000){
    Serial.print("entro");
    Blynk.logEvent("gass_almost_finished");
   }
}

BLYNK_WRITE(V0){
  int state = param.asInt();
  digitalWrite(RELAY, state);
  if (state == 1){
    Blynk.virtualWrite(V5, 0);
  }
}

BLYNK_WRITE(V1){
  int state1 = param.asInt();
  if(state1 == 1){
    pulseCount = 0;
   flowRate = 0.0;
   flowMilliLitres = 0;
   totalMilliLitres = 0;
   previousMillis = 0;
   Blynk.virtualWrite(V1, 0);
  }
} 
