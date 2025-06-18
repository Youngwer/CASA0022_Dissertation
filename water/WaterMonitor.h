/**
 * WaterMonitor.h - 水质监测系统主程序头文件
 * 
 * 整合所有模块的声明和全局变量定义
 * 适配 MKR WAN1310 + E-Paper显示
 */

#ifndef WATER_MONITOR_H
#define WATER_MONITOR_H

#include <Arduino.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "epd2in9_V2.h"
#include "epdpaint.h"

// ==================== 引脚定义 ====================
// 传感器引脚定义（适配MKR WAN1310）
#define PH_SENSOR_PIN A1
#define TURBIDITY_PIN A2
#define CONDUCTIVITY_PIN A3
#define ONE_WIRE_BUS 2
#define BUTTON_PIN 3

// E-Paper显示定义
#define COLORED     0
#define UNCOLORED   1

// ==================== 系统参数 ====================
// ADC参数
#define VREF 3.3
#define ADC_RESOLUTION 1024.0

// 按钮控制参数
#define BUTTON_COOLDOWN 10000  // 10秒冷却时间

// pH校准参数
#define PH4_VOLTAGE 1.73
#define PH7_VOLTAGE 1.98
#define PH10_VOLTAGE 2.21

// 电导率参数
#define SENSOR_MAX_V 2.3
#define MAX_CONDUCTIVITY 2000.0

// ==================== 全局变量声明 ====================
// 传感器对象
extern OneWire oneWire;
extern DallasTemperature temperatureSensor;
extern bool temperatureSensorFound;

// 传感器数据变量
extern float waterTemperature;
extern float tdsValue;
extern float pHValue;
extern float turbidityNTU;
extern float conductivityValue;

// pH校准参数（计算得出）
extern float pH_m;
extern float pH_b;

// E-Paper对象
extern unsigned char image[1024];
extern Paint paint;
extern Epd epd;

// 按钮控制变量
extern bool lastButtonState;
extern bool currentButtonState;
extern bool buttonPressed;
extern unsigned long lastButtonPress;
extern bool systemReady;

// ==================== 函数声明 ====================
// 系统初始化
void initializeSystem();
void initializeSensors();
void initializeEPaper();
void initializeButton();

// 传感器读取模块
void readAllSensors();
void readTemperature();
void readPH();
void readTurbidity();
void readConductivity();
void calculateTDSFromConductivity();
void printAllReadings();

// 显示模块
void showStartupScreen();
void updateWaterQualityDisplay();
void displaySensorData();
String getWaterQualityStatus();

// 按钮控制模块
void handleButtonInput();
bool isButtonPressed();
bool isCooldownPeriod();
void handleCooldownMessage();
void setSystemReady(bool ready);
bool isSystemReady();
unsigned long getRemainingCooldown();
String getButtonStatus();
void printButtonDebugInfo();

// 显示模块扩展函数
void displayError(const char* errorMsg);
void displayProgress(const char* progressMsg);

// 主程序流程
void performWaterQualityTest();
void printSystemStatus();
void handleSystemError(const char* errorMsg);
void runDiagnostics();
int getFreeMemory();
void handleSerialCommands();

#endif // WATER_MONITOR_H