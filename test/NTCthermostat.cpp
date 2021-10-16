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

void NTCthermostat::loop()
{
    if (millis() % _msInterval == 0)
    {
        float t = getCelsius();
        if (t < _lowerLimit)
        {
            _onLowTemp();
        }

        if (t > _upperLimit)
        {
            _onHighTemp();
        }
        //printSensorValues();
        //printState(); 
    }
}