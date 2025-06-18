// epd2in9_V2.h - Header file for E-Paper display
#ifndef EPD2IN9_V2_H
#define EPD2IN9_V2_H

#include "epdif.h"

// Display resolution
#define EPD_WIDTH       128
#define EPD_HEIGHT      296

class Epd : EpdIf {
public:
    unsigned long width;
    unsigned long height;

    Epd();
    ~Epd();
    int Init();
    int Init_Fast();
    int Init_4Gray();
    void SendCommand(unsigned char command);
    void SendData(unsigned char data);
    void WaitUntilIdle(void);
    void Reset(void);
    void SetFrameMemory(const unsigned char* image_buffer, int x, int y, int image_width, int image_height);
    void SetFrameMemory(const unsigned char* image_buffer);  // 添加这个重载版本
    void SetFrameMemory_Base(const unsigned char* image_buffer);
    void SetFrameMemory_Partial(const unsigned char* image_buffer, int x, int y, int image_width, int image_height);
    void ClearFrameMemory(unsigned char color);
    void DisplayFrame(void);
    void DisplayFrame_Partial(void);
    void Sleep(void);
    void Display4Gray(const unsigned char *Image);  // 添加这个函数
    void SetLut(unsigned char *lut);  // 添加这个函数
    void SetLut_by_host(unsigned char *lut);  // 修改参数类型

private:
    unsigned int reset_pin;
    unsigned int dc_pin;
    unsigned int cs_pin;
    unsigned int busy_pin;
    
    void SetMemoryArea(int x_start, int y_start, int x_end, int y_end);
    void SetMemoryPointer(int x, int y);
};

#endif