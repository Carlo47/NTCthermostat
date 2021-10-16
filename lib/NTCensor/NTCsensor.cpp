/**
 * Class        NTCsensor.cpp
 * Author       2021-05-13 Charles Geiser (https://www.dodeka.ch)
 *
 * Purpose      Implements the class NTCsensor which measures the voltage at the midpoint
 *              of a voltage divider consisting of a thermistor (NTC) and a series resistor.
 *              The resistance of the NTC is calculated from this voltage value and the 
 *              series resistance. The temperature is then obtained with the help of the 
 *              Steinhart-Hart equation.
 *                                                
 * Board        Arduino uno, Wemos D1 R2
 * 
 * Wiring          ---+-- Vcc                            ---+--- Vcc
 *                    |                                     |
 *                   .-.                                   .-. 
 *                   | | Rs                                |\| NTC
 *                   '-'                                   '-' 
 *                    |                                     | 
 *                    +---> analog input                    +---> analog input
 *                    |                                     |   
 *                   .-.                                   .-.
 *                   |\| NTC                               | | Rs
 *                   '-'                                   '-'
 *                    |                                     |
 *                 ---+--- GND                           ---+--- GND
 *              ntcToGround = true                   ntcToGround = false
 *              (default connection)                (alternative connection)
 * 
 * Remarks      The voltage Vcc doesn't matter. The accuracy of the temperature
 *              measurement depends on how exact we know the parameters of the
 *              NTC resistor (Ro at T=25°C and BETA). 
 * 
 * Equations    R(T) = Ro * exp(BETA * (1/T - 1/To)  Steinhart–Hart equation  
 *              To     Nominal temperature. Normally 25°C, 🚩 for calculations use degrees Kelvin 🚩
 *              Ro     Resistance of the NTC resistor at nominal temperature R(To)
 * 
 *              Roo  = Ro * exp(-BETA / To)          Resistance for T --> oo
 * 
 *              Rt   = Roo * exp(BETA / T)           Resistance at temperature T
 * 
 *              T    = BETA / ln(Rt / Roo)           Temperature as a function of measured resistance
 *              ==========================
 * 
 *              With 2 measured resistance values at 2 different temperatures we can determine the
 *              characteristic parameter BETA of an unknown NTC:
 * 
 *                     T2 * T1
 *              BETA = ------- * ln(R1 / R2)         R1, R2 measured resitance values at 
 *                     T2 - T1                       two different temperatures T1, T2
 * 
 *              Amax    maximal analog value         4095 for 12-bit ADC or 1023 for 10-bit ADC
 *              Aval    measured analog value
 * 
 *              Rt   = Rs * Aval / (Amax - Aval)     NTC to GND
 *                   = Rs * 1 / k                    k = (Amax / Aval) - 1
 * 
 *              Rt   = Rs * (Amax - Aval) / Aval     NTC to Vcc
 *                   = Rs * k                        k = (Amax / Aval) -  1
 *  
 * References   http://www.resistorguide.com/ntc-thermistor/#Voltage-current_characteristic
 * 
 * Attention 🔴 To be able to use snprintf() with float format for the Arduino uno, the following flags 
 *              must be set in platformio.ini: build_flags = -Wl,-u,vfprintf -lprintf_flt -lm 
 */

#include "NTCsensor.h"

/**
 * Read sensor and update calculated values
 */
void NTCsensor::_update()
{
    _analogValue = analogRead(_pinNTC);
    if (_analogValue < 1)      // only if NTC to ground and short-circuited
    {
        _tKelvin = 1.0 / 0.0;  // T = oo
        _Rt = _Roo;            // Roo = R(T-->oo)
    }
    else
    {
        _k = (float)_analogMax / (float)_analogValue - 1.0;
        _k = _ntcToGround == true ? 1.0 / _k : _k;
        _Rt = (float)_Rs * _k;
        _tKelvin = (float)_beta / log(_Rt/ _Roo);  // calculate T from Rt, Roo and BETA       
    }
    _tCelsius = _tKelvin + _Tabs;                  // convert Kelvin to Celcius
    _tFahrenheit = _tCelsius * 9.0 / 5.0 + 32.0;   // convert Celcius to Fahrenheit
}

float NTCsensor::getCelsius()
{
    _update();
    return _tCelsius;
}

float NTCsensor::getKelvin()
{
    _update();
    return _tKelvin;
}

float NTCsensor::getFahrenheit()
{
    _update();
    return _tFahrenheit;
}

float NTCsensor::getRt()
{
    _update();
    return _Rt;
}

float NTCsensor::getRoo()
{
    return _Roo;
}

float NTCsensor::getAnalogValue()
{
    _update();
    return (_analogValue);
}

/**
 * Print sensor parameters to monitor
 * 
 * beta        material constant of NTC
 * Ro          resistance of NTC at nominal temoperature of 25°C
 * Rs          series resistanceconnected to NTC
 * Roo         calculated resistance for temperature --> oo
 * k           converion factor (analogMax/analogValue) - 1
 * To          nominal temperature To = 25 °C 
 * Tabs        absolute temperature -273.15 °C
 * analogMax   maximum value of Arduinos ADC
 * ntcToGround true if NTC is connected to ground, if connected to Vcc false
 */
void NTCsensor::printSensorParams()
{
    char buf[240];
    _update();
    snprintf(buf, sizeof(buf)/sizeof(buf[0]), 
        "Sensor Parameters\n-----------------\nbeta        %d\nRo          %d\nRs          %d\nRoo         %11.5e\nk           %7.5f\nTo          %7.2f °C\nTabs        %.2f °C\nanalogMax   %d\nntcToGround %s\r\n",
        _beta, _Ro, _Rs, _Roo, _k, _To, _Tabs, _analogMax, _ntcToGround ? "true" : "false");
    Serial.println(buf);
}

/**
 * Print sensor readings to monitor
 * 
 * analogValue  reading from analog pin
 * Rt           calculated resistanc of NTC at temperature T
 * Tc           calculated temperature in °-Celsius
 * Tf           calculated temperature in °-Fahrenheit
 * Tk           calculated temperature in °-Kelvin
 */
void NTCsensor::printSensorValues()
{
    char buf[120];
    snprintf(buf, sizeof(buf)/sizeof(buf[0]),
        "Sensor Readings\n---------------\nanalogValue %d\nRt %5.0f\nTc %5.1f °C\nTf %5.1f °F\nTk %5.1f °K",
        _analogValue, _Rt, _tCelsius, _tFahrenheit, _tKelvin);
    Serial.println(buf);
}
