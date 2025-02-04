#ifndef __EVSE_Tester__
#define __EVSE_Tester__
#include <Arduino.h>

boolean LineVoltage_OnTime (uint8_t line);
boolean LineVoltage_OffTime (uint8_t line);
boolean LineVoltage_OnTime (uint8_t line1,uint8_t line2,uint8_t line3);
boolean LineVoltage_OffTime (uint8_t line1,uint8_t line2,uint8_t line3);
String requestPackageFromAVR(char c);
void State_Control();
String requestPackageFromAVR(char c);
void read_Meter(void);

void state_A_to_B (void);
void state_B_to_C (void);
void state_B_to_D (void);
void state_C_to_B (void);
void diode_PE_test (void);
void RCD0_Test (void);
void Insulator_Test (void);
void state_Manual (void);

#endif