/**
 * @file TCTempProbeClass.cpp
 * @author Leonardo Cavagnis
 * @brief Source file for the Thermocouple (TC) temperature sensor connector of the Portenta Machine Control.
 */

/* Includes -----------------------------------------------------------------*/
#include "TCTempProbeClass.h"

/* Functions -----------------------------------------------------------------*/
TCTempProbeClass::TCTempProbeClass(PinName tc_cs_pin,
                                     PinName ch_sel0_pin,
                                     PinName ch_sel1_pin,
                                     PinName ch_sel2_pin)
: TempProbeClass(ch_sel0_pin, ch_sel1_pin, ch_sel2_pin, tc_cs_pin, MC_RTD_CS_PIN, MC_RTD_TH_PIN) {
}

TCTempProbeClass::~TCTempProbeClass() {
}

bool TCTempProbeClass::begin() {
    TempProbeClass::beginTC();

    return true;
}

void TCTempProbeClass::end() {
    TempProbeClass::endTC();
}

void TCTempProbeClass::selectChannel(int channel) {
    TempProbeClass::selectChannel(channel, PROBE_TC_K);
}

float TCTempProbeClass::readTemperature(uint8_t type) {
    TempProbeClass::setTCType(type);
    return TempProbeClass::readTCTemperature();
}

TCTempProbeClass MachineControl_TCTempProbe;
/**** END OF FILE ****/
