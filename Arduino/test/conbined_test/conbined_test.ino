/*
 * ESP32 Water Quality Sensor Test - 简化版（仅显示数据）
 * 浊度传感器分压电路配置: 6.8kΩ + 6.8kΩ (1:1分压)
 * 传感器输出范围: 0-4.5V -> 分压后ESP32输入: 0-2.25V
 */

#include <OneWire.h>
#include <DallasTemperature.h>

// 引脚定义
#define PH_PIN 34
#define TDS_PIN 12
#define TURBIDITY_PIN 36
#define CONDUCTIVITY_PIN 39
#define TEMPERATURE_PIN 13

// 温度传感器设置
OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature tempSensor(&oneWire);

// ===== 浊度传感器分压电路配置 =====
#define DIVIDER_R1 6.8              // 上电阻值 (kΩ)
#define DIVIDER_R2 6.8              // 下电阻值 (kΩ)
const float VOLTAGE_DIVIDER_RATIO = DIVIDER_R2 / (DIVIDER_R1 + DIVIDER_R2); // = 0.5

// ===== 浊度传感器校准参数 =====
#define TURBIDITY_CLEAR_WATER_V 4.2    // 清水时传感器电压 (V) - 0 NTU
#define TURBIDITY_MAX_V 1.0            // 最高浊度时传感器电压 (V) - 对应1000 NTU
#define MAX_TURBIDITY_NTU 1000.0       // 最大测量浊度值

// ESP32 ADC配置
#define ADC_RESOLUTION 4095.0
#define ESP32_VREF 3.3                 // ESP32 ADC参考电压
#define TURBIDITY_SAMPLES 30           // 浊度测量采样次数

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // 初始化温度传感器
  tempSensor.begin();
  
  // 配置ADC
  analogReadResolution(12);           // 12位分辨率
  analogSetAttenuation(ADC_11db);     // 最大输入约3.3V
  
  Serial.println("ESP32 水质传感器数据显示");
  Serial.println("========================");
  Serial.println("浊度传感器分压电路: 6.8kΩ + 6.8kΩ");
  Serial.print("分压比例: ");
  Serial.println(VOLTAGE_DIVIDER_RATIO, 3);
  Serial.println("========================");
  
  delay(2000);
  Serial.println("开始测量...\n");
}

void loop() {
  Serial.println("--- 传感器数据 ---");
  
  // 读取温度
  tempSensor.requestTemperatures();
  float temperature = tempSensor.getTempCByIndex(0);
  if(temperature != DEVICE_DISCONNECTED_C) {
    Serial.print("温度: ");
    Serial.print(temperature, 2);
    Serial.println(" °C");
  } else {
    Serial.println("温度: 传感器未连接");
    temperature = 25.0; // 默认值用于TDS计算
  }
  
  // 读取pH
  float pH = readPH();
  Serial.print("pH值: ");
  Serial.println(pH, 2);
  
  // 读取TDS
  float tds = readTDS(temperature);
  Serial.print("TDS: ");
  Serial.print(tds, 0);
  Serial.println(" ppm");
  
  // 读取浊度
  float turbidity = readTurbidity();
  Serial.print("浊度: ");
  Serial.print(turbidity, 2);
  Serial.println(" NTU");
  
  // 读取电导率
  float conductivity = readConductivity();
  Serial.print("电导率: ");
  Serial.print(conductivity, 2);
  Serial.println(" μS/cm");
  
  // 显示浊度传感器详细信息
  displayTurbidityRawData();
  
  Serial.println("------------------");
  delay(3000);
}

// ===== 浊度传感器读取函数 =====
float readTurbidity() {
  // 多次采样求平均
  long adcSum = 0;
  for(int i = 0; i < TURBIDITY_SAMPLES; i++) {
    adcSum += analogRead(TURBIDITY_PIN);
    delay(20);
  }
  
  float avgADC = adcSum / (float)TURBIDITY_SAMPLES;
  
  // 将ADC值转换为ESP32输入电压
  float esp32InputVoltage = (avgADC / ADC_RESOLUTION) * ESP32_VREF;
  
  // 计算传感器实际输出电压 (逆向分压计算)
  float sensorOutputVoltage = esp32InputVoltage / VOLTAGE_DIVIDER_RATIO;
  
  // 电压范围保护
  sensorOutputVoltage = constrain(sensorOutputVoltage, 0, 4.5);
  
  // 转换为浊度值
  float turbidityNTU = convertVoltageToTurbidity(sensorOutputVoltage);
  
  return turbidityNTU;
}

// ===== 电压到浊度转换函数 =====
float convertVoltageToTurbidity(float voltage) {
  float turbidityNTU;
  
  if(voltage >= TURBIDITY_CLEAR_WATER_V) {
    turbidityNTU = 0.0;
  }
  else if(voltage >= 3.5) {
    // 极低浊度范围 (3.5V - 4.2V) -> 0-10 NTU
    turbidityNTU = (TURBIDITY_CLEAR_WATER_V - voltage) / (TURBIDITY_CLEAR_WATER_V - 3.5) * 10.0;
  }
  else if(voltage >= 2.5) {
    // 低浊度范围 (2.5V - 3.5V) -> 10-100 NTU
    turbidityNTU = 10.0 + (3.5 - voltage) / (3.5 - 2.5) * 90.0;
  }
  else if(voltage >= 1.5) {
    // 中浊度范围 (1.5V - 2.5V) -> 100-500 NTU
    turbidityNTU = 100.0 + (2.5 - voltage) / (2.5 - 1.5) * 400.0;
  }
  else if(voltage >= TURBIDITY_MAX_V) {
    // 高浊度范围 (1.0V - 1.5V) -> 500-1000 NTU
    turbidityNTU = 500.0 + (1.5 - voltage) / (1.5 - TURBIDITY_MAX_V) * 500.0;
  }
  else {
    turbidityNTU = MAX_TURBIDITY_NTU;
  }
  
  return turbidityNTU;
}

// ===== 显示浊度原始数据 =====
void displayTurbidityRawData() {
  int rawADC = analogRead(TURBIDITY_PIN);
  float esp32Voltage = (rawADC / ADC_RESOLUTION) * ESP32_VREF;
  float sensorVoltage = esp32Voltage / VOLTAGE_DIVIDER_RATIO;
  
  Serial.println("浊度传感器原始数据:");
  Serial.print("  ADC: ");
  Serial.println(rawADC);
  Serial.print("  ESP32输入: ");
  Serial.print(esp32Voltage, 3);
  Serial.println(" V");
  Serial.print("  传感器输出: ");
  Serial.print(sensorVoltage, 3);
  Serial.println(" V");
}

// ===== pH传感器读取 =====
float readPH() {
  int phRaw = analogRead(PH_PIN);
  float phVoltage = phRaw * (ESP32_VREF / ADC_RESOLUTION);
  float pH = 12.020 * phVoltage - 15.287; // 校准公式
  return constrain(pH, 0, 14);
}

// ===== TDS传感器读取 =====
float readTDS(float temperature) {
  int tdsRaw = analogRead(TDS_PIN);
  float tdsVoltage = tdsRaw * (ESP32_VREF / ADC_RESOLUTION);
  
  // 温度补偿
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensatedVoltage = tdsVoltage / compensationCoefficient;
  
  // TDS计算公式
  float tds = (133.42 * pow(compensatedVoltage, 3) 
              - 255.86 * pow(compensatedVoltage, 2) 
              + 857.39 * compensatedVoltage) * 0.5;
  
  return (tds > 0) ? tds : 0;
}

// ===== 电导率传感器读取 =====
float readConductivity() {
  int conductivityRaw = analogRead(CONDUCTIVITY_PIN);
  float conductivityVoltage = conductivityRaw * (ESP32_VREF / ADC_RESOLUTION);
  return conductivityVoltage * 1000; // 简单转换
}