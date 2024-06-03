/**
 * @file TCTempProbeClass.cpp
 * @author Jan Henrik Sawatzki
 * @brief Source file for the shared Resistance Temperature Detector (RTD) and Thermocouple (TC) temperature sensor connectors of the Portenta Machine Control.
 */

/* Includes -----------------------------------------------------------------*/
#include "TempProbeClass.h"

#if __has_include("portenta_info.h")
#include "portenta_info.h"
#define TRY_REV2_RECOGNITION
uint8_t* boardInfo();
#define PMC_R2_SKU  (24 << 8 | 3)
#endif

/* Functions -----------------------------------------------------------------*/
TempProbeClass::TempProbeClass(PinName ch_sel0_pin,
                                PinName ch_sel1_pin,
                                PinName ch_sel2_pin,
                                PinName tc_cs_pin,
                                PinName rtd_cs_pin,
                                PinName rtd_th_pin)
: MAX31855Class(tc_cs_pin), MAX31865Class(rtd_cs_pin), _ch_sel0{ch_sel0_pin}, _ch_sel1{ch_sel1_pin}, _ch_sel2{ch_sel2_pin},/* _tc_cs{tc_cs_pin}, _rtd_cs{rtd_cs_pin},*/ _rtd_th{rtd_th_pin} {
}

TempProbeClass::~TempProbeClass() {
}

bool TempProbeClass::beginTC() {
    if(!_tc_init) {
        if(!_rtd_init) {
            pinMode(_ch_sel0, OUTPUT);
            pinMode(_ch_sel1, OUTPUT);
            pinMode(_ch_sel2, OUTPUT);

            digitalWrite(_ch_sel0, LOW);
            digitalWrite(_ch_sel1, LOW);
            digitalWrite(_ch_sel2, LOW);
        }

        MAX31855Class::begin();

        _tc_init = true;
    }

    return true;
}

bool TempProbeClass::beginRTD() {
    if(!_rtd_init) {
        if(!_tc_init) {
            pinMode(_ch_sel0, OUTPUT);
            pinMode(_ch_sel1, OUTPUT);
            pinMode(_ch_sel2, OUTPUT);

            digitalWrite(_ch_sel0, LOW);
            digitalWrite(_ch_sel1, LOW);
            digitalWrite(_ch_sel2, LOW);
        }

        pinMode(_rtd_th, OUTPUT);

        digitalWrite(_rtd_th, LOW);

        MAX31865Class::begin();

        _rtd_init = true;
    }

    return true;
}

void TempProbeClass::endTC() {
    if(_tc_init) {
        MAX31855Class::end();

        _tc_init = false;

        if(!_rtd_init) {
            pinMode(_ch_sel0, INPUT);
            pinMode(_ch_sel1, INPUT);
            pinMode(_ch_sel2, INPUT);

            digitalWrite(_ch_sel0, LOW);
            digitalWrite(_ch_sel1, LOW);
            digitalWrite(_ch_sel2, LOW);
        }
    }
}

void TempProbeClass::endRTD() {
    if(_rtd_init) {
        MAX31865Class::end();

        pinMode(_rtd_th, INPUT);

        digitalWrite(_rtd_th, LOW);

        _rtd_init = false;

        if(!_tc_init) {
            pinMode(_ch_sel0, INPUT);
            pinMode(_ch_sel1, INPUT);
            pinMode(_ch_sel2, INPUT);

            digitalWrite(_ch_sel0, LOW);
            digitalWrite(_ch_sel1, LOW);
            digitalWrite(_ch_sel2, LOW);
        }
    }
}

void TempProbeClass::selectChannel(uint8_t channel, uint8_t probeType) {
#ifdef TRY_REV2_RECOGNITION
    // check if OTP data is present AND the board is mounted on a r2 carrier
    auto info = (PortentaBoardInfo*)boardInfo();
    if (info->magic == 0xB5 && info->carrier == PMC_R2_SKU) {
        // reverse channels 0 and 2
        switch (channel) {
            case 0:
                channel = 2;
                break;
            case 2:
                channel = 0;
                break;
            default:
                break;
        }
    }
#endif
#undef TRY_REV2_RECOGNITION
    if (_current_channel != channel) {
        switch(channel) {
            case 0:
                digitalWrite(_ch_sel0, HIGH);
                digitalWrite(_ch_sel1, LOW);
                digitalWrite(_ch_sel2, LOW);
                break;
            case 1:
                digitalWrite(_ch_sel0, LOW);
                digitalWrite(_ch_sel1, HIGH);
                digitalWrite(_ch_sel2, LOW);
                break;
            case 2:
                digitalWrite(_ch_sel0, LOW);
                digitalWrite(_ch_sel1, LOW);
                digitalWrite(_ch_sel2, HIGH);
                break;
            default:
                digitalWrite(_ch_sel0, LOW);
                digitalWrite(_ch_sel1, LOW);
                digitalWrite(_ch_sel2, LOW);
                break;
        }
        uint8_t switch_delay;
        switch (probeType) {
            case PROBE_TC_K:
            case PROBE_TC_J:
            case PROBE_TC_T:
                digitalWrite(_rtd_th, LOW);
                if (_current_probe_type != probeType) {
                    MAX31855Class::setTCType(probeType);
                }
                switch_delay = 150;
                break;

            case PROBE_RTD_2W:
            case PROBE_RTD_3W:
                digitalWrite(_rtd_th, HIGH);
                if (_current_probe_type != probeType) {
                    MAX31865Class::setRTDType(probeType);
                }
                switch_delay = 75;
                break;

            default:
                switch_delay = 150;
                break;
        }
        delay(switch_delay);
        _current_channel = channel;
        _current_probe_type = probeType;
    }
}

TempProbeClass MachineControl_TempProbe;
/**** END OF FILE ****/
