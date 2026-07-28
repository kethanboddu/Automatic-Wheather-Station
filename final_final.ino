#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "DFRobot_RainfallSensor.h"
#include "RTClib.h"

// ==================== OBJECTS ====================
Adafruit_BME280 bme;
DFRobot_RainfallSensor_I2C rainSensor(&Wire);
RTC_DS3231 rtc;

// ==================== PIN DEFINITIONS ====================
const int LDR_PIN = A0;
const int SOIL_PIN = A1;
const int WIND_PIN = 2;

// ==================== CONSTANTS ====================
const int DAY_NIGHT_THRESHOLD = 400;
const float CALIBRATION_FACTOR = 0.45;   // Wind speed
const int MEASURE_TIME_MS = 1000;

// ==================== GLOBAL VARIABLES ====================
volatile int pulseCount = 0;

// ==================== INTERRUPT ====================
void countPulse() {
  pulseCount++;
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(9600);
  Wire.begin();

  // ---- BME280 ----
  if (!bme.begin(0x76)) {
    Serial.println("❌ BME280 not detected!");
    while (1);
  }

  // ---- Rain Sensor ----
  if (!rainSensor.begin()) {
    Serial.println("❌ Rain sensor not detected!");
    while (1);
  }

  // ---- RTC ----
  if (!rtc.begin()) {
    Serial.println("❌ RTC not detected!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("⚠ RTC lost power, resetting time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // ---- Wind Sensor ----
  pinMode(WIND_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WIND_PIN), countPulse, FALLING);

  Serial.println("\n✅ Weather Monitoring System Started\n");
}

// ==================== LOOP ====================
void loop() {

  // -------- RTC --------
  DateTime now = rtc.now();

  // -------- BME280 --------
  float temperature = bme.readTemperature();
  float humidity    = bme.readHumidity();
  float pressure    = bme.readPressure() / 100.0;

  // -------- Rain Sensor --------
  float totalRain = rainSensor.getRainfall();
  float hourlyRain = rainSensor.getRainfall(1);

  // -------- Wind Speed --------
  pulseCount = 0;
  delay(MEASURE_TIME_MS);
  float frequency = pulseCount / (MEASURE_TIME_MS / 1000.0);
  float windSpeed = frequency * CALIBRATION_FACTOR;

  // -------- LDR --------
  int ldrValue = analogRead(LDR_PIN);
  int brightness = map(ldrValue, 0, 1023, 0, 100);
  String dayStatus = (ldrValue > DAY_NIGHT_THRESHOLD) ? "Night" : "Day";

  // -------- Soil Moisture --------
  int soilValue = analogRead(SOIL_PIN);
  String soilStatus;
  if (soilValue > 700) soilStatus = "Dry";
  else if (soilValue > 400) soilStatus = "Moist";
  else soilStatus = "Wet";

  // ================== OUTPUT ==================
  Serial.println("==========================================");
  Serial.print("📅 Date: ");
  Serial.print(now.day()); Serial.print("/");
  Serial.print(now.month()); Serial.print("/");
  Serial.print(now.year());

  Serial.print("  ⏰ Time: ");
  Serial.print(now.hour()); Serial.print(":");
  Serial.print(now.minute()); Serial.print(":");
  Serial.println(now.second());

  Serial.print("🌡 Temp: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("💧 Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("🌬 Pressure: "); Serial.print(pressure); Serial.println(" hPa");

  Serial.print("☔ Rainfall: "); Serial.print(totalRain); Serial.println(" mm");
  Serial.print("💨 Wind Speed: "); Serial.print(windSpeed, 2); Serial.println(" m/s");

  Serial.print("🔆 LDR: "); Serial.print(ldrValue);
  Serial.print(" | Light: "); Serial.println(dayStatus);

  Serial.print("🌱 Soil Moisture: "); 
  Serial.print(soilValue);
  Serial.print(" → ");
  Serial.println(soilStatus);

  Serial.println("==========================================\n");

  delay(2000);
}