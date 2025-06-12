// 引脚定义
const int analogPin = A4;         // 模拟信号输入
const float Vref = 3.3;           // MKR WAN 1310 的 ADC 参考电压
const int ADC_resolution = 1023;  // 10位ADC
const float sensorMaxV = 2.3;     // 传感器最大输出电压
const float maxConductivity = 2000.0; // μS/cm，对应2.3V时的最大电导率

void setup() {
  Serial.begin(9600);
}

void loop() {
  int raw = analogRead(analogPin); // 读取原始ADC值
  float voltage = raw * Vref / ADC_resolution; // 转换为电压

  // 将电压转换为电导率（μS/cm）
  float conductivity = (voltage / sensorMaxV) * maxConductivity;

  // 输出结果
  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V\t");

  Serial.print("Conductivity: ");
  Serial.print(conductivity, 1);
  Serial.println(" uS/cm");

  delay(1000); // 每秒输出一次
}
