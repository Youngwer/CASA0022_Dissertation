/**
 * ButtonControl.cpp - 按钮控制模块
 * 
 * 处理按钮输入、防抖和冷却时间控制
 */

#include "WaterMonitor.h"

// ==================== 全局变量定义 ====================
bool lastButtonState = HIGH;
bool currentButtonState = HIGH;
bool buttonPressed = false;
unsigned long lastButtonPress = 0;
bool systemReady = false;

// ==================== 按钮初始化 ====================
void initializeButton() {
  Serial.println("正在初始化按钮控制...");
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // 初始化按钮状态
  lastButtonState = digitalRead(BUTTON_PIN);
  currentButtonState = lastButtonState;
  buttonPressed = false;
  lastButtonPress = 0;
  
  Serial.println("✓ 按钮控制初始化完成");
}

// ==================== 按钮处理主函数 ====================
void handleButtonInput() {
  // 只有在系统准备就绪后才处理按钮
  if (!systemReady) {
    return;
  }
  
  // 读取当前按钮状态
  currentButtonState = digitalRead(BUTTON_PIN);
  
  // 检测按钮按下（下降沿 - 按钮被按下）
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    buttonPressed = true;  // 标记按钮被按下
    Serial.println("按钮按下...");
  }
  
  // 检测按钮松开（上升沿 - 按钮被松开）- 这是完整的按钮操作
  if (lastButtonState == LOW && currentButtonState == HIGH && buttonPressed) {
    Serial.println("按钮松开 - 检测到完整按钮操作！");
    
    if (!isCooldownPeriod()) {
      // 按钮操作成功处理
      Serial.println("\n=== 完整按钮操作触发 - 开始水质检测 ===");
      
      // 执行水质检测
      performWaterQualityTest();
      
      // 更新按钮状态
      lastButtonPress = millis();
      buttonPressed = false;
      
      Serial.print("下次可以按钮的时间: ");
      Serial.print(BUTTON_COOLDOWN / 1000);
      Serial.println(" 秒后");
      Serial.println("=========================");
      
    } else {
      // 在冷却时间内
      handleCooldownMessage();
      buttonPressed = false;  // 重置按钮状态
    }
  }
  
  // 更新按钮状态
  lastButtonState = currentButtonState;
}

// ==================== 按钮状态检查函数 ====================
bool isButtonPressed() {
  return buttonPressed;
}

bool isCooldownPeriod() {
  unsigned long currentTime = millis();
  return (currentTime - lastButtonPress < BUTTON_COOLDOWN);
}

// ==================== 按钮处理辅助函数 ====================
void handleCooldownMessage() {
  unsigned long currentTime = millis();
  unsigned long remainingCooldown = BUTTON_COOLDOWN - (currentTime - lastButtonPress);
  
  Serial.print("按钮冷却中，请等待 ");
  Serial.print(remainingCooldown / 1000);
  Serial.println(" 秒");
  
  // 显示冷却信息到屏幕
  char cooldownMsg[30];
  sprintf(cooldownMsg, "Wait %lus", remainingCooldown / 1000);
  displayProgress(cooldownMsg);
}

// ==================== 系统状态控制 ====================
void setSystemReady(bool ready) {
  systemReady = ready;
  if (ready) {
    Serial.println("✓ 系统已准备就绪，按钮控制已激活");
    Serial.print("按钮冷却时间: ");
    Serial.print(BUTTON_COOLDOWN / 1000);
    Serial.println(" 秒");
  } else {
    Serial.println("⚠ 系统未就绪，按钮控制已禁用");
  }
}

bool isSystemReady() {
  return systemReady;
}

// ==================== 状态查询函数 ====================
unsigned long getRemainingCooldown() {
  if (!isCooldownPeriod()) {
    return 0;
  }
  
  unsigned long currentTime = millis();
  return BUTTON_COOLDOWN - (currentTime - lastButtonPress);
}

String getButtonStatus() {
  if (!systemReady) {
    return "System Not Ready";
  }
  
  if (isCooldownPeriod()) {
    unsigned long remaining = getRemainingCooldown();
    return "Cooldown " + String(remaining / 1000) + "s";
  }
  
  return "Ready";
}

// ==================== 调试信息函数 ====================
void printButtonDebugInfo() {
  Serial.println("\n=== 按钮状态调试信息 ===");
  Serial.print("系统就绪: ");
  Serial.println(systemReady ? "是" : "否");
  Serial.print("当前按钮状态: ");
  Serial.println(currentButtonState == HIGH ? "高" : "低");
  Serial.print("按钮按下标志: ");
  Serial.println(buttonPressed ? "是" : "否");
  Serial.print("上次按钮时间: ");
  Serial.print(lastButtonPress);
  Serial.println(" ms");
  Serial.print("冷却状态: ");
  Serial.println(isCooldownPeriod() ? "冷却中" : "就绪");
  if (isCooldownPeriod()) {
    Serial.print("剩余冷却时间: ");
    Serial.print(getRemainingCooldown() / 1000);
    Serial.println(" 秒");
  }
  Serial.println("========================");
}