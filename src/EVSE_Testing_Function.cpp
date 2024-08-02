#include <Arduino.h>
#include <EVSE_Testing_Functin.h>
#include <Public_Variable.h>
#include <Hardware_def.h>
#include <ArduinoJson.h>
#include <DLT645.h>

void state_A_to_B (void) {
  if(req == "State_A_to_B") {
    digitalWrite(RE_L_HV, LOW);
    digitalWrite(RE_N_HV, LOW);
    digitalWrite(RE_S,    LOW);
    digitalWrite(RE_B,    LOW);
    digitalWrite(RE_C,    LOW);
    digitalWrite(RE_D,    LOW);
    digitalWrite(RE_PE,   LOW);
    digitalWrite(RE_LEAK, LOW);
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

    delay(1000);
    digitalWrite(RE_B,    HIGH); delay(3000);
    String data = requestPackageFromAVR('A');
    if(data != "") {
      data = data.substring(1);
      char record[20], *i;
      data.toCharArray(record,20);
      state["PWM_DutyCycle"]  = strtok_r(record, ";", &i);
      state["PWM_Freq"]       = strtok_r(NULL, "~", &i);
    }
    data = requestPackageFromAVR('B');
    if(data != "") {
      data = data.substring(1);
      char record[50], *i;
      data.toCharArray(record,50);
      state["PWM_Amplitude"]     = strtok_r(record, ";", &i);
      state["PWM_NveAmplitude"]  = strtok_r(NULL, ";", &i);
    }
    if(int(state["PWM_Freq"]) > 1005 || int(state["PWM_Freq"]) < 995)                        {state["PWM_Freq_Result"] = false;}
    if(float(state["PWM_Amplitude"])    < 8.37  || float(state["PWM_Amplitude"])    > 9.59)  {state["PWM_Amplitude_Result"] = false;}
    if(float(state["PWM_NveAmplitude"]) < -12.6 || float(state["PWM_NveAmplitude"]) > -11.0) {state["PWM_NveAmplitude_Result"] = false;}
    state_count = 1;
    serializeJson(state, DEBUG);
    DEBUG.println();
  }
}

void state_B_to_C (void) {
  if(req == "State_B_to_C" && state_count == 1) {
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

    if(OnRelay_Check()){
      state["Voltage_Result"] = false;
      serializeJson(state, DEBUG);
      DEBUG.println();
      ESP.restart();
    }
    nowTime = micros();
    digitalWrite(RE_C, HIGH);
    while(1) {
      if(!digitalRead(V1)){
        state["MainsOnDelay"] = String(float(micros() - nowTime)/1000.0);
        break;
      }
      if((micros() - nowTime) > 5000000) {
        state["MainsOnDelay_Result"]  = false;
        serializeJson(state, DEBUG);
        DEBUG.println();
        ESP.restart();
      }
    }
    String data = requestPackageFromAVR('A');
    if(data != "") {
      data = data.substring(1);
      char record[20], *i;
      data.toCharArray(record,20);
      state["PWM_DutyCycle"]  = strtok_r(record, ";", &i);
      state["PWM_Freq"]       = strtok_r(NULL, "~", &i);
      state["PWM_Imax"]       = String(float(state["PWM_DutyCycle"])*0.6);
    }
    delay(3000);
    data =requestPackageFromAVR('E');
    if(data != "") {
      data = data.substring(1);
      char record[20], *i;
      data.toCharArray(record,20);
      state["PP"]     = strtok_r(record, "~", &i);
    }
    delay(3000);
    data = requestPackageFromAVR('B');
    if(data != "") {
      data = data.substring(1);
      char record[50], *i;
      data.toCharArray(record,50);
      state["PWM_Amplitude"]     = strtok_r(record, ";", &i);
      state["PWM_NveAmplitude"]  = strtok_r(NULL, ";", &i);
    }
    delay(3000);
    for(uint8_t i=0; i<5; i++) {
      if(DLT645_init()) {
        delay(3000);
        read_Meter();
        state["Voltage"] = String(V);
        state["MainsFreq"] = String(f);
        break;
      }
    }
    if(int(state["PWM_Freq"]) > 1005 || int(state["PWM_Freq"]) < 995)                        {state["PWM_Freq_Result"]        = false;}
    if(float(state["PWM_Amplitude"])    < 5.47  || float(state["PWM_Amplitude"])    > 6.53)  {state["PWM_Amplitude_Result"]   = false;}
    if(float(state["PWM_NveAmplitude"]) < -12.6 || float(state["PWM_NveAmplitude"]) > -11.0) {state["PWM_NveAmplitude_Result"]= false;}
    if(float(state["PWM_Imax"]) > float(state["PP"]))                                        {state["PWM_Imax"]               = false;}
    if(float(state["MainsOnDelay"]) > 3000)                                                  {state["MainsOnDelay_Result"]    = false;}
    if(float(state["Voltage"]) < 210 || float(state["Voltage"]) > 240)                       {state["Voltage_Result"]         = false;}
    if(float(state["MainsFreq"]) < 49 || float(state["MainsFreq"]) > 51)                     {state["MainsFreq_Result"]       = false;}
    state_count = 2;
    serializeJson(state, DEBUG);
    DEBUG.println();
  }
}

void state_B_to_D (void) {
  if(req == "State_B_to_D" && state_count == 2) {
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

    if(OffRelay_Check()){
      state["Voltage_Result"] = false;
      serializeJson(state, DEBUG);
      DEBUG.println();
      ESP.restart();
    }

    digitalWrite(RE_D, HIGH);
    String data = requestPackageFromAVR('A');
    if(data != "") {
      data = data.substring(1);
      char record[20], *i;
      data.toCharArray(record,20);
      state["PWM_DutyCycle"]  = strtok_r(record, ";", &i);
      state["PWM_Freq"]       = strtok_r(NULL, "~", &i);
      state["PWM_Imax"]       = String(float(state["PWM_DutyCycle"])*0.6);
    }
    delay(3000);
    data = requestPackageFromAVR('B');
    if(data != "") {
      data = data.substring(1);
      char record[50], *i;
      data.toCharArray(record,50);
      state["PWM_Amplitude"]     = strtok_r(record, ";", &i);
      state["PWM_NveAmplitude"]  = strtok_r(NULL, ";", &i);
    }
    delay(3000);
    data =requestPackageFromAVR('E');
    if(data != "") {
      data = data.substring(1);
      char record[20], *i;
      data.toCharArray(record,20);
      state["PP"]     = strtok_r(record, "~", &i);
    }
    delay(3000);
    for(uint8_t i=0; i<5; i++) {
      if(DLT645_init()) {
        delay(3000);
        read_Meter();
        state["Voltage"] = String(V);
        state["MainsFreq"] = String(f);
        break;
      }
    }
    digitalWrite(RE_D, LOW); delay(1000);
    if(int(state["PWM_Freq"]) > 1005 || int(state["PWM_Freq"]) < 995)                        {state["PWM_Freq_Result"]        = false;}
    if(float(state["PWM_Amplitude"])    < 2.59  || float(state["PWM_Amplitude"])    > 3.28)  {state["PWM_Amplitude_Result"]   = false;}
    if(float(state["PWM_NveAmplitude"]) < -12.6 || float(state["PWM_NveAmplitude"]) > -11.0) {state["PWM_NveAmplitude_Result"]= false;}
    if(float(state["PWM_Imax"]) > float(state["PP"]))                                        {state["PWM_Imax"]               = false;}
    if(float(state["MainsOnDelay"]) > 3000)                                                  {state["MainsOnDelay_Result"]    = false;}
    if(float(state["Voltage"]) < 210 || float(state["Voltage"]) > 240)                       {state["Voltage_Result"]         = false;}
    if(float(state["MainsFreq"]) < 49 || float(state["MainsFreq"]) > 51)                     {state["MainsFreq_Result"]       = false;}
    state_count = 3;
    serializeJson(state, DEBUG);
    DEBUG.println();
  }
}

void state_C_to_B (void) {
  if(req == "State_C_to_B" && state_count == 3) {
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
    state["MainsOffDelay_Result"]       = true;
    state["MainsFreq_Result"]          = true;
    state["Voltage_Result"]            = true;
    state["PP_Result"]                 = true;

    delay(1000);

    if(OffRelay_Check()){
      state["Voltage_Result"] = false;
      serializeJson(state, DEBUG);
      DEBUG.println();
      ESP.restart();
    }

    nowTime = micros();
    digitalWrite(RE_C, LOW);
    while(1) {
      if(digitalRead(V1)){
        delay(2);
        if(digitalRead(V1)) {
          state["MainsOffDelay"] = String(float(micros() - nowTime)/1000.0);
          break;
        }
      }
      if((micros() - nowTime) > 5000000) {
        state["MainsOffDelay_Result"] = false;
        serializeJson(state, DEBUG);
        DEBUG.println();
        ESP.restart();
      }
    }
    String data = requestPackageFromAVR('A');
    if(data != "") {
      data = data.substring(1);
      char record[20], *i;
      data.toCharArray(record,20);
      state["PWM_DutyCycle"]  = strtok_r(record, ";", &i);
      state["PWM_Freq"]       = strtok_r(NULL, "~", &i);
    }
    delay(3000);
    data = requestPackageFromAVR('B');
    if(data != "") {
      data = data.substring(1);
      char record[50], *i;
      data.toCharArray(record,50);
      state["PWM_Amplitude"]     = strtok_r(record, ";", &i);
      state["PWM_NveAmplitude"]  = strtok_r(NULL, ";", &i);
    }
    digitalWrite(RE_B, LOW); delay(1000);
    if(int(state["PWM_Freq"]) > 1005 || int(state["PWM_Freq"]) < 995)                        {state["PWM_Freq_Result"] = false;}
    if(float(state["PWM_Amplitude"])    < 8.37  || float(state["PWM_Amplitude"])    > 9.59)  {state["PWM_Amplitude_Result"] = false;}
    if(float(state["PWM_NveAmplitude"]) < -12.6 || float(state["PWM_NveAmplitude"]) > -11.0) {state["PWM_NveAmplitude_Result"] = false;}
    if(float(state["MainsOffDelay"]) > 100)                                                  {state["MainsOffDelay_Result"] = false;}
    state_count = 0;
    serializeJson(state, DEBUG);
    DEBUG.println();
  }
}

void diode_PE_test (void) {
  if(req == "Diode_Test") {
    digitalWrite(RE_L_HV, LOW);
    digitalWrite(RE_N_HV, LOW);
    digitalWrite(RE_S,    LOW);
    digitalWrite(RE_B,    LOW);
    digitalWrite(RE_C,    LOW);
    digitalWrite(RE_D,    LOW);
    digitalWrite(RE_PE,   LOW);
    digitalWrite(RE_LEAK, LOW);
    DynamicJsonDocument diode(512);
    diode["Diode_ShortCircuit_Result"]              = true;
    diode["PE_OpenCircuit_Result"]                  = true;
    diode["Diode_OpenCircuit_Result"]               = true;
    diode["Diode_ShortCircuit_MainsOffDelay"]       = "0";
    diode["PE_OpenCircuit_MainsOffDelay"]           = "0";
    diode["Diode_OpenCircuit_MainsOffDelay"]        = "0";
    delay(3000);
    digitalWrite(RE_B, HIGH); delay(1000);
    digitalWrite(RE_C, HIGH); delay(1000);
    if(OffRelay_Check()){
      diode["Diode_ShortCircuit_Result"] = false;
      diode["PE_OpenCircuit_Result"]     = false;
      diode["Diode_OpenCircuit_Result"]  = false;
      serializeJson(diode, DEBUG);
      DEBUG.println();
      ESP.restart();
    }
    nowTime = micros();
    digitalWrite(RE_S, HIGH);
    while(1) {
      if(digitalRead(V1)){
        delay(2);
        if(digitalRead(V1)) {
          diode["Diode_ShortCircuit_MainsOffDelay"] = String(float(micros() - nowTime)/1000.0);
          break;
        }
      }
      if((micros() - nowTime) > 5000000) {
        diode["Diode_ShortCircuit_Result"] = false;
        diode["PE_OpenCircuit_Result"]     = false;
        diode["Diode_OpenCircuit_Result"]  = false;
        
        serializeJson(diode, DEBUG);
        DEBUG.println();
        ESP.restart();
      }
    }
    digitalWrite(RE_C, LOW); delay(1000);
    digitalWrite(RE_S, LOW); delay(1000);
    delay(3000);
    digitalWrite(RE_C, HIGH); delay(1000);
    if(OffRelay_Check()){
      diode["PE_OpenCircuit_Result"]     = false;
      diode["Diode_OpenCircuit_Result"]  = false;
      serializeJson(diode, DEBUG);
      DEBUG.println();
      ESP.restart();
    }
    nowTime = micros();
    digitalWrite(RE_PE, HIGH);
    while(1) {
      if(digitalRead(V1)){
        delay(2);
        if(digitalRead(V1)) {
          diode["PE_OpenCircuit_MainsOffDelay"] = String(float(micros() - nowTime)/1000.0);
          break;
        }
      }
      if((micros() - nowTime) > 5000000) {
        diode["PE_OpenCircuit_Result"]     = false;
        diode["Diode_OpenCircuit_Result"]  = false;
        serializeJson(diode, DEBUG);
        DEBUG.println();
        ESP.restart();
      }
    }
    digitalWrite(RE_C, LOW); delay(1000);
    digitalWrite(RE_PE, LOW); delay(1000);
    delay(3000);
    digitalWrite(RE_C, HIGH); delay(1000);
    if(OffRelay_Check()){
      diode["Diode_OpenCircuit_Result"] = false;
      serializeJson(diode, DEBUG);
      DEBUG.println();
      ESP.restart();
    }
    nowTime = micros();
    digitalWrite(RE_B, LOW);
    while(1) {
      if(digitalRead(V1)){
        delay(2);
        if(digitalRead(V1)) {
          diode["Diode_OpenCircuit_MainsOffDelay"] = String(float(micros() - nowTime)/1000.0);
          break;
        }
      }
      if((micros() - nowTime) > 5000000) {
        diode["Diode_OpenCircuit_Result"] = false;
        serializeJson(diode, DEBUG);
        DEBUG.println();
        ESP.restart();
      }
    }
    digitalWrite(RE_C, LOW); delay(1000);
    serializeJson(diode, DEBUG);
    DEBUG.println();
  }
}

void RCD0_Test (void) {
  if(req == "RCD0_Test") {
    digitalWrite(RE_L_HV, LOW);
    digitalWrite(RE_N_HV, LOW);
    digitalWrite(RE_S,    LOW);
    digitalWrite(RE_B,    LOW);
    digitalWrite(RE_C,    LOW);
    digitalWrite(RE_D,    LOW);
    digitalWrite(RE_PE,   LOW);
    digitalWrite(RE_LEAK, LOW);
    DynamicJsonDocument rcd(256);
    rcd["RCD0_Result"] = true;
    rcd["Trip_Time"]   = "0";
    rcd["Limit"]       = "100";
    rcd["Current"]     = "30";
    delay(3000);

    digitalWrite(RE_B, HIGH); delay(1000);
    digitalWrite(RE_C, HIGH); delay(1000);
    if(OffRelay_Check()){
      rcd["RCD0_Result"] = false;
      serializeJson(rcd, DEBUG);
      DEBUG.println();
      ESP.restart();
    }
    digitalWrite(RE_LEAK, HIGH);
    nowTime = micros();
    while(1) {
      if(digitalRead(V1)){
        delay(2);
        if(digitalRead(V1)) {
          rcd["Trip_Time"] = String(float(micros() - nowTime)/1000.0);
          break;
        }
      }
      if((micros() - nowTime) > 300000) {
        rcd["RCD0_Result"] = false;
        serializeJson(rcd, DEBUG);
        DEBUG.println();
        ESP.restart();
      }
    }
    digitalWrite(RE_LEAK, LOW); delay(1000);
    digitalWrite(RE_C,    LOW); delay(1000);
    digitalWrite(RE_B,    LOW); delay(1000);
    serializeJson(rcd, DEBUG);
    DEBUG.println();
  }
}

void Insulator_Test (void) {
  if(req == "Insulator_Test") {
    digitalWrite(RE_L_HV, LOW);
    digitalWrite(RE_N_HV, LOW);
    digitalWrite(RE_PE_HV,LOW);
    digitalWrite(RE_S,    LOW);
    digitalWrite(RE_B,    LOW);
    digitalWrite(RE_C,    LOW);
    digitalWrite(RE_D,    LOW);
    digitalWrite(RE_PE,   LOW);
    digitalWrite(RE_LEAK, LOW);
    DynamicJsonDocument insulation(256);
    insulation["Insulation_Testing"] = true;
    insulation["L_PE"]               = "0";
    insulation["N_PE"]               = "0";
    insulation["Voltage"]            = "0";
    delay(3000);
     if(OnRelay_Check()){
      insulation["Insulation_Testing"] = false;
      serializeJson(insulation, DEBUG);
      ESP.restart();
    }
    String data = requestPackageFromAVR('C');
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
    data = requestPackageFromAVR('D');
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
    data = requestPackageFromAVR('D');
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
    serializeJson(insulation, DEBUG);
    DEBUG.println();
  }
}

void state_Manual (void) {
  if(req == "A") { 
    digitalWrite(RE_B, LOW); 
    digitalWrite(RE_C, LOW);
    digitalWrite(RE_D, LOW); 
    digitalWrite(RE_S, LOW); 
    digitalWrite(RE_PE, LOW); 
    digitalWrite(RE_LEAK, LOW); 
  }
  else if(req == "B") {
    digitalWrite(RE_B, HIGH); 
    digitalWrite(RE_C, LOW);
    digitalWrite(RE_D, LOW); 
    digitalWrite(RE_S, LOW); 
  }
  else if(req == "C") {
    digitalWrite(RE_B, HIGH); 
    digitalWrite(RE_C, HIGH);
    digitalWrite(RE_D, LOW); 
    digitalWrite(RE_S, LOW); 
  }
  else if(req == "D") {
    digitalWrite(RE_B, HIGH); 
    digitalWrite(RE_C, LOW);
    digitalWrite(RE_D, HIGH); 
    digitalWrite(RE_S, LOW); 
  }
  else if(req == "S") {digitalWrite(RE_S, HIGH);}
  else if(req == "PE") {digitalWrite(RE_PE, HIGH);}
  else if(req == "LEAK") {
    digitalWrite(RE_LEAK, HIGH);
    delay(200);
    digitalWrite(RE_LEAK, LOW);
  }
  else if(req == "METER") {
    if(DLT645_init()) {
      delay(3000);
      read_Meter();
      DEBUG.println(V);
      DEBUG.println(f);
    }
    else {
      DEBUG.println("Meter Fail");
    }
  }
}