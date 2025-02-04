#ifndef __hardware_def__
#define __hardware_def__

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <SoftwareSerial.h>
#include <MCP23S08.h>
#include <SPI.h>

#define TEST_NO_LINE
#define TEST_NO_METER
#define DEBUG_AVR

#define RE_S    14          // OUTPUT
#define RE_B    27          // OUTPUT
#define RE_C    26          // OUTPUT
#define RE_D    25          // OUTPUT
#define RE_PE   33          // OUTPUT
#define RE_LEAK 32          // OUTPUT

#define EXP_RE_CP       0
#define EXP_RE_Diode    1
#define EXP_RE_1k3      2
#define EXP_RE_330      4
#define EXP_RE_PE       3
#define EXP_RE_LeakL1   5
#define EXP_RE_LeakL2   6
#define EXP_RE_LeakL3   7

#define Test_State_A    0x00
#define Test_State_B    0x01
#define Test_State_C    0x05
#define Test_State_D    0x15

#define V1      25
#define V2      26
#define V3      27
#define SW      36
// #define DEBUG   Serial
// #define AVR     Serial2

#define req_Frequency_and_duty     'A'
#define req_MinMaxAverage          'B'
#define req_InsulationVoltage      'C'
#define req_Insulation             'D'
#define req_PP                     'E'

#define RX1     21
#define TX1     22
extern SoftwareSerial AVR;

#define RE_L_HV     4       // OUTPUT
#define RE_N_HV     21      // OUTPUT
#define RE_PE_HV    22      // OUTPUT 
extern BluetoothSerial DEBUG_Bluetooth;

extern SPIClass *spi_exp;
extern MCP23S08 *relay_io;

extern hw_timer_t * timer;

#endif