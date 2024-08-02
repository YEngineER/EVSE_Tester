#ifndef __DLT645___
#define __DLT645___
#include <Arduino.h>

#define METER_  Serial

extern byte DLT645_Data[100];
const byte DLT645_Configure_1[] PROGMEM = {0xFE,0xFE,0xFE,0xFE,0x68,0x99,0x99,0x99,0x99,0x99,0x99,0x68,0x23,0x0A,0x60,0x00,0x34,0x12,0x78,0x56,0xBC,0x9A,0xF0,0xDE,0x2B,0x16};
const byte DLT645_Configure_2[] PROGMEM = {0xFE,0xFE,0xFE,0xFE,0x68,0x99,0x99,0x99,0x99,0x99,0x99,0x68,0x23,0x02,0x56,0x00,0xE1,0x16};
const byte DLT645_HEAD[]        PROGMEM = {0xFE,0xFE,0xFE,0xFE,0x68,0x99,0x99,0x99,0x99,0x99,0x99,0x68,0x23,0x02};
const byte DLT645_RTC[]         PROGMEM = {0x59,0x00,0xE4,0x16};
const byte DLT645_PHASE_1[]     PROGMEM = {0x61,0x00,0xEC,0x16};
const byte DLT645_FOOTER        PROGMEM = 0x16;


byte DLT645_Checksum(byte text[], uint8_t siz);
boolean DLT645_read(uint8_t siz);
void DLT645_write(const byte * data, uint8_t siz);
boolean DLT645_init(void);


#endif
