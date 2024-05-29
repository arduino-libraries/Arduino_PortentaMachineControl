#include "MAX31865.h"

MAX31865Class::MAX31865Class(PinName cs, SPIClass& spi) : _cs(cs), _spi(&spi), _spiSettings(1000000, MSBFIRST, SPI_MODE1) {
}

bool MAX31865Class::begin() {
    _spi->begin();

    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);

    // disable bias
    writeByte(MAX31856_CONFIG_REG, readByte(MAX31856_CONFIG_REG) & MAX31856_CONFIG_BIAS_MASK);

    // disable auto convert mode
    writeByte(MAX31856_CONFIG_REG, readByte(MAX31856_CONFIG_REG) & MAX31856_CONFIG_CONV_MODE_MASK);

    // clear fault
    writeByte(MAX31856_CONFIG_REG, (readByte(MAX31856_CONFIG_REG) & MAX31856_CONFIG_CLEAR_FAULT_CYCLE) | MAX31856_CONFIG_CLEAR_FAULT);

    // set filter frequency
    writeByte(MAX31856_CONFIG_REG, readByte(MAX31856_CONFIG_REG) & MAX31856_CONFIG_60_50_HZ_FILTER_MASK);

    return true;
}

bool MAX31865Class::begin(uint8_t probeType) { // Deprecate in future
    begin();

    setRTDType(probeType);

    return true;
}

void MAX31865Class::setRTDType(uint8_t probeType) {
    // sets 2 or 4 wire
    if (probeType == PROBE_RTD_3W) {
        writeByte(MAX31856_CONFIG_REG, readByte(MAX31856_CONFIG_REG) | MAX31856_CONFIG_3_WIRE);
    } else {
        writeByte(MAX31856_CONFIG_REG, readByte(MAX31856_CONFIG_REG) & MAX31856_CONFIG_WIRE_MASK);
    }
    _current_probe_type = probeType;
}

uint8_t MAX31865Class::getRTDType() {
    return _current_probe_type;
}

void MAX31865Class::clearFault(void) {
    return clearRTDFault();
}

void MAX31865Class::clearRTDFault(void) {
    writeByte(MAX31856_CONFIG_REG, (readByte(MAX31856_CONFIG_REG) & MAX31856_CONFIG_CLEAR_FAULT_CYCLE) | MAX31856_CONFIG_CLEAR_FAULT);
}

uint8_t MAX31865Class::readFault(void) {
    return readRTDFault();
}

uint8_t MAX31865Class::readRTDFault(void) {
    return readByte(MAX31856_FAULT_STATUS_REG);
}

bool MAX31865Class::getHighThresholdFault(uint8_t fault) {
    return getRTDHighThresholdFault(fault);
}

bool MAX31865Class::getRTDHighThresholdFault(uint8_t fault) {
    if (fault & MAX31865_FAULT_HIGH_THRESH) {
        return true;
    }
    return false;
}

bool MAX31865Class::getLowThresholdFault(uint8_t fault) {
    return getRTDLowThresholdFault(fault);
}

bool MAX31865Class::getRTDLowThresholdFault(uint8_t fault) {
    if (fault & MAX31865_FAULT_LOW_THRESH) {
        return true;
    }
    return false;
}

bool MAX31865Class::getLowREFINFault(uint8_t fault) {
    return getRTDLowREFINFault(fault);
}

bool MAX31865Class::getRTDLowREFINFault(uint8_t fault) {
    if (fault & MAX31865_FAULT_LOW_REFIN) {
        return true;
    }
    return false;
}

bool MAX31865Class::getHighREFINFault(uint8_t fault) {
    return getRTDHighREFINFault(fault);
}

bool MAX31865Class::getRTDHighREFINFault(uint8_t fault) {
    if (fault & MAX31865_FAULT_HIGH_REFIN) {
        return true;
    }
    return false;
}

bool MAX31865Class::getLowRTDINFault(uint8_t fault) {
    return getRTDLowRTDINFault(fault);
}

bool MAX31865Class::getRTDLowRTDINFault(uint8_t fault) {
    if (fault & MAX31865_FAULT_LOW_RTDIN) {
        return true;
    }
    return false;
}

bool MAX31865Class::getVoltageFault(uint8_t fault) {
    return getRTDVoltageFault(fault);
}

bool MAX31865Class::getRTDVoltageFault(uint8_t fault) {
    if (fault & MAX31865_FAULT_OVER_UNDER_VOLTAGE) {
        return true;
    }
    return false;
}

float MAX31865Class::convertRTDTemperature(float RTDnominal, float refResistor) {
    float Z1, Z2, Z3, Z4, Rt, temp;

    Rt = readRTD();
    Rt /= 32768;
    Rt *= refResistor;

    Z1 = -RTD_A;
    Z2 = RTD_A * RTD_A - (4 * RTD_B);
    Z3 = (4 * RTD_B) / RTDnominal;
    Z4 = 2 * RTD_B;

    temp = Z2 + (Z3 * Rt);
    temp = (sqrt(temp) + Z1) / Z4;

    if (temp >= 0)
        return temp;

    // ugh.
    Rt /= RTDnominal;
    Rt *= 100; // normalize to 100 ohm

    float rpoly = Rt;

    temp = -242.02;
    temp += 2.2228 * rpoly;
    rpoly *= Rt; // square
    temp += 2.5859e-3 * rpoly;
    rpoly *= Rt; // ^3
    temp -= 4.8260e-6 * rpoly;
    rpoly *= Rt; // ^4
    temp -= 2.8183e-8 * rpoly;
    rpoly *= Rt; // ^5
    temp += 1.5243e-10 * rpoly;

    return temp;
}

uint32_t MAX31865Class::readRTD() {
    // clear fault
    writeByte(MAX31856_CONFIG_REG, (readByte(MAX31856_CONFIG_REG) & MAX31856_CONFIG_CLEAR_FAULT_CYCLE) | MAX31856_CONFIG_CLEAR_FAULT);

    // enable bias
    writeByte(MAX31856_CONFIG_REG, readByte(MAX31856_CONFIG_REG) | MAX31856_CONFIG_BIAS_ON);
    delay(10);

    // one shot config and make readings change with readByte
    writeByte(MAX31856_CONFIG_REG, readByte(MAX31856_CONFIG_REG) | MAX31856_CONFIG_ONE_SHOT);
    delay(65);

    //reading word
    uint16_t read = readWord(MAX31856_RTD_MSB_REG);
    read = read >> 1;

    // disable bias
    writeByte(MAX31856_CONFIG_REG, readByte(MAX31856_CONFIG_REG) & MAX31856_CONFIG_BIAS_MASK);

    return read;
}

uint8_t MAX31865Class::readByte(uint8_t addr) {
    addr &= 0x7F;
    uint8_t read = 0;

    digitalWrite(_cs, LOW);

    _spi->beginTransaction(_spiSettings);
    _spi->transfer(addr);
    _spi->transfer(&read,1);
    _spi->endTransaction();

    digitalWrite(_cs, HIGH);

    return read;
}

uint16_t MAX31865Class::readWord(uint8_t addr) {
    uint16_t read = 0x00;

    digitalWrite(_cs, LOW);

    _spi->beginTransaction(_spiSettings);
    _spi->transfer(addr);
    for (int i = 0; i < 2; i++) {
        read = read << 8;
        read |= _spi->transfer(0);
    }
    _spi->endTransaction();

    digitalWrite(_cs, HIGH);

    return read;
}

void MAX31865Class::writeByte(uint8_t addr, uint8_t data) {
    addr |= 0x80; // make sure top bit is set
    uint8_t buffer[2] = {addr, data};

    digitalWrite(_cs, LOW);

    _spi->beginTransaction(_spiSettings);
    _spi->transfer(buffer,2);
    _spi->endTransaction();

    digitalWrite(_cs, HIGH);
}
