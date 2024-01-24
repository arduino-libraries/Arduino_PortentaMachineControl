/*
 * Portenta Machine Control - CAN Read Example
 *
 * This sketch shows the usage of the CAN transceiver on the Machine Control
 * and demonstrates how to receive data from the RX CAN channel.
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

  if (!MachineControl_CANComm.begin(CanBitRate::BR_500k)) {
    Serial.println("CAN init failed.");
    while(1) ;
  }
}

void loop() {
  if (MachineControl_CANComm.available()) {
    CanMsg const msg = MachineControl_CANComm.read();
    Serial.println(msg);
  }
}
