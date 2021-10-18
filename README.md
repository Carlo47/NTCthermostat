# NTCthermostat
Shows how to realize a thermostat with a NTC resistor

Maybe you also have some of these strange components in your tinkering box, 
which you removed from a defective battery pack or an old PC power supply. 
These are NTC resistors, i.e. resistors with a negative temperature 
coefficient. Their resistance therefore decreases with increasing temperature. 
They are suitable for temperature measurement or, as the title of this project 
says, for building a thermostat. 

![NTC](thermistors.jpg)

## Some Formulas
If we connect an NTC resistor in series with another resistor whose value we 
know, we can measure the voltage at this voltage divider and calculate the 
unknown resistance value of the NTC from it. We can set up the voltage divider 
in two different ways, namely connect the NTC to ground or to Vcc. 
```
  ---+-- Vcc                            ---+--- Vcc
     |                                     |
    .-.                                   .-. 
    | | Rs                                |\| NTC
    '-'                                   '-' 
     |                                     | 
     +---> analog input                    +---> analog input
     |                                     |   
    .-.                                   .-.
    |\| NTC                               | | Rs
    '-'                                   '-'
     |                                     |
  ---+--- GND                           ---+--- GND
```
If we assume that the ADC (analog to digital converter) of our microcontroller 
delivers the value Aval between 0 and Amax (e.g. 0..1023), for input voltages 
from 0V to Vcc , the resistance of the NTC for the two cases is calculated as 
follows: 
```
  Rt = Rs * Aval / (Amax - Aval         Rt = Rs * (Amax - Aval) / Aval
    or with                               or with
  k = (Amax -Aval) / Aval               k = (Amax -Aval) / Aval 
    = Amax/Aval - 1                       = Amax/Aval - 1
    we get                                we get
  Rt = Rs * 1/k (NTC to GND)            Rt = Rs * k (NTC to Vcc)
```
Now we know the resistance of the NTC. But how do we calculate the temperature 
from this? The formula of Steinhart-Hart helps us here: 
```
  R(T) = Ro * exp(BETA * (1/T - 1/To)  Steinhart–Hart equation
  ===================================  
  To     Nominal temperature, normally 25°C 🚩 for calculations use degrees Kelvin 🚩
  Ro     Resistance of the NTC resistor at nominal temperature R(To)
 
  Roo  = Ro * exp(-BETA / To)          Resistance for T --> oo

  Rt   = Roo * exp(BETA / T)           Resistance at temperature T
  
  T    = BETA / ln(Rt / Roo)           Temperature as a function of measured resistance
```
In the Steinhart-Hart equation BETA denotes the characteristic parameter of the NTC. 
It can be taken from the data sheet of the NTC or calculated from 2 resistance 
measurements at different temperatures: 
```
         T2 * T1
  BETA = ------- * ln(R1 / R2)         R1, R2 measured resitance values at 
         T2 - T1                       two different temperatures T1, T2	
```
## Design Steps
With the above knowledge we are now able to design two classes, one for the NTC sensor, 
the other for the thermostat. The sensor class must know to which pin the sensor is 
connected and how large BETA and the nominal resistance of the NTC are. Further it must 
be known whether the NTC is connected to Vcc or to GND and how large the maximum analog 
value of the ADC is. The constructor of `NTCsensor` then looks like this. The member 
variables are initialized, the constant Roo is calculated and the pin mode of the 
analog pin is set. 
```
  NTCsensor(uint8_t pinNTC, uint16_t beta, uint16_t Ro, uint16_t Rs, bool ntcToGround = false, uint16_t analogMax = 1023)  :
    _pinNTC(pinNTC), _beta(beta), _Ro(Ro), _Rs(Rs), _ntcToGround(ntcToGround), _analogMax(analogMax)
    {
      pinMode(_pinNTC, INPUT);
      _Roo = _Ro * exp(-(float)_beta / (_To - _Tabs)); // calculate the resistance of the NTC for T --> oo
    }
```
Of course, we would like to be able to query the temperature in °C, °F and °K and 
also output the parameters of the NTC. The corresponding methods are provided for 
this purpose.

A thermostat is really nothing more than a switch that triggers appropriate actions 
at certain temperatures. That is why we pass to the constructor of the `NTCthermostat` 
class only a reference to a sensor object and the references to two functions that 
are called when the lower or upper temperature limits are reached. 
```
  typedef void (*CallbackFunction)();

  NTCthermostat(NTCsensor &ntc, CallbackFunction onLowTemp, CallbackFunction onHighTemp) : 
      _ntc(ntc), _onLowTemp(onLowTemp), _onHighTemp(onHighTemp)  {}
```
The temperature is measured periodically in the loop() method. The measurement 
interval and the two temperature limits are set with corresponding methods. All 
other details can be seen in the code. 
