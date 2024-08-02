#ifndef __EVSE_Testing_function_lowlevel__
#define __EVSE_Testing_function_lowlevel__
#include <Arduino.h>
#include <Hardware_def.h>
#include <EVSE_Testing_Functin.h>
#include <Public_Variable.h>
#include <DLT645.h>

uint64_t nowTime;

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
#endif