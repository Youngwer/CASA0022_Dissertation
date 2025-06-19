/**
 * WaterQualityLED.cpp - 水质LED指示模块实现
 * 
 * 处理基于水质参数的三色LED指示功能
 */

#include "WaterQualityLED.h"

// ==================== LED初始化 ====================
void initializeLEDs() {
  Serial.println("初始化水质指示LED...");
  
  // 设置LED引脚为输出模式
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  
  // 关闭所有LED
  turnOffAllLEDs();
  
  Serial.println("✓ LED初始化完成");
}

// ==================== LED控制函数 ====================
void setLEDStatus(int ledStatus) {
  // 先关闭所有LED
  turnOffAllLEDs();
  
  // 根据状态点亮对应LED
  switch(ledStatus) {
    case GREEN_LED:
      digitalWrite(GREEN_LED_PIN, HIGH);
      Serial.println("LED状态: 绿灯 - 水质优秀");
      break;
      
    case YELLOW_LED:
      digitalWrite(YELLOW_LED_PIN, HIGH);
      Serial.println("LED状态: 黄灯 - 水质一般");
      break;
      
    case RED_LED:
      digitalWrite(RED_LED_PIN, HIGH);
      Serial.println("LED状态: 红灯 - 水质不安全");
      break;
      
    default:
      Serial.println("LED状态: 全部关闭");
      break;
  }
}

void turnOffAllLEDs() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
}

// ==================== 水质评估主函数 ====================
int evaluateWaterQuality(float pH, float turbidity, float tds, float ec) {
  Serial.println("\n=== 水质评估 ===");
  Serial.print("pH: "); Serial.println(pH, 2);
  Serial.print("浊度: "); Serial.print(turbidity, 1); Serial.println(" NTU");
  Serial.print("TDS: "); Serial.print(tds, 0); Serial.println(" ppm");
  Serial.print("电导率: "); Serial.print(ec, 0); Serial.println(" µS/cm");
  
  // 检查红灯条件 - 任何一个参数不合格就显示红灯
  if (!isAcceptablepH(pH) || !isAcceptableTurbidity(turbidity) || 
      !isAcceptableTDS(tds) || !isAcceptableEC(ec)) {
    Serial.println("评估结果: 不适合饮用");
    return RED_LED;
  }
  
  // 检查是否所有参数都优秀
  if (isExcellentpH(pH) && isExcellentTurbidity(turbidity) && 
      isExcellentTDS(tds) && isExcellentEC(ec)) {
    Serial.println("评估结果: 优秀，适合饮用");
    return GREEN_LED;
  }
  
  // 其他情况显示黄灯
  Serial.println("评估结果: 一般，勉强可接受");
  return YELLOW_LED;
}

// ==================== 水质描述函数 ====================
String getWaterQualityDescription(int ledStatus) {
  switch(ledStatus) {
    case GREEN_LED:
      return "Status: EXCELLENT";
    case YELLOW_LED:
      return "Status: MARGINAL";
    case RED_LED:
      return "Status: UNSAFE";
    default:
      return "Status: UNKNOWN";
  }
}

// ==================== pH检查函数 ====================
bool isExcellentpH(float pH) {
  return (pH >= PH_EXCELLENT_MIN && pH <= PH_EXCELLENT_MAX);
}

bool isAcceptablepH(float pH) {
  return (pH >= PH_ACCEPTABLE_MIN && pH <= PH_ACCEPTABLE_MAX);
}

// ==================== 浊度检查函数 ====================
bool isExcellentTurbidity(float turbidity) {
  return (turbidity >= 0 && turbidity <= TURBIDITY_EXCELLENT_MAX);
}

bool isAcceptableTurbidity(float turbidity) {
  return (turbidity >= 0 && turbidity <= TURBIDITY_ACCEPTABLE_MAX);
}

// ==================== TDS检查函数 ====================
bool isExcellentTDS(float tds) {
  return (tds >= TDS_EXCELLENT_MIN && tds <= TDS_EXCELLENT_MAX);
}

bool isAcceptableTDS(float tds) {
  return (tds >= TDS_ACCEPTABLE_MIN && tds <= TDS_ACCEPTABLE_MAX);
}

// ==================== 电导率检查函数 ====================
bool isExcellentEC(float ec) {
  return (ec >= EC_EXCELLENT_MIN && ec <= EC_EXCELLENT_MAX);
}

bool isAcceptableEC(float ec) {
  return (ec >= EC_ACCEPTABLE_MIN && ec <= EC_ACCEPTABLE_MAX);
}