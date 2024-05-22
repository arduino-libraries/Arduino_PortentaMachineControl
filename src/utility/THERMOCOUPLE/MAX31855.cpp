#include "MAX31855.h"

const double MAX31855Class::Jm210_760[];
const double MAX31855Class::J760_1200[];
const double MAX31855Class::Km270_0[];
const double MAX31855Class::K0_1372[];

const double MAX31855Class::InvJ_neg[];
const double MAX31855Class::InvJ0_760[];
const double MAX31855Class::InvJ760_1200[];

const double MAX31855Class::InvK_neg[];
const double MAX31855Class::InvK0_500[];
const double MAX31855Class::InvK500_1372[];

const MAX31855Class::coefftable MAX31855Class::CoeffJ[];
const MAX31855Class::coefftable MAX31855Class::CoeffK[];

const MAX31855Class::coefftable MAX31855Class::InvCoeffJ[];
const MAX31855Class::coefftable MAX31855Class::InvCoeffK[];

MAX31855Class::MAX31855Class(PinName cs, SPIClass& spi) : _cs(cs), _spi(&spi), _spiSettings(4000000, MSBFIRST, SPI_MODE0), _coldOffset(2.10f) {
}

bool MAX31855Class::begin() {
    uint32_t rawword;

    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    _spi->begin();

    rawword = readSensor();
    if (rawword == 0xFFFFFF) {
        end();

        return false;
    }

    return true;
}

void MAX31855Class::end() {
    pinMode(_cs, INPUT);
    digitalWrite(_cs, LOW);
    _spi->end();
}

uint32_t MAX31855Class::readSensor() {
    uint32_t read = 0x00;

    digitalWrite(_cs, LOW);
    delayMicroseconds(1);

    _spi->beginTransaction(_spiSettings);


    for (int i = 0; i < 4; i++) {
        read <<= 8;
        read |= _spi->transfer(0);
    }

    _spi->endTransaction();

    digitalWrite(_cs, HIGH);
    return read;
}

double MAX31855Class::polynomial(double value, int tableEntries, coefftable const (*table)) {
    double output = 0;
    double valuePower = 1;
    for (int i = 0; i < tableEntries; i++) {
        if (value < table[i].max) {
            if (table[i].size == 0) {
                return NAN;
            } else {
                output = 0;
                for (int j = 0; j < table[i].size; j++) {
                    output += valuePower*table[i].coeffs[j];
                    valuePower *= value;
                }
                return output;
            }
        }
    }
    return NAN;
}

double MAX31855Class::coldTempTomv(double temp) {
    coefftable const (*table);
    int tableEntries;
    double voltage;

    switch (_current_probe_type) {
        case PROBE_TC_J:
            table = CoeffJ;
            tableEntries = sizeof(CoeffJ) / sizeof(coefftable);
        break;
        case PROBE_TC_K:
            table = CoeffK;
            tableEntries = sizeof(CoeffK) / sizeof(coefftable);
        break;
    }
    voltage = polynomial(temp, tableEntries, table);
    // special case... for K probes in temperature range 0-1372 we need
    // to add an extra term
    if (_current_probe_type == PROBE_TC_K && temp>0) {
        voltage += 0.118597600000E+00 * exp(-0.118343200000E-03 * pow(temp - 0.126968600000E+03, 2));
    }
    return voltage;
}

double MAX31855Class::mvtoTemp(double voltage) {
    coefftable const (*table);
    int tableEntries;

    switch (_current_probe_type) {
        case PROBE_TC_J:
            table = InvCoeffJ;
            tableEntries = sizeof(InvCoeffJ) / sizeof(coefftable);
        break;
        case PROBE_TC_K:
            table = InvCoeffK;
            tableEntries = sizeof(InvCoeffJ) / sizeof(coefftable);
        break;
    }
    return polynomial(voltage, tableEntries, table);
}

float MAX31855Class::readTCTemperature() {
    uint32_t rawword;
    int32_t measuredTempInt;
    int32_t measuredColdInt;
    double measuredTemp;
    double measuredCold;
    double measuredVolt;

    rawword = readSensor();

    // Check for reading error
    if (rawword & 0x7) {
        return NAN;
    }
    // The cold junction temperature is stored in the last 14 word's bits
    // whereas the ttermocouple temperature (non linearized) is in the topmost 18 bits
    // sent by the Thermocouple-to-Digital Converter

    // sign extend thermocouple value
    if (rawword & 0x80000000) {
        // Negative value, drop the lower 18 bits and explicitly extend sign bits.
        measuredTempInt = 0xFFFC0000 | ((rawword >> 18) & 0x00003FFFF);
    } else {
        // Positive value, just drop the lower 18 bits.
        measuredTempInt = rawword >> 18;
    }

    // convert it to degrees
    measuredTemp = measuredTempInt * 0.25f;

    // sign extend cold junction temperature
    measuredColdInt = (rawword >> 4) & 0xfff;
    if (measuredColdInt & 0x800) {
        // Negative value, sign extend
        measuredColdInt |= 0xfffff000;
    }

    // convert it to degrees
    measuredCold = (measuredColdInt / 16.0f);
    // now the tricky part... since MAX31855K is considering a linear response
    // and is trimed for K thermocouples, we have to convert the reading back
    // to mV and then use NIST polynomial approximation to determine temperature
    // we know that reading from chip is calculated as:
    // temp = chip_temperature + thermocouple_voltage/0.041276f
    //
    // convert temperature to mV is accomplished converting the chip temperature
    // to mV using NIST polynomial and then by adding the measured voltage
    // calculated inverting the function above
    // this way we calculate the voltage we would have measured if cold junction
    // was at 0 degrees celsius

    measuredVolt = coldTempTomv(measuredCold - _coldOffset) + (measuredTemp - measuredCold) * 0.041276f;

    // finally from the cold junction compensated voltage we calculate the temperature
    // using NIST polynomial approximation for the thermocouple type we are using
    return mvtoTemp(measuredVolt);
}

float MAX31855Class::readReferenceTemperature() {
    //TODO. Actually use TC _current_probe_type and _coldOffset
    uint32_t rawword;
    float ref;

    rawword = readSensor();

    // ignore first 4 FAULT bits
    rawword >>= 4;

    // The cold junction reference temperature is stored in the first 11 word's bits
    // sent by the Thermocouple-to-Digital Converter
    rawword = rawword & 0x7FF;
    // check sign bit and convert to negative value.
    if (rawword & 0x800) {
        ref = (0xF800 | (rawword & 0x7FF)) * 0.0625;
    } else {
        // multiply for the LSB value
        ref = rawword * 0.0625f;
    }

    return ref;
}

void MAX31855Class::setColdOffset(float offset) {
    _coldOffset = offset;
}


void MAX31855Class::setTCType(uint8_t type) {
    _current_probe_type = type;
}