/**
 * Header       NTCsensor.h
 * Author       2021-05-13 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Declaration of the class NTCsensor
 * 
 * Constructor
 * arguments    pinNTC       analog input pin internally connected to the ADC (A0 for Arduino uno)
 *              beta         characteristic parameter of the NTC
 *              Ro           resistance of the NTC at nomnal temperature of 25°C
 *              Rs           value of the series resistor in ohm
 *              ntcToGround  pass true if NTC is connected to ground, otherwise pass false
 *              analogMax    pass 1023 for Arduino Uno 10-bit ADC
 */
#ifndef _NTCSENSOR_H_
#define _NTCSENSOR_H_
#include <Arduino.h>

class NTCsensor
{
  public:
    NTCsensor(uint8_t pinNTC, uint16_t beta, uint16_t Ro, uint16_t Rs, bool ntcToGround = false, uint16_t analogMax = 1023)  :
      _pinNTC(pinNTC), _beta(beta), _Ro(Ro), _Rs(Rs), _ntcToGround(ntcToGround), _analogMax(analogMax)
      {
        pinMode(_pinNTC, INPUT);
        _Roo = _Ro * exp(-(float)_beta / (_To - _Tabs)); // calculate the resistance of the NTC for T --> oo
      }

    float getCelsius();
    float getKelvin();
    float getFahrenheit();
    float getAnalogValue();    // returns the actual analog value
    float getRt();             // returns resistance of NTC at actual temperature
    float getRoo();            // returns R(T-->oo)
    void  printSensorParams(); // print the sensors (NTC) parameters
    void  printSensorValues(); // print the measured values

  private:
    uint8_t  _pinNTC;
    uint16_t _beta;
    uint16_t _Ro;           // resistance at nominal temperature
    uint16_t _Rs;           // series resistance to NTC
    bool     _ntcToGround;
    uint16_t _analogMax;    // Amax maximum value of the ADC

    float       _Roo;            // resistance for T --> oo
    float       _k;              // k = (Amax / Aval) - 1
    const float _To = 25;        // nominal temperature
    const float _Tabs = -273.15; // absolute temperature

    float    _Rt;           // calculated resistance at temperature T
    float    _tKelvin;      // calculated temperature in °K
    float    _tCelsius;     // calculated temperature in °C
    float    _tFahrenheit;  // calculated temperature in °F
    uint16_t _analogValue;  // measured analog value Aval

    void  _update();        // read the sensor and update the measured values
};

#endif
