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
    8,                    // Task Priority
    NULL                  // Task Handler -> no handler
  );
  
  METER_.begin(9600);

  timer = timerBegin(1,2,true);
  
}

void loop() {
  State_Control();
  state_A_to_B();
  state_B_to_C();
  state_B_to_D();
  state_C_to_B();
  diode_PE_test();
  RCD0_Test();
  // Insulator_Test();

  
  
  
  // if(AVR.available() > 0){
  //   String msg = AVR.readStringUntil('\n');
  //   Serial.println(msg);
  // }

  state_Manual();
  vTaskDelay(100);

  if(req_fsm == standby_State){
    if(req == "read_CP"){
      // AVR.println("read CP");
      // req_fsm = req_CP_State;
      CP_Package test;
      req_CP(&test);

      Serial.printf("Duty %f,Freq %f,Max %f,Min %f\n",test.CP_Duty,test.CP_Freq,test.CP_Max,test.CP_Min);
    }else if(req == "read_PP"){
      // AVR.println("read PP");
      // req_fsm = req_PP_State;
      PP_Package test;
      req_PP_amp(&test);
      
      switch(test.PP_rating_enum){
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
      // Serial.printf("%d\n",test.PP_rating_enum);
    }else if(req == "read_INS"){
      // AVR.println("read Insul");
      // req_fsm = req_ins_State;
      Insulation_Package test;
      req_Ins(&test);
      Serial.printf("Resistance %f\n",test.Resistance);
    }
  }
}


void AVR_HandleTask(void* param){

  AVR.begin(9600);
  AVR_Reset_command();
  req_fsm = standby_State;
  while(1){
    
    if(AVR.available() > 0){
      avr_msg = AVR.readStringUntil('~');
      req_fsm = return_State;
    }
    vTaskDelay(50);
  }
}