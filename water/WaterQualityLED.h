/**
 * WaterQualityLED.h - 水质LED指示模块头文件
 * 
 * 处理基于水质参数的三色LED指示功能
 * 适配MKR WAN1310开发板
 */

#ifndef WATER_QUALITY_LED_H
#define WATER_QUALITY_LED_H

#include <Arduino.h>

// ==================== LED引脚定义 ====================
#define RED_LED_PIN     0   // D0引脚 - 红色LED (不适合饮用)
#define GREEN_LED_PIN   1   // D1引脚 - 绿色LED (优秀，适合饮用)
#define YELLOW_LED_PIN  10  // D10引脚 - 黄色LED (一般)

// ==================== LED状态定义 ====================
#define LED_OFF         0
#define GREEN_LED       1   // 优秀 - 适合饮用
#define YELLOW_LED      2   // 一般 - 勉强可接受
#define RED_LED         3   // 不安全 - 不适合饮用

// ==================== 水质标准参数 ====================
// pH标准
#define PH_EXCELLENT_MIN    6.5
#define PH_EXCELLENT_MAX    8.0
#define PH_ACCEPTABLE_MIN   6.0
#define PH_ACCEPTABLE_MAX   9.0

// 浊度标准 (NTU)
#define TURBIDITY_EXCELLENT_MAX     1.0
#define TURBIDITY_ACCEPTABLE_MAX    4.0

// TDS标准 (ppm)
#define TDS_EXCELLENT_MIN       80
#define TDS_EXCELLENT_MAX       300
#define TDS_ACCEPTABLE_MIN      50
#define TDS_ACCEPTABLE_MAX      500

// 电导率标准 (µS/cm)
#define EC_EXCELLENT_MIN        100
#define EC_EXCELLENT_MAX        400
#define EC_ACCEPTABLE_MIN       50
#define EC_ACCEPTABLE_MAX       800

// ==================== 函数声明 ====================
// LED初始化和控制
void initializeLEDs();
void setLEDStatus(int ledStatus);
void turnOffAllLEDs();

// 水质评估
int evaluateWaterQuality(float pH, float turbidity, float tds, float ec);
String getWaterQualityDescription(int ledStatus);

// 参数检查函数
bool isExcellentpH(float pH);
bool isAcceptablepH(float pH);
bool isExcellentTurbidity(float turbidity);
bool isAcceptableTurbidity(float turbidity);
bool isExcellentTDS(float tds);
bool isAcceptableTDS(float tds);
bool isExcellentEC(float ec);
bool isAcceptableEC(float ec);

#endif // WATER_QUALITY_LED_H