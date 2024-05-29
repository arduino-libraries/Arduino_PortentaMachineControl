/**
 * @file RTDTempProbeClass.h
 * @author Leonardo Cavagnis
 * @brief Header file for the Resistance Temperature Detector (RTD) temperature sensor connector of the Portenta Machine Control.
 *
 * This library allows interfacing with RTD temperature sensors using the MAX31865 digital converter.
 * It provides methods to select input channel, enabling and disabling the sensor, and reading temperature values.
 */

#ifndef __RTD_TEMPPROBE_CLASS_H
#define __RTD_TEMPPROBE_CLASS_H

/* Includes -------------------------------------------------------------------*/
#include "TempProbeClass.h"
#include <Arduino.h>
#include <mbed.h>
#include "pins_mc.h"

/* Class ----------------------------------------------------------------------*/

/**
 * @class RTDTempProbeClass
 * @brief Class for managing RTD temperature sensor inputs of the Portenta Machine Control.
 *
 * This class allows interfacing with RTD temperature sensors through the use of the MAX31865 digital converter.
 * It provides methods to configure and read temperature values from the selected input channel.
 */
class RTDTempProbeClass: public TempProbeClass {
public:
    /**
     * @brief Construct a RTDTempProbeClass object.
     *
     * This constructor initializes a RTDTempProbeClass object with the specified pin assignments for channel selection and RTD connection.
     *
     * @param rtd_cs_pin The pin number for the chip select (CS) pin of the RTD temperature sensor.
     * @param ch_sel0_pin The pin number for the first channel selection bit.
     * @param ch_sel1_pin The pin number for the second channel selection bit.
     * @param ch_sel2_pin The pin number for the third channel selection bit.
     * @param rtd_th_pin The pin number for the RTD connection.
     */
    RTDTempProbeClass(PinName rtd_cs_pin = MC_RTD_CS_PIN,
                     PinName ch_sel0_pin = MC_TP_SEL0_PIN,
                     PinName ch_sel1_pin = MC_TP_SEL1_PIN,
                     PinName ch_sel2_pin = MC_TP_SEL2_PIN,
                     PinName rtd_th_pin = MC_RTD_TH_PIN);

    /**
     * @brief Destruct the RTDTempProbeClass object.
     *
     * This destructor releases any resources used by the RTDTempProbeClass object.
     */
    ~RTDTempProbeClass();

    /**
     * @brief Initialize the RTDTempProbeClass with the specified RTD wiring type.
     *
     * @param rtd_type The RTD wiring type for connection to the digital converters (default is PROBE_RTD_3W).
     * @return true If initialization is successful, false otherwise.
     */
    bool begin(uint8_t rtd_type = PROBE_RTD_3W);

    /**
     * @brief Disable the temperature sensors and release any resources.
     */
    void end();

    /**
     * @brief Select the input channel to be read (3 channels available).
     *
     * @param channel The channel number (0-2) to be selected for temperature reading.
     */
    void selectChannel(int channel);

    /**
     * @brief Read temperature value of the connected RTD
     *
     * @param RTDnominal The 'nominal' resistance of the RTD sensor at 0 °C
     * @param refResistor The value of the reference sensor
     */
    float readTemperature(float RTDnominal, float refResistor);

private:
    uint8_t _current_probe_type;
};

extern RTDTempProbeClass MachineControl_RTDTempProbe;

#endif /* __RTD_TEMPPROBE_CLASS_H */
