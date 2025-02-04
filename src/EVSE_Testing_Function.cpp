#include <Arduino.h>
#include <EVSE_Testing_Functin.h>
#include <Public_Variable.h>
#include <Hardware_def.h>
#include <ArduinoJson.h>
#include <DLT645.h>
#include <MCP23S08.h>
#include <AVR128DA48/avr128.h>

void state_A_to_B (void) {
  if(req == "State_A_to_B") {
    Serial.println("Begin test State A to B");
    relay_io->write8(0x00); // Reset all Relay

    DynamicJsonDocument state(512);
    state["State_To_Test"]     = "A_to_B";
    state["PWM_StartupDelay"]   = "0";
    state["PWM_Amplitude"]      = "0";
    state["PWM_NveAmplitude"]   = "0";
    state["PWM_Freq"]           = "0";
    state["PWM_DutyCycle"]      = "0";
    state["PWM_Imax"]           = "0";
    state["MainsOnDelay"]       = "0";
    state["MainsFreq"]          = "0";
    state["PP"]                 = "0";

    state["PWM_StartupDelay_Result"]   = true;
    state["PWM_Amplitude_Result"]      = true;
    state["PWM_NveAmplitude_Result"]   = true;
    state["PWM_Freq_Result"]           = true;
    state["PWM_DutyCycle_Result"]      = true;
    state["PWM_Imax_Result"]           = true;
    state["MainsOnDelay_Result"]       = true;
    state["MainsFreq_Result"]          = true;
    state["PP_Result"]                 = true;

    relay_io->write1(EXP_RE_CP,HIGH);
    delay(500);
    uint8_t retry_req = 0;
    boolean req_cp_ret;
    CP_Package cp_package;
    do{
        req_cp_ret = req_CP(&cp_package);
        if(!req_cp_ret) {
          AVR_Reset_command();
          vTaskDelay(500);
        }
    }while((retry_req++ < 3) && (req_cp_ret == false));
    if(req_cp_ret){
      state["PWM_DutyCycle"]    = String(cp_package.CP_Duty);
      state["PWM_Freq"]         = String(cp_package.CP_Freq);
      state["PWM_Amplitude"]    = String(cp_package.CP_Max);
      state["PWM_NveAmplitude"] = String(cp_package.CP_Min);
      
      if((cp_package.CP_Freq > 1005.0f) || (cp_package.CP_Freq < 995.0f)) {state["PWM_Freq_Result"] = false;}
      if((cp_package.CP_Max  <   8.37f) || (cp_package.CP_Max  >  9.59f)) {state["PWM_Amplitude_Result"] = false;}
      if((cp_package.CP_Min  <  -12.6f) || (cp_package.CP_Min  > -11.0f)) {state["PWM_NveAmplitude_Result"] = false;}
      state_count = 1;

      Serial.printf("CP request complete : attemp %d\n",retry_req);
    }
    serializeJson(state, DEBUG_Bluetooth);
    DEBUG_Bluetooth.println();
    DEBUG_Bluetooth.flush();
    Serial.println("Done test State A to B");
  }
}

void state_B_to_C (void) {
  if(req == "State_B_to_C" && state_count == 1) {
    Serial.println("Begin test State B to C");

    DynamicJsonDocument state(512);
    state["State_To_Test"]     = "B_to_C";
    state["PWM_StartupDelay"]   = "0";
    state["PWM_Amplitude"]      = "0";
    state["PWM_NveAmplitude"]   = "0";
    state["PWM_Freq"]           = "0";
    state["PWM_DutyCycle"]      = "0";
    state["PWM_Imax"]           = "0";
    state["MainsOnDelay"]       = "0";
    state["MainsFreq"]          = "0";
    state["Voltage"]            = "0";
    state["PP"]                 = "0";  

    state["PWM_StartupDelay_Result"]   = true;
    state["PWM_Amplitude_Result"]      = true;
    state["PWM_NveAmplitude_Result"]   = true;
    state["PWM_Freq_Result"]           = true;
    state["PWM_DutyCycle_Result"]      = true;
    state["PWM_Imax_Result"]           = true;
    state["MainsOnDelay_Result"]       = true;
    state["MainsFreq_Result"]          = true;
    state["Voltage_Result"]            = true;
    state["PP_Result"]                 = true;

#ifndef TEST_NO_LINE
    if(LineVoltage_OnTime(V1)){ // when line voltage detected
      state["Voltage_Result"] = false;
      serializeJson(state, DEBUG_Bluetooth);
      DEBUG_Bluetooth.println();
      ESP.restart();
    }
#endif

#ifndef TEST_NO_LINE
    // nowTime = micros();
    timerStart(timer);
    timerRestart(timer);
    // digitalWrite(RE_C, HIGH);
#endif

    relay_io->write1(EXP_RE_1k3,HIGH);

#ifndef TEST_NO_LINE
    while(1) {
      uint64_t timerNow = timerRead(timer);
      if(!digitalRead(V1)){
        state["MainsOnDelay"] = String(timerNow / 40e3);
        break;
      }
      if(timerNow > (5*40000000)) {
        state["MainsOnDelay_Result"]  = false;
        serializeJson(state, DEBUG_Bluetooth);
        DEBUG_Bluetooth.println();
        ESP.restart();
      }
    }
    timerStop(timer);
#else
    state["MainsOnDelay"] = "0";
    state["MainsOnDelay_Result"] = false;
#endif

    CP_Package cp_package;
    PP_Package pp_package;
    uint8_t retry_req = 0;
    boolean req_cp_ret;
    boolean req_pp_ret;
    do{
        req_cp_ret = req_CP(&cp_package);
        if(!req_cp_ret) {
          AVR_Reset_command();
          vTaskDelay(500);
        }
    }while((retry_req++ < 3) && (req_cp_ret == false));

    vTaskDelay(500);
    retry_req = 0;
    do{
      req_pp_ret = req_PP_amp(&pp_package);
      if(!req_pp_ret) {
        AVR_Reset_command();
        vTaskDelay(500);
      }
    }while((retry_req++ < 3) && (req_pp_ret == false));

    if(req_cp_ret){
      state["PWM_DutyCycle"]    = String(cp_package.CP_Duty);
      state["PWM_Freq"]         = String(cp_package.CP_Freq);
      state["PWM_Imax"]         = String(cp_package.CP_Duty * 0.6f);
      state["PWM_Amplitude"]    = String(cp_package.CP_Max);
      state["PWM_NveAmplitude"] = String(cp_package.CP_Min);

      Serial.printf("CP request complete : attemp %d\n",retry_req);
    }else{
      Serial.printf("CP request Fail : attemp %d\n",retry_req);
    }
    
    if(req_pp_ret){
      switch(pp_package.PP_rating_enum){
        case 0:  state["PP"] = "0";
          break;
        case 7:  state["PP"] = "13";
          break;
        case 8:  state["PP"] = "20";
          break;
        case 9:  state["PP"] = "32";
          break;
        case 10: state["PP"] = "63";
          break;
      }

      Serial.printf("PP request complete : attemp %d\n",retry_req);
    }else{
      Serial.printf("PP request Fail : attemp %d\n",retry_req);
    }

#ifndef TEST_NO_METER
    for(uint8_t i=0; i<5; i++) {
      if(DLT645_init()) {
        delay(3000);
        read_Meter();
        state["Voltage"] = String(V);
        state["MainsFreq"] = String(f);
        break;
      }
    }
#endif

    if((cp_package.CP_Freq    > 1005.0f ) || (cp_package.CP_Freq    < 995.0f  ))      {state["PWM_Freq_Result"]        = false;}
    if((cp_package.CP_Max     < 5.47f   ) || (cp_package.CP_Max     > 6.53f   ))      {state["PWM_Amplitude_Result"]   = false;}
    if((cp_package.CP_Min     < -12.6f  ) || (cp_package.CP_Min     > -11.0f  ))      {state["PWM_NveAmplitude_Result"]= false;}
    if(float(state["PWM_Imax"]) > float(state["PP"]))                                 {state["PWM_Imax_Result"]        = false;}
    if(float(state["MainsOnDelay"]) > 3000.0f)                                        {state["MainsOnDelay_Result"]    = false;}
    if((float(state["Voltage"])   < 210.0f)  || (float(state["Voltage"])   > 240.0f)) {state["Voltage_Result"]         = false;}
    if((float(state["MainsFreq"]) < 49.0f)   || (float(state["MainsFreq"]) > 51.0f))  {state["MainsFreq_Result"]       = false;}
    state_count = 2;
    serializeJson(state, DEBUG_Bluetooth);
    DEBUG_Bluetooth.println();
    DEBUG_Bluetooth.flush();
    Serial.println("Done test State B to C");

    serializeJson(state,Serial);
    Serial.println();
  }
}

void state_B_to_D (void) {
  if(req == "State_B_to_D" && state_count == 2) {
    Serial.println("Begin test State B to D");
    DynamicJsonDocument state(512);
    state["State_To_Test"]     = "B_to_D";
    state["PWM_StartupDelay"]   = "0";
    state["PWM_Amplitude"]      = "0";
    state["PWM_NveAmplitude"]   = "0";
    state["PWM_Freq"]           = "0";
    state["PWM_DutyCycle"]      = "0";
    state["PWM_Imax"]           = "0";
    state["MainsOnDelay"]       = "0";
    state["MainsFreq"]          = "0";
    state["Voltage"]            = "0";
    state["PP"]                 = "0";

    state["PWM_StartupDelay_Result"]   = true;
    state["PWM_Amplitude_Result"]      = true;
    state["PWM_NveAmplitude_Result"]   = true;
    state["PWM_Freq_Result"]           = true;
    state["PWM_DutyCycle_Result"]      = true;
    state["PWM_Imax_Result"]           = true;
    state["MainsOnDelay_Result"]       = true;
    state["MainsFreq_Result"]          = true;
    state["Voltage_Result"]            = true;
    state["PP_Result"]                 = true;

#ifndef TEST_NO_LINE
    if(LineVoltage_OffTime(V1)){
      state["Voltage_Result"] = false;
      serializeJson(state, DEBUG_Bluetooth);
      DEBUG_Bluetooth.println();
      ESP.restart();
    }
#endif
    relay_io->write1(EXP_RE_330,HIGH);
    CP_Package cp_package;
    PP_Package pp_package;
    uint8_t retry_req = 0;
    boolean req_cp_ret;
    boolean req_pp_ret;
    do{
        req_cp_ret = req_CP(&cp_package);
        if(!req_cp_ret) {
          AVR_Reset_command();
          vTaskDelay(500);
        }
    }while((retry_req++ < 3) && (req_cp_ret == false));

    retry_req = 0;
    do{
      req_pp_ret = req_PP_amp(&pp_package);
      if(!req_pp_ret) {
        AVR_Reset_command();
        vTaskDelay(500);
      }
    }while((retry_req++ < 3) && (req_pp_ret == false));

    if(req_cp_ret){
      state["PWM_DutyCycle"]    = String(cp_package.CP_Duty);
      state["PWM_Freq"]         = String(cp_package.CP_Freq);
      state["PWM_Imax"]         = String(cp_package.CP_Duty * 0.6f);
      state["PWM_Amplitude"]    = String(cp_package.CP_Max);
      state["PWM_NveAmplitude"] = String(cp_package.CP_Min);
    }
    
    if(req_pp_ret){
      switch(pp_package.PP_rating_enum){
        case 0:  state["PP"] = "0";
          break;
        case 7:  state["PP"] = "13";
          break;
        case 8:  state["PP"] = "20";
          break;
        case 9:  state["PP"] = "32";
          break;
        case 10: state["PP"] = "63";
          break;
      }
    }
#ifndef TEST_NO_METER
    for(uint8_t i=0; i<5; i++) {
      if(DLT645_init()) {
        delay(3000);
        read_Meter();
        state["Voltage"] = String(V);
        state["MainsFreq"] = String(f);
        break;
      }
    }
#endif
    // digitalWrite(RE_D, LOW);
    relay_io->write1(EXP_RE_330,LOW); 
    delay(1000);
    if((cp_package.CP_Freq > 1005.0f)     || (cp_package.CP_Freq < 995.0f ))    {state["PWM_Freq_Result"]        = false;}
    if((cp_package.CP_Max    < 2.59f)     || (cp_package.CP_Max    > 3.28f))    {state["PWM_Amplitude_Result"]   = false;}
    if((cp_package.CP_Min < -12.6f)       || (cp_package.CP_Min > -11.0f  ))    {state["PWM_NveAmplitude_Result"]= false;}
    if(float(state["PWM_Imax"]) > float(state["PP"]))                           {state["PWM_Imax_Result"]        = false;}
    if(float(state["MainsOnDelay"]) > 3000.0f)                                  {state["MainsOnDelay_Result"]    = false;}
    if(float(state["Voltage"]) < 210.0f   || float(state["Voltage"]) > 240.0f)  {state["Voltage_Result"]         = false;}
    if(float(state["MainsFreq"]) < 49.0f  || float(state["MainsFreq"]) > 51.0f) {state["MainsFreq_Result"]       = false;}
    state_count = 3;
    serializeJson(state, DEBUG_Bluetooth);
    DEBUG_Bluetooth.println();
    DEBUG_Bluetooth.flush();
    Serial.println("Done test State B to D");

    
  }
}

void state_C_to_B (void) {
  if(req == "State_C_to_B" && state_count == 3) {
    Serial.println("Begin test State C to B");
    DynamicJsonDocument state(512);
    state["State_To_Test"]     = "C_to_B";
    state["PWM_StartupDelay"]   = "0";
    state["PWM_Amplitude"]      = "0";
    state["PWM_NveAmplitude"]   = "0";
    state["PWM_Freq"]           = "0";
    state["PWM_DutyCycle"]      = "0";
    state["PWM_Imax"]           = "0";
    state["MainsOffDelay"]      = "0";
    state["MainsFreq"]          = "0";
    state["PP"]                 = "0";

    state["PWM_StartupDelay_Result"]   = true;
    state["PWM_Amplitude_Result"]      = true;
    state["PWM_NveAmplitude_Result"]   = true;
    state["PWM_Freq_Result"]           = true;
    state["PWM_DutyCycle_Result"]      = true;
    state["PWM_Imax_Result"]           = true;
    state["MainsOffDelay_Result"]      = true;
    state["MainsFreq_Result"]          = true;
    state["Voltage_Result"]            = true;
    state["PP_Result"]                 = true;

    delay(1000);
#ifndef TEST_NO_LINE
    if(LineVoltage_OffTime(V1)){
      state["Voltage_Result"] = false;
      serializeJson(state, DEBUG_Bluetooth);
      DEBUG_Bluetooth.println();
      ESP.restart();
    }
#endif

#ifndef TEST_NO_LINE
    timerStart(timer);
    timerRestart(timer);
#endif

    relay_io->write1(EXP_RE_1k3,LOW);

#ifndef TEST_NO_LINE
    while(1) {
      uint64_t timerNow = timerRead(timer);
      if(digitalRead(V1)){
        delay(2);
        if(digitalRead(V1)) {
          state["MainsOffDelay"] = String(timerNow / 40e3); // ms
          break;
        }
      }
      if(timerNow > (5*40000000)) {
        state["MainsOffDelay_Result"] = false;
        serializeJson(state, DEBUG_Bluetooth);
        DEBUG_Bluetooth.println();
        timerStop(timer);
        ESP.restart();
      }
    }
    timerStop(timer);
#else
  state["MainsOffDelay"] = "0";
  state["MainsOffDelay_Result"] = false;

#endif
    
    uint8_t retry_req = 0;
    boolean req_cp_ret;
    CP_Package cp_package;
    do{
        req_cp_ret = req_CP(&cp_package);
        if(!req_cp_ret) {
          AVR_Reset_command();
          vTaskDelay(500);
        }
    }while((retry_req++ < 3) && (req_cp_ret == false));
    if(req_cp_ret){
      state["PWM_DutyCycle"]    = String(cp_package.CP_Duty);
      state["PWM_Freq"]         = String(cp_package.CP_Freq);
      state["PWM_Amplitude"]    = String(cp_package.CP_Max);
      state["PWM_NveAmplitude"] = String(cp_package.CP_Min);
      
    }
    relay_io->write1(EXP_RE_CP,LOW);
    delay(1000);
    if((cp_package.CP_Freq > 1005.0f) || (cp_package.CP_Freq < 995.0f) ) {state["PWM_Freq_Result"]          = false;}
    if((cp_package.CP_Max  <   8.37f) || (cp_package.CP_Max  >  9.59f) ) {state["PWM_Amplitude_Result"]     = false;}
    if((cp_package.CP_Min  <  -12.6f) || (cp_package.CP_Min  > -11.0f) ) {state["PWM_NveAmplitude_Result"]  = false;}
    if(float(state["MainsOffDelay"]) > 100.0f)                           {state["MainsOffDelay_Result"]     = false;}
    
    state_count = 0;
    serializeJson(state, DEBUG_Bluetooth);
    DEBUG_Bluetooth.println();
    DEBUG_Bluetooth.flush();
    Serial.println("Done test State C to B");
  }
}

void diode_PE_test (void) {
  if(req == "Diode_Test") {
    uint64_t timerNow;
    timerStart(timer);

    relay_io->write8(0x00); // Reset all Relay
    DynamicJsonDocument diode(512);
    diode["Diode_ShortCircuit_Result"]              = true;
    diode["PE_OpenCircuit_Result"]                  = true;
    diode["Diode_OpenCircuit_Result"]               = true;
    diode["Diode_ShortCircuit_MainsOffDelay"]       = "0";
    diode["PE_OpenCircuit_MainsOffDelay"]           = "0";
    diode["Diode_OpenCircuit_MainsOffDelay"]        = "0";
    // digitalWrite(RE_B, HIGH);
    relay_io->write1(EXP_RE_CP,HIGH); 
    delay(500);
    // digitalWrite(RE_C, HIGH); 
    relay_io->write1(EXP_RE_1k3,HIGH);
    delay(500);

#ifndef TEST_NO_LINE
    if(LineVoltage_OffTime(V1)){
      diode["Diode_ShortCircuit_Result"] = false;
      diode["PE_OpenCircuit_Result"]     = false;
      diode["Diode_OpenCircuit_Result"]  = false;
      serializeJson(diode, DEBUG_Bluetooth);
      DEBUG_Bluetooth.println();
      ESP.restart();
    }
#endif
    // nowTime = micros();
    // digitalWrite(RE_S, HIGH);

    timerRestart(timer);
    relay_io->write1(EXP_RE_Diode,HIGH);

#ifndef TEST_NO_LINE 
    while(1) {
      timerNow = timerRead(timer);
      if(digitalRead(V1)){
        delay(2);
        if(digitalRead(V1)) {
          diode["Diode_ShortCircuit_MainsOffDelay"] = String(timerNow / 40e3);
          break;
        }
      }
      if(timerNow > (5*40000000)) {
        diode["Diode_ShortCircuit_Result"] = false;
        diode["PE_OpenCircuit_Result"]     = false;
        diode["Diode_OpenCircuit_Result"]  = false;
        
        serializeJson(diode, DEBUG_Bluetooth);
        DEBUG_Bluetooth.println();
        timerStop(timer);
        ESP.restart();
      }
    }
#else
    diode["Diode_ShortCircuit_MainsOffDelay"] = "0";
#endif

    // digitalWrite(RE_C, LOW); 
    relay_io->write1(EXP_RE_1k3,LOW);
    delay(500);
    // digitalWrite(RE_S, LOW); 
    relay_io->write1(EXP_RE_Diode,LOW);
    delay(500);
    // digitalWrite(RE_C, HIGH); 
    relay_io->write1(EXP_RE_1k3,HIGH);
    delay(500);
#ifndef TEST_NO_LINE 
    timerStop(timer);
    if(LineVoltage_OffTime(V1)){
      diode["PE_OpenCircuit_Result"]     = false;
      diode["Diode_OpenCircuit_Result"]  = false;
      serializeJson(diode, DEBUG_Bluetooth);
      DEBUG_Bluetooth.println();
      ESP.restart();
    }
#endif

    timerStart(timer);
    // nowTime = micros();
    timerRestart(timer);
    // digitalWrite(RE_PE, HIGH);
    relay_io->write1(EXP_RE_PE,HIGH);

#ifndef TEST_NO_LINE 
    while(1) {
      timerNow = timerRead(timer);
      if(digitalRead(V1)){
        delay(2);
        if(digitalRead(V1)) {
          diode["PE_OpenCircuit_MainsOffDelay"] = String(timerNow / 40e3);
          break;
        }
      }
      if(timerNow > (5 * 40000000)) {
        diode["PE_OpenCircuit_Result"]     = false;
        diode["Diode_OpenCircuit_Result"]  = false;
        serializeJson(diode, DEBUG_Bluetooth);
        DEBUG_Bluetooth.println();
        timerStop(timer);
        ESP.restart();
      }
    }
#endif
    // digitalWrite(RE_C, LOW); 
    relay_io->write1(EXP_RE_1k3,LOW);
    delay(500);
    // digitalWrite(RE_PE, LOW); 
    relay_io->write1(EXP_RE_PE,LOW);
    delay(500);
    // digitalWrite(RE_C, HIGH);
    relay_io->write1(EXP_RE_1k3,HIGH);
    delay(500);
#ifndef TEST_NO_LINE 
    timerStop(timer);
    if(LineVoltage_OffTime(V1)){
      diode["Diode_OpenCircuit_Result"] = false;
      serializeJson(diode, DEBUG_Bluetooth);
      DEBUG_Bluetooth.println();
      
      ESP.restart();
    }
#endif
    // nowTime = micros();
    // digitalWrite(RE_B, LOW);
    timerRestart(timer);
    relay_io->write1(EXP_RE_CP,LOW);
#ifndef TEST_NO_LINE 
    while(1) {
      timerNow = timerRead(timer);
      if(digitalRead(V1)){
        delay(2);
        if(digitalRead(V1)) {
          diode["Diode_OpenCircuit_MainsOffDelay"] = String(timerNow / 40e3);
          break;
        }
      }
      if(timerNow > (5 * 40000000)) {
        diode["Diode_OpenCircuit_Result"] = false;
        serializeJson(diode, DEBUG_Bluetooth);
        DEBUG_Bluetooth.println();
        DEBUG_Bluetooth.flush();
        timerStop(timer);
        ESP.restart();
      }
    }
#endif
    // digitalWrite(RE_C, LOW); 
    relay_io->write1(EXP_RE_1k3,LOW);
    delay(1000);
    serializeJson(diode, DEBUG_Bluetooth);
    DEBUG_Bluetooth.println();
    DEBUG_Bluetooth.flush();
    timerStop(timer);
  }
}

void RCD0_Test (void) {
  if(req == "RCD0_Test") {
    timerStart(timer);
    timerRestart(timer);

    relay_io->write8(0x00); // Reset all Relay
    DynamicJsonDocument rcd(256);
    rcd["RCD0_Result"] = true;
    rcd["Trip_Time"]   = "0";
    rcd["Limit"]       = "100";
    rcd["Current"]     = "30";

    // digitalWrite(RE_B, HIGH);
    relay_io->write1(EXP_RE_CP,HIGH); 
    delay(500);
    // digitalWrite(RE_C, HIGH); 
    relay_io->write1(EXP_RE_1k3,HIGH);
    delay(500);
    if(LineVoltage_OffTime(V1)){
      rcd["RCD0_Result"] = false;
      serializeJson(rcd, DEBUG_Bluetooth);
      DEBUG_Bluetooth.println();
      DEBUG_Bluetooth.flush();
      ESP.restart();
    }
    // digitalWrite(RE_LEAK, HIGH);
    uint8_t phase_testing = 0;
    if(digitalRead(V1)){
      relay_io->write1(EXP_RE_LeakL1,HIGH);
      phase_testing = 1;
    }else if(digitalRead(V2)){
      relay_io->write1(EXP_RE_LeakL2,HIGH);
      phase_testing = 2;
    }else if(digitalRead(V3)){
      relay_io->write1(EXP_RE_LeakL3,HIGH);
      phase_testing = 4;
    }else{
      relay_io->write8(relay_io->read8() | ((1<<EXP_RE_LeakL1) | (1<<EXP_RE_LeakL2) | (1<<EXP_RE_LeakL3)));
      phase_testing = 8;
    }
    // nowTime = micros();
    timerRestart(timer);
    while(1) {
      uint64_t timerNow = timerRead(timer);
      switch (phase_testing) {
      case 1:
        if(digitalRead(V1)){
          delay(2);
          if(digitalRead(V1)) {
            rcd["Trip_Time"] = String(timerNow / 40e3);
            break;
          }
        }
        break;
      case 2:
        if(digitalRead(V2)){
          delay(2);
          if(digitalRead(V2)) {
            rcd["Trip_Time"] = String(timerNow / 40e3);
            break;
          }
        }
        break;
      case 4:
        if(digitalRead(V3)){
          delay(2);
          if(digitalRead(V3)) {
            rcd["Trip_Time"] = String(timerNow / 40e3);
            break;
          }
        }
        break;
      case 8:
        /* code */
        break;
      default:
        break;
      }
      
      if(timerNow > (40000 * 300)) {
        rcd["RCD0_Result"] = false;
        serializeJson(rcd, DEBUG_Bluetooth);
        DEBUG_Bluetooth.println();
        DEBUG_Bluetooth.flush();
        ESP.restart();
      }
    }
    // digitalWrite(RE_LEAK, LOW);
    relay_io->write8(relay_io->read8() & ~((1<<EXP_RE_LeakL1) | (1<<EXP_RE_LeakL2) | (1<<EXP_RE_LeakL3)));
    delay(1000);

    // digitalWrite(RE_C,    LOW);
    relay_io->write1(EXP_RE_1k3,LOW); 
    delay(1000);
    // digitalWrite(RE_B,    LOW); 
    relay_io->write1(EXP_RE_CP,LOW);
    delay(1000);
    serializeJson(rcd, DEBUG_Bluetooth);
    DEBUG_Bluetooth.println();
    DEBUG_Bluetooth.flush();
  }
}

void Insulator_Test (void) {
  if(req == "Insulator_Test") {
    relay_io->write8(0x00); // Reset all Relay
    DynamicJsonDocument insulation(256);
    insulation["Insulation_Testing"] = true;
    insulation["L_PE"]               = "0";
    insulation["N_PE"]               = "0";
    insulation["Voltage"]            = "0";
    delay(3000);
     if(LineVoltage_OnTime(V1)){
      insulation["Insulation_Testing"] = false;
      serializeJson(insulation, DEBUG_Bluetooth);
      DEBUG_Bluetooth.println();
      DEBUG_Bluetooth.flush();
      ESP.restart();
    }
    String data = requestPackageFromAVR(req_InsulationVoltage);
    if(data != "") {
      data = data.substring(1);
      char record[20], *i;
      data.toCharArray(record,20);
      insulation["Voltage"]  = strtok_r(record, "~", &i);
    }
    else { insulation["Insulation_Testing"] = false;}
    delay(3000);
    digitalWrite(RE_L_HV, HIGH);
    digitalWrite(RE_PE_HV,HIGH);
    delay(60000);
    data = requestPackageFromAVR(req_Insulation);
    if(data != "") {
      data = data.substring(1);
      char record[20], *i;
      data.toCharArray(record,20);
      String dat = strtok_r(record, "~", &i);
      if(dat == "50000000") {insulation["L_PE"]  = ">50000000";}
      else {
        insulation["L_PE"]  = dat;
        if(float(insulation["L_PE"]) < 7000000) {insulation["Insulation_Testing"] = false;}
      }
    }
    else { insulation["Insulation_Testing"] = false;}
    digitalWrite(RE_L_HV,  LOW);
    digitalWrite(RE_PE_HV, LOW);
    delay(3000);
    digitalWrite(RE_N_HV,  HIGH);
    digitalWrite(RE_PE_HV, HIGH);
    delay(60000);
    data = requestPackageFromAVR(req_Insulation);
    if(data != "") {
      data = data.substring(1);
      char record[20], *i;
      data.toCharArray(record,20);
      String dat = strtok_r(record, "~", &i);
      if(dat == "50000000") {insulation["N_PE"]  = ">50000000";}
      else {
        insulation["N_PE"]  = dat;
        if(float(insulation["N_PE"]) < 7000000) {insulation["Insulation_Testing"] = false;}
      }
    }
    else { insulation["Insulation_Testing"] = false;}
    digitalWrite(RE_N_HV,  LOW);
    digitalWrite(RE_PE_HV, LOW);
    serializeJson(insulation, DEBUG_Bluetooth);
    DEBUG_Bluetooth.println();
    DEBUG_Bluetooth.flush();
  }
}

void state_Manual (void) {
  if(req == "A") {
    // digitalWrite(RE_B, LOW); 
    // digitalWrite(RE_C, LOW);
    // digitalWrite(RE_D, LOW); 
    // digitalWrite(RE_S, LOW); 
    // digitalWrite(RE_PE, LOW); 
    // digitalWrite(RE_LEAK, LOW);
    relay_io->write8(Test_State_A); 
  }
  else if(req == "B") {
    // digitalWrite(RE_B, HIGH); 
    // digitalWrite(RE_C, LOW);
    // digitalWrite(RE_D, LOW); 
    // digitalWrite(RE_S, LOW);
    relay_io->write8(Test_State_B); 
  }
  else if(req == "C") {
    // digitalWrite(RE_B, HIGH); 
    // digitalWrite(RE_C, HIGH);
    // digitalWrite(RE_D, LOW); 
    // digitalWrite(RE_S, LOW); 
    relay_io->write8(Test_State_C);
  }
  else if(req == "D") {
    // digitalWrite(RE_B, HIGH); 
    // digitalWrite(RE_C, LOW);
    // digitalWrite(RE_D, HIGH); 
    // digitalWrite(RE_S, LOW); 
    relay_io->write8(Test_State_D);
  }
  else if(req == "S") {
    // digitalWrite(RE_S, HIGH);
    relay_io->write1(EXP_RE_Diode,HIGH);
  } else if(req == "PE") {
    // digitalWrite(RE_PE, HIGH);
    
  } else if(req == "LEAK") {
    // digitalWrite(RE_LEAK, HIGH);
    // delay(200);
    // digitalWrite(RE_LEAK, LOW);
    uint8_t phase_testing = 0;
    if(digitalRead(V1)){
      relay_io->write1(EXP_RE_LeakL1,HIGH);
      phase_testing = 1;
    }else if(digitalRead(V2)){
      relay_io->write1(EXP_RE_LeakL2,HIGH);
      phase_testing = 2;
    }else if(digitalRead(V3)){
      relay_io->write1(EXP_RE_LeakL3,HIGH);
      phase_testing = 4;
    }else{
      relay_io->write8(relay_io->read8() | ((1<<EXP_RE_LeakL1) | (1<<EXP_RE_LeakL2) | (1<<EXP_RE_LeakL3)));
      phase_testing = 8;
    }
    delay(1000);
    relay_io->write8(relay_io->read8() & ~((1<<EXP_RE_LeakL1) | (1<<EXP_RE_LeakL2) | (1<<EXP_RE_LeakL3)));
  }
  else if(req == "METER") {
    if(DLT645_init()) {
      delay(3000);
      read_Meter();
      DEBUG_Bluetooth.println(V);
      DEBUG_Bluetooth.println(f);
    }
    else {
      DEBUG_Bluetooth.println("Meter Fail");
    }
  }
}