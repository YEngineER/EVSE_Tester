#ifndef __Public_Var__
#define __Public_Var__
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <EVSE_Testing_Functin.h>

/*------------METER VARIABLE-----------*/
extern uint8_t sizeof_PHASE;

extern float V;
extern float f;
/*------------METER VARIABLE-----------*/
extern String req;

extern uint8_t AVR_count;
extern uint8_t state_count;
extern String avr_msg;

// extern uint64_t nowTime;
extern EVSE_Test_FSM evse_fsm;

#endif