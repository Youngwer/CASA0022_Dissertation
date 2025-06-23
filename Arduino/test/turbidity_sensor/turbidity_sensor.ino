/*
 * ESP32S 浊度传感器简化测试代码
 * 分压电路: 两个6.8kΩ电阻 (1:1分压)
 * 传感器输出: 0-4.5V -> 分压后: 0-2.25V
 */

#define TURBIDITY_PIN 36    // 浊度传感器连接GPIO36
#define LED_PIN 2           // 内置LED

// 分压参数
#define VOLTAGE_DIVIDER_RATIO 2.0  // 两个6.8kΩ电阻，1:1分压

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  // 设置ADC
  analogReadResolution(12);      // 12位分辨率
  analogSetAttenuation(ADC_11db); // 最大输入3.3V
  
  Serial.println("ESP32S 浊度传感器测试 (6.8kΩ分压)");
  Serial.println("传感器预热中...");
  delay(3000);
  Serial.println("开始测量...\n");
}

void loop() {
  // 读取ADC值
  int adcValue = analogRead(TURBIDITY_PIN);
  
  // 转换为ESP32输入电压
  float inputVoltage = (adcValue / 4095.0) * 3.3;
  
  // 计算传感器实际输出电压
  float sensorVoltage = inputVoltage * VOLTAGE_DIVIDER_RATIO;
  
  // 简单的浊度计算 (需要校准)
  // 假设: 清水时电压约4.0V，浑水时电压约1.0V
  float turbidityNTU;
  if(sensorVoltage >= 4.0) {
    turbidityNTU = 0;  // 清水
  } else if(sensorVoltage <= 0.0) {
    turbidityNTU = 1000;  // 最大浊度
  } else {
    // 线性转换
    turbidityNTU = (4.0 - sensorVoltage) / 4.0 * 1000;
  }
  
  // 显示结果
  Serial.println("=== 浊度测量 ===");
  Serial.print("ADC值: ");
  Serial.println(adcValue);
  Serial.print("输入电压: ");
  Serial.print(inputVoltage, 2);
  Serial.println("V");
  Serial.print("传感器电压: ");
  Serial.print(sensorVoltage, 2);
  Serial.println("V");
  Serial.print("浊度: ");
  Serial.print(turbidityNTU, 1);
  Serial.println(" NTU");
  
  // 水质判断
  if(turbidityNTU < 1) {
    Serial.println("水质: 优秀");
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
  } else if(turbidityNTU < 5) {
    Serial.println("水质: 良好");
  } else {
    Serial.println("水质: 较差");
  }
  
  Serial.println("================\n");
  delay(2000);
}