/**
 *  @filename   :   water_quality_monitor.ino
 *  @brief      :   2.9inch e-paper V2 Water Quality Monitor
 *  @author     :   Modified from Waveshare demo
 *
 *  Water Quality Monitoring System using E-Paper Display
 *  Displays: Temperature, Turbidity, Conductivity (TDS), and pH
 */

#include <SPI.h>
#include "epd2in9_V2.h"
#include "epdpaint.h"

#define COLORED     0
#define UNCOLORED   1

// Simulated sensor values - replace with actual sensor readings
float temperature = 23.5;  // °C
float turbidity = 4.2;     // NTU
float tds = 145.8;         // ppm
float ph = 7.1;            // pH
float ec = 290.5;          // µS/cm (Electrical Conductivity)

/**
  * Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
  * In this case, a smaller image buffer is allocated and you have to 
  * update a partial display several times.
  * 1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
  */
unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8 
Epd epd;

void setup() {
  Serial.begin(115200);
  if (epd.Init() != 0) {
      Serial.print("e-Paper init failed");
      return;
  }

  // Clear the display
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  delay(1000);
  
  // 先回到竖屏显示，确保基本功能正常
  paint.SetRotate(ROTATE_0);  // 改回ROTATE_0
  paint.SetWidth(128);        // 使用8的倍数
  paint.SetHeight(24);        // 适当的高度
  
  // 标题 - 参考原始示例的坐标
  paint.Clear(COLORED);  // 黑色背景
  paint.DrawStringAt(0, 4, "   Water Monitor", &Font12, UNCOLORED);  // 白色文字
  epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());
  
  // 调整为16高度用于参数
  paint.SetHeight(16);
  
  // Temperature - Y坐标参考原示例: 0, 30
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 2, " Temp: 23.5 C", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 40, paint.GetWidth(), paint.GetHeight());

  // Turbidity
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 2, " Turb: 4.2 NTU", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 60, paint.GetWidth(), paint.GetHeight());

  // TDS
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 2, " TDS: 145.8 ppm", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 80, paint.GetWidth(), paint.GetHeight());

  // EC
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 2, " EC: 290.5 uS/cm", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 100, paint.GetWidth(), paint.GetHeight());

  // pH
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 2, " pH: 7.1", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 120, paint.GetWidth(), paint.GetHeight());

  // 状态
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 2, "Status: GOOD :)", &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 140, paint.GetWidth(), paint.GetHeight());

  // Display frame
  epd.DisplayFrame();
  
  // 调试信息
  Serial.println("=== Water Quality Monitor (Vertical) ===");
  Serial.println("Back to ROTATE_0 for reliability");
  Serial.println("Using coordinates from original example");
  Serial.println("[T] Temperature: 23.5°C");
  Serial.println("[*] Turbidity: 4.2 NTU"); 
  Serial.println("[+] TDS: 145.8 ppm");
  Serial.println("[E] EC: 290.5 µS/cm");
  Serial.println("[H] pH: 7.1");
  Serial.println("Status: GOOD :)");
  Serial.println("Should display reliably now");
  
  epd.Sleep();
}

void loop() {
  // In a real implementation, you would:
  // 1. Read sensor values
  // 2. Update the display if values changed significantly
  // 3. Put the system to sleep for power saving
  
  // For demonstration, we'll just wait
  delay(60000); // Wait 1 minute
}