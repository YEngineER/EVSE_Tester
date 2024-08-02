#include <Arduino.h>
#include <string.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>
#include <SoftwareSerial.h>

#define RE_S    25
#define RE_B    22
#define RE_C    23
#define RE_D    15
#define RE_PE   12
#define RE_LEAK 14

#define V1      37
#define V2      38
#define V3      39
#define SW      0
#define DEBUG   Serial
#define AVR     Serial1
//#define METER_  Serial2

#define RX1     9
#define TX1     10

#define RX2     36
#define TX2     25
SoftwareSerial  METER_(RX2, TX2);

#define RE_L_HV     4
#define RE_N_HV     2
#define RE_PE_HV    26 
//BluetoothSerial DEBUG;
/*------------METER VARIABLE-----------*/
uint8_t sizeof_PHASE   = 51;
byte DLT645_Data[100];
byte DLT645_Configure_1[] = {0xFE,0xFE,0xFE,0xFE,0x68,0x99,0x99,0x99,0x99,0x99,0x99,0x68,0x23,0x0A,0x60,0x00,0x34,0x12,0x78,0x56,0xBC,0x9A,0xF0,0xDE,0x2B,0x16};
byte DLT645_Configure_2[] = {0xFE,0xFE,0xFE,0xFE,0x68,0x99,0x99,0x99,0x99,0x99,0x99,0x68,0x23,0x02,0x56,0x00,0xE1,0x16};
byte DLT645_HEAD[]        = {0xFE,0xFE,0xFE,0xFE,0x68,0x99,0x99,0x99,0x99,0x99,0x99,0x68,0x23,0x02};
byte DLT645_RTC[]         = {0x59,0x00,0xE4,0x16};
byte DLT645_PHASE_1[]     = {0x61,0x00,0xEC,0x16};
byte DLT645_FOOTER        = 0x16;
float V = 0;
float f = 0;
/*------------METER VARIABLE-----------*/

String req = "";

uint8_t AVR_count = 0;
static uint64_t nowTime;
uint8_t state_count = 0;



byte DLT645_Checksum(byte text[], uint8_t siz) {
	byte CS = 0x00;
	for(int i=4; i<(siz-2); i++) {
		CS += text[i];
	}
	return CS;
}

boolean DLT645_read(uint8_t siz) {
	memset(DLT645_Data, 0x00, sizeof(DLT645_Data));
	unsigned long currentMillis = millis();
	while((millis() - currentMillis) < 3000) {
		if(METER_.available()){
			if(METER_.readBytes(DLT645_Data, siz) == siz){
				if(DLT645_Checksum(DLT645_Data,siz) == DLT645_Data[siz-2]){
					return 1;
				}
			}
		}
	}
	return 0;
}

void DLT645_write(byte data[], uint8_t siz) {
	METER_.write(data, siz);
}

boolean DLT645_init(void) {
	for(uint8_t i=0; i<3; i++) {
		DLT645_write(DLT645_Configure_1, sizeof(DLT645_Configure_1));
		if(DLT645_read(18)){
			DLT645_write(DLT645_Configure_2, sizeof(DLT645_Configure_2));
			if(DLT645_read(23)){
				return 1;
			}
		}
	}
	return 0;
}

void read_Meter(void) {
  DLT645_init();
	uint8_t chk = 0;
	DLT645_write(DLT645_HEAD, sizeof(DLT645_HEAD)); DLT645_write(DLT645_PHASE_1, sizeof(DLT645_PHASE_1));
	if(DLT645_read(sizeof_PHASE)) {
		// chk++;
		V   = (((uint32_t)DLT645_Data[17]<< 8) | ((uint32_t)DLT645_Data[16])) /100.0;
		// tx.I   = (((uint32_t)DLT645_Data[19]<< 8) | ((uint32_t)DLT645_Data[18])) /1000.0;
		// tx.P   = (((uint32_t)DLT645_Data[23]<<24) | ((uint32_t)DLT645_Data[22]<<16) | ((uint32_t)DLT645_Data[21]<< 8) | ((uint32_t)DLT645_Data[20]));
		// tx.Q   = (((uint32_t)DLT645_Data[27]<<24) | ((uint32_t)DLT645_Data[26]<<16) | ((uint32_t)DLT645_Data[25]<< 8) | ((uint32_t)DLT645_Data[24]));
		// tx.S   = (((uint32_t)DLT645_Data[31]<<24) | ((uint32_t)DLT645_Data[30]<<16) | ((uint32_t)DLT645_Data[29]<< 8) | ((uint32_t)DLT645_Data[28])) /100.0;
		// tx.pf  = (((uint32_t)DLT645_Data[33]<< 8) | ((uint32_t)DLT645_Data[32]));
		f   = (((uint32_t)DLT645_Data[35]<< 8) | ((uint32_t)DLT645_Data[34]))/100.0;
		// tx.Vdc = (((uint32_t)DLT645_Data[39]<<24) | ((uint32_t)DLT645_Data[38]<<16) | ((uint32_t)DLT645_Data[37]<< 8) | ((uint32_t)DLT645_Data[36]));
		// tx.Idc = (((uint32_t)DLT645_Data[43]<<24) | ((uint32_t)DLT645_Data[42]<<16) | ((uint32_t)DLT645_Data[41]<< 8) | ((uint32_t)DLT645_Data[40]));
		// tx.importP = (((uint32_t)DLT645_Data[47]<<24) | ((uint32_t)DLT645_Data[46]<<16) | ((uint32_t)DLT645_Data[45]<< 8) | ((uint32_t)DLT645_Data[44]));

  }
	// if(tx.P     > 0xFF000000) {tx.P    = (tx.P    - 0xFFFFFFFF) /100.0;    } else { tx.P    = tx.P   /100.0; }
	// if(tx.Q     > 0xFF000000) {tx.Q    = (tx.Q    - 0xFFFFFFFF) /100.0;    } else { tx.Q    = tx.Q   /100.0; }
	// if(tx.pf    > 0x2710    ) {tx.pf   = (tx.pf   - 0xFFFF)     /10000.0;  } else { tx.pf   = tx.pf  /10000.0;}    // (- = L), (+ = C)
	// if(tx.pf    < 0.0       ) {tx.Q    = (tx.Q    * -1)         /100.0;    } else { tx.Q    = tx.Q  /100.0;}
	// if(tx.Vdc   > 0xFF000000) {tx.Vdc  = (tx.Vdc  - 0xFFFFFFFF) /65535.0;  } else { tx.Vdc  = tx.Vdc /65535.0; }
	// if(tx.Idc   > 0xFF000000) {tx.Idc  = (tx.Idc  - 0xFFFFFFFF) /65535.0;  } else { tx.Idc  = tx.Idc /65535.0; }
	// if(chk == 1) {return 1;}
	// return 0;
}

boolean OnRelay_Check (void) {
  nowTime = millis();
  while(millis()-nowTime < 20) {
    if(!digitalRead(V1)) {
      return true;
    }
  }
  return false;
}

boolean OffRelay_Check (void) {
  nowTime = millis();
  while(millis()-nowTime < 20) {
    if(digitalRead(V1)) {
      delay(2);
      if(digitalRead(V1)) {
        return true;
      }
    }
  }
  return false;
}

void Direction(void) {
  pinMode(SW,       INPUT_PULLUP);
  pinMode(V1,       INPUT_PULLUP);
  pinMode(V2,       INPUT_PULLUP);
  pinMode(V3,       INPUT_PULLUP);

  pinMode(RE_S,     OUTPUT);
  pinMode(RE_B,     OUTPUT);
  pinMode(RE_C,     OUTPUT);
  pinMode(RE_D,     OUTPUT);
  pinMode(RE_PE,    OUTPUT);
  pinMode(RE_LEAK,  OUTPUT);
  pinMode(RE_L_HV,  OUTPUT);
  pinMode(RE_N_HV,  OUTPUT);
  pinMode(RE_PE_HV, OUTPUT);

  digitalWrite(RE_L_HV, LOW);
  digitalWrite(RE_N_HV, LOW);
  digitalWrite(RE_S,    LOW);
  digitalWrite(RE_B,    LOW);
  digitalWrite(RE_C,    LOW);
  digitalWrite(RE_D,    LOW);
  digitalWrite(RE_PE,   LOW);
  digitalWrite(RE_LEAK, LOW);
}

void State_Control() {
  if(DEBUG.available()) {
    req = DEBUG.readStringUntil('\n');
  }
  else {req = "";}
}

String requestPackageFromAVR(char c){
  unsigned long nowtime = millis();
  while(millis()-nowtime < 5000) {
    AVR.print(c);
    delay(500);
    if(AVR.available()) {
      String msg = AVR.readStringUntil('\n');
      if(msg[0] == '~' && msg[sizeof(msg) == '~']) {
        return msg;
      }
    }
  }
  return "";
}

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

void setup() {
  Direction();
  DEBUG.begin(115200);
  AVR.begin(9600, SERIAL_8N1, RX1, TX1);
  METER_.begin(9600);
  String data = requestPackageFromAVR('A');
}

void loop() {
  State_Control();
  state_A_to_B();
  state_B_to_C();
  state_B_to_D();
  state_C_to_B();
  diode_PE_test();
  RCD0_Test();
  Insulator_Test();
  state_Manual();
}

