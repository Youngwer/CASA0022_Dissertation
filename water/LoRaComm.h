/**
 * LoRaComm.h - LoRa通信模块头文件
 * 
 * 处理水质数据的LoRaWAN上传功能
 * 适配MKR WAN1310开发板
 */

#ifndef LORA_COMM_H
#define LORA_COMM_H

#include <Arduino.h>
#include <MKRWAN.h>

// ==================== LoRaWAN配置 ====================
// 请替换为你的TTN应用凭证
#define APP_EUI "ADAFEEEFADAFADEF"
#define APP_KEY "DA328F8AD1EA167A0EAA8A9265BE70DD"

// LoRa发送配置
#define LORA_SEND_INTERVAL 300000  // 5分钟发送一次（调试时可改为60000=1分钟）
#define LORA_MAX_RETRIES 3
#define LORA_TIMEOUT 30000

// ==================== 数据包结构 ====================
// 完整的水质数据包（10字节，包含所有主要参数）
struct WaterQualityPacket {
  uint16_t temperature;    // 温度 * 100 (例: 25.67°C = 2567)
  uint16_t ph;            // pH * 100 (例: 7.45 = 745) 
  uint16_t turbidity;     // 浊度 * 10 (例: 15.3 NTU = 153)
  uint16_t conductivity;  // 电导率 (μS/cm)
  uint16_t tds;          // TDS (ppm)
};

// ==================== 全局变量声明 ====================
extern LoRaModem loraModem;
extern bool loraInitialized;
extern bool loraConnected;
extern unsigned long lastLoRaSend;
extern int loraRetryCount;

// ==================== 函数声明 ====================
// 主要函数
bool initializeLoRa();
bool connectToNetwork();
bool sendWaterQualityData();
bool shouldSendLoRaData();
void handleLoRaCommunication();
void printLoRaStatus();

// 辅助函数
WaterQualityPacket packWaterQualityData();
bool sendDataPacket(const WaterQualityPacket& packet);
bool reconnectLoRa();
void printLoRaDiagnostics();

#endif // LORA_COMM_H