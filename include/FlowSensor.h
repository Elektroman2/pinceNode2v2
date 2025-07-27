/**
 * @file FlowSensor.h
 * @author Hafidh Hidayat (hafidhhidayat@hotmail.com)
 * @brief 
 * 
 * @copyright Copyright (c) 2022
 * 
 * Github :
 * https://github.com/hafidhh
 * https://github.com/hafidhh/FlowSensor-Arduino
 */

#ifndef FlowSensor_h
#define FlowSensor_h

#include "Arduino.h"
#include "FlowSensor_Type.h"
#include "math.h"

/**
 * @brief FlowSensor Class > Sensor(uint8_t Type, uint8_t Pin)
 * 
 */
typedef struct
{
	float A;
	float B;
}calib_t;


class FlowSensor
{
private:
	uint8_t _pin;
	calib_t _calib;
	volatile unsigned long _totalpulse;
	volatile unsigned long _pulse;
	volatile unsigned long pulseInLastRead;
	volatile unsigned long timeElapsed;
	float _hz;
	float _hzSumma;
	uint32_t _hzSummaCount;
	float _pulse1liter;
	float _flowratehour;
	float _flowrateminute;
	float _flowratesecound;
	float _volume;
	unsigned long _timebefore = 0;

public:
	FlowSensor(calib_t calib ,uint8_t pin);
	void begin(void (*userFunc)(void));
	void count();
	void read();
	void calculateHz();
	void calculateFlowVolume(long calibration=0);

	unsigned long getPulse();
	void reset();
	float getHz();
	float getHzAvg();
	void resetPulse();
	void setPulseperLiter(unsigned long ppl);
	unsigned long getPulseperLiter() ;
	float getFlowRate_h();
	float getFlowRate_m();
	float getFlowRate_s();
	float getVolume();
	void resetVolume();
};

#endif
//Added by Sloeber 
#pragma once
