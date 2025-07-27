/**
 * @file FlowSensor.cpp
 * @author Hafidh Hidayat (hafidhhidayat@hotmail.com)
 * @brief 
 * 
 * @copyright Copyright (c) 2022
 * 
 * Github :
 * https://github.com/hafidhh
 * https://github.com/hafidhh/FlowSensor-Arduino
 */

#include "FlowSensor.h"

/**
 * @brief Construct a new FlowSensor::FlowSensor object
 * 
 * @param type Sensor Type
 * @param pin Interrupt Pin
 */
FlowSensor::FlowSensor(calib_t calib ,uint8_t pin)
{
	this->_calib.A=calib.A;
	this->_calib.B=calib.B;
	this->_pin = pin;
	this->_pulse1liter = 100;
	this->_hzSummaCount=0;
	this->_hzSumma=0;
	this->_flowrateminute=0;
}

/**
 * @brief 
 * 
 * @param userFunc 
 */
void FlowSensor::begin(void (*userFunc)(void))
{
	pinMode(this->_pin, INPUT);
	digitalWrite(this->_pin, INPUT_PULLUP); // Optional Internal Pull-Up
	attachInterrupt(digitalPinToInterrupt(this->_pin), userFunc, RISING); // For better compatibility with any board, for example Arduino Leonardo Boards
}

/**
 * @brief count pulse
 * 
 */
void FlowSensor::count()
{
	this->_pulse++;
}


void FlowSensor::read()
{
	unsigned long now= millis();
	this->pulseInLastRead= this->_pulse;
	this->timeElapsed=now-this->_timebefore;
	this->_timebefore=now;
}

/**
 * @brief
 * 
 * @param calibration Calibration pulse/liter
 */
void FlowSensor::calculateHz()
{
	this->_hz= this->pulseInLastRead /((this->timeElapsed) / 1000.0);
	this->_hzSumma+=this->_hz;
	this->_hzSummaCount+=1;
	if(this->_hz >2)
		this->_pulse1liter=this->_calib.A*log(this->_hz)+this->_calib.B;
	else
		this->_pulse1liter=10;
}



void FlowSensor::calculateFlowVolume(long calibration)
{
	this->_flowratesecound = (this->pulseInLastRead / (this->_pulse1liter + calibration)) / ((this->timeElapsed) / 1000);
	this->_volume += (this->pulseInLastRead / (this->_pulse1liter + calibration));
	this->_totalpulse += this->pulseInLastRead;
	this->_pulse = 0;

}




void FlowSensor::reset()
{
	this->_pulse=0;
	this->_totalpulse=0;
	this->_volume=0;
	this->_hzSumma=0;
	this->_hzSummaCount=0;
}



float FlowSensor::getHz()
{
	return this->_hz;
}
float FlowSensor::getHzAvg()
{
	return ((this->_hzSumma)/(this->_hzSummaCount));
}

/**
 * @brief 
 * 
 * @return unsigned long  _totalpulse
 */
unsigned long FlowSensor::getPulse()
{
	return this->_totalpulse;
}

/**
 * @brief Reset pulse count
 * 
 */
void FlowSensor::setPulseperLiter(unsigned long ppl)
{
	this->_pulse1liter=ppl;
}

unsigned long FlowSensor::getPulseperLiter()
{
	return (unsigned long)this->_pulse1liter;
}

void FlowSensor::resetPulse()
{
	this->_pulse=0;
	this->_totalpulse=0;
}

/**
 * @brief 
 * 
 * @return float flow rate / hour
 */
float FlowSensor::getFlowRate_h()
{
	this->_flowratehour = this->_flowratesecound * 3600;
	return this->_flowratehour;
}

/**
 * @brief 
 * 
 * @return float flow rate / minute
 */
float FlowSensor::getFlowRate_m()
{
	this->_flowrateminute = this->_flowratesecound * 60;
	return this->_flowrateminute;
}

/**
 * @brief 
 * 
 * @return float flow rate / secound
 */
float FlowSensor::getFlowRate_s()
{
	return this->_flowratesecound;
}

/**
 * @brief 
 * 
 * @return float volume
 */
float FlowSensor::getVolume()
{
	return this->_volume;
}

/**
 * @brief reset Volume
 * 
 */
void FlowSensor::resetVolume()
{
	this->_volume = 0;
}
