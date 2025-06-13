/*
 * 传感器函数实现
 */

#include "sensors.h"
#include <Arduino.h>

// 全局变量定义
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temperatureSensor(&oneWire);
bool temperatureSensorFound = false;

float waterTemperature = 25.0;
float tdsValue = 0;
float pHValue = 0;
float turbidityNTU = 0;
float conductivityValue = 0;

// pH校准参数
const float pH4_Voltage = 1.73;
const float pH7_Voltage = 1.98;
const float pH10_Voltage = 2.21;
const float pH_m = (7.0 - 4.0) / (pH7_Voltage - pH4_Voltage);
const float pH_b = 7.0 - pH_m * pH7_Voltage;

// 电导率参数
const float sensorMaxV = 2.3;
const float maxConductivity = 2000.0;

void initializeSensors() {
  // 初始化引脚
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(TURBIDITY_PIN, INPUT);
  pinMode(CONDUCTIVITY_PIN, INPUT);
  
  // 初始化温度传感器
  temperatureSensor.begin();
  int deviceCount = temperatureSensor.getDeviceCount();
  
  if (deviceCount > 0) {
    temperatureSensorFound = true;
    temperatureSensor.setResolution(12);
    Serial.println("DS18B20温度传感器检测成功!");
  } else {
    temperatureSensorFound = false;
    Serial.println("未检测到DS18B20，将使用默认温度25℃");
  }
}

void readAllSensors() {
  // 读取所有传感器数据
  readTemperature();
  readPH();
  readTurbidity();
  readConductivity();
  calculateTDSFromConductivity();
}

void readTemperature() {
  if (temperatureSensorFound) {
    temperatureSensor.requestTemperatures();
    float tempC = temperatureSensor.getTempCByIndex(0);
    
    if (tempC != DEVICE_DISCONNECTED_C && tempC > -50 && tempC < 100) {
      waterTemperature = tempC;
    }
  }
}

void readPH() {
  int sensorValue = analogRead(PH_SENSOR_PIN);
  float pH_Voltage = sensorValue * (VREF / 1024.0);
  
  // 使用线性插值
  if (pH_Voltage >= pH7_Voltage) {
    float m2 = (10.01 - 7.0) / (pH10_Voltage - pH7_Voltage);
    float b2 = 7.0 - m2 * pH7_Voltage;
    pHValue = m2 * pH_Voltage + b2;
  } else {
    pHValue = pH_m * pH_Voltage + pH_b;
  }
  
  if (pHValue < 0) pHValue = 0;
  if (pHValue > 14) pHValue = 14;
}

void readTurbidity() {
  int sensorValue = analogRead(TURBIDITY_PIN);
  float voltageA5 = sensorValue * (VREF / 1023.0);
  float sensorVoltage = voltageA5 * 2.0;
  
  turbidityNTU = (4.25 - sensorVoltage) * (1000.0 / 4.25);
  
  if (turbidityNTU < 0) turbidityNTU = 0;
  if (turbidityNTU > 1000) turbidityNTU = 1000;
}

void readConductivity() {
  int raw = analogRead(CONDUCTIVITY_PIN);
  float voltage = raw * VREF / 1023.0;
  
  conductivityValue = (voltage / sensorMaxV) * maxConductivity;
}

void calculateTDSFromConductivity() {
  // 使用电导率计算TDS: TDS (ppm) ≈ 电导率 (μS/cm) × 0.5
  tdsValue = conductivityValue * 0.5;
}

void printAllReadings() {
  Serial.println("=== 水质监测参数 ===");
  
  Serial.print("温度: ");
  Serial.print(waterTemperature, 2);
  Serial.println("℃");
  
  Serial.print("电导率: ");
  Serial.print(conductivityValue, 1);
  Serial.println(" μS/cm");
  
  Serial.print("TDS: ");
  Serial.print(tdsValue, 1);
  Serial.println(" ppm (通过电导率计算)");
  
  Serial.print("pH: ");
  Serial.println(pHValue, 2);
  
  Serial.print("浊度: ");
  Serial.print(turbidityNTU, 2);
  Serial.println(" NTU");
  
  Serial.println("========================================");
}