/*
 * Portenta Machine Control - Thermocouples Temperature Reading Example
 *
 * This example reads the temperatures measured by the thermocouples
 * connected to the temp probe inputs on the Portenta Machine Control Carrier.
 * It prints the temperature values to the Serial Monitor every second.
 *
 * Circuit:
 *  - Portenta H7
 *  - Portenta Machine Control
 *  - Two K Type thermocouple temperature sensors connected to TEMP PROBES CH0 and CH1
 *  - A J Type thermocouple temperature sensor connected to TEMP PROBES CH3
 *
 * This example code is in the public domain.
 * Copyright (c) 2024 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include <Arduino_PortentaMachineControl.h>

void setup() {
  Serial.begin(9600);
  while (!Serial) {
      ;
  }

  // Initialize TC temperature probes
  MachineControl_TCTempProbe.begin();
  Serial.println("TC Temperature probes initialization done");
}

void loop() {
  //Set CH0, has internal 150 ms delay
  MachineControl_TCTempProbe.selectChannel(0);
  Serial.println("CHANNEL 0 SELECTED");
  //Take CH0 measurement
  float temp_ch0 = MachineControl_TCTempProbe.readTemperature(PROBE_TC_K);
  // Check and print any faults
  if (!checkTCFault()) {
    Serial.print("TC Temperature CH0 [°C]: ");
    Serial.print(temp_ch0);
    Serial.println();
  }

  //Set CH1, has internal 150 ms delay
  MachineControl_TCTempProbe.selectChannel(1);
  Serial.println("CHANNEL 1 SELECTED");
  //Take CH1 measurement
  float temp_ch1 = MachineControl_TCTempProbe.readTemperature(PROBE_TC_K);
  // Check and print any faults
  if (!checkTCFault()) {
    Serial.print("TC Temperature CH1 [°C]: ");
    Serial.print(temp_ch1);
    Serial.println();
  }

  //Set CH2, has internal 150 ms delay
  MachineControl_TCTempProbe.selectChannel(2);
  Serial.println("CHANNEL 2 SELECTED");
  //Take CH2 measurement
  float temp_ch2 = MachineControl_TCTempProbe.readTemperature(PROBE_TC_J);
  // Check and print any faults
  if (!checkTCFault()) {
    Serial.print("TC Temperature CH2 [°C]: ");
    Serial.print(temp_ch2);
    Serial.println();
  }

  Serial.println();
}

bool checkTCFault() {
  // Check and print any faults
  uint8_t fault = MachineControl_TCTempProbe.getLastFault();
  if (fault & TC_FAULT_OPEN) {
    Serial.println("Thermocouple is open - no connections.");
    return true;
  }
  if (fault & TC_FAULT_SHORT_GND) {
    Serial.println("Thermocouple is short-circuited to GND.");
    return true;
  }
  if (fault & TC_FAULT_SHORT_VCC) {
    Serial.println("Thermocouple is short-circuited to VCC.");
    return true;
  }
  return false;
}