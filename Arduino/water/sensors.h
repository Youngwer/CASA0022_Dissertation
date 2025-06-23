/*
 * 传感器定义和函数声明 - MKR WAN1310版本
 * 适配Arduino MKR WAN1310开发板
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <OneWire.h>
#include <DallasTemperature.h>

// MKR WAN1310引脚定义
#define PH_SENSOR_PIN A1        // pH传感器 - 模拟引脚A1
#define TURBIDITY_PIN A2        // 浊度传感器 - 模拟引脚A2  
#define CONDUCTIVITY_PIN A3     // 电导率传感器 - 模拟引脚A3
#define ONE_WIRE_BUS 2          // DS18B20温度传感器 - 数字引脚2

// E-Paper SPI连接引脚（MKR WAN1310默认SPI引脚）
// MOSI: 8, MISO: 10, SCK: 9
#define EPD_CS_PIN 7            // E-Paper CS引脚
#define EPD_DC_PIN 6            // E-Paper DC引脚
#define EPD_RST_PIN 5           // E-Paper RST引脚
#define EPD_BUSY_PIN 4          // E-Paper BUSY引脚

// ADC参数（MKR WAN1310使用3.3V）
#define VREF 3.3
#define ADC_RESOLUTION 1024.0

// 温度传感器外部对象声明
extern OneWire oneWire;
extern DallasTemperature temperatureSensor;
extern bool temperatureSensorFound;

// 测量变量（供显示使用）
extern float waterTemperature;
extern float tdsValue;
extern float pHValue;
extern float turbidityNTU;
extern float conductivityValue;

// pH校准参数
extern const float pH4_Voltage;
extern const float pH7_Voltage;
extern const float pH10_Voltage;
extern const float pH_m;
extern const float pH_b;

// 电导率参数
extern const float sensorMaxV;
extern const float maxConductivity;

// 函数声明
void initializeSensors();
void readAllSensors();
void readTemperature();
void readPH();
void readTurbidity();
void readConductivity();
void calculateTDSFromConductivity();
void printAllReadings();

// E-Paper相关函数声明
void updateDisplay();
String getWaterQualityStatus();

// 电源管理相关
void enterSleepMode();
void wakeUpFromSleep();

#endif