/**
 * Header       NTCsensor.h
 * Author       2021-05-13 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Declaration of the class NTCsensor
 * 
 * Constructor
 * arguments    &ntc       A reference to a struct holding the NTC parameters
 *              &adc       A reference to a struct holding the ADC parameters
 */
#ifndef _NTCSENSOR_H_
#define _NTCSENSOR_H_
#include <Arduino.h> 

using ParamsNTC = struct paramsNtc { uint16_t Rs; uint16_t Ro; uint16_t beta; };
#ifdef ESP32
  using ParamsADC = struct parmsAdc{ uint8_t pin; bool ntcToGround; uint16_t Amax; adc_attenuation_t att; double Vcc; double Vref; double Voff; };
#else
    using ParamsADC = struct paramsAdc { uint8_t pin; bool ntcToGround; uint16_t Amax; double Vcc; double Vref; double Voff; };
#endif

class NTCsensor
{
  public:
    NTCsensor(ParamsNTC &ntc, ParamsADC &adc)  : _ntc(ntc), _adc(adc)
      {
        pinMode(_adc.pin, INPUT);
        #ifdef ESP32
          analogSetAttenuation(_adc.att);
        #endif
        _Roo = _ntc.Ro * exp(-(double)_ntc.beta / (_To - _Tabs)); // calculate the resistance of the NTC for T --> oo
      }

    double getCelsius();
    double getKelvin();
    double getFahrenheit();
    double getAnalogValue();    // returns the actual analog value
    double getRt();             // returns resistance of NTC at actual temperature
    double getRoo();            // returns R(T-->oo)
    double getFactorK();        // returns k (Rt = Rs * k) 
    double getFactorV();        // returns v (Vref - Voff)/Amax
    double getVin();            // returns the applied input voltage
    void  printParams();        // print the sensors (NTC) parameters
    void  printValues();        // print the measured values

  private:
    ParamsNTC &_ntc;
    ParamsADC &_adc;

    double       _Roo;            // resistance for T --> oo
    double       _k;              // k = Vin / (Vcc - Voff)
    double       _v;              // v = (Vref - Voff) / analogMax
    const double _To   = 25.0;    // nominal temperature
    const double _Tabs = -273.15; // absolute temperature

    double   _vin;          // input voltage on ADC-pin
    double   _Rt;           // calculated resistance at temperature T
    double   _tKelvin;      // calculated temperature in °K
    double   _tCelsius;     // calculated temperature in °C
    double   _tFahrenheit;  // calculated temperature in °F
    uint16_t _analogValue;  // measured analog value Aval

    void  _readSensor();        // read the sensor and update the measured values
};

#endif
