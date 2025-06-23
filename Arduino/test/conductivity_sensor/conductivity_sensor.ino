/*
  ESP32 Water Conductivity Sensor Test
  适用于论文项目的水质监测系统
*/

// 引脚定义
#define CONDUCTIVITY_PIN 34  // 使用ADC1_CH6引脚
#define POWER_PIN 2          // 可选：用于控制传感器电源

// 校准参数 (需要根据实际传感器调整)
float voltage_coefficient = 3.3 / 4095.0;  // ESP32 ADC参考电压
float conductivity_coefficient = 1.0;       // 电导率转换系数，需校准

void setup() {
  Serial.begin(115200);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);  // 打开传感器电源
  
  // ADC配置
  analogReadResolution(12);  // 12位分辨率
  analogSetAttenuation(ADC_11db);  // 最大输入电压约3.3V
  
  Serial.println("ESP32 Water Conductivity Sensor Test");
  Serial.println("传感器预热中...");
  delay(2000);  // 传感器预热时间
}

void loop() {
  // 读取传感器数值
  int sensorValue = analogRead(CONDUCTIVITY_PIN);
  float voltage = sensorValue * voltage_coefficient;
  
  // 转换为电导率值 (需要根据传感器规格书调整公式)
  float conductivity = voltage * conductivity_coefficient;
  
  // 输出读数
  Serial.println("=== 水质传感器读数 ===");
  Serial.print("ADC原始值: ");
  Serial.println(sensorValue);
  Serial.print("电压值: ");
  Serial.print(voltage, 3);
  Serial.println(" V");
  Serial.print("电导率: ");
  Serial.print(conductivity, 2);
  Serial.println(" μS/cm");
  
  // 基于相对值的水质评估（无需校准）
  String waterType = "";
  String quality = "";
  
  if (sensorValue < 500) {
    waterType = "极纯净水";
    quality = "excellent";
  } else if (sensorValue < 1000) {
    waterType = "纯净水";
    quality = "good";
  } else if (sensorValue < 2000) {
    waterType = "普通饮用水";
    quality = "acceptable";
  } else if (sensorValue < 3000) {
    waterType = "硬水/矿物质较多";
    quality = "marginal";
  } else {
    waterType = "水质较差";
    quality = "poor";
  }
  
  Serial.print("水质类型: ");
  Serial.println(waterType);
  Serial.print("质量等级: ");
  Serial.println(quality);
  
  // LED颜色指示（适合你的论文项目）
  if (quality == "excellent" || quality == "good") {
    Serial.println("LED: 绿色 ✓");
  } else if (quality == "acceptable") {
    Serial.println("LED: 黄色 ⚠");
  } else {
    Serial.println("LED: 红色 ✗");
  }
  
  Serial.println("===================");
  delay(2000);  // 每2秒读取一次
}

// 校准函数 - 使用已知电导率溶液进行校准
void calibrate() {
  Serial.println("开始校准程序...");
  Serial.println("请将传感器放入已知电导率的标准溶液中");
  
  delay(5000);  // 等待5秒稳定
  
  float sum = 0;
  int samples = 10;
  
  for (int i = 0; i < samples; i++) {
    sum += analogRead(CONDUCTIVITY_PIN);
    delay(100);
  }
  
  float average = sum / samples;
  Serial.print("校准读数平均值: ");
  Serial.println(average);
  
  // 根据标准溶液调整coefficient
  // conductivity_coefficient = known_conductivity / (average * voltage_coefficient);
}