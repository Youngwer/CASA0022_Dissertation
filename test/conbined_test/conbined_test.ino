/*
 * Simple ESP32 Water Quality Sensor Test
 * Basic sensor reading for water quality parameters
 */

#include <OneWire.h>
#include <DallasTemperature.h>

// Pin definitions
#define PH_PIN 34
#define TDS_PIN 12
#define TURBIDITY_PIN 36
#define CONDUCTIVITY_PIN 39
#define TEMPERATURE_PIN 13

// Temperature sensor setup
OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature tempSensor(&oneWire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize temperature sensor
  tempSensor.begin();
  
  Serial.println("ESP32 Water Quality Sensor Test");
  Serial.println("================================");
  delay(2000);
}

void loop() {
  Serial.println("\n--- Water Quality Readings ---");
  
  // Read Temperature
  tempSensor.requestTemperatures();
  float temperature = tempSensor.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  // Read pH
  int phRaw = analogRead(PH_PIN);
  float phVoltage = phRaw * (3.3 / 4095.0);
  float pH = 12.020 * phVoltage - 15.287;
  pH = constrain(pH, 0, 14);
  Serial.print("pH: ");
  Serial.println(pH, 2);
  
  // Read TDS
  int tdsRaw = analogRead(TDS_PIN);
  float tdsVoltage = tdsRaw * (3.3 / 4095.0);
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensatedVoltage = tdsVoltage / compensationCoefficient;
  float tds = (133.42 * compensatedVoltage * compensatedVoltage * compensatedVoltage 
              - 255.86 * compensatedVoltage * compensatedVoltage 
              + 857.39 * compensatedVoltage) * 0.5;
  Serial.print("TDS: ");
  Serial.print(tds, 0);
  Serial.println(" ppm");
  
  // Read Turbidity
  int turbidityRaw = analogRead(TURBIDITY_PIN);
  float turbidityVoltage = (turbidityRaw / 4095.0) * 3.3;
  float turbidity = (2.2 - turbidityVoltage) / (2.2 - 1.0) * 1000;
  turbidity = constrain(turbidity, 0, 1000);
  Serial.print("Turbidity: ");
  Serial.print(turbidity, 2);
  Serial.println(" NTU");
  
  // Read Conductivity
  int conductivityRaw = analogRead(CONDUCTIVITY_PIN);
  float conductivityVoltage = conductivityRaw * (3.3 / 4095.0);
  float conductivity = conductivityVoltage * 1000; // Simple conversion
  Serial.print("Conductivity: ");
  Serial.print(conductivity, 2);
  Serial.println(" μS/cm");
  
  Serial.println("------------------------------");
  
  delay(3000); // Read every 3 seconds
}