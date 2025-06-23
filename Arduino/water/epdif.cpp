/**
 *  @filename   :   epdif_mkr.cpp
 *  @brief      :   E-Paper驱动接口实现 - MKR WAN1310版本
 *  @author     :   适配MKR WAN1310
 *  
 *  Copyright (C) Waveshare     September 9 2017
 *
 *  适配Arduino MKR WAN1310开发板的SPI和GPIO接口
 */

#include "epdif.h"
#include <SPI.h>

// MKR WAN1310的E-Paper连接引脚定义
#define CS_PIN          7
#define RST_PIN         5  
#define DC_PIN          6
#define BUSY_PIN        4

EpdIf::EpdIf() {
};

EpdIf::~EpdIf() {
};

void EpdIf::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int EpdIf::DigitalRead(int pin) {
    return digitalRead(pin);
}

void EpdIf::DelayMs(unsigned int delaytime) {
    delay(delaytime);
}

void EpdIf::SpiTransfer(unsigned char data) {
    digitalWrite(CS_PIN, LOW);
    SPI.transfer(data);
    digitalWrite(CS_PIN, HIGH);
}

int EpdIf::IfInit(void) {
    // 初始化SPI引脚
    pinMode(CS_PIN, OUTPUT);
    pinMode(RST_PIN, OUTPUT);  
    pinMode(DC_PIN, OUTPUT);
    pinMode(BUSY_PIN, INPUT);
    
    // 初始化SPI接口
    SPI.begin();
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    
    return 0;
}

void EpdIf::IfEnd(void) {
    SPI.endTransaction();
    SPI.end();
}