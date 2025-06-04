#include <OneWire.h>
#include <DallasTemperature.h>

// 定义DS18B20数据线连接的引脚
#define ONE_WIRE_BUS 13  // D13引脚

// 创建OneWire和DallasTemperature实例
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void) {
  Serial.begin(115200);
  Serial.println("DS18B20温度传感器测试");
  
  // 启用ESP32内部上拉电阻
  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  
  // 初始化DS18B20
  sensors.begin();
}

void loop(void) {
  Serial.print("请求温度...");
  sensors.requestTemperatures(); // 发送读取温度命令
  Serial.println("完成");
  
  // 打印温度值
  float tempC = sensors.getTempCByIndex(0);
  
  if(tempC != DEVICE_DISCONNECTED_C) {
    Serial.print("温度: ");
    Serial.print(tempC);
    Serial.println(" ℃");
  } else {
    Serial.println("错误: 无法读取温度数据");
  }
  
  delay(2000); // 2秒读取一次
}