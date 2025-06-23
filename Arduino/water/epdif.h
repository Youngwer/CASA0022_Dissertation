/**
 *  @filename   :   epdif_mkr.h
 *  @brief      :   E-Paper驱动接口头文件 - MKR WAN1310版本
 *  @author     :   适配MKR WAN1310
 *  
 *  Copyright (C) Waveshare     September 9 2017
 *
 *  适配Arduino MKR WAN1310开发板的硬件接口定义
 */

#ifndef EPDIF_H
#define EPDIF_H

#include <Arduino.h>

// MKR WAN1310 E-Paper连接引脚
#define RST_PIN         5
#define DC_PIN          6
#define CS_PIN          7
#define BUSY_PIN        4

class EpdIf {
public:
    EpdIf(void);
    ~EpdIf(void);

    static int  IfInit(void);
    static void IfEnd(void);
    static void DigitalWrite(int pin, int value); 
    static int  DigitalRead(int pin);
    static void DelayMs(unsigned int delaytime);
    static void SpiTransfer(unsigned char data);
};

#endif