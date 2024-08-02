#include <Arduino.h>
#include <DLT645.h>

byte DLT645_Data[100];
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

void DLT645_write(const byte *data, uint8_t siz) {
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