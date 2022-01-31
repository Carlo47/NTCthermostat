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
using Callback = void (&)();

class NTCthermostat
{
    public:
        NTCthermostat(NTCsensor &ntcSensor, Callback onLowTemp, Callback onHighTemp, Callback onDataReady) : 
                      _ntcSensor(ntcSensor), _onLowTemp(onLowTemp), _onHighTemp(onHighTemp), _onDataReady(onDataReady) 
                       {}

        void  loop();
        void  enable();
        void  disable();
        bool  isEnabled();
        void  setLimitLow(float tLimitLow);
        void  setLimitHigh(float tLimitHigh);
        void  setRefreshInterval(uint32_t msInterval);
        float getLimitLow();
        float getLimitHigh();
        uint32_t getRefreshInterval();

    private:
        bool     _isEnabled = false;
        float    _tLimitLow  = 18.0;
        float    _tLimitHigh = 21.0;
        uint32_t _msRefresh = 5000;
        NTCsensor &_ntcSensor;
        Callback _onLowTemp;    // called when temperature is below lowwer limit
        Callback _onHighTemp;   // called when temperature is above upper limit 
        Callback _onDataReady;
};
#endif