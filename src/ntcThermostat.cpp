/**
 * Program      ntcThermostat.cpp
 * Author       2021-05-13 Charles Geiser
 * 
 * Purpose      The program shows how to regulate the room temperature or the temperature 
 *              of a closed container with the help of the class NTCthermostat.
 *              The thermostat requires a reference to an NTCsensor object and 2 references 
 *              to functions which are called when the specified limits are exceeded.
 *              The limits at which the thermostat switches on or off are set by the user
 *              The interval for the temperature measurement is also specified by the user.
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
 * 
 * 
 * Remarks      Analog Temp module from Elegoo Sensor Kit
 *                         .----------.              
 *                     .---|   Ro 10k  |--- GND   Analog NTC-Module
 *                    O    |   B 2800  |--- Vcc   NTC to GND, 10k to Vcc
 *                     `---|   Rs 10k  |--- A0    
 *                          `----------´     
 */

#include <Arduino.h>
#include "NTCthermostat.h"

#define PIN_NTC       A0     // analog input pin
#define ANALOG_MAX    1023   // Arduinos ADC range is 0..1023

// Nominal values of the Elegoo NTC module
#define BETA          2800   // characteristic constant of NTC
#define Ro            10000  // nominal resistance of NTC 
#define Rs            10000  // series resistance
#define NTC_TO_GROUND true

// Forward declaration of the 2 callbacks to be supplied to the thermostats constructor
void onLowerLimit();
void onUpperLimit();
 
NTCsensor     myNTC(PIN_NTC, BETA, Ro, Rs, NTC_TO_GROUND, ANALOG_MAX);   // NTCsensor object
NTCthermostat myThermostat(myNTC, onLowerLimit, onUpperLimit);           // NTCthermostat object
bool heatingIsOn = false;                                                // variable for the state of the heating

/**
 * To be performed when temperature drops below lower limit
 */
void onLowerLimit()
{
  if (! heatingIsOn)
  {
    Serial.print(myThermostat.getCelsius());
    Serial.println(" °C: switch heating on");
    heatingIsOn = true;
  }
}

/**
 * To be performed when temperature rises above upper limit
 */
void onUpperLimit()
{
  if (heatingIsOn)
  {
    Serial.print(myThermostat.getCelsius());
    Serial.println(" °C: switch heating off");
    heatingIsOn = false;
  }
}

/**
 * Print temperature every N milliseconds
 */
void printTemperature(uint32_t every_N_ms)
{
  if (millis() % every_N_ms == 0)
  {
    myNTC.printSensorValues();
  }
}

void setup() 
{
  Serial.begin(115200);
  myThermostat.setLimits(25.0, 30.0);  // sets lower and upper limit to switch a heating on or off
  myThermostat.setInterval(1000);      // sets the refresh interval of the temperature measurement
} 

void loop()
{
  myThermostat.loop();     // checks the temperature limits in the cycle of the set interval
  printTemperature(10000); // print sensor readings every 10 seconds
}