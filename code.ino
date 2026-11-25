// 1. Replace with your actual Blynk IoT credentials (no leading spaces)
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Transformer Monitor"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Wi-Fi Credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Pin Definitions (Matched to README.md)
#define TEMP_SENSOR_PIN 4
#define LIQUID_SENSOR_PIN 34
#define TRIG_PIN 5
#define ECHO_PIN 18
#define TOGGLE_SWITCH_PIN 19
#define RELAY_PIN 23

// Relay Logic (Change to HIGH if your relay module is Active-HIGH)
#define RELAY_TRIP_STATE LOW
#define RELAY_NORMAL_STATE HIGH

// Alert Thresholds
const float TEMPERATURE_ALERT_C = 70.0;
const float MIN_OIL_LEVEL_PERCENT = 30.0;
const float OBSTACLE_DISTANCE_CM = 10.0;

// Sensor Objects & Timer
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

// Global Monitoring Variables
float currentTempC = 0.0;
float oilLevelPercent = 0.0;
float distanceCm = 0.0;
bool protectionTripped = false;
String alertMessage = "NORMAL";

float readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 30ms timeout (~5 meters max) prevents code from hanging if disconnected
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1.0; // Out of range / sensor timeout
  return (duration * 0.0343) / 2.0;
}

void monitorSystem() {
  // 1. Read Temperature (DS18B20)
  sensors.requestTemperatures();
  float tempReading = sensors.getTempCByIndex(0);
  if (tempReading != DEVICE_DISCONNECTED_C) {
    currentTempC = tempReading;
  }

  // 2. Read Oil/Liquid Level (12-bit ADC: 0 - 4095)
  int rawLiquid = analogRead(LIQUID_SENSOR_PIN);
  // Calibrate 0 and 4095 to your sensor's actual empty/full ADC values
  oilLevelPercent = constrain(map(rawLiquid, 0, 4095, 0, 100), 0, 100);

  // 3. Read Ultrasonic Distance
  float distReading = readUltrasonicDistance();
  if (distReading > 0) {
    distanceCm = distReading;
  }

  // 4. Read Toggle Switch (LOW = Switch Closed / Short Protection Triggered)
  bool manualShortSwitch = (digitalRead(TOGGLE_SWITCH_PIN) == LOW);

  // 5. Evaluate Local Fail-Safe Protection Logic
  if (manualShortSwitch) {
    protectionTripped = true;
    alertMessage = "SHORT / MANUAL TRIP";
  } else if (currentTempC >= TEMPERATURE_ALERT_C) {
    protectionTripped = true;
    alertMessage = "HIGH TEMP ALERT";
  } else if (oilLevelPercent <= MIN_OIL_LEVEL_PERCENT) {
    protectionTripped = true;
    alertMessage = "LOW OIL LEVEL";
  } else if (distanceCm > 0 && distanceCm <= OBSTACLE_DISTANCE_CM) {
    protectionTripped = true;
    alertMessage = "PROXIMITY BREACH";
  } else {
    protectionTripped = false;
    alertMessage = "NORMAL";
  }

  // Actuate Relay Immediately (Works Offline)
  digitalWrite(RELAY_PIN, protectionTripped ? RELAY_TRIP_STATE : RELAY_NORMAL_STATE);

  // Debug Output to Serial Monitor
  Serial.printf("Temp: %.1f C | Oil: %.1f%% | Dist: %.1f cm | Status: %s\n",
                currentTempC, oilLevelPercent, distanceCm, alertMessage.c_str());

  // 6. Push Data to Blynk Virtual Pins (V0 - V4) if Connected
  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, currentTempC);
    Blynk.virtualWrite(V1, oilLevelPercent);
    Blynk.virtualWrite(V2, distanceCm);
    Blynk.virtualWrite(V3, protectionTripped ? 1 : 0);
    Blynk.virtualWrite(V4, alertMessage);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TOGGLE_SWITCH_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);

  // Keep relay in normal state on boot
  digitalWrite(RELAY_PIN, RELAY_NORMAL_STATE);

  sensors.begin();

  // Connect to Wi-Fi and Blynk (Non-blocking check)
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Blynk.config(BLYNK_AUTH_TOKEN);

  // Run sensor & protection loop every 1000ms (1 second)
  timer.setInterval(1000L, monitorSystem);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }
  timer.run();
}
