#include <Arduino.h>
#include <AVR128DA48/avr128.h>
#include <SoftwareSerial.h>
#include <Hardware_def.h>
#include <Public_Variable.h>
AVR_Req_FSM req_fsm;

boolean ping(){
    return false;
}

boolean req_CP(CP_Package* output){
    vTaskDelay(1000);
    AVR.println("read CP");
#ifdef DEBUG_AVR
    Serial.println("read CP");
#endif
    req_fsm = req_CP_State;

    unsigned long lastTime = millis();
    while(req_fsm == req_CP_State){ // while request CP was send -> wait until timeout
        if((millis() - lastTime) > return_timeout_ms){
            req_fsm = standby_State; // Return to standby state when timeout
            AVR.flush();
            return false;
        }
        vTaskDelay(100);
    }

    if(req_fsm == return_State){
        if(avr_msg.charAt(0) == '+'){
            char* tok = strtok((char*)avr_msg.c_str(),",");
            output->CP_Max = String(tok).toFloat();
            tok = strtok(NULL,",");
            output->CP_Min = String(tok).toFloat();
            tok = strtok(NULL,",");
            output->CP_Freq = String(tok).toFloat();
            tok = strtok(NULL,",");
            output->CP_Duty = String(tok).toFloat();

            req_fsm = standby_State;
            return true;
            
        }else{
            Serial.println("Msg invalid");
            req_fsm = standby_State;
            AVR.flush();
            return false;
        }
    }
    req_fsm = standby_State;
    AVR.flush();
    return false;
}

boolean req_PP_amp(PP_Package* output){
    vTaskDelay(1000);
    AVR.println("read PP");
#ifdef DEBUG_AVR
    Serial.println("read PP");
#endif
    req_fsm = req_PP_State;

    unsigned long lastTime = millis();
    while(req_fsm == req_PP_State){
        if((millis() - lastTime) > return_timeout_ms){
            req_fsm = standby_State;
            AVR.flush();
            return false;
        }
        vTaskDelay(100);
    }

    if(req_fsm == return_State){
        if(avr_msg.charAt(0) == '+'){
            output->PP_rating_enum = avr_msg.charAt(1);
            req_fsm = standby_State;
            return true;
        }else{
            Serial.println("Msg invalid");
            req_fsm = standby_State;
            AVR.flush();
            return false;
        }
    }

    req_fsm = standby_State;
    AVR.flush();
    return false;
}
boolean req_Ins(Insulation_Package* output){
    vTaskDelay(1000);
    AVR.println("read Insul");
#ifdef DEBUG_AVR
    Serial.println("read Insul");
#endif
    req_fsm = req_ins_State;

    unsigned long lastTime = millis();
    while(req_fsm == req_ins_State){
        if((millis() - lastTime) > return_timeout_ms){
            req_fsm = standby_State;
            AVR.flush();
            return false;
        }
        vTaskDelay(100);
    }

    if(req_fsm == return_State){
        output->Resistance = avr_msg.substring(1).toFloat();
        req_fsm = standby_State;
        return true;
    }
    
    AVR.flush();
    return false;
}

void AVR_Reset_command(){
#ifdef DEBUG_AVR
    Serial.println("AVR reset");
#endif
    AVR.write(255);
    AVR.println();
}