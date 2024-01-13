#ifndef TEMPSENSORLIST_H
#define TEMPSENSORLIST_H
#include "inttypes.h"
#include <DallasTemperature.h>
typedef struct
{

	  uint8_t addr[8];
	  float gain;
	  float offset;
}tempsensor;

void printAddress(DeviceAddress deviceAddress);
void InitTempSensors( uint8_t* GIDTable,uint8_t GIDTableLen, const char** tempSensorNames);
uint8_t getGID(uint8_t a[]);
uint8_t compareArray(uint8_t a[],uint8_t b[],int size=8); 
extern tempsensor tempsensors[38];

#endif