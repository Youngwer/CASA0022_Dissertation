/*
 * 修正版 MKR WAN 1310 TDS + DS18B20 温度传感器代码
 * 基于官方文档但修正了电压参考和滤波算法
 * TDS传感器: A2引脚
 * DS18B20: D2引脚
 */

#include <OneWire.h>
#include <DallasTemperature.h>

// 硬件定义
#define TDS_SENSOR_PIN A2
#define ONE_WIRE_BUS 2

// 修正的电压参考 - 适配MKR WAN 1310
#define VREF 3.3              // MKR WAN 1310的实际ADC参考电压
#define ADC_RESOLUTION 1024.0 // 10位ADC分辨率
#define SCOUNT 30             // 采样点数量

// 温度传感器设置
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temperatureSensor(&oneWire);

// 滤波缓冲区
int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;

// 测量变量
float averageVoltage = 0;
float tdsValue = 0;
float waterTemperature = 25.0;
bool temperatureSensorFound = false;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // 等待串口连接
  }
  
  pinMode(TDS_SENSOR_PIN, INPUT);
  temperatureSensor.begin();
  
  Serial.println("========================================");
  Serial.println("修正版 MKR WAN 1310 TDS监测系统");
  Serial.println("使用中值滤波和温度补偿");
  Serial.println("TDS传感器: A2 | DS18B20: D2");
  Serial.println("========================================");
  
  // 检测温度传感器
  detectTemperatureSensor();
  
  // 初始化缓冲区
  for (int i = 0; i < SCOUNT; i++) {
    analogBuffer[i] = 0;
  }
  
  Serial.println("系统初始化完成，开始监测...");
  Serial.println();
}

void loop() {
  // 每40ms采样一次 (与官方文档保持一致)
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TDS_SENSOR_PIN);
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
    }
  }
  
  // 每800ms输出一次结果
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    
    // 读取温度
    readTemperature();
    
    // 计算TDS
    calculateTDS();
    
    // 输出结果
    printResults();
  }
}

void detectTemperatureSensor() {
  Serial.print("检测DS18B20温度传感器...");
  
  int deviceCount = temperatureSensor.getDeviceCount();
  
  if (deviceCount > 0) {
    temperatureSensorFound = true;
    temperatureSensor.setResolution(12);
    Serial.println(" 成功!");
  } else {
    temperatureSensorFound = false;
    Serial.println(" 未检测到!");
    Serial.println("将使用默认温度25℃");
  }
}

void readTemperature() {
  if (temperatureSensorFound) {
    temperatureSensor.requestTemperatures();
    float tempC = temperatureSensor.getTempCByIndex(0);
    
    if (tempC != DEVICE_DISCONNECTED_C && tempC > -50 && tempC < 100) {
      waterTemperature = tempC;
    }
  }
}

void calculateTDS() {
  // 复制缓冲区进行中值滤波
  for (int i = 0; i < SCOUNT; i++) {
    analogBufferTemp[i] = analogBuffer[i];
  }
  
  // 使用中值滤波获得稳定的ADC值
  int medianADC = getMedianNum(analogBufferTemp, SCOUNT);
  
  // 修正的电压计算 - 使用3.3V参考电压
  averageVoltage = medianADC * VREF / ADC_RESOLUTION;
  
  // 温度补偿
  float compensationCoefficient = 1.0 + 0.02 * (waterTemperature - 25.0);
  float compensationVoltage = averageVoltage / compensationCoefficient;
  
  // TDS计算 (官方公式)
  tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage 
             - 255.86 * compensationVoltage * compensationVoltage 
             + 857.39 * compensationVoltage) * 0.5;
  
  // 确保TDS值有效
  if (tdsValue < 0) {
    tdsValue = 0;
  }
}

// 官方中值滤波算法 (已修正)
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  
  // 复制数组
  for (int i = 0; i < iFilterLen; i++) {
    bTab[i] = bArray[i];
  }
  
  // 冒泡排序
  for (int j = 0; j < iFilterLen - 1; j++) {
    for (int i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        int bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  
  // 返回中值
  int median;
  if ((iFilterLen & 1) > 0) {
    median = bTab[(iFilterLen - 1) / 2];
  } else {
    median = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  
  return median;
}

void printResults() {
  Serial.println("=== 高精度水质监测 ===");
  
  // 显示温度
  Serial.print("水温: ");
  Serial.print(waterTemperature, 2);
  Serial.print("℃");
  if (temperatureSensorFound) {
    Serial.println(" (实测)");
  } else {
    Serial.println(" (默认)");
  }
  
  // 显示详细的TDS计算过程
  Serial.print("中值ADC: ");
  Serial.println(getMedianNum(analogBufferTemp, SCOUNT));
  
  Serial.print("电压值: ");
  Serial.print(averageVoltage, 4);
  Serial.println("V");
  
  Serial.print("补偿系数: ");
  Serial.println(1.0 + 0.02 * (waterTemperature - 25.0), 4);
  
  Serial.print("补偿电压: ");
  Serial.print(averageVoltage / (1.0 + 0.02 * (waterTemperature - 25.0)), 4);
  Serial.println("V");
  
  Serial.print("TDS值: ");
  Serial.print(tdsValue, 1);
  Serial.println(" ppm");
  
  // 评估水质
  evaluateWaterQuality();
  
  Serial.println("========================================");
}

void evaluateWaterQuality() {
  Serial.print("水质评级: ");
  
  if (tdsValue < 50) {
    Serial.println("纯净水 (< 50 ppm)");
  } else if (tdsValue < 150) {
    Serial.println("优质 (50-150 ppm) ✅");
  } else if (tdsValue < 300) {
    Serial.println("良好 (150-300 ppm) ✅");
  } else if (tdsValue < 500) {
    Serial.println("可接受 (300-500 ppm) ⚠️");
  } else if (tdsValue < 1000) {
    Serial.println("较差 (500-1000 ppm) ❌");
  } else {
    Serial.println("很差 (> 1000 ppm) ❌");
  }
  
  // 测量精度指示
  Serial.print("测量精度: ");
  if (temperatureSensorFound) {
    Serial.println("高 (温度补偿+中值滤波)");
  } else {
    Serial.println("中等 (仅中值滤波)");
  }
}

// 调试函数：显示滤波效果
void showFilteringEffect() {
  Serial.println("=== 滤波效果对比 ===");
  
  // 计算简单平均值
  long sum = 0;
  for (int i = 0; i < SCOUNT; i++) {
    sum += analogBuffer[i];
  }
  float simpleAverage = sum / (float)SCOUNT;
  
  // 获取中值
  for (int i = 0; i < SCOUNT; i++) {
    analogBufferTemp[i] = analogBuffer[i];
  }
  int medianValue = getMedianNum(analogBufferTemp, SCOUNT);
  
  Serial.print("简单平均: ");
  Serial.print(simpleAverage, 1);
  Serial.print(" | 中值: ");
  Serial.print(medianValue);
  Serial.print(" | 差异: ");
  Serial.println(abs(simpleAverage - medianValue), 1);
}