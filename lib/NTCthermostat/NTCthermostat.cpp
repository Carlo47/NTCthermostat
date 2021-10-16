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

void NTCthermostat::setLimits(float lower, float upper)
{
    _lowerLimit = lower;
    _upperLimit = upper;
}

void NTCthermostat::getLimits(float &lower, float &upper)
{
    lower = _lowerLimit;
    upper = _upperLimit;
}

void NTCthermostat::setInterval(uint32_t msInterval)
{
    _msInterval = msInterval;
}

float NTCthermostat::getCelsius()
{
    return _ntc.getCelsius();
}

void NTCthermostat::loop()
{
    if (millis() % _msInterval == 0)
    {
        float t = _ntc.getCelsius();
        if (t < _lowerLimit)
        {
            _onLowTemp();
        }

        if (t > _upperLimit)
        {
            _onHighTemp();
        }
        // _ntc.printSensorValues(); 
    }
}