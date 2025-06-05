/*
 * ESP32S 浊度传感器测试代码 - 6.8kΩ + 6.8kΩ 分压电路专用版
 * 传感器规格：
 * - 工作电压: 5V (ESP32 VIN引脚供电)
 * - 输出电压: 0-4.5V (模拟信号)
 * - 测量范围: 0-1000 NTU
 * - 分压电路: 6.8kΩ + 6.8kΩ (输出2.25V最大)
 */

// 引脚定义
#define TURBIDITY_PIN 36    // 浊度传感器模拟输出连接到GPIO36 (ADC1_CH0)
#define LED_PIN 2           // 内置LED用于状态指示

// 分压电路配置 - 6.8kΩ + 6.8kΩ
#define R1 6.8              // 上电阻 (kΩ)
#define R2 6.8              // 下电阻 (kΩ)

// 根据实际测量数据的校准参数
#define CLEAR_WATER_VOLTAGE 4.5    // 0 NTU时传感器电压 (V)
#define HIGH_TURBIDITY_VOLTAGE 3.25 // 高浊度时传感器电压 (V) - 约678 NTU
#define HIGH_TURBIDITY_NTU 678.0   // 对应的NTU值

// 电压转换参数
#define ADC_RESOLUTION 4095.0      // ESP32 12位ADC分辨率
#define ESP32_MAX_VOLTAGE 3.3      // ESP32 ADC最大输入电压
#define SENSOR_MAX_VOLTAGE 4.5     // 传感器最大输出电压

// 分压比例计算
float voltage_divider_ratio = R2 / (R1 + R2); // 6.8/(6.8+6.8) = 0.5

void setup() {
  // 初始化串口
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("========================================");
  Serial.println("ESP32S 浊度传感器测试程序");
  Serial.println("分压电路: 6.8kΩ + 6.8kΩ");
  Serial.println("========================================");
  
  // 设置引脚
  pinMode(LED_PIN, OUTPUT);
  
  // 配置ADC
  analogReadResolution(12);           // 12位分辨率
  analogSetAttenuation(ADC_11db);     // 设置衰减，最大输入约3.3V
  
  // 显示分压电路信息
  displayCircuitInfo();
  
  // 传感器预热
  Serial.println("传感器预热中，请等待10秒...");
  for(int i = 10; i > 0; i--) {
    Serial.print("预热倒计时: ");
    Serial.print(i);
    Serial.println(" 秒");
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  
  Serial.println("传感器预热完成，开始测量...");
  Serial.println("使用基于实际校准数据的转换算法");
  Serial.println("校准点: 4.5V->0NTU, 3.97V->126NTU, 3.25V->678NTU");
  Serial.println();
  
  // 初始电压检查
  performSafetyCheck();
}

void loop() {
  // 读取浊度值
  float turbidityNTU = readTurbidity();
  
  // 显示测量结果
  displayMeasurement(turbidityNTU);
  
  // 水质状态指示
  indicateWaterQuality(turbidityNTU);
  
  delay(2000); // 每2秒测量一次
}

void displayCircuitInfo() {
  Serial.println("*** 分压电路配置信息 ***");
  Serial.print("上电阻 R1: ");
  Serial.print(R1);
  Serial.println(" kΩ");
  Serial.print("下电阻 R2: ");
  Serial.print(R2);
  Serial.println(" kΩ");
  Serial.print("分压比例: ");
  Serial.println(voltage_divider_ratio, 3);
  
  float maxInputVoltage = SENSOR_MAX_VOLTAGE * voltage_divider_ratio;
  Serial.print("传感器4.5V时ESP32输入: ");
  Serial.print(maxInputVoltage, 2);
  Serial.println(" V");
  
  if(maxInputVoltage <= 3.0) {
    Serial.println("✓ 电压安全 - 不会损坏ESP32");
  } else if(maxInputVoltage <= 3.3) {
    Serial.println("⚠ 电压临界 - 请谨慎使用");
  } else {
    Serial.println("✗ 电压危险 - 可能损坏ESP32！");
  }
  Serial.println();
}

void performSafetyCheck() {
  Serial.println("*** 安全检查 ***");
  Serial.println("正在检查电路电压...");
  
  // 读取5次取平均
  float voltageSum = 0;
  for(int i = 0; i < 5; i++) {
    int adc = analogRead(TURBIDITY_PIN);
    float esp32_voltage = (adc / ADC_RESOLUTION) * ESP32_MAX_VOLTAGE;
    voltageSum += esp32_voltage;
    delay(200);
  }
  
  float avgESP32Voltage = voltageSum / 5.0;
  float estimatedSensorVoltage = avgESP32Voltage / voltage_divider_ratio;
  
  Serial.print("ESP32测量电压: ");
  Serial.print(avgESP32Voltage, 3);
  Serial.println(" V");
  Serial.print("推算传感器电压: ");
  Serial.print(estimatedSensorVoltage, 3);
  Serial.println(" V");
  
  if(avgESP32Voltage > 3.0) {
    Serial.println("⚠ 警告：输入电压过高！请检查分压电路！");
    Serial.println("建议：立即断电检查连线");
  } else {
    Serial.println("✓ 电压检查通过，可以安全使用");
  }
  Serial.println("========================================");
  Serial.println();
}

float readTurbidity() {
  // 多次采样平均，提高测量精度
  long adcSum = 0;
  int samples = 100;
  
  for(int i = 0; i < samples; i++) {
    adcSum += analogRead(TURBIDITY_PIN);
    delay(10);
  }
  
  // 计算平均ADC值
  float avgADC = adcSum / (float)samples;
  
  // 将ADC值转换为ESP32输入电压 (0-3.3V)
  float esp32_voltage = (avgADC / ADC_RESOLUTION) * ESP32_MAX_VOLTAGE;
  
  // 计算传感器实际输出电压
  float sensor_voltage = esp32_voltage / voltage_divider_ratio;
  
  // 电压限制保护
  if(sensor_voltage > SENSOR_MAX_VOLTAGE) {
    sensor_voltage = SENSOR_MAX_VOLTAGE;
  }
  if(sensor_voltage < 0) {
    sensor_voltage = 0;
  }
  
  // 使用多点校准数据进行更精确的转换
  float turbidityNTU = calculateTurbidityFromVoltage(sensor_voltage);
  
  return turbidityNTU;
}

// 基于实际测量数据的浊度计算函数
float calculateTurbidityFromVoltage(float voltage) {
  // 实际测量的校准点数据
  // 电压高 -> 浊度低 (清水)
  // 电压低 -> 浊度高 (浑水)
  
  if(voltage >= CLEAR_WATER_VOLTAGE) {
    // 电压等于或高于清水电压，认为是0 NTU
    return 0.0;
  }
  else if(voltage <= HIGH_TURBIDITY_VOLTAGE) {
    // 电压等于或低于高浊度电压，使用最大值
    return HIGH_TURBIDITY_NTU;
  }
  else {
    // 在测量范围内，使用线性插值
    // 根据你的数据：4.5V->0 NTU, 3.25V->678 NTU
    float turbidityNTU = (CLEAR_WATER_VOLTAGE - voltage) / 
                         (CLEAR_WATER_VOLTAGE - HIGH_TURBIDITY_VOLTAGE) * HIGH_TURBIDITY_NTU;
    return turbidityNTU;
  }
}

// 更精确的多点插值函数 (可选使用)
float calculateTurbidityMultiPoint(float voltage) {
  // 基于你提供的多个校准点进行分段线性插值
  
  // 校准点数据 (电压, NTU)
  float calibrationPoints[][2] = {
    {4.50, 0.00},     // 清水
    {3.97, 126.14},   // 低浊度
    {3.68, 395.60},   // 中浊度
    {3.58, 418.20},   // 中高浊度
    {3.25, 678.00}    // 高浊度
  };
  
  int numPoints = 5;
  
  // 找到电压所在的区间进行插值
  for(int i = 0; i < numPoints - 1; i++) {
    float v1 = calibrationPoints[i][0];
    float ntu1 = calibrationPoints[i][1];
    float v2 = calibrationPoints[i+1][0];
    float ntu2 = calibrationPoints[i+1][1];
    
    if(voltage >= v2 && voltage <= v1) {
      // 在这个区间内，进行线性插值
      float ratio = (v1 - voltage) / (v1 - v2);
      return ntu1 + ratio * (ntu2 - ntu1);
    }
  }
  
  // 超出范围的处理
  if(voltage > 4.50) return 0.0;        // 超清水
  if(voltage < 3.25) return 1000.0;     // 超浑浊
  
  return 0.0; // 默认返回
}

void displayMeasurement(float turbidityNTU) {
  // 获取当前测量的原始数据用于显示
  int currentADC = analogRead(TURBIDITY_PIN);
  float currentESP32Voltage = (currentADC / ADC_RESOLUTION) * ESP32_MAX_VOLTAGE;
  float currentSensorVoltage = currentESP32Voltage / voltage_divider_ratio;
  
  Serial.println("=== 浊度传感器测量结果 ===");
  Serial.print("ADC原始值: ");
  Serial.print(currentADC);
  Serial.print(" / ");
  Serial.println((int)ADC_RESOLUTION);
  
  Serial.print("ESP32输入电压: ");
  Serial.print(currentESP32Voltage, 3);
  Serial.println(" V");
  
  Serial.print("传感器输出电压: ");
  Serial.print(currentSensorVoltage, 3);
  Serial.println(" V");
  
  Serial.print("浊度值: ");
  Serial.print(turbidityNTU, 2);
  Serial.println(" NTU");
  
  // 水质评估
  String quality = getWaterQuality(turbidityNTU);
  Serial.print("水质评估: ");
  Serial.println(quality);
  
  // 饮用水标准对比 (基于实际NTU范围)
  if(turbidityNTU <= 1.0) {
    Serial.println("状态: ✓ 符合饮用水标准 (WHO: <1 NTU)");
  } else if(turbidityNTU <= 4.0) {
    Serial.println("状态: ⚠ 可接受范围 (EU标准: <4 NTU)");
  } else if(turbidityNTU <= 25.0) {
    Serial.println("状态: ⚠ 需要过滤处理");
  } else if(turbidityNTU <= 100.0) {
    Serial.println("状态: ✗ 水质较差，需要深度处理");
  } else {
    Serial.println("状态: ✗ 水质很差，不适合直接使用");
  }
  
  Serial.println("========================================");
  Serial.println();
}

String getWaterQuality(float ntu) {
  if(ntu < 1.0) return "优秀 - 符合饮用水标准";
  else if(ntu < 4.0) return "良好 - 可接受范围";
  else if(ntu < 25.0) return "一般 - 需要简单过滤";
  else if(ntu < 100.0) return "较差 - 需要深度处理";
  else if(ntu < 300.0) return "差 - 工业用水级别";
  else return "极差 - 严重污染";
}

void indicateWaterQuality(float turbidityNTU) {
  // LED指示水质状态
  if(turbidityNTU <= 1.0) {
    // 水质优良 - LED慢闪绿灯效果
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
  } else if(turbidityNTU <= 4.0) {
    // 水质一般 - LED中速闪烁
    for(int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(150);
      digitalWrite(LED_PIN, LOW);
      delay(150);
    }
  } else {
    // 水质较差 - LED快闪
    for(int i = 0; i < 6; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(80);
      digitalWrite(LED_PIN, LOW);
      delay(80);
    }
  }
}

// 校准功能 - 在setup()中调用进行手动校准
void calibrationMode() {
  Serial.println("=== 进入校准模式 ===");
  Serial.println("准备校准液：0 NTU (蒸馏水), 100 NTU, 800 NTU");
  Serial.println();
  
  // 0 NTU校准
  Serial.println("第1步：0 NTU 校准");
  Serial.println("请将传感器放入0 NTU校准液（蒸馏水）中");
  Serial.println("等待15秒稳定...");
  delay(15000);
  
  float sum = 0;
  Serial.println("正在测量...");
  for(int i = 0; i < 200; i++) {
    int adc = analogRead(TURBIDITY_PIN);
    float esp32_voltage = (adc / ADC_RESOLUTION) * ESP32_MAX_VOLTAGE;
    float sensor_voltage = esp32_voltage / voltage_divider_ratio;
    sum += sensor_voltage;
    delay(50);
    
    if(i % 40 == 0) {
      Serial.print(".");
    }
  }
  Serial.println();
  
  float clearWaterVoltage = sum / 200.0;
  Serial.print("0 NTU 校准电压: ");
  Serial.print(clearWaterVoltage, 3);
  Serial.println(" V");
  Serial.print("建议更新代码中 CLEAR_WATER_VOLTAGE = ");
  Serial.println(clearWaterVoltage, 3);
  Serial.println();
  
  // 高浊度校准 (可选)
  Serial.println("第2步：高浊度校准 (可选)");
  Serial.println("请将传感器放入800 NTU校准液中，然后重复上述过程");
  Serial.println("或者直接使用默认的 MAX_TURBIDITY_VOLTAGE = 0.5V");
  Serial.println();
  
  Serial.println("校准完成！请根据结果更新代码中的校准常量");
  Serial.println("========================================");
}

// 故障诊断功能
void diagnosticMode() {
  Serial.println("=== 故障诊断模式 ===");
  
  Serial.println("检查1: 电源供电");
  Serial.println("传感器VCC应连接到ESP32 VIN (5V)");
  
  Serial.println("检查2: 分压电路");
  Serial.println("应该有两个6.8kΩ电阻串联");
  
  Serial.println("检查3: 信号线连接");
  Serial.println("传感器OUT -> 6.8kΩ -> [中间点] -> 6.8kΩ -> GND");
  Serial.println("ESP32 GPIO36 应连接到[中间点]");
  
  Serial.println("检查4: 实时电压监测");
  for(int i = 0; i < 10; i++) {
    int adc = analogRead(TURBIDITY_PIN);
    float voltage = (adc / ADC_RESOLUTION) * ESP32_MAX_VOLTAGE;
    float sensor_voltage = voltage / voltage_divider_ratio;
    
    Serial.print("第");
    Serial.print(i+1);
    Serial.print("次: ADC=");
    Serial.print(adc);
    Serial.print(", ESP32输入=");
    Serial.print(voltage, 3);
    Serial.print("V, 传感器输出=");
    Serial.print(sensor_voltage, 3);
    Serial.println("V");
    delay(1000);
  }
  
  Serial.println("========================================");
}

// 自动水质分级功能
void displayDetailedAnalysis(float turbidityNTU) {
  Serial.println("=== 详细水质分析 ===");
  
  Serial.print("浊度值: ");
  Serial.print(turbidityNTU, 2);
  Serial.println(" NTU");
  
  // WHO标准
  if(turbidityNTU <= 1.0) {
    Serial.println("WHO标准: ✓ 合格 (<1 NTU)");
  } else {
    Serial.println("WHO标准: ✗ 不合格 (>1 NTU)");
  }
  
  // EU标准
  if(turbidityNTU <= 4.0) {
    Serial.println("EU标准: ✓ 合格 (<4 NTU)");
  } else {
    Serial.println("EU标准: ✗ 不合格 (>4 NTU)");
  }
  
  // 用途建议
  Serial.print("适用用途: ");
  if(turbidityNTU <= 0.5) {
    Serial.println("直饮、医疗用水");
  } else if(turbidityNTU <= 1.0) {
    Serial.println("直饮、烹饪");
  } else if(turbidityNTU <= 4.0) {
    Serial.println("简单过滤后饮用");
  } else if(turbidityNTU <= 25.0) {
    Serial.println("工业用水、清洗");
  } else {
    Serial.println("需要深度处理");
  }
  
  Serial.println("========================================");
}