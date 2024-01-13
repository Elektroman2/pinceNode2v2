#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoHA.h>

#include "tempSensor.h"
#include "def.h"







WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);
unsigned long lastUpdateAt = 0;





// "led" is unique ID of the switch. You should define your own ID.
HASwitch led("led");



#define stringify( name ) # name

enum tempSensorIDs
  {
  padlo_elore = 0,
  padlo_vissza,
  mennyezet_elore,
  mennyezet_vissza,
  fancoil_elore,
  fancoil_vissza,
  OGY_elore,
  OGY_vissza,
  HCS_sec_elore,
  HCS_sec_vissza,
  HCS_pri_elore,
  HCS_pri_vissza,
  tempSensorNUM
  };


enum flowSensorIDs
{
	padlo_flow = 0,
	mennyezet_flow,
	OGY_flow,
	HSZ_flow,
	flowSensorNUM
};

enum powerSensorIDs
{
	padlo_teljesitmeny = 0,
	mennyezet_teljesitmeny,
	OGY_teljesitmeny,
	HSZ_teljesitmeny,
	powerSensorNUM
};

const char* tempSensorIDNames[] =
  {
		  stringify(padlo_elore ),
		  stringify(padlo_vissza),
		  stringify(mennyezet_elore),
		  stringify(mennyezet_vissza),
		  stringify(fancoil_elore),
		  stringify(fancoil_vissza),
		  stringify(OGY_elore),
		  stringify(OGY_vissza),
		  stringify(HCS_sec_elore),
		  stringify(HCS_sec_vissza),
		  stringify(HCS_pri_elore),
		  stringify(HCS_pri_vissza)

  };

const char* flowSensorIDNames[] =
  {
		  stringify(padlo_terfogataram ),
		  stringify(mennyezet_terfogataram),
		  stringify(OGY_terfogataram),
		  stringify(HSZ_terfogataram)

  };
const char* powerSensorIDNames[] =
  {
		  stringify(padlo_teljesitmeny),
		  stringify(mennyezet_teljesitmeny),
		  stringify(OGY_teljesitmeny),
		  stringify(HSZ_teljesitmeny),
		  
  };


const char* tempSensorNames[] =
  {
		  "Padlófűtés elore",
		  		  "Padlófűtés vissza",
		  		  "Mennyezet előre",
		  		  "Mennyezet vissza",
		  		  "Fancoil előre",
		  		  "Fancoil vissza",
		  		  "Osztógyújtő előre",
		  		  "Osztógyújtő vissza",
		  		  "Hőcserélő szekunder előre",
		  		  "Hőcserélő szekunder vissza",
				  "Hőcserélő primer előre",
		  		  "Hőcserélő primer vissza"

		    };

		  const char* flowSensorNames[] =
		    {
		  		  "Padlófűtés térfogatáram",
		  		  "Mennyezet térfogatáram",
		  		  "Osztógyűjtő térfogatáram",
		  		  "Hőszivattyú térfogatáram"

		    };
		  const char* powerSensorNames[] =
		    {
		  		  "Padlófűtés teljesítmény",
		  		  "Mennyezet teljesítmény",
		  		  "Osztógyűjtő teljesítmény",
		  		  "Hőszivattyú teljesítmény",
		    };


uint8_t tempSensorGIDs[12]=
  {
  18,
  3,
  19,
  24,
  8,
  14,
  29,
  30,
  27,
  28,
  12,
  7
  };



/* Function prototypes*/



void setup()
{
	Serial.begin(115200);
	Serial.println("Starting...");

	Serial.println("Init temp Sensors...");
  InitTempSensors(tempSensorGIDs,sizeof(tempSensorGIDs),tempSensorNames );

  HASensorNumber *HAsensors[(tempSensorNUM + flowSensorNUM + powerSensorNUM)];

  for (uint8_t i = 0; i < tempSensorNUM; i++)
  {
    HAsensors[i] = new HASensorNumber{tempSensorIDNames[i], HASensorNumber::PrecisionP2};
    HAsensors[i]->setName(tempSensorNames[i]);
    HAsensors[i]->setUnitOfMeasurement("°C");
    HAsensors[i]->setIcon("mdi:thermometer");
}
for(uint8_t i=0,k=tempSensorNUM;i<flowSensorNUM;i++,k++)
{
	HAsensors[k] = new HASensorNumber{flowSensorIDNames[i], HASensorNumber::PrecisionP2};
	HAsensors[k]->setName(flowSensorNames[i]);
	HAsensors[k]->setUnitOfMeasurement("l/min");
	HAsensors[i]->setIcon("mdi:waves-arrow-right");



}
for(uint8_t i=0,k=tempSensorNUM+flowSensorNUM;i<powerSensorNUM;i++,k++)
{
	HAsensors[k] = new HASensorNumber{powerSensorIDNames[i], HASensorNumber::PrecisionP2};
	HAsensors[k]->setName(powerSensorNames[i]);
	HAsensors[k]->setUnitOfMeasurement("W");
	HAsensors[i]->setIcon("mdi:gauge");
}


mqtt.begin(BROKER_ADDR,"HA","homeassistant");





//The setup function is called once at startup of the sketch


    device.setName("SensorNodePince1");
    device.setSoftwareVersion("1.0.0");
    // Unique ID must be set!
       byte mac[6];
       WiFi.macAddress(mac);
       device.setUniqueId(mac, sizeof(mac));




// Add your initialization code here
}

// The loop function is called in an endless loop
void loop()
{
   mqtt.loop();
//Add your repeated code here
}













