/*
 * 简单的MKR1310 LoRaWAN测试
 * 发送基本传感器数据到TTN
 */

#include <MKRWAN.h>

// LoRaWAN凭证 - 请替换为你的值！
const char *appEui = "ADAFEEEFADAFADEF";
const char *appKey = "DA328F8AD1EA167A0EAA8A9265BE70DD";

LoRaModem modem;

// 测试用的模拟输入引脚
const int sensorPin1 = A0;
const int sensorPin2 = A1;

// 发送间隔
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 60000;  // 1分钟（测试时使用较短间隔）

void setup() {
  Serial.begin(115200);
  
  // 等待串口或超时
  unsigned long startTime = millis();
  while (!Serial && millis() - startTime < 5000) {
    ;
  }
  
  Serial.println("=== MKR1310 LoRaWAN 测试 ===");
  Serial.println("开始初始化...");
  
  // 初始化LoRa模块
  if (!modem.begin(EU868)) {  // 改为你的地区：EU868, US915, AS923, AU915, KR920, IN865
    Serial.println("LoRa模块初始化失败!");
    while (1) {}
  }
  
  Serial.println("LoRa模块初始化成功");
  Serial.print("设备EUI: ");
  Serial.println(modem.deviceEUI());
  Serial.print("版本: ");
  Serial.println(modem.version());
  
  // 连接到网络
  Serial.println("\n尝试加入网络...");
  
  int connected = modem.joinOTAA(appEui, appKey);
  
  if (!connected) {
    Serial.println("加入网络失败! 请检查:");
    Serial.println("1. App EUI 和 App Key 是否正确");
    Serial.println("2. 是否有网关覆盖");
    Serial.println("3. 频段设置是否正确");
    while (1) {}
  }
  
  Serial.println("成功加入网络!");
  
  // 设置数据速率和自适应数据速率
  modem.dataRate(5);  // SF7BW125
  // modem.setADR(true);  // 启用自适应数据速率（可选）
  
  delay(1000);
  Serial.println("\n准备发送数据...");
}

void loop() {
  unsigned long currentTime = millis();
  
  // 检查是否到了发送时间
  if (currentTime - lastSendTime >= sendInterval) {
    lastSendTime = currentTime;
    
    // 读取模拟传感器
    int sensor1Value = analogRead(sensorPin1);
    int sensor2Value = analogRead(sensorPin2);
    
    // 转换为电压
    float voltage1 = sensor1Value * (3.3 / 1023.0);
    float voltage2 = sensor2Value * (3.3 / 1023.0);
    
    // 显示读数
    Serial.println("\n--- 传感器读数 ---");
    Serial.print("传感器1: ");
    Serial.print(sensor1Value);
    Serial.print(" (");
    Serial.print(voltage1, 2);
    Serial.println("V)");
    
    Serial.print("传感器2: ");
    Serial.print(sensor2Value);
    Serial.print(" (");
    Serial.print(voltage2, 2);
    Serial.println("V)");
    
    // 准备数据包
    byte payload[4];
    
    // 传感器1值（2字节）
    payload[0] = (sensor1Value >> 8) & 0xFF;
    payload[1] = sensor1Value & 0xFF;
    
    // 传感器2值（2字节）
    payload[2] = (sensor2Value >> 8) & 0xFF;
    payload[3] = sensor2Value & 0xFF;
    
    // 发送数据
    Serial.println("\n发送数据到TTN...");
    
    modem.beginPacket();
    modem.write(payload, sizeof(payload));
    
    int err = modem.endPacket(true);  // true = 需要确认
    
    if (err > 0) {
      Serial.println("消息发送成功!");
    } else {
      Serial.println("发送错误: " + String(err));
    }
  }
  
  // 检查下行消息
  if (modem.available()) {
    Serial.println("\n收到下行消息:");
    while (modem.available()) {
      uint8_t rcv = modem.read();
      Serial.print("0x");
      if (rcv < 16) Serial.print("0");
      Serial.print(rcv, HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

/*
 * TTN解码器 (Payload Formatter)
 * 
 * function Decoder(bytes, port) {
 *   var decoded = {};
 *   
 *   decoded.sensor1 = (bytes[0] << 8) | bytes[1];
 *   decoded.voltage1 = decoded.sensor1 * (3.3 / 1023.0);
 *   
 *   decoded.sensor2 = (bytes[2] << 8) | bytes[3];
 *   decoded.voltage2 = decoded.sensor2 * (3.3 / 1023.0);
 *   
 *   return decoded;
 * }
 */