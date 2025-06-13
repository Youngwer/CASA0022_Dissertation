/*
 * 水质监测系统 - 主文件
 * Arduino MKR WAN 1310
 * 整合pH、浊度、电导率和温度传感器
 * TDS通过电导率计算得出
 */

#include "sensors.h"

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // 等待串口连接
  }
  
  Serial.println("========================================");
  Serial.println("水质监测系统 - MKR WAN 1310");
  Serial.println("传感器: pH, 浊度, 电导率, 温度");
  Serial.println("TDS通过电导率计算: TDS = 电导率 × 0.5");
  Serial.println("========================================");
  
  // 初始化所有传感器
  initializeSensors();
  
  Serial.println("系统初始化完成，开始监测...");
  Serial.println();
}

void loop() {
  // 读取所有传感器数据
  readAllSensors();
  
  // 显示所有参数
  printAllReadings();
  
  delay(2000); // 每2秒读取一次
}