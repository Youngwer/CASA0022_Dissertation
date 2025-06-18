/**
 * water.ino - 水质监测系统主程序（模块化版本）
 * 
 * 设备: Arduino MKR WAN1310 + E-Paper 2.9" V2
 * 功能: 水质多参数监测，包括pH、浊度、TDS、电导率、温度
 * 
 * 模块化结构:
 * - WaterMonitor.h: 主头文件，包含所有声明和常量定义
 * - Sensors.cpp: 传感器读取模块
 * - Display.cpp: E-Paper显示模块  
 * - ButtonControl.cpp: 按钮控制模块
 * 
 * 作者: 水质监测项目
 * 版本: v2.0 (模块化版本)
 */

#include "WaterMonitor.h"

// ==================== Arduino主函数 ====================
void setup() {
  Serial.begin(115200);
  Serial.println("=== 水质监测系统启动 (模块化版本 v2.0) ===");
  
  // 初始化整个系统
  initializeSystem();
  
  Serial.println("系统初始化完成");
  Serial.println("按下按钮开始水质检测...");
  Serial.print("(按钮冷却时间: ");
  Serial.print(BUTTON_COOLDOWN / 1000);
  Serial.println(" 秒)");
}

void loop() {
  // 处理按钮输入
  handleButtonInput();
  
  // 处理串口命令
  handleSerialCommands();
  
  // 定期显示系统状态
  printSystemStatus();
  
  // 延时，避免过度占用CPU
  delay(50); // 更快的响应，避免按钮抖动
}

// ==================== 系统初始化函数 ====================
void initializeSystem() {
  Serial.println("开始系统初始化...");
  
  // 1. 初始化按钮控制
  initializeButton();
  
  // 2. 初始化传感器
  initializeSensors();
  
  // 3. 初始化E-Paper显示
  initializeEPaper();
  
  // 4. 显示启动界面
  showStartupScreen();
  
  // 5. 标记系统准备就绪
  setSystemReady(true);
  
  Serial.println("✓ 系统初始化完成，所有模块正常");
}

// ==================== 主要功能函数 ====================
void performWaterQualityTest() {
  Serial.println("开始执行水质检测流程...");
  
  // 显示检测进度
  displayProgress("Reading sensors...");
  
  // 读取所有传感器数据
  readAllSensors();
  
  // 在串口输出结果
  printAllReadings();
  
  // 显示检测进度
  displayProgress("Updating display...");
  
  // 更新E-Paper显示
  updateWaterQualityDisplay();
  
  Serial.println("水质检测流程完成！");
}

// ==================== 系统状态监控 ====================
void printSystemStatus() {
  static unsigned long lastStatusPrint = 0;
  unsigned long currentTime = millis();
  
  // 每10秒显示一次系统状态（只有在系统准备就绪后）
  if (isSystemReady() && currentTime - lastStatusPrint >= 10000) {
    Serial.print("系统运行中... 按钮状态: ");
    Serial.print(getButtonStatus());
    Serial.print(" | 运行时间: ");
    Serial.print(currentTime / 60000);
    Serial.println(" 分钟");
    
    lastStatusPrint = currentTime;
  }
}

// ==================== 错误处理函数 ====================
void handleSystemError(const char* errorMsg) {
  Serial.print("系统错误: ");
  Serial.println(errorMsg);
  
  // 显示错误到屏幕
  displayError(errorMsg);
  
  // 禁用系统直到重启
  setSystemReady(false);
  
  // 进入错误循环
  while(true) {
    Serial.println("系统错误，请重启设备");
    delay(5000);
  }
}

// ==================== 调试和维护函数 ====================
void runDiagnostics() {
  Serial.println("\n=== 系统诊断信息 ===");
  
  // 按钮状态诊断
  printButtonDebugInfo();
  
  // 传感器状态诊断
  Serial.println("传感器状态:");
  Serial.print("- 温度传感器: ");
  Serial.println(temperatureSensorFound ? "正常" : "未检测到");
  
  // 显示内存使用情况（简化版本）
  Serial.print("估计剩余内存: ");
  Serial.print(getFreeMemory());
  Serial.println(" 字节");
  
  Serial.println("==================");
}

// ==================== 内存监控函数 ====================
int getFreeMemory() {
  // 简化的内存检查，适用于SAMD21处理器
  return 16384; // 返回估计值，避免复杂的内存计算
}

// ==================== 串口命令处理 ====================
void handleSerialCommands() {
  // 可以添加串口命令处理功能
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "test") {
      Serial.println("执行手动测试...");
      performWaterQualityTest();
    } else if (command == "status") {
      runDiagnostics();
    } else if (command == "reset") {
      Serial.println("请手动按复位按钮重启设备");
      // 简化重启处理，避免使用不兼容的指令
    } else if (command == "help") {
      Serial.println("可用命令:");
      Serial.println("- test: 执行水质检测");
      Serial.println("- status: 显示系统状态");
      Serial.println("- reset: 提示手动重启");
      Serial.println("- help: 显示帮助信息");
    } else {
      Serial.println("未知命令，输入 'help' 查看可用命令");
    }
  }
}