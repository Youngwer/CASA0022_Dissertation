/*
 * ESP32 浊度传感器测试代码
 * 适用于模拟输出浊度传感器 (0-1000 NTU)
 * 电压范围: 3.3-5V
 * 输出: 模拟信号 (0-4.5V)
 */

// 定义引脚
#define TURBIDITY_PIN 36    // 浊度传感器连接到GPIO36 (VP引脚)
#define LED_PIN 2           // 内置蓝色LED引脚 (GPIO2)

// 校准参数 (需要根据实际校准液进行调整)
#define CLEAR_WATER_VOLTAGE 2.2    // 清水时的电压值 (V)
#define MAX_TURBIDITY_VOLTAGE 1.0  // 最大浊度时的电压值 (V)

void setup() {
  // 初始化串口通信
  Serial.begin(115200);
  Serial.println("ESP32 浊度传感器测试开始");
  Serial.println("========================================");
  
  // 设置LED引脚
  pinMode(LED_PIN, OUTPUT);
  
  // 设置ADC分辨率
  analogReadResolution(12);  // 12位分辨率 (0-4095)
  
  // 等待传感器稳定
  Serial.println("等待传感器稳定...");
  delay(2000);
  Serial.println("开始测量...");
  Serial.println();
}

void loop() {
  // 读取传感器数据
  float turbidityNTU = readTurbidity();
  
  // 显示结果
  displayResults(turbidityNTU);
  
  // 根据浊度值控制LED指示
  //controlLED(turbidityNTU);
  
  // 等待1秒再次测量
  delay(1000);
}

float readTurbidity() {
  // 多次采样求平均值，提高精度
  long sum = 0;
  int samples = 50;
  
  for(int i = 0; i < samples; i++) {
    sum += analogRead(TURBIDITY_PIN);
    delay(10);
  }
  
  // 计算平均ADC值
  float avgADC = sum / samples;
  
  // 转换为电压值 (ESP32 ADC参考电压为3.3V)
  float voltage = (avgADC / 4095.0) * 3.3;
  
  // 转换为浊度值 (NTU)
  // 注意：这个转换公式需要根据实际校准进行调整
  float turbidityNTU;
  
  if(voltage > CLEAR_WATER_VOLTAGE) {
    // 防止超出范围
    turbidityNTU = 0;
  } else if(voltage < MAX_TURBIDITY_VOLTAGE) {
    // 防止超出范围
    turbidityNTU = 1000;
  } else {
    // 线性插值计算浊度值
    turbidityNTU = (CLEAR_WATER_VOLTAGE - voltage) / 
                   (CLEAR_WATER_VOLTAGE - MAX_TURBIDITY_VOLTAGE) * 1000;
  }
  
  return turbidityNTU;
}

void displayResults(float turbidityNTU) {
  // 获取原始ADC值和电压值用于调试
  int rawADC = analogRead(TURBIDITY_PIN);
  float voltage = (rawADC / 4095.0) * 3.3;
  
  // 显示测量结果
  Serial.println("=== 浊度传感器读数 ===");
  Serial.print("原始ADC值: ");
  Serial.println(rawADC);
  Serial.print("电压值: ");
  Serial.print(voltage, 3);
  Serial.println(" V");
  Serial.print("浊度值: ");
  Serial.print(turbidityNTU, 2);
  Serial.println(" NTU");
  
  // 水质评估
  String waterQuality;
  if(turbidityNTU < 1) {
    waterQuality = "优秀 (非常清澈)";
  } else if(turbidityNTU < 5) {
    waterQuality = "良好 (清澈)";
  } else if(turbidityNTU < 25) {
    waterQuality = "一般 (轻微浑浊)";
  } else if(turbidityNTU < 100) {
    waterQuality = "较差 (浑浊)";
  } else {
    waterQuality = "很差 (非常浑浊)";
  }
  
  Serial.print("水质评估: ");
  Serial.println(waterQuality);
  Serial.println("----------------------------------------");
  Serial.println();
}

// void controlLED(float turbidityNTU) {
//   // 根据浊度值控制LED闪烁频率
//   if(turbidityNTU < 5) {
//     // 水质良好，LED慢闪
//     digitalWrite(LED_PIN, HIGH);
//     delay(100);
//     digitalWrite(LED_PIN, LOW);
//   } else if(turbidityNTU < 25) {
//     // 水质一般，LED中速闪
//     digitalWrite(LED_PIN, HIGH);
//     delay(50);
//     digitalWrite(LED_PIN, LOW);
//     delay(50);
//     digitalWrite(LED_PIN, HIGH);
//     delay(50);
//     digitalWrite(LED_PIN, LOW);
//   } else {
//     // 水质较差，LED快闪
//     for(int i = 0; i < 5; i++) {
//       digitalWrite(LED_PIN, HIGH);
//       delay(100);
//       digitalWrite(LED_PIN, LOW);
//       delay(100);
//     }
//   }
// }

// 校准函数 - 在setup()中调用进行校准
void calibrateSensor() {
  Serial.println("=== 传感器校准模式 ===");
  Serial.println("请将传感器放入清水中，等待10秒...");
  
  delay(10000);
  
  // 测量清水时的电压
  long sum = 0;
  for(int i = 0; i < 100; i++) {
    sum += analogRead(TURBIDITY_PIN);
    delay(50);
  }
  
  float clearWaterADC = sum / 100.0;
  float clearWaterVoltage = (clearWaterADC / 4095.0) * 3.3;
  
  Serial.print("清水校准电压: ");
  Serial.print(clearWaterVoltage, 3);
  Serial.println(" V");
  Serial.println("请将此值更新到代码中的 CLEAR_WATER_VOLTAGE 常量");
  Serial.println("校准完成！");
  Serial.println("========================================");
}