/**
 * Display.cpp - E-Paper显示模块 (消除残影版本)
 * 
 * 处理E-Paper显示屏的所有显示功能
 * 修正残影问题，改进状态显示
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
  
  // 完全清空显示
  Serial.println("完全清空E-Paper显示...");
  epd.ClearFrameMemory(0xFF);  // 清空帧缓冲
  epd.DisplayFrame();
  delay(2000);
  
  // 再次清空确保没有残影
  epd.ClearFrameMemory(0xFF);
  
  Serial.println("E-Paper初始化完成");
}

// ==================== 完全清屏函数 ====================
void clearEntireScreen() {
  Serial.println("执行完全清屏...");
  
  // 只使用驱动提供的清屏功能，避免大画布导致卡死
  epd.ClearFrameMemory(0xFF);
  epd.DisplayFrame();
  delay(2000);  // 给E-ink足够时间完成刷新
  
  Serial.println("清屏完成");
}

// ==================== 启动界面显示 ====================
void showStartupScreen() {
  Serial.println("显示启动界面...");
  
  // 先完全清屏
  clearEntireScreen();
  
  // 设置画布基本参数
  paint.SetRotate(ROTATE_0);
  paint.SetWidth(128);
  
  // === 主标题 - 水平和垂直都居中显示 ===
  paint.SetHeight(24);
  paint.Clear(COLORED);  // 黑色背景
  // "Water Monitor" = 13个字符，Font12每字符7像素，总宽91像素
  // 水平居中：(128 - 91) / 2 = 18像素
  // 垂直居中：(24 - 12) / 2 = 6像素（24像素画布高度，12像素字体高度）
  paint.DrawStringAt(18, 6, "Water Monitor", &Font12, UNCOLORED);  // 白色文字
  epd.SetFrameMemory(paint.GetImage(), 0, 15, paint.GetWidth(), paint.GetHeight());
  
  // === 系统状态信息 ===
  paint.SetHeight(20);  // 增加高度避免重叠
  const int LINE_SPACING = 25;
  int currentY = 50;
  
  // System Ready
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "System Ready", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // Initializing...
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "Initializing...", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // 传感器状态
  paint.Clear(UNCOLORED);
  String sensorStatus = temperatureSensorFound ? "Temp: OK" : "Temp: Default";
  paint.DrawStringAt(2, 2, sensorStatus.c_str(), &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // 版本信息
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "v2.0 Stable", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  
  // === 底部操作提示 - 大幅增加间距匹配数据行 ===
  paint.SetHeight(50);  // 大幅增加高度，从32到50
  paint.Clear(COLORED);  // 黑色背景
  
  // 第一行："Press Button" - 居中显示，增加更多上边距
  paint.DrawStringAt(22, 8, "Press Button", &Font12, UNCOLORED);   // Y从4改为8
  
  // 第二行："to Start" - 居中显示，大幅增加行间距匹配数据行
  paint.DrawStringAt(36, 30, "to Start", &Font12, UNCOLORED);     // Y从18改为30，间距22像素
  
  // 调整底部位置：296 - 50 = 246
  epd.SetFrameMemory(paint.GetImage(), 0, 246, paint.GetWidth(), paint.GetHeight());
  
  // 刷新显示
  Serial.println("刷新启动界面到屏幕...");
  epd.DisplayFrame();
  delay(3000);
  
  Serial.println("启动界面显示完成，系统准备就绪");
}


// ==================== 获取简化的水质状态 ====================
String getSimplifiedWaterStatus() {
  String fullStatus = getWaterQualityStatus();
  
  // 从完整状态中提取关键词
  if (fullStatus.indexOf("UNSAFE") != -1) {
    return "UNSAFE";
  } else if (fullStatus.indexOf("EXCELLENT") != -1) {
    return "EXCELLENT";
  } else if (fullStatus.indexOf("MARGINAL") != -1) {
    return "MARGINAL";
  } else {
    return "UNKNOWN";
  }
}

// ==================== 水质数据显示 ====================
void updateWaterQualityDisplay() {
  Serial.println("开始更新E-Paper显示水质数据...");
  
  displaySensorData();
  
  Serial.println("水质数据显示更新完成");
}

void displaySensorData() {
  Serial.println("更新传感器数据显示...");
  
  // === 关键：使用简化清屏避免卡死 ===
  Serial.println("清除屏幕内容...");
  epd.ClearFrameMemory(0xFF);
  // 注意：不立即DisplayFrame，等所有内容准备好后一次性刷新
  
  // 设置画布基本参数
  paint.SetRotate(ROTATE_0);
  paint.SetWidth(128);
  
  // === 主标题 - 水平和垂直都居中显示 ===
  paint.SetHeight(24);
  paint.Clear(COLORED);  // 黑色背景
  // "Water Monitor" = 13个字符，Font12每字符7像素，总宽91像素
  // 水平居中：(128 - 91) / 2 = 18像素
  // 垂直居中：(24 - 12) / 2 = 6像素（24像素画布高度，12像素字体高度）
  paint.DrawStringAt(18, 6, "Water Monitor", &Font12, UNCOLORED);  // 白色文字
  epd.SetFrameMemory(paint.GetImage(), 0, 15, paint.GetWidth(), paint.GetHeight());
  
  // === 传感器数据显示区域 ===
  paint.SetHeight(20);  // 统一使用20像素高度
  const int LINE_SPACING = 25;
  int currentY = 50;
  
  // 温度显示
  paint.Clear(UNCOLORED);
  char tempStr[25];
  sprintf(tempStr, " Temp: %.1f C", waterTemperature);
  paint.DrawStringAt(2, 2, tempStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // pH显示  
  paint.Clear(UNCOLORED);
  char phStr[25];
  sprintf(phStr, "   pH: %.2f", pHValue);
  paint.DrawStringAt(2, 2, phStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // 浊度显示
  paint.Clear(UNCOLORED);
  char turbStr[25];
  sprintf(turbStr, " Turb: %.1f NTU", turbidityNTU);
  paint.DrawStringAt(2, 2, turbStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // TDS显示
  paint.Clear(UNCOLORED);
  char tdsStr[25];
  sprintf(tdsStr, "  TDS: %.1f ppm", tdsValue);
  paint.DrawStringAt(2, 2, tdsStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // 电导率显示
  paint.Clear(UNCOLORED);
  char ecStr[30];
  sprintf(ecStr, "   EC: %.1f uS/cm", conductivityValue);
  paint.DrawStringAt(2, 2, ecStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
 // === 水质状态显示 - 使用Font16突出显示 ===
  paint.SetHeight(32);  // 增加高度适配Font16 (16px高度 + 上下边距)
  String status = getSimplifiedWaterStatus();
  
  if (status.indexOf("UNSAFE") != -1) {
    // UNSAFE - 黑底白字，使用Font16
    paint.Clear(COLORED);
    // "UNSAFE" = 6个字符，Font16每字符11像素，总宽66像素
    // 居中位置：(128 - 66) / 2 = 31像素
    paint.DrawStringAt(31, 8, status.c_str(), &Font16, UNCOLORED);
  } else if (status.indexOf("EXCELLENT") != -1) {
    // EXCELLENT - 白底黑字，使用Font16
    paint.Clear(UNCOLORED);
    // "EXCELLENT" = 9个字符，Font16每字符11像素，总宽99像素
    // 居中位置：(128 - 99) / 2 = 14像素
    paint.DrawStringAt(14, 8, status.c_str(), &Font16, COLORED);
  } else {
    // MARGINAL或其他 - 正常显示，使用Font16
    paint.Clear(UNCOLORED);
    // "MARGINAL" = 8个字符，Font16每字符11像素，总宽88像素
    // 居中位置：(128 - 88) / 2 = 20像素
    paint.DrawStringAt(20, 8, status.c_str(), &Font16, COLORED);
  }
  
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
// === 底部操作提示 - 大幅增加间距匹配数据行 ===
  paint.SetHeight(50);  // 大幅增加高度，从32到50
  paint.Clear(COLORED);  // 黑色背景
  
  // 第一行："Press Button" - 居中显示，增加更多上边距
  paint.DrawStringAt(22, 8, "Press Button", &Font12, UNCOLORED);
  
  // 第二行："to Update" - 居中显示，大幅增加行间距匹配数据行
  paint.DrawStringAt(33, 30, "to Update", &Font12, UNCOLORED);   // Y从18改为30，间距22像素
  
  // 调整底部位置：296 - 50 = 246  
  epd.SetFrameMemory(paint.GetImage(), 0, 246, paint.GetWidth(), paint.GetHeight());
  
  Serial.println("刷新显示到屏幕...");
  epd.DisplayFrame();
  delay(2000);
  
  Serial.println("传感器数据显示完成");
}

// ==================== 显示错误信息 ====================
void displayError(const char* errorMsg) {
  Serial.print("显示错误信息: ");
  Serial.println(errorMsg);
  
  // 先清屏
  clearEntireScreen();
  
  paint.SetRotate(ROTATE_0);
  paint.SetWidth(128);
  paint.SetHeight(24);
  
  // 错误标题
  paint.Clear(COLORED);
  paint.DrawStringAt(20, 4, "ERROR", &Font12, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 50, paint.GetWidth(), paint.GetHeight());
  
  // 错误信息
  paint.SetHeight(20);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, errorMsg, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 80, paint.GetWidth(), paint.GetHeight());
  
  epd.DisplayFrame();
  delay(3000);
}

// ==================== 显示进度信息 ====================
void displayProgress(const char* progressMsg) {
  Serial.print("显示进度: ");
  Serial.println(progressMsg);
  // 避免频繁刷新E-Paper，只在串口显示
}