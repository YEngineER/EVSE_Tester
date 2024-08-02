#ifndef __hardware_def__
#define __hardware_def__

#include <Arduino.h>
#include <BluetoothSerial.h>

#define RE_S    25
#define RE_B    22
#define RE_C    23
#define RE_D    15
#define RE_PE   12
#define RE_LEAK 14

#define V1      37
#define V2      38
#define V3      39
#define SW      0
// #define DEBUG   Serial
#define AVR     Serial1


#define RX1     9
#define TX1     10

#define RX2     36
#define TX2     25
// SoftwareSerial  METER_(RX2, TX2);

#define RE_L_HV     4
#define RE_N_HV     2
#define RE_PE_HV    26 
extern BluetoothSerial DEBUG;

#endif