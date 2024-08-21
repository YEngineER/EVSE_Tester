#ifndef __hardware_def__
#define __hardware_def__

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <SoftwareSerial.h>

#define RE_S    14          // OUTPUT
#define RE_B    27          // OUTPUT
#define RE_C    26          // OUTPUT
#define RE_D    25          // OUTPUT
#define RE_PE   33          // OUTPUT
#define RE_LEAK 32          // OUTPUT

#define V1      35          // INPUT
#define V2      34          // INPUT
#define V3      39          // VN INPUT
#define SW      36          // VP INPUT
// #define DEBUG   Serial
// #define AVR     Serial2


#define RX1     18
#define TX1     19
extern SoftwareSerial AVR;

#define RE_L_HV     4       // OUTPUT
#define RE_N_HV     21      // OUTPUT
#define RE_PE_HV    22      // OUTPUT 
extern BluetoothSerial DEBUG_Bluetooth;

#endif