#ifndef __EVSE_Tester__
#define __EVSE_Tester__
#include <Arduino.h>

typedef enum{
    EVSE_Test_Standby,

    State_Test_A_to_B_Begin,
    State_Test_A_to_B_Req_CP,
    State_Test_A_to_B_ProcessResukt,
    State_Test_A_to_B_SendJSON,
    State_Test_A_to_B_WaitNextCommand,
    
    State_Test_B_to_C_Begin,
    State_Test_B_to_C_Req_CP,
    State_Test_B_to_C_Req_PP,
    State_Test_B_to_C_ProcessResukt,
    State_Test_B_to_C_SendJSON,
    State_Test_B_to_C_WaitNextCommand,
    
    State_Test_B_to_D_Begin,
    State_Test_B_to_D_Req_CP,
    State_Test_B_to_D_Req_PP,
    State_Test_B_to_D_ProcessResuky,
    State_Test_B_to_D_SendJSON,
    State_Test_B_to_D_WaitNextCommand,

    State_Test_C_to_B_Begin,
    State_Test_C_to_B_Req_CP,
    State_Test_C_to_B_test_mainOffDelay,
    State_Test_C_to_B_ProcessResuly,
    State_Test_C_to_B_SendJSON,
    
    Diode_Short_Test_Begin,
    Diode_Short_Trip_Test,
    PE_Open_Test_Begin,
    PE_Open_Trip_Test,
    Diode_Open_Test_Begin,
    Diode_Open_Trip_Test,
    Diode_Test_PRocessResult,
    Diode_Test_SendJSON,

    RCD_Test_Begin,
    RCD_Test_TripTime,
    RCD_Process_Result,
    RCD_SendJSON,

    returnWhatAreYou,
    setManual_A,
    setManual_B,
    setManual_C,
    setManual_D,
    setModeManual,
    setModeAuto,
    returnWhatYourStatus
    
}EVSE_Test_FSM;

typedef enum{
    null_state,
    State_A_TO_B,
    State_B_TO_C,
    State_B_TO_D,
    State_C_TO_B
}PWM_State_Test;

typedef struct{
    PWM_State_Test State_To_Test;
    
    float pwm_start_delay_ms;
    float pwm_amplitude_v;
    float pwm_nve_amplitude_v;
    float pwm_freq_hz;
    float duty_ratio;
    float pwm_Imax;
    float pp_Imax;
    float main_voltage_onDelay_ms;
    float main_voltage_offDelay_ms;
    float main_freq_hz;
    float main_voltage;

    boolean pwm_start_delay_result;
    boolean pwm_amplitude_result;
    boolean pwm_nve_amplitude_result;
    boolean pwm_freq_result;
    boolean pwm_duty_result;
    boolean pwm_Imax_result;
    boolean pp_rating_result;
    boolean main_on_delay_result;
    boolean main_off_delay_result;
    boolean main_freq_result;
    boolean main_voltage_result;
}PWM_test_package;

typedef struct{
    float Diode_ShortCircuit_MainsOffDelay;
    float PE_OpenCircuit_MainsOffDelay;
    float Diode_OpenCircuit_MainsOffDelay;

    boolean Diode_ShortCircuit_Result;
    boolean PE_OpenCircuit_Result;
    boolean Diode_OpenCircuit_Result;
}Diode_test_package;

typedef struct{
    float TripTime_ms;
    float LimitTrip_time;
    float TestingCurrent;
    boolean RCD0_Result;
    boolean RCD1_Result;
    boolean RCD2_Result;

    boolean noTrip;
}RCD_Test_package;

void send_JSON_state(PWM_test_package package);
void clear_PWM_Test_Package(PWM_test_package * package);
void Test_main_on_Delay(
    uint8_t line, 
    void (*onStart)(), 
    void (*onPass)(float delay), 
    void (*onTimeout)(),
    uint32_t timeout_ms = 5000
);
void Test_main_off_Delay(
    uint8_t line, 
    void (*onStart)(), 
    void (*onPass)(float delay), 
    void (*onTimeout)(),
    uint32_t timeout_ms = 5000
);


void send_JSON_Diode(Diode_test_package package);
void clear_Diode_Test_Package(Diode_test_package * package);

void clear_RCD_Test_Package(RCD_Test_package * package);
void send_JSON_RCD(RCD_Test_package package);

boolean LineVoltage_beforeOn_Test   (uint8_t line);
boolean LineVoltage_beforeOff_Test  (uint8_t line);
boolean LineVoltage_beforeOn_Test   (uint8_t line1,uint8_t line2,uint8_t line3);
boolean LineVoltage_beforeOff_Test  (uint8_t line1,uint8_t line2,uint8_t line3);

void waitLineOn(uint8_t line, uint32_t timeout_ms = 60000);


String requestPackageFromAVR(char c);
void State_Control();
void read_Meter(float * output_V,float * output_freq);

void diode_PE_test (void);
void RCD0_Test (void);
void Insulator_Test (void);
void state_Manual (void);

#endif