/**
 * Display.cpp - E-Paper显示模块
 * 
 * 处理E-Paper显示屏的所有显示功能
 */

#include "WaterMonitor.h"

// ==================== 全局变量定义 ====================
unsigned char image[1024];
Paint paint(image, 0, 0);
Epd epd;

// ==================== E-Paper初始化 ====================
void initializeEPaper() {
  Serial.println("正在初始化E-Paper显示屏...");
  
  // 使用稳定的普通初始化模式
  if (epd.Init() != 0) {
    Serial.println("✗ E-Paper初始化失败!");
    return;
  }
  
  Serial.println("✓ E-Paper初始化成功");
  
  // 只在第一次清空显示
  Serial.println("清空E-Paper显示...");
  epd.ClearFrameMemory(0xFF);
  epd.DisplayFrame();
  delay(2000); // 必要的等待时间
  
  Serial.println("E-Paper初始化完成");
}

// ==================== 启动界面显示 ====================
void showStartupScreen() {
  Serial.println("显示启动界面...");
  
  // 设置画布
  paint.SetRotate(ROTATE_0);
  paint.SetWidth(128);
  paint.SetHeight(24);
  
  // 绘制标题
  paint.Clear(COLORED);  // 黑色背景
  paint.DrawStringAt(5, 4, "Water Monitor", &Font12, UNCOLORED);  // 白色文字
  epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());
  
  // 调整高度
  paint.SetHeight(16);
  
  // 系统状态信息
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "System Ready", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 40, paint.GetWidth(), paint.GetHeight());
  
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "Initializing...", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 60, paint.GetWidth(), paint.GetHeight());
  
  // 显示传感器状态
  paint.Clear(UNCOLORED);
  String sensorStatus = temperatureSensorFound ? "Temp: OK" : "Temp: Default";
  paint.DrawStringAt(2, 2, sensorStatus.c_str(), &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 80, paint.GetWidth(), paint.GetHeight());
  
  // 版本信息
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "v2.0 Stable", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 100, paint.GetWidth(), paint.GetHeight());
  
  // 底部操作提示（黑色背景，分两行显示，置于屏幕底部）
  paint.SetHeight(28);
  paint.Clear(COLORED);  // 黑色背景
  
  // 第一行："Press Button" - 居中显示
  paint.DrawStringAt(22, 2, "Press Button", &Font12, UNCOLORED);  // 白色文字
  
  // 第二行："to Start" - 居中显示
  // "to Start" = 8个字符，总宽56像素
  // 居中位置：(128 - 56) / 2 = 36像素
  paint.DrawStringAt(36, 15, "to Start", &Font12, UNCOLORED);  // 白色文字
  
  // 移到屏幕底部：296 - 28 = 268
  epd.SetFrameMemory(paint.GetImage(), 0, 268, paint.GetWidth(), paint.GetHeight());
  
  // 刷新显示
  Serial.println("刷新启动界面到屏幕...");
  epd.DisplayFrame();
  delay(3000); // 等待刷新完成
  
  Serial.println("启动界面显示完成，系统准备就绪");
}

// ==================== 水质数据显示 ====================
void updateWaterQualityDisplay() {
  Serial.println("开始更新E-Paper显示水质数据...");
  
  displaySensorData();
  
  Serial.println("水质数据显示更新完成");
}

void displaySensorData() {
  Serial.println("更新传感器数据显示...");
  
  // 设置画布
  paint.SetRotate(ROTATE_0);
  paint.SetWidth(128);
  paint.SetHeight(24);
  
  // 标题
  paint.Clear(COLORED);
  paint.DrawStringAt(10, 4, "Water Monitor", &Font12, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());
  
  // 调整高度用于参数显示
  paint.SetHeight(16);
  
  // 恢复原来的详细显示格式
  
  // 温度显示
  paint.Clear(UNCOLORED);
  char tempStr[20];
  sprintf(tempStr, "Temp: %.1f C", waterTemperature);
  paint.DrawStringAt(2, 2, tempStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 40, paint.GetWidth(), paint.GetHeight());
  
  // pH显示
  paint.Clear(UNCOLORED);
  char phStr[20];
  sprintf(phStr, "pH: %.2f", pHValue);
  paint.DrawStringAt(2, 2, phStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 60, paint.GetWidth(), paint.GetHeight());
  
  // 浊度显示
  paint.Clear(UNCOLORED);
  char turbStr[20];
  sprintf(turbStr, "Turb: %.1f NTU", turbidityNTU);
  paint.DrawStringAt(2, 2, turbStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 80, paint.GetWidth(), paint.GetHeight());
  
  // TDS显示
  paint.Clear(UNCOLORED);
  char tdsStr[20];
  sprintf(tdsStr, "TDS: %.1f ppm", tdsValue);
  paint.DrawStringAt(2, 2, tdsStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 100, paint.GetWidth(), paint.GetHeight());
  
  // 电导率显示
  paint.Clear(UNCOLORED);
  char ecStr[20];
  sprintf(ecStr, "EC: %.1f uS/cm", conductivityValue);
  paint.DrawStringAt(2, 2, ecStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 120, paint.GetWidth(), paint.GetHeight());
  
  // 水质状态显示
  paint.Clear(UNCOLORED);
  String status = getWaterQualityStatus();
  paint.DrawStringAt(2, 2, status.c_str(), &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 140, paint.GetWidth(), paint.GetHeight());
  
  // 时间戳显示
  paint.Clear(UNCOLORED);
  char timeStr[25];
  sprintf(timeStr, "Time: %lu min", millis()/60000);
  paint.DrawStringAt(2, 2, timeStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 160, paint.GetWidth(), paint.GetHeight());
  
  // 底部操作提示（黑色背景，分两行显示，置于屏幕底部）
  paint.SetHeight(28);  // 增加高度以容纳两行文字
  paint.Clear(COLORED);  // 黑色背景
  
  // 第一行："Press Button" - 居中显示
  // "Press Button" = 12个字符，Font12每个字符宽7像素，总宽84像素
  // 居中位置：(128 - 84) / 2 = 22像素
  paint.DrawStringAt(22, 2, "Press Button", &Font12, UNCOLORED);  // 白色文字
  
  // 第二行："to Update" - 居中显示  
  // "to Update" = 9个字符，总宽63像素
  // 居中位置：(128 - 63) / 2 = 32.5 ≈ 33像素
  paint.DrawStringAt(33, 15, "to Update", &Font12, UNCOLORED);  // 白色文字，Y=15（第一行12px + 间距3px）
  
  // 移到屏幕底部：296 - 28 = 268
  epd.SetFrameMemory(paint.GetImage(), 0, 268, paint.GetWidth(), paint.GetHeight());
  
  Serial.println("刷新显示到屏幕...");
  // 使用标准刷新，确保可靠显示
  epd.DisplayFrame();
  delay(2000); // 等待刷新完成
  
  Serial.println("传感器数据显示完成");
}

// ==================== 显示错误信息 ====================
void displayError(const char* errorMsg) {
  Serial.print("显示错误信息: ");
  Serial.println(errorMsg);
  
  // 设置画布
  paint.SetRotate(ROTATE_0);
  paint.SetWidth(128);
  paint.SetHeight(24);
  
  // 错误标题
  paint.Clear(COLORED);
  paint.DrawStringAt(20, 4, "ERROR", &Font12, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 50, paint.GetWidth(), paint.GetHeight());
  
  // 错误信息
  paint.SetHeight(16);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, errorMsg, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 80, paint.GetWidth(), paint.GetHeight());
  
  // 刷新显示
  epd.DisplayFrame();
  delay(3000);
}

// ==================== 显示进度信息 ====================
void displayProgress(const char* progressMsg) {
  Serial.print("显示进度: ");
  Serial.println(progressMsg);
  
  // 简单在串口显示进度，避免E-Paper频繁刷新
  // 如果需要屏幕显示，可以只更新一行
  /*
  paint.SetRotate(ROTATE_0);
  paint.SetWidth(128);
  paint.SetHeight(16);
  
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, progressMsg, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 140, paint.GetWidth(), paint.GetHeight());
  
  epd.DisplayFrame();
  delay(1000);
  */
}