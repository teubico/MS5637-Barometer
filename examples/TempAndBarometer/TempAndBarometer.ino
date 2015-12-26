/*
 * MS5637 Barometer Library for Arduino
 * Copyright 2015 - Mario Gomez <mario.gomez@teubi.co>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or 
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "MS5637Barometer.h"

// Create a new barometer object;
MS5637Barometer barometer;
void setup() {
  // Initialize barometer
  barometer.init();
  // Optionally: Change oversampling
  barometer.setOSR(OSR_4096);
  
  // Wait for serial port (Leonardo)
  while(!Serial);
  Serial.begin(115200);
}

void loop() {
  // Read current temperature.
  int32_t temperature = barometer.readTemperature();
  Serial.print("Temperature: ");
  Serial.prin(temperature/100.0, 2);
  Serial.println("C");

  // Read temperature compensated pressure
  int32_t pressure = barometer.readTCPressure();
  Serial.print("Pressure: ");
  Serial.prin(temperature/100.0, 2);
  Serial.println("mbar");

  delay(5000);
}
