#ifndef _MAX31855_H_
#define _MAX31855_H_

#include <Arduino.h>
#include <mbed.h>
#include <SPI.h>
#include "pins_mc.h"

#define PROBE_TC_K 0
#define PROBE_TC_J 1
#define PROBE_TC_T 2

#define PROBE_K PROBE_TC_K
#define PROBE_J PROBE_TC_J
#define PROBE_T PROBE_TC_T

#define TC_FAULT_NONE      (0x00) // Disable all fault checks
#define TC_FAULT_OPEN      (0x01) // Enable open circuit fault check
#define TC_FAULT_SHORT_GND (0x02) // Enable short to GND fault check
#define TC_FAULT_SHORT_VCC (0x04) // Enable short to VCC fault check
#define TC_FAULT_ALL       (0x07) // Enable all fault checks

class MAX31855Class {
public:
    MAX31855Class(PinName cs = MC_TC_CS_PIN, SPIClass& spi = SPI);

    bool begin();
    void end();

    float readTCTemperature();
    float readReferenceTemperature();
    void setColdOffset(float offset);

    void setFaultChecks(uint8_t faults);
    uint8_t getLastFault();

    void setTCType(uint8_t type);

private:
    float _coldOffset;
    uint8_t _faultMask = TC_FAULT_ALL;
    uint8_t _lastFault = 0;
    uint8_t _current_probe_type;
    PinName _cs;
    SPIClass* _spi;
    SPISettings _spiSettings;

    // NIST coefficient tables
    static constexpr double Jm210_760[]    = {  0.000000000000E+00,  0.503811878150E-01, 0.304758369300E-04, -0.856810657200E-07,  0.132281952950E-09, -0.170529583370E-12,  0.209480906970E-15, -0.125383953360E-18,  0.156317256970E-22 };
    static constexpr double J760_1200[]    = {  0.296456256810E+03, -0.149761277860E+01, 0.317871039240E-02, -0.318476867010E-05,  0.157208190040E-08, -0.306913690560E-12 };

    static constexpr double Km270_0[]      = {  0.000000000000E+00,  0.394501280250E-01, 0.236223735980E-04, -0.328589067840E-06, -0.499048287770E-08, -0.675090591730E-10, -0.574103274280E-12, -0.310888728940E-14, -0.104516093650E-16, -0.198892668780E-19, -0.163226974860E-22 };
    static constexpr double K0_1372[]      = { -0.176004136860E-01,  0.389212049750E-01, 0.185587700320E-04, -0.994575928740E-07,  0.318409457190E-09, -0.560728448890E-12,  0.560750590590E-15, -0.320207200030E-18,  0.971511471520E-22, -0.121047212750E-25 };

    static constexpr double Tm270_0[]      = {  0.000000000000E+00,  0.387481063640E-01, 0.441944343470E-04,  0.118443231050E-06,  0.200329735540E-07,  0.901380195590E-09,  0.226511565930E-10,  0.360711542050E-12,  0.384939398830E-14,  0.282135219250E-16,  0.142515947790E-18, 0.487686622860E-21, 0.107955392700E-23, 0.139450270620E-26, 0.797951539270E-30 };
    static constexpr double T0_400[]       = {  0.000000000000E+00,  0.387481063640E-01, 0.332922278800E-04,  0.206182434040E-06, -0.218822568460E-08,  0.109968809280E-10, -0.308157587720E-13,  0.454791352900E-16, -0.275129016730E-19 };

    // NIST inverse coefficient tables
    static constexpr double InvJ_neg[]     = {  0.0000000E+00,  1.9528268E+01,  -1.2286185E+00, -1.0752178E+00,  -5.9086933E-01, -1.7256713E-01, -2.8131513E-02, -2.3963370E-03, -8.3823321E-05};
    static constexpr double InvJ0_760[]    = {  0.000000E+00,   1.978425E+01,   -2.001204E-01,   1.036969E-02,   -2.549687E-04,   3.585153E-06,  -5.344285E-08,   5.099890E-10 };
    static constexpr double InvJ760_1200[] = { -3.11358187E+03, 3.00543684E+02, -9.94773230E+00, 1.70276630E-01, -1.43033468E-03, 4.73886084E-06 };

    static constexpr double InvK_neg[]     = {  0.0000000E+00,  2.5173462E+01,   1.1662878E+00,  1.0833638E+00,   8.9773540E-01,  3.7342377E-01,  8.6632643E-02,  1.0450598E-02,  5.1920577E-04 };
    static constexpr double InvK0_500[]    = {  0.000000E+00,   2.508355E+01,    7.860106E-02,  -2.503131E-01,    8.315270E-02,  -1.228034E-02,   9.804036E-04,  -4.413030E-05,   1.057734E-06, -1.052755E-08 };
    static constexpr double InvK500_1372[] = { -1.318058E+02,   4.830222E+01,   -1.646031E+00,   5.464731E-02,   -9.650715E-04,   8.802193E-06,  -3.110810E-08 };

    static constexpr double InvT_m200_0[]  = {  0.0000000E+00,  2.5949192E+01,  -2.1316967E-01,  7.9018692E-01,   4.2527777E-01,  1.3304473E-01,  2.0241446E-02,  1.2668171E-03 };
    static constexpr double InvT0_400[]    = {  0.000000E+00,   2.592800E+01,   -7.602961E-01,   4.637791E-02,   -2.165394E-03,   6.048144E-05,  -7.293422E-07,   0.000000E+00 };

    typedef struct {
        int size;
        double max;
        const double *coeffs;
    } coefftable;

    static constexpr coefftable CoeffJ[] = {
        {0, -210.0f, NULL},
        {sizeof(Jm210_760) / sizeof(double), 760.0f, &Jm210_760[0]},
        {sizeof(J760_1200) / sizeof(double), 1200.0f, &J760_1200[0]}
    };

    static constexpr coefftable CoeffK[] = {
        {0, -270.0f, NULL},
        {sizeof(Km270_0) / sizeof(double), 0.0f, &Km270_0[0]},
        {sizeof(K0_1372) / sizeof(double), 1372.0f, &K0_1372[0]}
    };

    static constexpr coefftable CoeffT []= {
        {0,-270, NULL},
        {sizeof(Tm270_0) / sizeof(double), 0.0f, &Tm270_0[0]},
        {sizeof(T0_400) / sizeof(double), 400.0f, &T0_400[0]}
    };

    static constexpr coefftable InvCoeffJ[] = {
        {0, -0.895f, NULL},
        {sizeof(InvJ_neg) / sizeof(double), 0.0f, &InvJ_neg[0]},
        {sizeof(InvJ0_760) / sizeof(double), 42.919f, &InvJ0_760[0]},
        {sizeof(InvJ760_1200) / sizeof(double), 69.533f, &InvJ760_1200[0]}
    };

    static constexpr coefftable InvCoeffK[] = {
        {0, -5.891f, NULL},
        {sizeof(InvK_neg) / sizeof(double), 0.0f, &InvK_neg[0]},
        {sizeof(InvK0_500) / sizeof(double), 20.644f, &InvK0_500[0]},
        {sizeof(InvK500_1372) / sizeof(double), 54.886f, &InvK500_1372[0]},
    };

    static constexpr coefftable InvCoeffT []= {
        {0, -5.603f, NULL},
        {sizeof(InvT_m200_0) / sizeof(double), 0.0f, &InvT_m200_0[0]},
        {sizeof(InvT0_400) / sizeof(double), 20.872f, &InvT0_400[0]},
    };

    uint32_t readSensor();
    double mvtoTemp(double voltage);
    double tempTomv(double temp);
    double polynomial(double value, int tableEntries, coefftable const (*table));
};

#endif
