/*
 * Portenta Machine Control - Encoder Read Example
 *
 * This sketch shows the functionality of the encoders on the Machine Control.
 * It demonstrates how to periodically read and interpret the values from the two encoder channels.
 *
 * Circuit:
 *  - Portenta H7
 *  - Portenta Machine Control
 *
 * This example code is in the public domain. 
 * Copyright (c) 2024 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include <Arduino_PortentaMachineControl.h>

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
}

void loop() {
  Serial.print("Encoder 0 State: ");
  Serial.println(MachineControl_Encoders.getCurrentState(0),BIN);
  Serial.print("Encoder 0 Pulses: ");
  Serial.println(MachineControl_Encoders.getPulses(0));
  Serial.print("Encoder 0 Revolutions: ");
  Serial.println(MachineControl_Encoders.getRevolutions(0));
  Serial.println();
 
  Serial.print("Encoder 1 State: ");
  Serial.println(MachineControl_Encoders.getCurrentState(1),BIN);
  Serial.print("Encoder 1 Pulses: ");
  Serial.println(MachineControl_Encoders.getPulses(1));
  Serial.print("Encoder 1 Revolutions: ");
  Serial.println(MachineControl_Encoders.getRevolutions(1));
  Serial.println();

  delay(25);
}
