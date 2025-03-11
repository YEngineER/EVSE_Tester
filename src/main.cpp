#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

#include <BluetoothSerial.h>
#include <SoftwareSerial.h>

#include <DLT645.h>
#include <Hardware_def.h>
#include <EVSE_Testing_Functin.h>
#include <Public_Variable.h>

#include <MCP23S08.h>
#include <AVR128DA48/avr128.h>


SPIClass *spi_exp;
MCP23S08 *relay_io;

/*------------METER VARIABLE-----------*/
uint8_t sizeof_PHASE   = 51;

float V = 0;
float f = 0;
/*------------METER VARIABLE-----------*/
String req = "";

uint8_t AVR_count = 0;
uint8_t state_count = 0;
BluetoothSerial DEBUG_Bluetooth;
SoftwareSerial AVR(RX1,TX1);

String avr_msg = "";

hw_timer_t * timer;
EVSE_Test_FSM evse_fsm = EVSE_Test_Standby;

#define in_Between(x,Mn,Mx) ((x > Mn) && (x < Mx))

void AVR_HandleTask(void* param);

void Direction(void) {
  pinMode(SW,       INPUT_PULLUP);
  pinMode(V1,       INPUT_PULLUP);
  pinMode(V2,       INPUT_PULLUP);
  pinMode(V3,       INPUT_PULLUP);

}

void setup() {
  Direction();

  pinMode(19,INPUT_PULLUP);
  pinMode(18,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(23,OUTPUT);

  spi_exp = new SPIClass(VSPI);
  spi_exp->begin();

  relay_io = new MCP23S08(5,0,spi_exp);
  relay_io->begin();
  relay_io->pinMode8(0x00);
  relay_io->write8(0x00);

  Serial.begin(115200);
  DEBUG_Bluetooth.begin("EVSE Tester");
  xTaskCreate(
    AVR_HandleTask, 
    "AVR_Task", 
    4096,                 // Stack Dept
    NULL,                 // Task Parameter
    1,                    // Task Priority
    NULL                  // Task Handler -> no handler
  );
  
  METER_.begin(9600);

  timer = timerBegin(1,2,true);
  
}

void loop() {
  static PWM_test_package pwm_test_pck;
  static boolean Voltage_Error = false;
  
  static Diode_test_package diode_test_pck;
  static RCD_Test_package rcd_test_pck;
  // State_Control();
  // state_A_to_B();
  // state_B_to_C();
  // state_B_to_D();
  // state_C_to_B();
  // diode_PE_test();
  // RCD0_Test();
  // Insulator_Test();

  if(DEBUG_Bluetooth.available() > 0){
    String msg = DEBUG_Bluetooth.readStringUntil('\n');
    switch(evse_fsm){
      case EVSE_Test_Standby :
        if(msg == "State_A_to_B"){
          evse_fsm = State_Test_A_to_B_Begin;
        }else if(msg == "State_B_to_C"){
          evse_fsm = State_Test_B_to_C_Begin;
        }else if(msg == "State_B_to_D"){
          evse_fsm = State_Test_B_to_D_Begin;
        }else if(msg == "State_C_to_B"){
          evse_fsm = State_Test_C_to_B_Begin;
        }else if(msg == "Diode_Test"){
          evse_fsm = Diode_Short_Test_Begin;
        }else if(msg == "PE_Open_Test"){
          evse_fsm = PE_Open_Test_Begin;
        }else if(msg == "Diode_Open_Test"){
          evse_fsm = Diode_Open_Test_Begin;
        }else if(msg == "RCD0_Test"){
          evse_fsm = RCD_Test_Begin;
        }else if(msg == "Insulator_Test"){

        }
        break;
      case State_Test_A_to_B_Begin :
        break;
      case State_Test_A_to_B_Req_CP :
        break;
      case State_Test_A_to_B_ProcessResukt :
        break;
      case State_Test_A_to_B_SendJSON :
        break;
      case State_Test_A_to_B_WaitNextCommand :
        if(msg == "State_B_to_C"){
          if(Voltage_Error)
            evse_fsm = State_Test_B_to_C_SendJSON;
          else
            evse_fsm = State_Test_B_to_C_Begin;
        }
        break;
      case State_Test_B_to_C_Begin :
        break;
      case State_Test_B_to_C_Req_CP :
        break;
      case State_Test_B_to_C_Req_PP :
        break;
      case State_Test_B_to_C_ProcessResukt :
        break;
      case State_Test_B_to_C_SendJSON :
        break;
      case State_Test_B_to_C_WaitNextCommand :
        if(msg == "State_B_to_D"){
          if(Voltage_Error)
            evse_fsm = State_Test_B_to_D_SendJSON;
          else
            evse_fsm = State_Test_B_to_D_Begin;
        }
        break;
      case State_Test_B_to_D_Begin :
        break;
      case State_Test_B_to_D_Req_CP :
        break;
      case State_Test_B_to_D_Req_PP :
        break;
      case State_Test_B_to_D_ProcessResuky :
        break;
      case State_Test_B_to_D_SendJSON :
        break;
      case State_Test_B_to_D_WaitNextCommand :
        if(msg == "State_C_to_B"){
          if(Voltage_Error)
            evse_fsm = State_Test_C_to_B_SendJSON;
          else
            evse_fsm = State_Test_C_to_B_Begin;
        }
        break;
      case State_Test_C_to_B_Begin :
        break;
      case State_Test_C_to_B_Req_CP :
        break;
      case State_Test_C_to_B_ProcessResuly :
        break;
      case State_Test_C_to_B_SendJSON :
        break;
      case  Diode_Short_Test_Begin :
        break;
      case  Diode_Short_Trip_Test :
        break;
      case  PE_Open_Test_Begin :
        break;
      case  PE_Open_Trip_Test :
        break;
      case  Diode_Open_Test_Begin :
        break;
      case  Diode_Open_Trip_Test :
        break;
      case  Diode_Test_SendJSON :
        break;
      default:
        evse_fsm = EVSE_Test_Standby;
        break;
    }
  }

  switch (evse_fsm) {
    case EVSE_Test_Standby :
      break;
    case State_Test_A_to_B_Begin :
      // initiate PWM state A to B test
      // Serial.println("Begin PWM Test State A to B");
      
      relay_io->write8(0x00); // Reset all Relay
      clear_PWM_Test_Package(&pwm_test_pck);

      // Turn on Relay State B
      relay_io->write1(EXP_RE_CP,HIGH);
      
      evse_fsm = State_Test_A_to_B_Req_CP;
      break;
    case State_Test_A_to_B_Req_CP :
      req_CP_NonBlocking(
        [](){
          vTaskDelay(10);
        },[](CP_Package output){
          pwm_test_pck.duty_ratio = output.CP_Duty;
          pwm_test_pck.pwm_freq_hz = output.CP_Freq;
          pwm_test_pck.pwm_amplitude_v = output.CP_Max;
          pwm_test_pck.pwm_nve_amplitude_v = output.CP_Min;
          pwm_test_pck.pwm_Imax_result = true;
          pwm_test_pck.pwm_duty_result = true;

          evse_fsm = State_Test_A_to_B_ProcessResukt;
        },[](){
          pwm_test_pck.duty_ratio = 0.0f;
          pwm_test_pck.pwm_freq_hz = 0.0f;
          pwm_test_pck.pwm_amplitude_v = 0.0f;
          pwm_test_pck.pwm_nve_amplitude_v = 0.0f;
          pwm_test_pck.pwm_Imax_result = false;
          pwm_test_pck.pwm_duty_result = false;

          evse_fsm = State_Test_A_to_B_SendJSON;
        }
      );
      break;
    case State_Test_A_to_B_ProcessResukt :
      if(pwm_test_pck.duty_ratio < ((64.0f * 2.5f) / (2.5f-0.6f))){
        pwm_test_pck.pwm_Imax = (pwm_test_pck.duty_ratio * 0.6f);
      }else{
        pwm_test_pck.pwm_Imax = ((pwm_test_pck.duty_ratio - 64.0f) * 2.5f);
      }
      pwm_test_pck.pwm_freq_result            = in_Between(pwm_test_pck.pwm_freq_hz         ,995.0f ,1005.0f  );
      pwm_test_pck.pwm_amplitude_result       = in_Between(pwm_test_pck.pwm_amplitude_v     ,8.37f  ,9.59f    );
      pwm_test_pck.pwm_nve_amplitude_result   = in_Between(pwm_test_pck.pwm_nve_amplitude_v ,-12.6f ,-11.0f   );
      
      evse_fsm = State_Test_A_to_B_SendJSON;
      break;
    case State_Test_A_to_B_SendJSON :
      pwm_test_pck.State_To_Test = State_A_TO_B;
      send_JSON_state(pwm_test_pck);
      clear_PWM_Test_Package(&pwm_test_pck);
      evse_fsm = State_Test_A_to_B_WaitNextCommand;
      break;
    case State_Test_A_to_B_WaitNextCommand :
      break;
/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */      
    case State_Test_B_to_C_Begin :
      
      // Check Line Voltage before PWM testing
      // The line voltage must not be detected otherwise -> return a voltge error
      if(LineVoltage_beforeOn_Test(V1)){                                                           // When line voltage was not detected -> Normal case
        Test_main_on_Delay(
          V1, [](){
            // Turn on Relay State C
            relay_io->write1(EXP_RE_1k3,HIGH);
            
          },[](float delay){ // When Delay pass -> Read Meter
            pwm_test_pck.main_voltage_onDelay_ms = delay;
            pwm_test_pck.main_on_delay_result = delay < 3000.0f;
#ifndef TEST_NO_METER
            for(uint8_t i=0; i<5; i++) {
              if(DLT645_init()) {
                vTaskDelay(3000);
                read_Meter(&pwm_test_pck.main_voltage, &pwm_test_pck.main_freq_hz);
                break;
              }
            }
#endif
            evse_fsm = State_Test_B_to_C_Req_CP; // Goto Next State
          },[](){             // WHen Delay Fail -> Skip to send JSON
            Serial.println("Test Main on Voltage Error State BC");
            pwm_test_pck.main_on_delay_result = false;
            Voltage_Error = true;
            evse_fsm = State_Test_B_to_C_SendJSON;
          }, 5000
        );

      } else {                                                                                     // when line voltage detected -> Abnormal case
        Serial.println("Before on VOltage Error State BC");
        Voltage_Error = true;
        pwm_test_pck.main_voltage_result = false;
        evse_fsm = State_Test_B_to_C_SendJSON;
      }      
      break;
    case State_Test_B_to_C_Req_CP :
      req_CP_NonBlocking(
        [](){ // While waiting msg
          vTaskDelay(10);
        },
        [](CP_Package output){ // When Request CP complete
          pwm_test_pck.duty_ratio = output.CP_Duty;
          pwm_test_pck.pwm_freq_hz = output.CP_Freq;
          pwm_test_pck.pwm_amplitude_v = output.CP_Max;
          pwm_test_pck.pwm_nve_amplitude_v = output.CP_Min;
          pwm_test_pck.pwm_duty_result = true;

          evse_fsm = State_Test_B_to_C_Req_PP;
        },
        [](){ // When Request CP fail
          pwm_test_pck.duty_ratio = 0.0f;
          pwm_test_pck.pwm_freq_hz = 0.0f;
          pwm_test_pck.pwm_amplitude_v = 0.0f;
          pwm_test_pck.pwm_nve_amplitude_v = 0.0f;
          pwm_test_pck.pwm_duty_result = false;

          evse_fsm = State_Test_B_to_C_SendJSON;
        }
      );
      break;
    case State_Test_B_to_C_Req_PP :
      req_PP_amp_NonBlocking(
        [](){ // While waiting msg
          vTaskDelay(10);
        },
        [](PP_Package output){ // When Request PP complete
          pwm_test_pck.pp_rating_result = true;
          switch (output.PP_rating_enum) {
            case 0:
              pwm_test_pck.pp_Imax = 0.0f;
              pwm_test_pck.pp_rating_result = false;
              break;
            case 7:
              pwm_test_pck.pp_Imax = 13.0f;
              break;
            case 8:
              pwm_test_pck.pp_Imax = 20.0f;
              break;
            case 9:
              pwm_test_pck.pp_Imax = 32.0f;
              break;
            case 10:
              pwm_test_pck.pp_Imax = 63.0f;
              break;
          }

          evse_fsm = State_Test_B_to_C_ProcessResukt;
        },
        [](){ // When Request PP fail
          pwm_test_pck.pp_Imax = 0.0f;
          evse_fsm = State_Test_B_to_C_SendJSON;
        }
      );
      break;
    case State_Test_B_to_C_ProcessResukt :
      pwm_test_pck.pwm_freq_result = in_Between(pwm_test_pck.pwm_freq_hz, 995.0f, 1005.0f);
      pwm_test_pck.pwm_amplitude_result = in_Between(pwm_test_pck.pwm_amplitude_v, 5.47f, 6.53f);
      pwm_test_pck.pwm_nve_amplitude_result = in_Between(pwm_test_pck.pwm_nve_amplitude_v,-12.6f,-11.0f);

      if(pwm_test_pck.duty_ratio < ((64.0f * 2.5f) / (2.5f-0.6f))){
        pwm_test_pck.pwm_Imax = (pwm_test_pck.duty_ratio * 0.6f);
        pwm_test_pck.pwm_Imax_result = (pwm_test_pck.duty_ratio * 0.6f) < pwm_test_pck.pp_Imax;
      }else{
        pwm_test_pck.pwm_Imax = ((pwm_test_pck.duty_ratio - 64.0f) * 2.5f);
        pwm_test_pck.pwm_Imax_result = ((pwm_test_pck.duty_ratio - 64.0f) * 2.5f) < pwm_test_pck.pp_Imax;
      }
      pwm_test_pck.main_on_delay_result = pwm_test_pck.main_voltage_onDelay_ms < 3000.0f;
      pwm_test_pck.main_voltage_result = in_Between(pwm_test_pck.main_voltage, 210.0f, 240.0f);
      pwm_test_pck.main_freq_result = in_Between(pwm_test_pck.main_freq_hz,49.0f, 51.0f);

      evse_fsm = State_Test_B_to_C_SendJSON;
      break;
    case State_Test_B_to_C_SendJSON :
      pwm_test_pck.State_To_Test = State_B_TO_C;
      send_JSON_state(pwm_test_pck);
      clear_PWM_Test_Package(&pwm_test_pck);
      evse_fsm = State_Test_B_to_C_WaitNextCommand;
      break;
    case State_Test_B_to_C_WaitNextCommand :
      break;
    case State_Test_B_to_D_Begin :
      // Check Line Voltage before PWM testing
      // The line voltage must be detected otherwise -> return a voltge error
      if(LineVoltage_beforeOff_Test(V1)){ // When Voltage not detected -> Abnormal case
        relay_io->write1(EXP_RE_330,HIGH);
#ifndef TEST_NO_METER
        for(uint8_t i=0; i<5; i++) {
          if(DLT645_init()) {
            // delay(3000);
            read_Meter(&pwm_test_pck.main_voltage, &pwm_test_pck.main_freq_hz);
            break;
          }
        }
#endif
        evse_fsm = State_Test_B_to_D_Req_CP;
      } else { // When line voltage was detected -> Normal case

        Voltage_Error = true;
        pwm_test_pck.main_voltage_result = false;
        evse_fsm = State_Test_B_to_D_SendJSON;
      }
      break;
    case State_Test_B_to_D_Req_CP :
      req_CP_NonBlocking(
        [](){
          vTaskDelay(10);
        },[](CP_Package output){ // Request Complete -> goto the next state
          pwm_test_pck.duty_ratio = output.CP_Duty;
          pwm_test_pck.pwm_freq_hz = output.CP_Freq;
          pwm_test_pck.pwm_amplitude_v = output.CP_Max;
          pwm_test_pck.pwm_nve_amplitude_v = output.CP_Min;

          pwm_test_pck.pwm_duty_result = true;

          evse_fsm = State_Test_B_to_D_Req_PP;
        },[](){                   // Request Fail -> Skip to send JSON
          pwm_test_pck.duty_ratio = 0.0f;
          pwm_test_pck.pwm_freq_hz = 0.0f;
          pwm_test_pck.pwm_amplitude_v = 0.0f;
          pwm_test_pck.pwm_nve_amplitude_v = 0.0f;

          pwm_test_pck.pwm_duty_result = false;

          evse_fsm = State_Test_B_to_D_SendJSON;
        }
      );
      break;
    case State_Test_B_to_D_Req_PP :
      req_PP_amp_NonBlocking(
        [](){
          vTaskDelay(10);
        },[](PP_Package output){
          pwm_test_pck.pp_rating_result = true;
          switch (output.PP_rating_enum) {
            case 0:
              pwm_test_pck.pp_Imax = 0.0f;
              pwm_test_pck.pp_rating_result = false;
              break;
            case 7:
              pwm_test_pck.pp_Imax = 13.0f;
              break;
            case 8:
              pwm_test_pck.pp_Imax = 20.0f;
              break;
            case 9:
              pwm_test_pck.pp_Imax = 32.0f;
              break;
            case 10:
              pwm_test_pck.pp_Imax = 63.0f;
              break;
          }

          evse_fsm = State_Test_B_to_D_ProcessResuky;
        },[](){
          pwm_test_pck.pp_Imax = 0.0f;
          evse_fsm = State_Test_B_to_C_SendJSON;
        }
      );
      break;
    case State_Test_B_to_D_ProcessResuky :
      relay_io->write1(EXP_RE_330,LOW);

      pwm_test_pck.pwm_freq_result = in_Between(pwm_test_pck.pwm_freq_hz, 995.0f, 1005.0f);
      pwm_test_pck.pwm_amplitude_result = in_Between(pwm_test_pck.pwm_amplitude_v, 2.59f, 3.28f);
      pwm_test_pck.pwm_nve_amplitude_result = in_Between(pwm_test_pck.pwm_nve_amplitude_v,-12.6f,-11.0f);
      if(pwm_test_pck.duty_ratio < ((64.0f * 2.5f) / (2.5f-0.6f))){
        pwm_test_pck.pwm_Imax = (pwm_test_pck.duty_ratio * 0.6f);
        pwm_test_pck.pwm_Imax_result = (pwm_test_pck.duty_ratio * 0.6f) < pwm_test_pck.pp_Imax;
      }else{
        pwm_test_pck.pwm_Imax = ((pwm_test_pck.duty_ratio - 64.0f) * 2.5f);
        pwm_test_pck.pwm_Imax_result = ((pwm_test_pck.duty_ratio - 64.0f) * 2.5f) < pwm_test_pck.pp_Imax;
      }
      pwm_test_pck.main_voltage_result = in_Between(pwm_test_pck.main_voltage, 210.0f, 240.0f);
      pwm_test_pck.main_freq_result = in_Between(pwm_test_pck.main_freq_hz,49.0f, 51.0f);

      evse_fsm = State_Test_B_to_D_SendJSON;
      break;
    case State_Test_B_to_D_SendJSON :
      pwm_test_pck.State_To_Test = State_B_TO_D;
      send_JSON_state(pwm_test_pck);
      clear_PWM_Test_Package(&pwm_test_pck);
      evse_fsm = State_Test_B_to_D_WaitNextCommand;
      break;
    case State_Test_B_to_D_WaitNextCommand :
      break;
    case State_Test_C_to_B_Begin :
#ifndef TEST_NO_LINE
      if(!LineVoltage_beforeOff_Test(V1)){ // when Line voltage was not detected -> Abnormanl case
        Voltage_Error = true;
        pwm_test_pck.main_voltage_result = false;
        evse_fsm = State_Test_C_to_B_SendJSON;
      } else {
#endif
        

        evse_fsm = State_Test_C_to_B_Req_CP;
#ifndef TEST_NO_LINE
      }
#endif
      break;
    case State_Test_C_to_B_Req_CP :
      req_CP_NonBlocking(
        [](){
          vTaskDelay(10);
        },[](CP_Package output){
          pwm_test_pck.duty_ratio = output.CP_Duty;
          pwm_test_pck.pwm_freq_hz = output.CP_Freq;
          pwm_test_pck.pwm_amplitude_v = output.CP_Max;
          pwm_test_pck.pwm_nve_amplitude_v = output.CP_Min;
          pwm_test_pck.pwm_duty_result = true;
          pwm_test_pck.pwm_Imax_result = true;

          evse_fsm = State_Test_C_to_B_test_mainOffDelay;
          
        },[](){
          pwm_test_pck.duty_ratio = 0.0f;
          pwm_test_pck.pwm_freq_hz = 0.0f;
          pwm_test_pck.pwm_amplitude_v = 0.0f;
          pwm_test_pck.pwm_nve_amplitude_v = 0.0f;

          pwm_test_pck.pwm_duty_result = false;
          pwm_test_pck.pwm_Imax_result = false;
          
          evse_fsm = State_Test_C_to_B_SendJSON;
        }
      );
      break;
    
    case State_Test_C_to_B_test_mainOffDelay:
      Test_main_off_Delay(V1,
        [](){
          relay_io->write1(EXP_RE_1k3,LOW);
        },[](float delay){
          pwm_test_pck.main_voltage_offDelay_ms = delay;
          pwm_test_pck.main_off_delay_result = delay < 100.0f;
        },[](){
          pwm_test_pck.main_off_delay_result = false;
        }
      );
      evse_fsm = State_Test_C_to_B_ProcessResuly;
      break;
    case State_Test_C_to_B_ProcessResuly :
      if(pwm_test_pck.duty_ratio < ((64.0f * 2.5f) / (2.5f-0.6f))){
        pwm_test_pck.pwm_Imax = (pwm_test_pck.duty_ratio * 0.6f);
      }else{
        pwm_test_pck.pwm_Imax = ((pwm_test_pck.duty_ratio - 64.0f) * 2.5f);
      }
      pwm_test_pck.pwm_freq_result            = in_Between(pwm_test_pck.pwm_freq_hz         ,995.0f ,1005.0f  );//(pwm_test_pck.pwm_freq_hz < 1005.0f) && (pwm_test_pck.pwm_freq_hz > 995.0f);
      pwm_test_pck.pwm_amplitude_result       = in_Between(pwm_test_pck.pwm_amplitude_v     ,5.47f  ,6.53f    );//(pwm_test_pck.pwm_amplitude_v > 8.37f) && (pwm_test_pck.pwm_amplitude_v < 9.59f);
      pwm_test_pck.pwm_nve_amplitude_result   = in_Between(pwm_test_pck.pwm_nve_amplitude_v ,-12.6f ,-11.0f   );//(pwm_test_pck.pwm_nve_amplitude_v > -12.6f) && (pwm_test_pck.pwm_nve_amplitude_v < -11.0f);
      
      evse_fsm = State_Test_C_to_B_SendJSON;
      break;
    case State_Test_C_to_B_SendJSON :
      pwm_test_pck.State_To_Test = State_C_TO_B;
      send_JSON_state(pwm_test_pck);
      clear_PWM_Test_Package(&pwm_test_pck);
      relay_io->write1(EXP_RE_CP,LOW);
      evse_fsm = EVSE_Test_Standby;
      relay_io->write8(0x00);
      Voltage_Error = false;      // Clear Voltage error flag
      // AVR_Reset_command();
      AVR.flush();
      DEBUG_Bluetooth.flush();
      break;
    case  Diode_Short_Test_Begin :
      relay_io->write8(0x00); // Reset all Relay
      clear_Diode_Test_Package(&diode_test_pck);

      relay_io->write1(EXP_RE_CP, HIGH);
      vTaskDelay(100);
      relay_io->write1(EXP_RE_1k3,HIGH);
      
      waitLineOn(V1);

      if(LineVoltage_beforeOff_Test(V1)){
        evse_fsm = Diode_Short_Trip_Test;
      }else{
        evse_fsm = Diode_Test_SendJSON;
      }
      break;
    case  Diode_Short_Trip_Test :
      Test_main_off_Delay(V1,
        [](){
          relay_io->write1(EXP_RE_Diode,HIGH);
        },
        [](float tripTime){
          diode_test_pck.Diode_ShortCircuit_Result = true;
          diode_test_pck.Diode_ShortCircuit_MainsOffDelay = tripTime;
          relay_io->write8(0x00);
          
          evse_fsm = Diode_Test_PRocessResult;
        },
        [](){
          evse_fsm = Diode_Test_SendJSON;
        }
      );
      break;
    case  PE_Open_Test_Begin :
      relay_io->write1(EXP_RE_CP, HIGH);
      vTaskDelay(100);
      relay_io->write1(EXP_RE_1k3,HIGH);
      waitLineOn(V1);

      if(LineVoltage_beforeOff_Test(V1)){
        evse_fsm = PE_Open_Trip_Test;
      }else{
        evse_fsm = Diode_Test_SendJSON;
      }
      break;
    case  PE_Open_Trip_Test :
      Test_main_off_Delay(V1,
        [](){
          relay_io->write1(EXP_RE_PE,HIGH);
        },
        [](float tripTime){
          diode_test_pck.PE_OpenCircuit_Result = true;
          diode_test_pck.PE_OpenCircuit_MainsOffDelay = tripTime;
          relay_io->write8(0x00);
          
          evse_fsm = Diode_Test_PRocessResult;
        },
        [](){
          evse_fsm = Diode_Test_SendJSON;
        }
      );
      break;
    case  Diode_Open_Test_Begin :
      relay_io->write1(EXP_RE_CP, HIGH);
      vTaskDelay(100);
      relay_io->write1(EXP_RE_1k3,HIGH);
      waitLineOn(V1);

      if(LineVoltage_beforeOff_Test(V1)){
        evse_fsm = Diode_Open_Trip_Test;
      }else{
        evse_fsm = Diode_Test_SendJSON;
      }
      break;
    case  Diode_Open_Trip_Test :
      Test_main_off_Delay(V1,
        [](){
          relay_io->write1(EXP_RE_CP,LOW);
        },
        [](float tripTime){
          diode_test_pck.Diode_OpenCircuit_Result = true;
          diode_test_pck.Diode_OpenCircuit_MainsOffDelay = tripTime;
          relay_io->write8(0x00);
          
          evse_fsm = Diode_Test_PRocessResult;
        },
        [](){
          evse_fsm = Diode_Test_SendJSON;
        }
      );
      break;
    case Diode_Test_PRocessResult:
      
      evse_fsm = Diode_Test_SendJSON;
      break;
    case  Diode_Test_SendJSON :
      send_JSON_Diode(diode_test_pck);
      clear_Diode_Test_Package(&diode_test_pck);
      evse_fsm = EVSE_Test_Standby;
      relay_io->write8(0x00);
      break;
    case RCD_Test_Begin :
      relay_io->write8(0x00);

      relay_io->write1(EXP_RE_CP, HIGH);
      vTaskDelay(100);
      relay_io->write1(EXP_RE_1k3,HIGH);
      waitLineOn(V1);

      if(LineVoltage_beforeOff_Test(V1)){
        evse_fsm = RCD_Test_TripTime;
      }else{
        evse_fsm = RCD_SendJSON;
      }
      break;
    case RCD_Test_TripTime :
      Test_main_off_Delay(V1,
        [](){
          relay_io->write1(EXP_RE_LeakL1,HIGH);
        },
        [](float TripTime){
          relay_io->write8(0x00);
          rcd_test_pck.TripTime_ms = TripTime;
          evse_fsm = RCD_Process_Result;
        },
        [](){
          evse_fsm = RCD_SendJSON;
        },
        300
      );
      break;
    case RCD_Process_Result :
      rcd_test_pck.LimitTrip_time = 100.0f;
      rcd_test_pck.TestingCurrent = 30.0f;
      rcd_test_pck.RCD0_Result = rcd_test_pck.TripTime_ms < rcd_test_pck.LimitTrip_time;
      evse_fsm = RCD_SendJSON;
      break;
    case RCD_SendJSON :
      send_JSON_RCD(rcd_test_pck);
      clear_RCD_Test_Package(&rcd_test_pck);
      relay_io->write8(0x00);
      evse_fsm = EVSE_Test_Standby;
      break;
    default:
      evse_fsm = EVSE_Test_Standby;
      relay_io->write8(0x00);
      break;
  }
  
  
  // if(AVR.available() > 0){
  //   String msg = AVR.readStringUntil('\n');
  //   Serial.println(msg);
  // }

  // state_Manual();
  // vTaskDelay(100);

  

  vTaskDelay(50);
}


void AVR_HandleTask(void* param){

  AVR.begin(9600);
  req_fsm = standby_State;
  while(1){
    
    if(AVR.available() > 0){
      avr_msg = AVR.readStringUntil('~');
      req_fsm = return_State;
    }
    vTaskDelay(10);
  }
}