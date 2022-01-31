/**
 * Class        NTCthermostat.cpp
 * Author       2021-05-13 Charles Geiser (https://www.dodeka.ch)
 *
 * Purpose      Implements the class NTCthermistor, which monitors the room temperature 
 *              and calls a callback function when the temperature falls below the lower limit 
 *              and calls another callback function when the temperature exceeds the upper limit.
 * 
 *              The monitoring cycle is determined by the user set interval.  
 *                                                            
 * Board        Arduino uno, Wemos D1 R2
 * 
 **/

#include "NTCthermostat.h"

void NTCthermostat::setLimitLow(float tLimitLow)
{
    _tLimitLow = tLimitLow;
}

void NTCthermostat::setLimitHigh(float tLimitHigh)
{
    _tLimitHigh = tLimitHigh;
}

float NTCthermostat::getLimitLow()
{
    return(_tLimitLow);
}

float NTCthermostat::getLimitHigh()
{
    return(_tLimitHigh);
}

void NTCthermostat::setRefreshInterval(uint32_t msInterval)
{
    _msRefresh = msInterval;
}

void NTCthermostat::loop()
{
  if((millis() % _msRefresh) == 0 && _isEnabled) 
  {
    float t = _ntcSensor.getCelsius();
    _onDataReady();
    if (t < _tLimitLow) _onLowTemp();
    if (t > _tLimitHigh) _onHighTemp();
  }
}

void NTCthermostat::enable()
{
  _isEnabled = true;
}

void NTCthermostat::disable()
{
  _isEnabled = false;
}

bool NTCthermostat::isEnabled()
{
  return _isEnabled;
}