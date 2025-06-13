/*
 * 传感器定义和函数声明
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <OneWire.h>
#include <DallasTemperature.h>

// 引脚定义
#define PH_SENSOR_PIN A1
#define TURBIDITY_PIN A5
#define CONDUCTIVITY_PIN A4
#define ONE_WIRE_BUS 2

// ADC参数
#define VREF 3.3
#define ADC_RESOLUTION 1024.0

// 温度传感器
extern OneWire oneWire;
extern DallasTemperature temperatureSensor;
extern bool temperatureSensorFound;

// 测量变量
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

#endif