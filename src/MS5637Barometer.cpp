#include <Wire.h>
#include <Math.h>
#include "MS5637Barometer.h"

MS5637Barometer::MS5637Barometer() {
  this->PSENS = 0;
  this->POFFSET = 0;
  this->TCS = 0;
  this->TCO = 0;
  this->TREF = 0;
  this->TEMPSENS = 0;
  this->device_available = false;
  this->D1 = 0;
  this->D2 = 0;
  this->dT = 0;
  this->TEMP = 0;
  this->AOFF = 0;
  this->SENS = 0;
  this->TCP = 0;
  this->oversampling = OSR_1024;
};

/*
 * Reads factory-programmed EEPROM values for temperature-compensated
 * pressure calculation.
 */
uint16_t MS5637Barometer::readEEPROM(uint8_t addr) {
  uint16_t value = 0;
  addr = (addr & 0x07)<<1;
  Wire.requestFrom(0xED,2);
  uint8_t i = 0;
  while(Wire.available() && i<2) {
    value = value<<((i++)*8);
    value = value | Wire.read();
  }
  return value;
}

/*
 * This method starts temperature/pressure conversion and 
 * reads the result from the ADC. This method is blocking
 * and locks the execution thread until the read is finished.
 * Execution time depends of the over-sampling value.
 * (MIN .56ms MAX: 16.45ms)
 */
uint32_t MS5637Barometer::readADC(enum CONV_TYP type,enum OSR sampling) {
  uint32_t value = 0;
  uint8_t cmd = 0x80;
  cmd = cmd | (type<<4) | (sampling<<1);
  Wire.beginTransmission(0xEC);
    Wire.send(cmd);
  Wire.endTransmission();
  uint16_t wait_for_read = 0;
  switch(sampling) {
    case OSR_256:
      wait_for_read = 560;
      break;
    case OSR_512:
      wait_for_read = 1100;
      break;
    case OSR_1024:
      wait_for_read = 2100;
      break;
    case OSR_2048:
      wait_for_read = 4200;
      break;
    case OSR_4096:
      wait_for_read = 8250;
      break;
    case OSR_8192:
      wait_for_read = 16450;
      break;
  }
  delayMicroseconds(wait_for_read);
  
  Wire.beginTransmission(0xEC);
    Wire.send(0x00);
  Wire.endTransmission();
  
  Wire.requestFrom(0xED,3);
  uint8_t i = 0;
  while(Wire.available() && i<3) {
    value = value<<((i++)*8);
    value = value | Wire.read();
  }
  return value;
}

/*
 * Initializes the sensor and reads the EEPROM data
 * for calibration.
 */
boolean MS5637Barometer::init() {
  // Reset device
  Wire.beginTransmission(0xEC);
    Wire.send(0x1E);
  Wire.endTransmission();

  // Read Factory EEPROM values
  this->PSENS = this->readEEPROM(1);
  this->POFFSET = this->readEEPROM(2);
  this->TCS = this->readEEPROM(3);
  this->TCO = this->readEEPROM(4);
  this->TREF = this->readEEPROM(5);
  this->TEMPSENS = this->readEEPROM(6);

  this->device_available = true;
}

/*
 * Sets the oversampling value (accuracy) for the ADC.
 * More accuracy implies longer conversion times.
 */
void MS5637Barometer::setOSR(enum OSR sampling) {
  this->oversampling = sampling;
}

/*
 * Calculates the temperature-compensated pressure for
 * the sensor using the pre-defined accuracy.
 * OSR       Time-to-Convert(ms) Resolution(mbar) Resolution(°C)
 * OSR_256          0.54            0.110            0.012
 * OSR_512          1.06            0.062            0.009
 * OSR_1024         2.08            0.039            0.006
 * OSR_2048         4.13            0.028            0.004
 * OSR_4096         8.22            0.021            0.003
 * OSR_8192        16.44            0.016            0.002
 */
int32_t MS5637Barometer::readTCPressure() {
  int32_t value;
  this->D1 = this->readADC(PRESSURE,this->oversampling);
  this->D2 = this->readADC(TEMPERATURE,this->oversampling);
  this->dT = this->D2 - (this->TREF*pow(2,8));
  this->TEMP = 20 + ((this->dT*this->TEMPSENS)/pow(2,23));
  this->AOFF = this->POFFSET*pow(2,17) + (this->TCO*dT)/pow(2,6);
  this->SENS = this->PSENS*pow(2,16) + (this->TCS*dT)/pow(2,7);
  this->TCP = (this->D1*(this->SENS/pow(2,21)) - this->AOFF)/pow(2,15);
  return this->TCP;
}
