/*
 * Portenta Machine Control - Temperature Probes Mixed Example
 *
 * This example provides a method to test the RTD/TC inputs
 * on the Machine Control Carrier for mixed setups.
 * 
 * Acquisition of 2-wire RTDs is possible by shorting the RTDx pin to the TPx pin.
 * The Machine Control Carrier features a precise 400 ohm 0.1% reference resistor,
 * which serves as a reference for the MAX31865.
 *
 * Circuit:
 *  - Portenta H7
 *  - Portenta Machine Control
 *  - RTDs 3-wire or 2-wire
 *  - TCs type K/J/T
 *
 * This example code is in the public domain.
 * Copyright (c) 2024 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include <Arduino_PortentaMachineControl.h>

// The value of the Rref resistor.
#define RREF      400.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100
#define RNOMINAL  100.0

void setup() {
  Serial.begin(9600);
  while (!Serial) {
      ;
  }

  // Initialize RTD temperature probes
  MachineControl_TempProbe.beginRTD();
  Serial.println("RTD Temperature probes initialization done");

  // Initialize TC temperature probes
  MachineControl_TCTempProbe.beginTC();
  Serial.println("TC Temperature probes initialization done");
}

void loop() {
  MachineControl_TempProbe.selectChannel(0, PROBE_RTD_3W);
  Serial.println("CHANNEL 0 SELECTED");
  uint16_t rtd = MachineControl_TempProbe.readRTD();
  float ratio = rtd;
  ratio /= 32768;

  // Check and print any faults
  if (!checkRTDFault()) {
    Serial.print("RTD value: "); Serial.println(rtd);
    Serial.print("Ratio = "); Serial.println(ratio, 8);
    Serial.print("Resistance = "); Serial.println(RREF * ratio, 8);
    Serial.print("Temperature = "); Serial.println(MachineControl_TempProbe.convertRTDTemperature(RNOMINAL, RREF));
  }
  Serial.println();
  delay(100);

  //Set CH1, has internal 150 ms delay
  MachineControl_TempProbe.selectChannel(0, PROBE_TC_T);
  Serial.println("CHANNEL 0 SELECTED");
  //Take CH0 measurement
  float temp_ch1 = MachineControl_TempProbe.readTCTemperature();
  // Check and print any faults
  if (!checkTCFault()) {
    Serial.print("TC Temperature CH1 [°C]: ");
    Serial.print(temp_ch1);
    Serial.println();
  }

  MachineControl_TempProbe.selectChannel(2, PROBE_RTD_3W);
  Serial.println("CHANNEL 2 SELECTED");
  rtd = MachineControl_TempProbe.readRTD();
  ratio = rtd;
  ratio /= 32768;

  // Check and print any faults
  if (!checkRTDFault()) {
    Serial.print("RTD value: "); Serial.println(rtd);
    Serial.print("Ratio = "); Serial.println(ratio, 8);
    Serial.print("Resistance = "); Serial.println(RREF * ratio, 8);
    Serial.print("Temperature = "); Serial.println(MachineControl_TempProbe.convertRTDTemperature(RNOMINAL, RREF));
  }
  Serial.println();
  delay(1000);
}

bool checkRTDFault() {
  // Check and print any faults
  uint8_t fault = MachineControl_TempProbe.readRTDFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (MachineControl_TempProbe.getRTDHighThresholdFault(fault)) {
      Serial.println("RTD High Threshold");
    }
    if (MachineControl_TempProbe.getRTDLowThresholdFault(fault)) {
      Serial.println("RTD Low Threshold");
    }
    if (MachineControl_TempProbe.getRTDLowREFINFault(fault)) {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (MachineControl_TempProbe.getRTDHighREFINFault(fault)) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (MachineControl_TempProbe.getRTDLowRTDINFault(fault)) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (MachineControl_TempProbe.getRTDVoltageFault(fault)) {
      Serial.println("Under/Over voltage");
    }
    MachineControl_TempProbe.clearRTDFault();
    return true;
  } else {
    return false;
  }
}

bool checkTCFault() {
  // Check and print any faults
  uint8_t fault = MachineControl_TempProbe.getTCLastFault();
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