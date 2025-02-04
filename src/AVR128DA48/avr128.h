#ifndef _AVR128__
#define _AVR128__

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Hardware_def.h>
#define return_timeout_ms       10000

typedef struct{
    float CP_Max;
    float CP_Min;
    float CP_Freq;
    float CP_Duty;
}CP_Package;

typedef struct{
    uint8_t PP_rating_enum;
}PP_Package;

typedef struct{
    float Resistance;
}Insulation_Package;

typedef enum{
    standby_State,
    req_CP_State,
    req_PP_State,
    req_ins_State,
    req_ping_State,
    return_State

}AVR_Req_FSM;

extern AVR_Req_FSM req_fsm;

boolean ping();

boolean req_CP(CP_Package* output);
// msg : ~<PP_amp>~
// ex:  '+' 0x00 '~' ; No cable
// ex:  '+' 0x07 '~' ; 13A
// ex:  '+' 0x08 '~' ; 20A
// ex:  '+' 0x09 '~' ; 32A
// ex:  '+' 0x0A '~' ; 63A

boolean req_PP_amp(PP_Package* output);

boolean req_Ins(Insulation_Package* output);

void AVR_Reset_command();

#endif