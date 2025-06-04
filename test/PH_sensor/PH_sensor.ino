const int pH_Pin = 34; // ESP32的D34引脚
const int numReadings = 10; // 平均滤波的读数次数
float pH_Value, voltage;

void setup() {
  Serial.begin(115200); // 初始化串口，波特率115200
  pinMode(pH_Pin, INPUT); // 设置D34为输入
  delay(1000); // 等待串口稳定
  Serial.println("pH传感器测试开始");
}

void loop() {
  // 取多次读数求平均以减少噪声
  long sum = 0;
  for (int i = 0; i < numReadings; i++) {
    sum += analogRead(pH_Pin);
    delay(10); // 每次读数间隔10ms
  }
  int analogValue = sum / numReadings;
  
  // 转换为电压（ESP32 ADC范围：0-3.3V，12位）
  voltage = analogValue * (3.3 / 4095.0);
  
  // 使用新校准公式转换为pH值
  pH_Value = 12.020 * voltage - 15.287;
  
  // 限制pH值在0-14
  if (pH_Value < 0) pH_Value = 0;
  if (pH_Value > 14) pH_Value = 14;
  
  // 输出到串口监视器
  Serial.print("模拟值: ");
  Serial.print(analogValue);
  Serial.print(" | 电压: ");
  Serial.print(voltage, 3);
  Serial.print("V | pH: ");
  Serial.println(pH_Value, 2);
  
  delay(1000); // 每秒读取一次
}