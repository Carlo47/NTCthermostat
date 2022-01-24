/**
 * Header       NTCthermostat.h
 * Author       2021-05-13 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Declaration of the class NTCthermostat
 * 
 * Constructor
 * arguments    &ntc          a reference to a NTCsensor object
 *              onLowTemp     a callback function to be called when temperature is below lower limit
 *              onHighTemp    a callback function to be called when temperature is above upper limit
 */

#ifndef _NTCTHERMOSTAT_H_
#define _NTCTHERMOSTAT_H_
#include <Arduino.h>
#include "NTCsensor.h"

//typedef void (*CallbackFunction)();
using CallbackFunction = void (&)();

class NTCthermostat
{
    public:
        NTCthermostat(NTCsensor &ntc, CallbackFunction onLowTemp, CallbackFunction onHighTemp) : 
            _ntc(ntc), _onLowTemp(onLowTemp), _onHighTemp(onHighTemp)  {}

        void  setLimits(float lower, float upper);
        void  getLimits(float &lower, float &upper);
        void  setInterval(uint32_t msInterval);
        float getCelsius();
        void  loop();

    private:
        NTCsensor  &_ntc;
        float    _lowerLimit = 18.0;
        float    _upperLimit = 22.0;
        uint32_t _msInterval = 5000;
        CallbackFunction _onLowTemp;    // called when temperature is below lowwer limit
        CallbackFunction _onHighTemp;   // called when temperature is above upper limit 
};
#endif