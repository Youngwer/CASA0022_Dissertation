/**
 * water.ino - 水质监测系统主程序 (带LED指示)
 * 
 * 包含LED三色指示水质状态功能
 */

#include "WaterMonitor.h"

void setup() {
  Serial.begin(115200);
  
  // 等待串口
  unsigned long startTime = millis();
  while (!Serial && millis() - startTime < 5000) {
    ;
  }
  
  Serial.println("\n=================================");
  Serial.println("   水质监测系统 v2.2 (LED版)     ");
  Serial.println("=================================");
  
  // 初始化系统
  initializeSimpleSystem();
  
  Serial.println("\n系统启动完成!");
  Serial.println("按下按钮开始水质检测");
  Serial.println("LED指示: 绿灯=优秀 黄灯=一般 红灯=不安全");
  Serial.println("---------------------------------");
}

void loop() {
  // 处理按钮输入
  handleButtonInput();
  
  // 处理LoRa通信（如果初始化成功）
  if (loraConnected) {
    handleLoRaCommunication();
  }
  
  // 处理简单串口命令
  handleSimpleSerialCommands();
  
  // 检查是否需要自动发送数据
  checkAutoSend();
  
  delay(100);
}

// ==================== 简化的系统初始化 ====================
void initializeSimpleSystem() {
  Serial.println("开始系统初始化...");
  
  // 1. 初始化传感器（包含LED初始化）
  initializeSensors();
  delay(1000);
  
  // 2. 初始化E-Paper显示
  initializeEPaper();
  delay(1000);
  
  // 3. 初始化按钮控制
  initializeButton();
  delay(500);
  
  // 4. 尝试初始化LoRa（不强制要求成功）
  Serial.println("尝试初始化LoRa...");
  if (initializeLoRa() && connectToNetwork()) {
    Serial.println("✓ LoRa初始化成功!");
  } else {
    Serial.println("⚠ LoRa初始化失败，系统在离线模式下运行");
  }
  
  // 5. 显示启动界面
  showStartupScreen();
  
  // 6. 系统准备就绪
  setSystemReady(true);
  
  Serial.println("✓ 系统初始化完成!");
}

// ==================== 水质检测主流程 ====================
void performWaterQualityTest() {
  Serial.println("\n>>> 开始水质检测 <<<");
  
  // 读取所有传感器（包含LED更新）
  readAllSensors();
  
  // 更新显示
  updateWaterQualityDisplay();
  
  // 打印读数
  printAllReadings();
  
  // 如果LoRa可用，发送到云端
  if (loraConnected) {
    Serial.println("发送数据到云端...");
    if (sendWaterQualityData()) {
      Serial.println("✓ 数据已上传到TTN");
      displayProgress("Cloud: OK");
    } else {
      Serial.println("✗ 云端上传失败");
      displayProgress("Cloud: Failed");
    }
  }
  
  Serial.println(">>> 水质检测完成 <<<\n");
}

// ==================== 简化的串口命令处理 ====================
void handleSimpleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command == "test") {
      performWaterQualityTest();
      
    } else if (command == "status") {
      printSimpleSystemStatus();
      
    } else if (command == "led") {
      // 手动更新LED显示
      readAllSensors();
      Serial.println("LED状态已更新");
      
    } else if (command == "send" && loraConnected) {
      readAllSensors();
      if (sendWaterQualityData()) {
        Serial.println("✓ 数据发送成功");
      } else {
        Serial.println("✗ 数据发送失败");
      }
      
    } else if (command == "help") {
      Serial.println("\n=== 可用命令 ===");
      Serial.println("test   - 执行水质检测");
      Serial.println("status - 显示系统状态");
      Serial.println("led    - 手动更新LED显示");
      if (loraConnected) {
        Serial.println("send   - 手动发送数据");
      }
      Serial.println("help   - 显示此帮助");
      Serial.println("===============");
    }
  }
}

// ==================== 简化的系统状态 ====================
void printSimpleSystemStatus() {
  Serial.println("\n=== 系统状态 ===");
  Serial.print("系统就绪: ");
  Serial.println(systemReady ? "是" : "否");
  
  Serial.print("温度传感器: ");
  Serial.println(temperatureSensorFound ? "已连接" : "使用默认值");
  
  Serial.print("LED状态: ");
  Serial.println("已初始化");
  
  Serial.print("LoRa状态: ");
  if (loraInitialized && loraConnected) {
    Serial.println("已连接");
    Serial.print("设备EUI: ");
    Serial.println(loraModem.deviceEUI());
  } else if (loraInitialized) {
    Serial.println("已初始化但未连接");
  } else {
    Serial.println("未初始化");
  }
  
  Serial.print("按钮状态: ");
  Serial.println(getButtonStatus());
  
  // 显示当前LED状态
  Serial.print("当前水质LED: ");
  Serial.println(getWaterQualityStatus());
  
  Serial.println("================");
}

// ==================== 自动发送检查 ====================
void checkAutoSend() {
  static unsigned long lastAutoCheck = 0;
  
  // 每30秒检查一次
  if (millis() - lastAutoCheck > 30000) {
    lastAutoCheck = millis();
    
    if (loraConnected && shouldSendLoRaData()) {
      Serial.println("自动发送数据到云端...");
      readAllSensors();
      sendWaterQualityData();
    }
  }
}

// ==================== 其他必需函数 ====================
void printSystemStatus() {
  printSimpleSystemStatus();
}

int getFreeMemory() {
  // 简单的内存检测（Arduino方式）
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void runDiagnostics() {
  Serial.println("\n=== 系统诊断 ===");
  
  // 传感器诊断
  Serial.println("传感器诊断:");
  readAllSensors();
  Serial.print("- 温度: ");
  Serial.print(waterTemperature);
  Serial.println("°C");
  Serial.print("- pH: ");
  Serial.println(pHValue);
  Serial.print("- 浊度: ");
  Serial.print(turbidityNTU);
  Serial.println(" NTU");
  Serial.print("- 电导率: ");
  Serial.print(conductivityValue);
  Serial.println(" μS/cm");
  Serial.print("- TDS: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");
  
  // LED诊断
  Serial.println("LED诊断:");
  Serial.print("- 当前状态: ");
  Serial.println(getWaterQualityStatus());
  
  // LoRa诊断
  if (loraInitialized) {
    Serial.println("LoRa诊断:");
    Serial.print("- 版本: ");
    Serial.println(loraModem.version());
    Serial.print("- 连接状态: ");
    Serial.println(loraConnected ? "已连接" : "未连接");
  }
  
  // 内存诊断
  Serial.print("可用内存: ");
  Serial.print(getFreeMemory());
  Serial.println(" 字节");
  
  Serial.println("===============");
}