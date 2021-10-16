/**
 * Header       NTCthermostat.h
 * Author       2021-05-13 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Declaration of the class NTCthermostat
 * 
 * Constructor
 * arguments    pinNTC       analog input pin internally connected to the ADC (A0 for Arduino uno)
 *              beta         characteristic parameter of the NTC
 *              Ro           resistance of the NTC at nomnal temperature of 25°C
 *              Rs           value of the series resistor in ohm
 *              ntcToGround  pass true if NTC is connected to ground, otherwise pass false
 *              analogMax    pass 1023 for Arduino Uno 10-bit ADC
 */

#ifndef _NTCTHERMOSTAT_H_
#define _NTCTHERMOSTAT_H_
#include <Arduino.h>
#include "NTCsensor.h"

class NTCthermostat : public NTCsensor
{
    public:
        NTCthermostat(void (&onLowTemp)(), void (&onHighTemp)(), uint8_t pinNTC, uint16_t beta, uint16_t Ro, uint16_t Rs, bool ntcToGround = true, uint16_t analogMax = 1023) :
            NTCsensor(pinNTC, beta, Ro, Rs, ntcToGround, analogMax), _onLowTemp(onLowTemp), _onHighTemp(onHighTemp)
            {}

        void setLimits(float lower, float upper);
        void getLimits(float &lower, float &upper);
        void setInterval(uint32_t msInterval);
        void loop();

    private:
        float    _lowerLimit = 18.0;
        float    _upperLimit = 22.0;
        uint32_t _msInterval = 5000;
        void (&_onLowTemp)();  // called when temperature is below lowwer limit
        void (&_onHighTemp)(); // called when temperature is above upper limit
};
#endif