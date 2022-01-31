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
 *                         .-----------.  
 *                         |         - |             
 *                     .---|   Ro 10k  |--- GND   Analog NTC-Module
 *                    O    |   B 2800  |--- Vcc   NTC to GND, 10k to Vcc
 *                     `---|   Rs 10k  |--- A0
 *                         |         S |       
 *                         '-----------'     
 */

#include "NTCthermostat.h"

const uint8_t PIN_NTC     = A0;     // analog input pin (A0 UNO/WEMOS, 34 ESP32)
const uint16_t ANALOG_MAX = 1023;   // 1023 UNO/WEMOS, 4095 ESP32

// Nominal values of the Elegoo NTC module
const uint16_t BETA = 2800;   // characteristic constant of NTC
const uint16_t Ro   = 10000;  // nominal resistance of NTC 
const uint16_t Rs   = 10000;  // series resistance
bool NTC_TO_GROUND  = false;   // NTC is connected to ground

ParamsNTC ntcRs10k  = { 10000, 10000, 2800 };
ParamsNTC ntcRs20k  = { 20000, 10000, 2800 };

#ifdef ESP32
  ParamsADC adcEsp32_0   = { 34, true, 4095, ADC_0db,   3300.0, 1100.0,  65.0 };
  ParamsADC adcEsp32_2_5 = { 34, true, 4095, ADC_2_5db, 3300.0, 1300.0,  65.0 };
  ParamsADC adcEsp32_6   = { 34, true, 4095, ADC_6db,   3300.0, 1800.0,  90.0 };
  ParamsADC adcEsp32_11  = { 34, true, 4095, ADC_11db,  3300.0, 3200.0, 130.0 };
#else
  ParamsADC adcUno   = { A0, true, 1023, 5000.0, 5000.0, 0.0};
  ParamsADC adcWemos = { A0, true, 1023, 3300.0, 3200.0, -41.0};
#endif

// Forward declaration of the 3 callbacks
void turnHeatingOn();
void turnHeatingOff();
void processData();

NTCsensor     ntcSensor(ntcRs10k, adcUno);
NTCthermostat thermostat(ntcSensor, turnHeatingOn, turnHeatingOff, processData); // NTCthermostat object
bool heatingIsOn = false;                                                   // variable for the state of the heating


/**
 * Called when temperature drops below low limit
 */
void turnHeatingOn()
  {
    char buf[80];
    if (! heatingIsOn)
    {
      snprintf(buf, sizeof(buf), "Turn heating on, temperature dropped below limit of %4.1f : %4.1f °C", 
              (double)thermostat.getLimitLow(), (double)ntcSensor.getCelsius());
      Serial.println(buf);
      digitalWrite(LED_BUILTIN, HIGH);            // Simulates turning heating on
      heatingIsOn = true;
    }
  }

/**
 * Called when tempereature exceeds high limit
 */
void turnHeatingOff()
  {
    char buf[80];
    if (heatingIsOn)
    {
      snprintf(buf, sizeof(buf), "Turn heating off, temperature exeeds limit of %4.1f : %4.1f °C", 
              (double)thermostat.getLimitHigh(), (double)ntcSensor.getCelsius());
      Serial.println(buf);
      digitalWrite(LED_BUILTIN, LOW);  // Simulates turning off heating
      heatingIsOn = false;
    }
  }

void showValues()
{
  char buf[32];
  ntcSensor.printParams();
  ntcSensor.printValues();
  snprintf(buf, sizeof(buf), "Heating is %s\n", heatingIsOn ? "ON" : "OFF");
  Serial.println(buf);
}

/**
 * Called every msRefresh milliseconds
 * Do something with the sensor readings
 */
void processData()
{
  showValues();
}

/**
 * Show default pins of SPI and I2C interface
 */
void ShowSPIandI2CdefaultPins()
{
  char buf[160];
  snprintf(buf, sizeof(buf), R"(--- SPI / I2C default pins ---
SPI MOSI = %d
SPI MISO = %d
SPI SCK  = %d
SPI SS   = %d
I2C SCL  = %d
I2C SDA  = %d
)", MOSI, MISO, SCK, SS, SCL, SDA);
  Serial.print(buf);
}

void setup() 
{
  Serial.begin(115200);
  thermostat.setLimitLow(21.0);
  thermostat.setLimitHigh(22.0);       // sets lower and upper limit to switch a heating on or off
  thermostat.setRefreshInterval(5000); // sets the refresh interval of the temperature measurement
  thermostat.enable();
} 

void loop()
{
  thermostat.loop();     // checks the temperature limits in the cycle of the set interval
}