#ifndef _PCF8563T_H_INCLUDED
#define _PCF8563T_H_INCLUDED

#include "Arduino.h"
#include "mbed.h"
#include "time.h"
#include "mbed_mktime.h"
#include "Wire.h"
#define RTC_INT PB_9
class PCF8563TClass {

public:
  PCF8563TClass();
  bool begin();
  void setYear(uint8_t years);
  void setMonth(uint8_t months);
  void setDay(uint8_t days);
  void setHours(uint8_t hours);
  void setMinutes(uint8_t minutes);
  void setSeconds(uint8_t seconds);

  uint8_t getYear();
  uint8_t getMonth();
  uint8_t getDay();
  uint8_t getHours();
  uint8_t getMinutes();
  uint8_t getSeconds();

  void setEpoch();
  void setEpoch(uint8_t years, uint8_t months, uint8_t days, uint8_t hours, uint8_t minutes, uint8_t seconds);
  void setEpoch(time_t seconds);
  time_t getEpoch();

void enableAlarm();
void disableAlarm();
void clearAlarm();
void setMinuteAlarm(uint8_t minutes);
void disableMinuteAlarm();
void setHourAlarm(uint8_t hours);
void disableHourAlarm();
void setDayAlarm(uint8_t days);
void disableDayAlarm();


private:
  void writeByte(uint8_t regAddres, uint8_t data);
  uint8_t readByte(uint8_t regAddres);
};

#endif
