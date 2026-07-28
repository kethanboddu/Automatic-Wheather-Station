#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Create the BME280 object
Adafruit_BME280 bme; 

void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 Test"));

  // Initialize I2C with ESP32 pins (SDA=21, SCL=22)
  Wire.begin(21, 22);

  // Default address is 0x77. If that fails, the code tries 0x76.
  if (!bme.begin(0x76)) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1); // Halt
  }

  Serial.println(F("--- BME280 Ready ---"));
}

void loop() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.println("-----------------------");
  delay(2000);
}