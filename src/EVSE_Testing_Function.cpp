#include <Arduino.h>
#include <EVSE_Testing_Functin.h>
#include <Public_Variable.h>
#include <Hardware_def.h>
#include <ArduinoJson.h>
#include <DLT645.h>
#include <MCP23S08.h>
#include <AVR128DA48/avr128.h>

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
    relay_io->write1(EXP_RE_CP,HIGH); // Goto State A
    delay(500);
    // digitalWrite(RE_C, HIGH); 
    relay_io->write1(EXP_RE_1k3,HIGH); // Goto State B
    delay(500);

#ifndef TEST_NO_LINE
    if(LineVoltage_beforeOff_Test(V1)){
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
      if(!digitalRead(V1)){
        delay(2);
        if(!digitalRead(V1)) {
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
    // digitalWrite(RE_S, LOW); 
    relay_io->write1(EXP_RE_Diode,LOW);
    delay(500);
    // digitalWrite(RE_C, HIGH); 
    relay_io->write1(EXP_RE_1k3,HIGH);
    delay(500);
#ifndef TEST_NO_LINE 
    timerStop(timer);
    if(LineVoltage_beforeOff_Test(V1)){
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
      if(!digitalRead(V1)){
        delay(2);
        if(!digitalRead(V1)) {
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
    if(LineVoltage_beforeOff_Test(V1)){
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
    if(LineVoltage_beforeOff_Test(V1)){
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
     if(LineVoltage_beforeOn_Test(V1)){
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
      read_Meter(&V,&f);
    }
    else {
      
    }
  }
}

void send_JSON_state(PWM_test_package package){
  DynamicJsonDocument state(512);
  switch(package.State_To_Test){
    case null_state :
      state["State_To_Test"] = "nullState";
      break;
    case State_A_TO_B :
      state["State_To_Test"] = "A_to_B";
      break;
    case State_B_TO_C :
      state["State_To_Test"] = "B_to_C";
      break;
    case State_B_TO_D :
      state["State_To_Test"] = "B_to_D";
      break;
    case State_C_TO_B :
      state["State_To_Test"] = "C_to_B";
      break;
    default:
      state["State_To_Test"] = "-";
      break;
  }
  
  state["PWM_StartupDelay"]   = String(package.pwm_start_delay_ms);
  state["PWM_Amplitude"]      = String(package.pwm_amplitude_v);
  state["PWM_NveAmplitude"]   = String(package.pwm_nve_amplitude_v);
  state["PWM_Freq"]           = String(package.pwm_freq_hz);
  state["PWM_DutyCycle"]      = String(package.duty_ratio);
  state["PWM_Imax"]           = String(package.pwm_Imax);
  state["MainsOnDelay"]       = String(package.main_voltage_onDelay_ms);
  state["MainsOffDelay"]      = String(package.main_voltage_offDelay_ms);
  state["MainsFreq"]          = String(package.main_freq_hz);
  state["Voltage"]            = String(package.main_voltage);
  state["PP"]                 = String(package.pp_Imax);

  state["PWM_StartupDelay_Result"]   = package.pwm_start_delay_result;
  state["PWM_Amplitude_Result"]      = package.pwm_amplitude_result;
  state["PWM_NveAmplitude_Result"]   = package.pwm_nve_amplitude_result;
  state["PWM_Freq_Result"]           = package.pwm_freq_result;
  state["PWM_DutyCycle_Result"]      = package.pwm_duty_result;
  state["PWM_Imax_Result"]           = package.pwm_Imax_result;
  state["MainsOnDelay_Result"]       = package.main_on_delay_result;
  state["MainsOffDelay_Result"]      = package.main_off_delay_result;
  state["MainsFreq_Result"]          = package.main_freq_result;
  state["Voltage_Result"]            = package.main_voltage_result;
  state["PP_Result"]                 = package.pp_rating_result;

  serializeJson(state, DEBUG_Bluetooth);
  DEBUG_Bluetooth.println();

#ifdef DEBUG_Bluetooth_Package
  serializeJson(state,Serial);
  Serial.println();
#endif

}
void send_JSON_Diode(Diode_test_package package){
  DynamicJsonDocument diode_package(512);

  diode_package["Diode_ShortCircuit_Result"]              = package.Diode_ShortCircuit_Result;
  diode_package["PE_OpenCircuit_Result"]                  = package.PE_OpenCircuit_Result;
  diode_package["Diode_OpenCircuit_Result"]               = package.Diode_OpenCircuit_Result;
  diode_package["Diode_ShortCircuit_MainsOffDelay"]       = String(package.Diode_ShortCircuit_MainsOffDelay);
  diode_package["PE_OpenCircuit_MainsOffDelay"]           = String(package.PE_OpenCircuit_MainsOffDelay);
  diode_package["Diode_OpenCircuit_MainsOffDelay"]        = String(package.Diode_OpenCircuit_MainsOffDelay);

  serializeJson(diode_package, DEBUG_Bluetooth);
  DEBUG_Bluetooth.println();
#ifdef DEBUG_Bluetooth_Package
  serializeJson(diode_package,Serial);
  Serial.println();
#endif
}
void clear_Diode_Test_Package(Diode_test_package * package){
  package->Diode_OpenCircuit_MainsOffDelay = 0.0f;
  package->Diode_ShortCircuit_MainsOffDelay = 0.0f;
  package->Diode_OpenCircuit_MainsOffDelay = 0.0f;

  package->Diode_OpenCircuit_Result = false;
  package->Diode_ShortCircuit_Result = false;
  package->PE_OpenCircuit_Result = false;
}

void Test_main_on_Delay( uint8_t line, void (*onStart)(), void (*onPass)(float delay), void (*onTimeout)(),uint32_t timeout_ms){
#ifndef TEST_NO_LINE
  timerStart(timer);
  timerRestart(timer);
  onStart();

  uint64_t timerNow = timerRead(timer);
  while(timerNow <= (timeout_ms*40000)) {
    timerNow = timerRead(timer);
    if(digitalRead(line)){ // If line avaliable -> Pass
      onPass(timerNow / 40e3);
      break;
    } else if(timerNow > (timeout_ms*40000)) {
      onTimeout();
    }
  }
  timerStop(timer);
#else
  onStart();
  onPass(0.0f);
#endif
}
void Test_main_off_Delay(uint8_t line, void (*onStart)(), void (*onPass)(float delay), void (*onTimeout)(),uint32_t timeout_ms){
#ifndef TEST_NO_LINE
  timerStart(timer);
  timerRestart(timer);

  onStart();
  uint64_t timerNow = timerRead(timer);
  while(timerNow <= (timeout_ms*40000)) {
    timerNow = timerRead(timer);
    if(!digitalRead(line)){
      vTaskDelay(2);
      if(!digitalRead(line)) {
        onPass(timerNow / 40e3);
        break;
      }
    } else if(timerNow > (timeout_ms*40000)) {
      onTimeout();
    }
  }
  timerStop(timer);
#else
  onStart();
  onPass(0.0f);
#endif
}

void clear_PWM_Test_Package(PWM_test_package * package){
  package->State_To_Test = null_state;
    
  package->pwm_start_delay_ms = 0.0f;
  package->pwm_amplitude_v = 0.0f;
  package->pwm_nve_amplitude_v = 0.0f;
  package->pwm_freq_hz = 0.0f;
  package->duty_ratio = 0.0f;
  package->pwm_Imax = 0.0f;
  package->pp_Imax = 0.0f;
  package->main_voltage_onDelay_ms = 0.0f;
  package->main_voltage_offDelay_ms = 0.0f;
  package->main_freq_hz = 0.0f;
  package->main_voltage = 0.0f;

  package->pwm_start_delay_result = false;
  package->pwm_amplitude_result = false;
  package->pwm_nve_amplitude_result = false;
  package->pwm_freq_result = false;
  package->pwm_duty_result = false;
  package->pwm_Imax_result = false;
  package->pp_rating_result = false;
  package->main_on_delay_result = false;
  package->main_off_delay_result = false;
  package->main_freq_result = false;
  package->main_voltage_result = false;
}
void clear_RCD_Test_Package(RCD_Test_package * package){
  package->LimitTrip_time = 0.0f;
  package->TestingCurrent = 0.0f;
  package->TripTime_ms = 0.0f;
  package->RCD0_Result = false;
  package->RCD1_Result = false;
  package->RCD2_Result = false;
}
void send_JSON_RCD(RCD_Test_package package){
  DynamicJsonDocument rcd(256);
  if(!package.noTrip){
    rcd["RCD0_Result"] = package.RCD0_Result;
    rcd["Trip_Time"]   = String(package.TripTime_ms);
    rcd["Limit"]       = String(package.LimitTrip_time);
    rcd["Current"]     = String(package.TestingCurrent);
  } else { // RCD not trip
    rcd["RCD0_Result"] = false;
    rcd["Trip_Time"]   = "> 250ms";
    rcd["Limit"]       = String(package.LimitTrip_time);
    rcd["Current"]     = String(package.TestingCurrent);
  }
  
  serializeJson(rcd, DEBUG_Bluetooth);
  DEBUG_Bluetooth.println();
#ifdef DEBUG_Bluetooth_Package
  serializeJson(rcd,Serial);
  Serial.println();
#endif
}
void waitLineOn(uint8_t line, uint32_t timeout_ms){
  timerStart(timer);
  timerRestart(timer);
  while (timerRead(timer) < (timeout_ms * 40000)) {
    if(digitalRead(line)){
      break;
    }
  }
  timerStop(timer);
}