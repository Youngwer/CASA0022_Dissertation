/**
 * Display.cpp - E-Paper显示模块 (消除残影版本)
 * 
 * 处理E-Paper显示屏的所有显示功能
 * 修正残影问题，改进状态显示
 */

#include "WaterMonitor.h"
#include "WaterQualityLED.h"  // 添加这一行来使用水质评估函数

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
  
  // === 主标题 - 保持与数据界面一致 ===
  paint.SetHeight(24);
  paint.Clear(COLORED);  // 黑色背景
  // "Water Monitor" 居中显示
  paint.DrawStringAt(18, 6, "Water Monitor", &Font12, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 15, paint.GetWidth(), paint.GetHeight());
  
  // === 内容信息 - 使用与数据行一致的间距，包含空行设计 ===
  paint.SetHeight(20);  // 与数据行相同高度
  const int LINE_SPACING = 25;  // 与数据行相同间距
  int currentY = 50;
  
  // Water Quality
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "Water Quality", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // Testing System
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "Testing System", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // 空行 - 增加视觉间距
  currentY += LINE_SPACING;
  
  // Ready for Analysis - 上下都有大间距
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "Ready for Analysis", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  
  // 空行 - 增加视觉间距
  currentY += LINE_SPACING;
  
  // UCL CASA Project
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(2, 2, "UCL CASA Project", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  
  // === 底部操作提示 - 与数据界面完全一致 ===
  paint.SetHeight(50);  // 大幅增加高度，与数据界面一致
  paint.Clear(COLORED);  // 黑色背景
  
  // 第一行："Press Button" - 居中显示
  paint.DrawStringAt(22, 8, "Press Button", &Font12, UNCOLORED);
  
  // 第二行："to Start" - 居中显示，大幅增加行间距
  paint.DrawStringAt(36, 30, "to Start", &Font12, UNCOLORED);
  
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

  // 温度显示 (温度没有excellent判断，保持原样)
  paint.Clear(UNCOLORED);
  char tempStr[25];
  sprintf(tempStr, " Temp: %.1f C", waterTemperature);
  paint.DrawStringAt(2, 2, tempStr, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;

// pH显示 - 根据是否excellent决定显示样式
  if (isExcellentpH(pHValue)) {
    // excellent - 白底黑字
    paint.Clear(UNCOLORED);
    char phStr[25];
    sprintf(phStr, "   pH: %.2f", pHValue);
    paint.DrawStringAt(2, 2, phStr, &Font12, COLORED);
  } else {
    // 不是excellent - 黑底白字，垂直居中
    paint.Clear(COLORED);  // 黑色背景
    char phStr[25];
    sprintf(phStr, "   pH: %.2f", pHValue);
    // 垂直居中计算：(画布高度20 - 字体高度12) / 2 = 4
    paint.DrawStringAt(2, 4, phStr, &Font12, UNCOLORED);
  }
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;

  // 浊度显示 - 根据是否excellent决定显示样式
  if (isExcellentTurbidity(turbidityNTU)) {
    // excellent - 白底黑字
    paint.Clear(UNCOLORED);
    char turbStr[25];
    sprintf(turbStr, " Turb: %.1f NTU", turbidityNTU);
    paint.DrawStringAt(2, 2, turbStr, &Font12, COLORED);
  } else {
    // 不是excellent - 黑底白字，垂直居中
    paint.Clear(COLORED);  // 黑色背景
    char turbStr[25];
    sprintf(turbStr, " Turb: %.1f NTU", turbidityNTU);
    // 垂直居中计算：(画布高度20 - 字体高度12) / 2 = 4
    paint.DrawStringAt(2, 4, turbStr, &Font12, UNCOLORED);
  }
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;

  // TDS显示 - 根据是否excellent决定显示样式
  if (isExcellentTDS(tdsValue)) {
    // excellent - 白底黑字
    paint.Clear(UNCOLORED);
    char tdsStr[25];
    sprintf(tdsStr, "  TDS: %.1f ppm", tdsValue);
    paint.DrawStringAt(2, 2, tdsStr, &Font12, COLORED);
  } else {
    // 不是excellent - 黑底白字，垂直居中
    paint.Clear(COLORED);  // 黑色背景
    char tdsStr[25];
    sprintf(tdsStr, "  TDS: %.1f ppm", tdsValue);
    // 垂直居中计算：(画布高度20 - 字体高度12) / 2 = 4
    paint.DrawStringAt(2, 4, tdsStr, &Font12, UNCOLORED);
  }
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;

  // 电导率显示 - 根据是否excellent决定显示样式
  if (isExcellentEC(conductivityValue)) {
    // excellent - 白底黑字
    paint.Clear(UNCOLORED);
    char ecStr[30];
    sprintf(ecStr, "   EC: %.1f uS/cm", conductivityValue);
    paint.DrawStringAt(2, 2, ecStr, &Font12, COLORED);
  } else {
    // 不是excellent - 黑底白字，垂直居中
    paint.Clear(COLORED);  // 黑色背景
    char ecStr[30];
    sprintf(ecStr, "   EC: %.1f uS/cm", conductivityValue);
    // 垂直居中计算：(画布高度20 - 字体高度12) / 2 = 4
    paint.DrawStringAt(2, 4, ecStr, &Font12, UNCOLORED);
  }
  epd.SetFrameMemory(paint.GetImage(), 0, currentY, paint.GetWidth(), paint.GetHeight());
  currentY += LINE_SPACING;
  // === 添加空行 ===
  currentY += LINE_SPACING;  // 这会创建一个空行

  // === 水质状态显示 - 使用Font16并统一黑底白字显示 ===
  paint.SetHeight(32);  // 32像素高的画布
  String status = getSimplifiedWaterStatus();
  
  // 统一使用黑底白字显示，突出所有状态
  paint.Clear(COLORED);  // 黑色背景
  
  if (status.indexOf("UNSAFE") != -1) {
    // UNSAFE - 黑底白字
    // "UNSAFE" = 6个字符，Font16每字符11像素，总宽66像素
    // 水平居中：(128 - 66) / 2 = 31像素
    // 垂直居中：(32 - 16) / 2 = 8像素
    paint.DrawStringAt(31, 8, status.c_str(), &Font16, UNCOLORED);
  } else if (status.indexOf("EXCELLENT") != -1) {
    // EXCELLENT - 黑底白字（与UNSAFE统一）
    // "EXCELLENT" = 9个字符，Font16每字符11像素，总宽99像素
    // 水平居中：(128 - 99) / 2 = 14像素
    // 垂直居中：(32 - 16) / 2 = 8像素
    paint.DrawStringAt(14, 8, status.c_str(), &Font16, UNCOLORED);
  } else {
    // MARGINAL或其他 - 黑底白字（与UNSAFE统一）
    // "MARGINAL" = 8个字符，Font16每字符11像素，总宽88像素
    // 水平居中：(128 - 88) / 2 = 20像素
    // 垂直居中：(32 - 16) / 2 = 8像素
    paint.DrawStringAt(20, 8, status.c_str(), &Font16, UNCOLORED);
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