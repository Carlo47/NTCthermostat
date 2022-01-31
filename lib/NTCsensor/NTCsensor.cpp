/**
 * Class        NTCsensor.cpp
 * Author       2022-01-31 Charles Geiser (https://www.dodeka.ch)
 *
 * Purpose      Implements the class NTCsensor which measures the voltage at the midpoint
 *              of a voltage divider consisting of a thermistor (NTC) and a series resistor.
 *              The resistance of the NTC is calculated from this voltage value and the 
 *              series resistance. The temperature is then obtained with the help of the 
 *              Steinhart-Hart equation.
 *                                                
 * Board        Arduino UNO R3, Wemos D1 R2, ESP32 DevKit V1
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
 *              Vcc:  Connect the resistors to GND and 3.3V for Wemos D1 and ESP32
 *                    Connect the resistors to GND and 5V   for Arduino UNO  
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
void NTCsensor::_readSensor()
{
    _analogValue = analogRead(_adc.pin);
    _v = (_adc.Vref - _adc.Voff) / (double)_adc.Amax;
    _vin = (_analogValue * _v) + _adc.Voff;
    _k = _vin / ( _adc.Vcc - _vin);
    if (_adc.ntcToGround == false) _k = 1.0 / _k;
    _Rt = (double)_ntc.Rs * _k;
    _tKelvin = (double)_ntc.beta / log(_Rt/_Roo);  // Calculate  T from Rt, Roo and BETA
    _tCelsius = _tKelvin + _Tabs;                  // Convert Kelvin to Celcius
    _tFahrenheit = _tCelsius * 9.0 / 5.0 + 32.0;   // Convert Celcius to Fahrenheit 
}

double NTCsensor::getCelsius()
{
    _readSensor();
    return _tCelsius;
}

double NTCsensor::getKelvin()
{
    _readSensor();
    return _tKelvin;
}

double NTCsensor::getFahrenheit()
{
    _readSensor();
    return _tFahrenheit;
}

double NTCsensor::getRt()
{
    _readSensor();
    return _Rt;
}

double NTCsensor::getRoo()
{
    return _Roo;
}

double NTCsensor::getAnalogValue()
{
    _readSensor();
    return _analogValue;
}

double NTCsensor::getFactorV()
{
    _readSensor();
    return _v;
}

double NTCsensor::getVin()
{
    _readSensor();
    return _vin;
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
void NTCsensor::printParams()
{
    char buf[272];

    snprintf(buf, sizeof(buf), R"(--- NTC Parameters ---
beta        %d
Ro         %d
Rs         %d
Roo      %7.5f
To       %7.2f °C
Tabs     %7.2f °C
--- ADC Parameters ---
Pin         %d
Analog Max  %d
NTC to GND  %s
Vcc        %5.0f mV
Vref       %5.0f mV
Voff       %5.0f mV
)",
_ntc.beta, _ntc.Ro, _ntc.Rs, _Roo, _To, _Tabs, 
_adc.pin, _adc.Amax, _adc.ntcToGround ? "true" : "false", _adc.Vcc, _adc.Vref, _adc.Voff );
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
void NTCsensor::printValues()
{
    char buf[184];

    _readSensor();
    snprintf(buf, sizeof(buf), R"(--- Sensor Readings ---
Analog Value %d
v        %7.5f
Vin      %7.0f mV
k        %7.5f
Rt         %5.0f
Tc         %5.1f °C
Tf         %5.1f °F
Tk         %5.1f °K
)", _analogValue, _v, _vin, _k, _Rt, _tCelsius, _tFahrenheit, _tKelvin);
Serial.println(buf);
}
