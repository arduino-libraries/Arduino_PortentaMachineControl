/**
 * @file TempProbeClass.h
 * @author Jan Henrik Sawatzki
 * @brief Header file for the shared Resistance Temperature Detector (RTD) and Thermocouple (TC) temperature sensor connectors of the Portenta Machine Control.
 *
 * This library allows interfacing with TC temperature sensors using the MAX31855 digital converter and RTD temperature sensors using the MAX31865 digital converter.
 * It provides methods to select input channel, enabling and disabling the sensor, and reading temperature values.
 */

#ifndef __TEMPPROBE_CLASS_H
#define __TEMPPROBE_CLASS_H

/* Includes -------------------------------------------------------------------*/
#include "utility/RTD/MAX31865.h"
#include "utility/THERMOCOUPLE/MAX31855.h"
#include <Arduino.h>
#include <mbed.h>
#include "pins_mc.h"

/* Class ----------------------------------------------------------------------*/

/**
 * @class TempProbeClass
 * @brief Class for managing thermocouples temperature sensor of the Portenta Machine Control.
 *
 * This allows interfacing with TC temperature sensors using the MAX31855 digital converter and RTD temperature sensors using the MAX31865 digital converter.
 * It provides methods to select input channel, enabling and disabling the sensor, and reading temperature values.
 */
class TempProbeClass: public MAX31855Class, public MAX31865Class {
public:
    /**
     * @brief Construct a TempProbeClass object.
     *
     * This constructor initializes a TempProbeClass object with the specified pin assignments for channel selection and TC connection.
     *
     * @param tc_cs_pin The pin number for the chip select (CS) pin of the Thermocouple temperature sensor.
     * @param ch_sel0_pin The pin number for the first channel selection bit.
     * @param ch_sel1_pin The pin number for the second channel selection bit.
     * @param ch_sel2_pin The pin number for the third channel selection bit.
     */
    TempProbeClass(PinName ch_sel0_pin = MC_TP_SEL0_PIN,
                     PinName ch_sel1_pin = MC_TP_SEL1_PIN,
                     PinName ch_sel2_pin = MC_TP_SEL2_PIN,
                     PinName tc_cs_pin = MC_TC_CS_PIN,
                     PinName rtd_cs_pin = MC_RTD_CS_PIN,
                     PinName rtd_th_pin = MC_RTD_TH_PIN);

    /**
     * @brief Destruct the TempProbeClass object.
     *
     * This destructor releases any resources used by the TempProbeClass object.
     */
    ~TempProbeClass();

    /**
     * @brief Initialize the TempProbeClass for TC measurements.
     *
     * @return true If initialization is successful, false otherwise.
     */
    bool beginTC();

    /**
     * @brief Initialize the TempProbeClass for RTD measurements.
     *
     * @return true If initialization is successful, false otherwise.
     */
    bool beginRTD();

    /**
     * @brief Disable the TC temperature sensors and release any resources.
     */
    void endTC();

    /**
     * @brief Disable the RTD temperature sensors and release any resources.
     */
    void endRTD();

    /**
     * @brief Select the input channel and probe type to be read (3 channels available).
     *
     * @param channel The channel number (0-2) to be selected for temperature reading.
     * @param probeType The probe type(PROBE_TC_K, PROBE_TC_J, PROBE_TC_T, PROBE_RTD_2W, PROBE_RTD_3W) to be selected for temperature reading.
     */
    void selectChannel(uint8_t channel, uint8_t probeType);

private:
    PinName _ch_sel0; // Pin for the first channel selection bit
    PinName _ch_sel1; // Pin for the second channel selection bit
    PinName _ch_sel2; // Pin for the third channel selection bit
    PinName _rtd_th;  // Pin for the RTD connection
    uint8_t _current_channel;
    uint8_t _current_probe_type;
    bool _tc_init = false;
    bool _rtd_init = false;
};

extern TempProbeClass MachineControl_TempProbe;

#endif /* __TEMPPROBE_CLASS_H */
