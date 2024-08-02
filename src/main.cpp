#include <Arduino.h>
#include <string.h>

#include <BluetoothSerial.h>
#include <SoftwareSerial.h>

#include <DLT645.h>
#include <Hardware_def.h>
#include <EVSE_Testing_Functin.h>
#include <Public_Variable.h>


/*------------METER VARIABLE-----------*/
uint8_t sizeof_PHASE   = 51;

float V = 0;
float f = 0;
/*------------METER VARIABLE-----------*/
String req = "";

uint8_t AVR_count = 0;
uint8_t state_count = 0;
BluetoothSerial DEBUG;

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

void setup() {
  Direction();
  // DEBUG.begin(115200);
  DEBUG.begin("EVSE Tester");
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

