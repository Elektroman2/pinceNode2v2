
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "def.h"
#include "tempSensor.h"
tempsensor tempsensors[38] =
{
		{.addr=	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, .gain = (float)1.0,	.offset = 0.0},
		{.addr=	{0x10, 0x1D, 0xA1, 0xD2, 0x01, 0x08, 0x00, 0xCF}, .gain = (float)1.0000201922300305,	.offset = (float)0.065000302883450461},
		{.addr=	{0x28, 0x60, 0x79, 0x7A, 0xB4, 0x21, 0x06, 0x0A}, .gain = (float)1.0040292947467115,	.offset = (float)0.29046501609183739},
		{.addr=	{0x28, 0x28, 0x8D, 0xDB, 0x05, 0x00, 0x00, 0x8E}, .gain = (float)1.0009853212399891,	.offset = (float)-0.01156059725625936},
		{.addr=	{0x28, 0xD8, 0xC4, 0xFD, 0xB4, 0x21, 0x06, 0x50}, .gain = (float)1.0069638590962231,	.offset = (float)-0.016459614700350761},
		{.addr=	{0x28, 0x74, 0xFD, 0x9A, 0xB4, 0x21, 0x06, 0x10}, .gain = (float)1.0038037456422453,	.offset = (float)0.043977177526146528},
		{.addr=	{0x28, 0x02, 0xDA, 0xDA, 0x05, 0x00, 0x00, 0xBC}, .gain = (float)1.0047965806821391,	.offset = (float)-0.21878308533247198},
		{.addr=	{0x28, 0x32, 0xBC, 0xBD, 0xB0, 0x21, 0x08, 0x84}, .gain = (float)1.0026098560445917,	.offset = (float)0.14775446096434777},
		{.addr=	{0x28, 0xFA, 0xE9, 0xDB, 0x05, 0x00, 0x00, 0x2A}, .gain = (float)1.0040521944951992,	.offset = (float)-0.064210937123828929},
		{.addr=	{0x28, 0x76, 0xED, 0x9C, 0xB4, 0x21, 0x06, 0x0B}, .gain = (float)1.0044620221072913,	.offset = (float)0.050000000000000003},
		{.addr=	{0x28, 0xBE, 0x2D, 0x02, 0xB5, 0x21, 0x06, 0x48}, .gain = (float)1.005542922114838,	.offset = (float)-0.14406978396816389},
		{.addr=	{0x28, 0x61, 0xB4, 0xDB, 0x05, 0x00, 0x00, 0x1A}, .gain = (float)1.0010510783659776,	.offset = (float)-0.028081984112546296},
		{.addr=	{0x28, 0xB1, 0x8F, 0xB5, 0xB0, 0x21, 0x08, 0xE1}, .gain = (float)1.0019117751186011,	.offset = (float)0.15820647171280899},
		{.addr=	{0x28, 0x89, 0xA4, 0xE9, 0xB4, 0x21, 0x06, 0x6E}, .gain = (float)1.0047643039010492,	.offset = (float)0.037942828353187408},
		{.addr=	{0x28, 0x89, 0xB7, 0xDA, 0x05, 0x00, 0x00, 0x53}, .gain = (float)1.0056739436869979,	.offset = (float)-0.085263145425901257},
		{.addr=	{0x28, 0xC9, 0x71, 0x09, 0xB0, 0x21, 0x07, 0x1E}, .gain = (float)1.0040191174160071,	.offset = (float)0.14889588306547674},
		{.addr=	{0x28, 0x99, 0xAC, 0x06, 0xB5, 0x21, 0x06, 0x73}, .gain = (float)1.0065852320074793,	.offset = (float)0.31372533078595966},
		{.addr=	{0x28, 0xD5, 0x44, 0xDB, 0x05, 0x00, 0x00, 0x67}, .gain = (float)1.0026335368518018,	.offset = (float)-0.098891080222492697},
		{.addr=	{0x28, 0xF5, 0x59, 0xDB, 0x05, 0x00, 0x00, 0x41}, .gain = (float)0.99914930246164269,	.offset = (float)0.020025520926150706},
		{.addr=	{0x28, 0x0D, 0xF2, 0xDB, 0x05, 0x00, 0x00, 0xD1}, .gain = (float)1.0049325056689151,	.offset = (float)-0.13440511479024592},
		{.addr=	{0x28, 0xED, 0x5E, 0x96, 0xB4, 0x21, 0x06, 0x58}, .gain = (float)1.0038953097497609,	.offset = (float)0.034941570353753587},
		{.addr=	{0x28, 0xA3, 0xAD, 0x7C, 0xB4, 0x21, 0x06, 0x84}, .gain = (float)1.0050310742823325,	.offset = (float)0.048492453388576502},
		{.addr=	{0x28, 0xD3, 0xE8, 0x99, 0xB4, 0x21, 0x06, 0xEF}, .gain = (float)1.003204752182431,	.offset = (float)0.22681483757215359},
		{.addr=	{0x28, 0xA7, 0x7C, 0x7F, 0xB4, 0x21, 0x06, 0x43}, .gain = (float)1.0050761163777686,	.offset = (float)-0.033672586688449274},
		{.addr=	{0x28, 0xB7, 0xB4, 0xDB, 0x05, 0x00, 0x00, 0x6C}, .gain = (float)1.0077159878897737,	.offset = (float)-0.10292090116227327},
		{.addr=	{0x28, 0x77, 0x86, 0x9C, 0xB4, 0x21, 0x06, 0x5D}, .gain = (float)1.0038308740068109,	.offset = (float)0.11324134506242912},
		{.addr=	{0x28, 0xCF, 0xFB, 0xDB, 0x05, 0x00, 0x00, 0xD3}, .gain = (float)1.0029897296079426,	.offset = (float)-0.10170219660320136},
		{.addr=	{0x28, 0x9F, 0x4D, 0xCB, 0xB0, 0x21, 0x09, 0x06}, .gain = (float)1.0012450278909841,	.offset = (float)0.16564380072140875},
		{.addr=	{0x28, 0x3F, 0x8B, 0x97, 0xB4, 0x21, 0x06, 0xF1}, .gain = (float)1.0002389936683509,	.offset = (float)0.1280186415061314},
		{.addr=	{0x28, 0xFF, 0x64, 0x01, 0xBE, 0x66, 0x13, 0x0E}, .gain = (float)1.0073256968903925,	.offset = (float)-0.32472515924322576},
		{.addr=	{0x28, 0xFF, 0x64, 0x01, 0xBE, 0x13, 0x7D, 0x3D}, .gain = (float)1.0056348037971474,	.offset = (float)-0.24062845829737567},
		{.addr=	{0x28, 0xFF, 0x64, 0x01, 0xBE, 0x47, 0x94, 0x04}, .gain = (float)1.0052733293018652,	.offset = (float)-0.33828682344284516},
		{.addr=	{0x28, 0xFF, 0x64, 0x01, 0xBE, 0x47, 0x3D, 0x37}, .gain = (float)1.0052129671288763,	.offset = (float)-0.36917380729274124},
		{.addr=	{0x28, 0xFF, 0x64, 0x01, 0xBE, 0x7F, 0x96, 0xE3}, .gain = (float)1.0063074122677051,	.offset = (float)-0.27252152563179965},
		{.addr=	{0x28, 0xFF, 0x64, 0x01, 0xB9, 0x7A, 0x94, 0xDA}, .gain = (float)1.0057922327385969,	.offset = (float)-0.34502489940808356},
		{.addr=	{0x28, 0xFF, 0x64, 0x01, 0xBD, 0xFA, 0x35, 0x9A}, .gain = (float)1.0053456034103592,	.offset = (float)-0.34107943972662941},
		{.addr=	{0x28, 0xFF, 0x64, 0x01, 0xBF, 0x83, 0xFB, 0x04}, .gain = (float)1.0032580988233708,	.offset = (float)-0.20081452470584271},
		{.addr=	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, .gain = (float)1.0,	.offset = (float)0.0}};


int count = 0;
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void InitTempSensors( uint8_t* GIDTable,uint8_t GIDTableLen, const char** tempSensorNames)
{
  uint8_t tempSensorfoundGIDs[12];
  sensors.begin();
  count = sensors.getDS18Count();
  Serial.print("Found ");
  Serial.print(count);
  Serial.println(" devices.");
  if (count < GIDTableLen)
  {
    Serial.println(" SENSORS MISSING! ");
  }
  for (uint8_t i = 0; i < count; i++)
  {
    uint8_t addr[8];
    bool sensorFoundinGIDTable = false;
    sensors.getAddress(addr, i);
    uint8_t foundGID = getGID(addr);
    if (foundGID == 0)
    {
      Serial.print("Unknown Sensor Found: ");
      printAddress(addr);
    }

    for (uint8_t k = 0; k < GIDTableLen; k++)
    {

      if (foundGID == GIDTable[k])
      {
        tempSensorfoundGIDs[k] = foundGID;
        sensorFoundinGIDTable = true;
        Serial.print("Found ");
        Serial.print(GIDTable[k]);
        Serial.print(". Sensor: ");
        Serial.println(tempSensorNames[k]);
        break;
      }
    }
    if (sensorFoundinGIDTable == false)
    {
      Serial.print("Known but foreign Sensor Found: ");
      Serial.print(foundGID);

      Serial.print(". Sensor: ");
      printAddress(addr);
      Serial.println();
    }
  }
}



void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if(i<7)
      Serial.print('-');
  }
  
}



uint8_t getGID(uint8_t a[])
{
for(uint8_t i=1;i<38;i++)
{ if( 0 == compareArray(a, tempsensors[i].addr) )
    return i;
}
return 0;
}


void getAddress(uint8_t GID, uint8_t a[])
{
    if(GID > 0 && GID < 38)
    {a=tempsensors[GID].addr;}
    else
    a=tempsensors[0].addr;
}


uint8_t compareArray(uint8_t a[],uint8_t b[],int size)	{
	int i;
	for(i=0;i<size;i++){
		if(a[i]!=b[i])
			return 1;
	}
	return 0;
}
