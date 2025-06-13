const int turbidityPin = A5;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(turbidityPin);  // 10位ADC，范围0~1023

  // 计算 A5 引脚实际电压（ADC值 -> 电压）
  float voltageA5 = sensorValue * (3.3 / 1023.0);  

  // 恢复原始传感器输出电压（分压为1/2）
  float sensorVoltage = voltageA5 * 2.0;

  // 根据你的描述：4.25V = 0 NTU，0V = 1000 NTU（线性反比）
  float ntu = (4.25 - sensorVoltage) * (1000.0 / 4.25);

  // 限制范围
  if (ntu < 0) ntu = 0;
  if (ntu > 1000) ntu = 1000;

  // 打印调试信息
  Serial.print("ADC: ");
  Serial.print(sensorValue);
  Serial.print("\tA5 Voltage: ");
  Serial.print(voltageA5, 3);
  Serial.print(" V\tSensor Voltage: ");
  Serial.print(sensorVoltage, 3);
  Serial.print(" V\tNTU: ");
  Serial.println(ntu, 2);

  delay(1000);
}
