/**
 * Sensors.cpp - 传感器读取模块
 * 
 * 包含所有传感器的初始化、读取和数据处理功能
 * 包含LED水质指示功能
 */

#include "WaterMonitor.h"
#include "WaterQualityLED.h"  // 添加LED头文件

// ==================== 全局变量定义 ====================
// 温度传感器对象
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temperatureSensor(&oneWire);
bool temperatureSensorFound = false;

// 传感器数据变量
float waterTemperature = 25.0;
float tdsValue = 0;
float pHValue = 0;
float turbidityNTU = 0;
float conductivityValue = 0;

// pH校准参数（计算得出）
float pH_m = (7.0 - 4.0) / (PH7_VOLTAGE - PH4_VOLTAGE);
float pH_b = 7.0 - pH_m * PH7_VOLTAGE;

// ==================== 传感器初始化 ====================
void initializeSensors() {
  Serial.println("正在初始化传感器...");
  
  // 初始化随机数种子
  randomSeed(analogRead(A0) + millis());
  
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
    Serial.println("✓ DS18B20温度传感器检测成功!");
  } else {
    temperatureSensorFound = false;
    Serial.println("⚠ 未检测到DS18B20，将使用默认温度25℃");
  }
  
  // 初始化LED指示系统
  initializeLEDs();
  
  Serial.println("传感器初始化完成");
}

// ==================== 传感器读取函数 ====================
void readAllSensors() {
  Serial.println("正在读取所有传感器数据...");
  
  readTemperature();
  readPH();
  readTurbidity();
  readConductivity();
  calculateTDSFromConductivity();
  
  // 显示读取到的值
  Serial.println("=== 传感器读数 ===");
  Serial.print("pH: "); Serial.println(pHValue, 2);
  Serial.print("浊度: "); Serial.print(turbidityNTU, 1); Serial.println(" NTU");
  Serial.print("TDS: "); Serial.print(tdsValue, 0); Serial.println(" ppm");
  Serial.print("电导率: "); Serial.print(conductivityValue, 0); Serial.println(" μS/cm");
  
  // 直接评估水质并更新LED显示
  Serial.println("开始LED评估...");
  int ledStatus = evaluateWaterQuality(pHValue, turbidityNTU, tdsValue, conductivityValue);
  Serial.print("LED评估结果: ");
  Serial.println(ledStatus);
  
  setLEDStatus(ledStatus);
  Serial.println("LED状态已设置");
  
  Serial.println("传感器数据读取完成");
}

void readTemperature() {
  if (temperatureSensorFound) {
    temperatureSensor.requestTemperatures();
    float tempC = temperatureSensor.getTempCByIndex(0);
    
    // 验证温度数据有效性
    if (tempC != DEVICE_DISCONNECTED_C && tempC > -50 && tempC < 100) {
      waterTemperature = tempC;
    } else {
      Serial.println("⚠ 温度传感器读取异常，使用上次数值");
    }
  }
  // 如果没有传感器，保持默认值25.0℃
}

void readPH() {
  int sensorValue = analogRead(PH_SENSOR_PIN);
  float pH_Voltage = sensorValue * (VREF / ADC_RESOLUTION);
  
  // 使用线性插值计算pH值
  if (pH_Voltage >= PH7_VOLTAGE) {
    // pH 7-10 范围
    float m2 = (10.01 - 7.0) / (PH10_VOLTAGE - PH7_VOLTAGE);
    float b2 = 7.0 - m2 * PH7_VOLTAGE;
    pHValue = m2 * pH_Voltage + b2;
  } else {
    // pH 4-7 范围
    pHValue = pH_m * pH_Voltage + pH_b;
  }
  
  // 限制pH值范围
  if (pHValue < 0) pHValue = 0;
  if (pHValue > 14) pHValue = 14;
}

void readTurbidity() {
  // 生成0.1-1.0之间的随机浊度值（保留一位小数）
  float randomValue = random(1, 11) / 10.0;  // 生成0.1到1.0
  turbidityNTU = randomValue;
  
  // 调试输出
  Serial.print("随机浊度值: ");
  Serial.print(turbidityNTU, 1);
  Serial.println(" NTU");
}

void readConductivity() {
  int raw = analogRead(CONDUCTIVITY_PIN);
  float voltage = raw * VREF / 1023.0;
  
  // 计算电导率值
  conductivityValue = (voltage / SENSOR_MAX_V) * MAX_CONDUCTIVITY;
  
  // 确保电导率值为正
  if (conductivityValue < 0) conductivityValue = 0;
}

void calculateTDSFromConductivity() {
  // 使用经验公式：TDS (ppm) ≈ 电导率 (μS/cm) × 0.5
  // 考虑温度补偿（简化版本）
  float tempCompensation = 1.0 + 0.02 * (waterTemperature - 25.0);
  tdsValue = (conductivityValue * 0.5) / tempCompensation;
  
  // 确保TDS值为正
  if (tdsValue < 0) tdsValue = 0;
}

// ==================== 数据输出函数 ====================
void printAllReadings() {
  Serial.println("\n=== 水质监测参数 ===");
  Serial.print("系统时间: ");
  Serial.print(millis() / 1000);
  Serial.print(" 秒 (");
  Serial.print(millis() / 60000);
  Serial.println(" 分钟)");
  
  Serial.print("温度: ");
  Serial.print(waterTemperature, 2);
  Serial.print("℃");
  if (!temperatureSensorFound) {
    Serial.print(" (默认值)");
  }
  Serial.println();
  
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
  
  Serial.print("水质状态: ");
  Serial.println(getWaterQualityStatus());
  
  Serial.println("====================================");
}

// ==================== 水质状态评估 ====================
String getWaterQualityStatus() {
  // 使用LED评估系统获取水质状态
  int ledStatus = evaluateWaterQuality(pHValue, turbidityNTU, tdsValue, conductivityValue);
  return getWaterQualityDescription(ledStatus);
}