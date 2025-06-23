const int ledPin0 = 0;  // 数字引脚 D0
const int ledPin1 = 1;  // 数字引脚 D1
const int ledPin10 = 10; // 数字引脚 D10

void setup() {
  pinMode(ledPin0, OUTPUT);  // 设置 D0 为输出模式
  pinMode(ledPin1, OUTPUT);  // 设置 D1 为输出模式
  pinMode(ledPin10, OUTPUT); // 设置 D10 为输出模式
  
  digitalWrite(ledPin0, HIGH);  // 点亮 D0 的 LED
  digitalWrite(ledPin1, HIGH);  // 点亮 D1 的 LED
  digitalWrite(ledPin10, HIGH); // 点亮 D10 的 LED
}

void loop() {
  // 空循环，三个 LED 将保持点亮状态
}