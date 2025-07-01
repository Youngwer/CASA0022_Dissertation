// ==================== 文件2：Arduino/water/LoRaComm.cpp ====================
/**
 * LoRaComm.cpp - LoRa通信模块实现
 * 
 * 处理水质数据的LoRaWAN上传功能
 */

#include "LoRaComm.h"
#include "WaterMonitor.h"

// ==================== 全局变量定义 ====================
LoRaModem loraModem;
bool loraInitialized = false;
bool loraConnected = false;
unsigned long lastLoRaSend = 0;
int loraRetryCount = 0;
bool autoSendEnabled = false;  // 新增：默认关闭自动发送

// ==================== LoRa初始化 ====================
bool initializeLoRa() {
  Serial.println("正在初始化LoRa模块...");
  
  // 初始化LoRa模块
  if (!loraModem.begin(EU868)) {  // 根据你的地区调整：EU868, US915, AS923等
    Serial.println("✗ LoRa模块初始化失败!");
    return false;
  }
  
  loraInitialized = true;
  Serial.println("✓ LoRa模块初始化成功");
  Serial.print("设备EUI: ");
  Serial.println(loraModem.deviceEUI());
  
  return true;
}

// ==================== 连接到网络 ====================
bool connectToNetwork() {
  if (!loraInitialized) {
    Serial.println("LoRa模块未初始化");
    return false;
  }
  
  Serial.println("正在连接到LoRaWAN网络...");
  
  unsigned long startTime = millis();
  int connected = loraModem.joinOTAA(APP_EUI, APP_KEY);
  
  if (!connected) {
    Serial.println("✗ 网络连接失败!");
    Serial.println("请检查:");
    Serial.println("  - App EUI和App Key是否正确");
    Serial.println("  - 是否有网关覆盖");
    Serial.println("  - 频段设置是否正确");
    return false;
  }
  
  loraConnected = true;
  Serial.println("✓ 成功连接到LoRaWAN网络!");
  
  // 设置更高的数据速率以支持更大的数据包
  loraModem.dataRate(3);  // SF9BW125 - 更好的兼容性
  Serial.println("数据速率设置为DR3 (SF9BW125)");
  
  Serial.print("连接耗时: ");
  Serial.print((millis() - startTime) / 1000);
  Serial.println(" 秒");
  
  return true;
}

// ==================== 数据打包 ====================
WaterQualityPacket packWaterQualityData() {
  WaterQualityPacket packet;
  
  // 温度（乘以100保留两位小数）
  packet.temperature = (uint16_t)(waterTemperature * 100);
  
  // pH（乘以100保留两位小数）
  packet.ph = (uint16_t)(pHValue * 100);
  
  // 浊度（乘以10保留一位小数）
  packet.turbidity = (uint16_t)(turbidityNTU * 10);
  
  // 电导率（乘以10保留一位小数）
  packet.conductivity = (uint16_t)(conductivityValue * 10);
  
  // TDS（乘以10保留一位小数）
  packet.tds = (uint16_t)(tdsValue * 10);
  
  return packet;
}

// ==================== 发送数据包 ====================
bool sendDataPacket(const WaterQualityPacket& packet) {
  if (!loraConnected) {
    Serial.println("LoRa未连接到网络");
    return false;
  }
  
  Serial.println("\n=== 发送水质数据到TTN ===");
  Serial.print("数据包大小: ");
  Serial.print(sizeof(packet));
  Serial.println(" 字节");
  
  // 显示要发送的数据
  Serial.print("温度: ");
  Serial.print(packet.temperature / 100.0, 2);
  Serial.println("°C");
  
  Serial.print("pH: ");
  Serial.print(packet.ph / 100.0, 2);
  Serial.println();
  
  Serial.print("浊度: ");
  Serial.print(packet.turbidity / 10.0, 1);
  Serial.println(" NTU");
  
  Serial.print("电导率: ");
  Serial.print(packet.conductivity / 10.0, 1);
  Serial.println(" μS/cm");
  
  Serial.print("TDS: ");
  Serial.print(packet.tds / 10.0, 1);
  Serial.println(" ppm");
  
  // 准备Big-Endian格式的数据包（TTN期望的格式）
  uint8_t payload[10];
  
  // 温度 - Big Endian
  payload[0] = (packet.temperature >> 8) & 0xFF;
  payload[1] = packet.temperature & 0xFF;
  
  // pH - Big Endian
  payload[2] = (packet.ph >> 8) & 0xFF;
  payload[3] = packet.ph & 0xFF;
  
  // 浊度 - Big Endian
  payload[4] = (packet.turbidity >> 8) & 0xFF;
  payload[5] = packet.turbidity & 0xFF;
  
  // 电导率 - Big Endian
  payload[6] = (packet.conductivity >> 8) & 0xFF;
  payload[7] = packet.conductivity & 0xFF;
  
  // TDS - Big Endian
  payload[8] = (packet.tds >> 8) & 0xFF;
  payload[9] = packet.tds & 0xFF;
  
  // 显示发送的字节数据
  Serial.print("发送数据: ");
  for (int i = 0; i < 10; i++) {
    Serial.print("0x");
    if (payload[i] < 16) Serial.print("0");
    Serial.print(payload[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // 发送数据
  loraModem.beginPacket();
  loraModem.write(payload, 10);
  
  int err = loraModem.endPacket(true);  // true = 需要确认
  
  if (err > 0) {
    Serial.println("✓ 数据发送成功!");
    Serial.println("请检查TTN Console获取解码结果");
    loraRetryCount = 0;  // 重置重试计数
    return true;
  } else {
    Serial.print("✗ 发送失败，错误代码: ");
    Serial.println(err);
    loraRetryCount++;
    return false;
  }
}

// ==================== 发送水质数据 ====================
bool sendWaterQualityData() {
  if (!loraConnected) {
    Serial.println("LoRa未连接，尝试重连...");
    if (!reconnectLoRa()) {
      return false;
    }
  }
  
  // 打包数据
  WaterQualityPacket packet = packWaterQualityData();
  
  // 发送数据
  bool success = sendDataPacket(packet);
  
  if (success) {
    lastLoRaSend = millis();
  }
  
  return success;
}

// ==================== 检查是否需要发送（修改版） ====================
bool shouldSendLoRaData() {
  // 如果自动发送被禁用，只允许重试失败的发送
  if (!autoSendEnabled) {
    if (loraRetryCount > 0 && loraRetryCount < LORA_MAX_RETRIES) {
      unsigned long currentTime = millis();
      // 重试间隔：1分钟
      if (currentTime - lastLoRaSend >= 60000) {
        return true;
      }
    }
    return false;  // 不再基于时间间隔自动发送
  }
  
  // 如果自动发送开启，使用原有逻辑
  unsigned long currentTime = millis();
  
  // 检查时间间隔
  if (currentTime - lastLoRaSend >= LORA_SEND_INTERVAL) {
    return true;
  }
  
  // 检查是否有重试需要
  if (loraRetryCount > 0 && loraRetryCount < LORA_MAX_RETRIES) {
    // 重试间隔：1分钟
    if (currentTime - lastLoRaSend >= 60000) {
      return true;
    }
  }
  
  return false;
}

// ==================== 处理LoRa通信（原版） ====================
void handleLoRaCommunication() {
  // 检查下行消息
  if (loraModem.available()) {
    Serial.println("\n收到下行消息:");
    while (loraModem.available()) {
      uint8_t rcv = loraModem.read();
      Serial.print("0x");
      if (rcv < 16) Serial.print("0");
      Serial.print(rcv, HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  // 检查是否需要发送数据
  if (shouldSendLoRaData()) {
    if (loraRetryCount >= LORA_MAX_RETRIES) {
      Serial.println("达到最大重试次数，重置重试计数");
      loraRetryCount = 0;
      lastLoRaSend = millis();  // 重置发送时间
    } else {
      sendWaterQualityData();
    }
  }
}

// ==================== 新增：只处理LoRa接收 ====================
void handleLoRaReceiveOnly() {
  // 只检查下行消息，不自动发送数据
  if (loraModem.available()) {
    Serial.println("\n收到下行消息:");
    while (loraModem.available()) {
      uint8_t rcv = loraModem.read();
      Serial.print("0x");
      if (rcv < 16) Serial.print("0");
      Serial.print(rcv, HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  // 只处理重试逻辑，不处理定时发送
  if (loraRetryCount > 0 && loraRetryCount < LORA_MAX_RETRIES) {
    unsigned long currentTime = millis();
    if (currentTime - lastLoRaSend >= 60000) {  // 1分钟重试间隔
      Serial.println("重试发送失败的数据...");
      sendWaterQualityData();
    }
  }
}

// ==================== 新增：控制自动发送开关 ====================
void enableAutoSend(bool enable) {
  autoSendEnabled = enable;
  Serial.print("LoRa自动发送已");
  Serial.println(enable ? "开启" : "关闭");
  
  if (enable) {
    Serial.print("发送间隔: ");
    Serial.print(LORA_SEND_INTERVAL / 1000);
    Serial.println(" 秒");
  }
}

// ==================== 重连LoRa网络 ====================
bool reconnectLoRa() {
  Serial.println("尝试重连LoRa网络...");
  
  loraConnected = false;
  
  // 重新初始化（如果需要）
  if (!loraInitialized) {
    if (!initializeLoRa()) {
      return false;
    }
  }
  
  // 重新连接
  return connectToNetwork();
}

// ==================== 状态检查 ====================
void printLoRaStatus() {
  Serial.println("\n=== LoRa状态 ===");
  Serial.print("初始化: ");
  Serial.println(loraInitialized ? "是" : "否");
  Serial.print("连接: ");
  Serial.println(loraConnected ? "已连接" : "未连接");
  Serial.print("自动发送: ");
  Serial.println(autoSendEnabled ? "开启" : "关闭");
  Serial.print("重试次数: ");
  Serial.println(loraRetryCount);
  
  if (lastLoRaSend > 0) {
    Serial.print("上次发送: ");
    Serial.print((millis() - lastLoRaSend) / 1000);
    Serial.println(" 秒前");
    
    if (autoSendEnabled && LORA_SEND_INTERVAL > 0) {
      Serial.print("下次发送: ");
      unsigned long nextSend = LORA_SEND_INTERVAL - (millis() - lastLoRaSend);
      if (nextSend > LORA_SEND_INTERVAL) {
        Serial.println("立即");
      } else {
        Serial.print(nextSend / 1000);
        Serial.println(" 秒后");
      }
    } else {
      Serial.println("下次发送: 仅手动触发");
    }
  } else {
    Serial.println("尚未发送过数据");
  }
  
  if (loraInitialized) {
    Serial.print("设备EUI: ");
    Serial.println(loraModem.deviceEUI());
  }
  
  Serial.println("================");
}

// ==================== 诊断信息 ====================
void printLoRaDiagnostics() {
  Serial.println("\n=== LoRa诊断信息 ===");
  
  if (loraInitialized) {
    Serial.print("版本: ");
    Serial.println(loraModem.version());
    Serial.print("设备EUI: ");
    Serial.println(loraModem.deviceEUI());
  } else {
    Serial.println("LoRa模块未初始化");
  }
  
  Serial.print("自动发送状态: ");
  Serial.println(autoSendEnabled ? "开启" : "关闭");
  
  Serial.println("===================");
}