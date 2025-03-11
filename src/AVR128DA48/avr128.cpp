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

void req_CP_NonBlocking(void (*whileWaiting)(), void (*onComplete)(CP_Package output), void (*onFail)()){
    static unsigned long lastTime;
    if(req_fsm == standby_State){
        AVR.println("read CP");
        Serial.println("Send : read CP");
        lastTime = millis();
        req_fsm = req_CP_State;
    }else if(req_fsm == req_CP_State){
        unsigned long deltaTime = millis() - lastTime;
        if(deltaTime <= return_timeout_ms){
            whileWaiting();
        }else{
            Serial.println("Read CP Fail: Request Timeout");
            onFail();
            AVR.flush();
            req_fsm = standby_State;
            
        }
    }else if(req_fsm == return_State){
        CP_Package package_receive;
        if(avr_msg.charAt(0) == '+'){
            char* tok = strtok((char*)avr_msg.c_str(),",");
            package_receive.CP_Max = String(tok).toFloat();
            tok = strtok(NULL,",");
            package_receive.CP_Min = String(tok).toFloat();
            tok = strtok(NULL,",");
            package_receive.CP_Freq = String(tok).toFloat();
            tok = strtok(NULL,",");
            package_receive.CP_Duty = String(tok).toFloat();

            onComplete(package_receive);
            AVR.flush();
            Serial.printf("Duty %f,Freq %f,Max %f,Min %f\n",package_receive.CP_Duty,package_receive.CP_Freq,package_receive.CP_Max,package_receive.CP_Min);
            req_fsm = standby_State;
        }else{ // invalid msg
            Serial.println("Read CP Fail: invalid MSG");
            onFail();
            AVR.flush();
            req_fsm = standby_State;
        }
    }
    
}
// msg : ~<PP_amp>~
// ex:  '+' 0x00 '~' ; No cable
// ex:  '+' 0x07 '~' ; 13A
// ex:  '+' 0x08 '~' ; 20A
// ex:  '+' 0x09 '~' ; 32A
// ex:  '+' 0x0A '~' ; 63A

void req_PP_amp_NonBlocking(void (*whileWaiting)(), void (*onComplete)(PP_Package output), void (*onFail)()){
    static unsigned long lastTime;
    
    if(req_fsm == standby_State){
        AVR.println("read PP");
        Serial.println("Send : read PP");
        lastTime = millis();
        req_fsm = req_PP_State;
    }else if(req_fsm == req_PP_State){
        unsigned long deltaTime = millis() - lastTime;
        if(deltaTime <= return_timeout_ms){
            whileWaiting();
        }else{
            Serial.println("Read CP Fail: Request Timeout");
            onFail();
            AVR.flush();
            req_fsm = standby_State;
        }
    }else if(req_fsm == return_State){
        PP_Package package_receive;
        if(avr_msg.charAt(0) == '+'){
            package_receive.PP_rating_enum = avr_msg.charAt(1);
            onComplete(package_receive);
            AVR.flush();
            switch(package_receive.PP_rating_enum){
                case 0: 
                Serial.println("No Cable");
                break;
                case 7:
                Serial.println("Rating 13 A");
                break;
                case 8:
                Serial.println("Rating 20 A");
                break;
                case 9:
                Serial.println("Rating 32 A");
                break;
                case 10:
                Serial.println("Rating 63 A");
                break;
            }
            req_fsm = standby_State;
        }else{ // invalid msg
            Serial.println("Read CP Fail: invalid MSG");
            onFail();
            AVR.flush();
            req_fsm = standby_State;
        }
    }
}

void req_Ins_NonBlocking(void (*whileWaiting)(), void (*onComplete)(Insulation_Package output), void (*onFail)()){
    static unsigned long lastTime;
    AVR.println("read Insul");

    if(req_fsm == standby_State){
        lastTime = millis();
        req_fsm = req_ins_State;
    }else if(req_fsm == req_ins_State){
        unsigned long deltaTime = millis() - lastTime;
        if(deltaTime <= return_timeout_ms){
            whileWaiting();
        }else{
            onFail();
            AVR.flush();
            req_fsm = standby_State;
        }
    }else if(req_fsm == return_State){
        Insulation_Package package_receive;
        if(avr_msg.charAt(0) == '+'){
            package_receive.Resistance = avr_msg.substring(1).toFloat();
            onComplete(package_receive);
            req_fsm = standby_State;
        }else{ // invalid msg
            onFail();
            req_fsm = standby_State;
        }
    }
}

void AVR_Reset_command(){
#ifdef DEBUG_AVR
    Serial.println("AVR reset");
#endif
    // AVR.write(255);
    // AVR.println();
    AVR.println("AVR_reset");
}