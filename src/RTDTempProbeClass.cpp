/**
 * @file RTDTempProbeClass.cpp
 * @author Leonardo Cavagnis
 * @brief Source file for the Resistance Temperature Detector (RTD) temperature sensor connector of the Portenta Machine Control.
 */

/* Includes -----------------------------------------------------------------*/
#include "RTDTempProbeClass.h"

/* Functions -----------------------------------------------------------------*/
RTDTempProbeClass::RTDTempProbeClass(PinName rtd_cs_pin,
                                     PinName ch_sel0_pin,
                                     PinName ch_sel1_pin,
                                     PinName ch_sel2_pin,
                                     PinName rtd_th_pin)
: TempProbeClass(ch_sel0_pin, ch_sel1_pin, ch_sel2_pin, MC_TC_CS_PIN, rtd_cs_pin, rtd_th_pin) {
}

RTDTempProbeClass::~RTDTempProbeClass() {
}

bool RTDTempProbeClass::begin(uint8_t rtd_type) {
    TempProbeClass::beginRTD();
    TempProbeClass::setRTDType(rtd_type);
    _current_probe_type = rtd_type;

    return true;
}

void RTDTempProbeClass::end() {
    TempProbeClass::endRTD();
}

void RTDTempProbeClass::selectChannel(int channel) {
    TempProbeClass::selectChannel(channel, _current_probe_type);
}

float RTDTempProbeClass::readTemperature(float RTDnominal, float refResistor) {
    return TempProbeClass::convertRTDTemperature(RTDnominal, refResistor);
}

RTDTempProbeClass MachineControl_RTDTempProbe;
/**** END OF FILE ****/