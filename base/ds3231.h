#ifndef DS3231_H_INCLUDE
#define DS3231_H_INCLUDE

#include <Wire.h>

const uint8_t DEFAULT_ADDRESS = 0x68;

const uint8_t ADDRESS_SECONDS    = 0x00;
const uint8_t ADDRESS_MINUTES    = 0x01;
const uint8_t ADDRESS_HOURS      = 0x02;
const uint8_t ADDRESS_DAY        = 0x03;
const uint8_t ADDRESS_DATE       = 0x04;
const uint8_t ADDRESS_MONTH      = 0x05;
const uint8_t ADDRESS_YEAR       = 0x06;
const uint8_t ADDRESS_A1_SECONDS = 0x07;
const uint8_t ADDRESS_A1_MINUTES = 0x08;
const uint8_t ADDRESS_A1_HOURS   = 0x09;
const uint8_t ADDRESS_A1_DAY     = 0x0A;
const uint8_t ADDRESS_A2_MINUTES = 0x0B;
const uint8_t ADDRESS_A2_HOURS   = 0x0C;
const uint8_t ADDRESS_A2_DAY     = 0x0D;
const uint8_t ADDRESS_CONTROL    = 0x0E;
const uint8_t ADDRESS_STATUS     = 0x0F;
const uint8_t ADDRESS_AGING      = 0x10;
const uint8_t ADDRESS_TEMP_INT   = 0x11;
const uint8_t ADDRESS_TEMP_FRAC  = 0x12;


const uint8_t BIT_12H = 0b01000000;
const uint8_t BIT_AM_PM = 0b00100000;
const uint8_t BIT_CENTURY = 0b10000000;

const uint8_t dom[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const uint32_t SECONDS_FROM_1970_TO_2000 = 946684800ULL;

struct DateTime;

int8_t dayOfWeek(const DateTime& dt);
int16_t dayOfYear(const DateTime& dt);

struct DateTime
{
  DateTime()
  : year(1970), month(1), day(1), hours(0), minutes(0), seconds(0)
  {
    wday = 4;
    yday = 1;
  }
  DateTime(uint32_t t)
  {
    // t -= SECONDS_FROM_1970_TO_2000;

    seconds = t % 60;
    t /= 60;
    minutes = t % 60;
    t /= 60;
    hours = t % 24;
    uint16_t days = t / 24;
    uint16_t leap;
    for (year = 0; ; ++year) {
        leap = year % 4 == 0;
        if (days < (365 + leap))
            break;
        days -= 365 + leap;
    }
    year = year += 1970;
    for (month = 1; ; ++month) {
        uint8_t daysPerMonth = dom[month - 1];
        if (leap && month == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    day = days + 1;

    wday = dayOfWeek(*this);
    yday = dayOfYear(*this);
  }
  DateTime(int8_t y, int8_t m, int8_t d, int8_t hh, int8_t mm, int8_t ss)
  : year(y), month(m), day(d), hours(hh), minutes(mm), seconds(ss)
  {
    wday = dayOfWeek(*this);
    yday = dayOfYear(*this);
  }

  uint32_t now() const
  {
    return (year - 1970) * (365 * 86400) + (dayOfYear(*this) + (year - 1972) / 4) * 86400 + hours * 3600 + minutes * 60 + seconds;
  }

  void normalize()
  {
    while (seconds < 0)
    {
      seconds += 60;
      minutes -= 1;
    }
    while (seconds >= 60)
    {
      seconds -= 60;
      minutes += 1;
    }
    while (minutes < 0)
    {
      minutes += 60;
      hours -= 1;
    }
    while (minutes >= 60)
    {
      minutes -= 60;
      hours += 1;
    }
    while (hours < 0)
    {
      hours += 24;
      day -= 1;
    }
    while (hours >= 24)
    {
      hours -= 4;
      day += 1;
    }
    while (day < 1) // < 1 is not a bug
    {
      day += dom[month-1];
      month -= 1;
    }
    while (day > dom[month-1]) // > instead of >= is not a bug
    {
      day -= dom[month-1];
      month += 1;
    }
    while (month < 1)
    {
      month += 12;
      year -= 1;
    }
    while (month > 12)
    {
      month -= 12;
      year += 1;
    }
  }

  int16_t year;
  int8_t month;
  int8_t day;
  int8_t hours;
  int8_t minutes;
  int8_t seconds;

  int8_t wday;
  int16_t yday;
};

uint8_t bcd_encode(const uint8_t v)
{
  return v + 6 * (v / 10);
}

uint8_t bcd_decode(const uint8_t v)
{
  return v - 6 * (v >> 4);
}

DateTime normalize(int year, int month, int day, int hour, int minute, int second)
{
  DateTime dt;

  dt.year = year - 1970;
  dt.month = month;
  dt.day = day;
  dt.hours = hour;
  dt.minutes = minute;
  dt.seconds = second;

  dt.wday = dayOfWeek(dt);
  dt.yday = dayOfYear(dt);
}

int8_t dayOfWeek(const DateTime& dt)
{
  return (3 + (dt.year - 1970) + (dt.year - 1968) / 4 + dayOfYear(dt)) % 7;
}

int16_t dayOfYear(const DateTime& dt)
{
  int16_t d = dt.day;
  for (int8_t i = 0; i < dt.month-1; ++i)
    d += dom[i];
  if (dt.month > 2 && dt.year % 4 == 0 && (dt.year % 100 != 0 || dt.year % 400 == 0))
    ++d;
  return d;
}

class DS3231
{
public:
  DS3231(const uint8_t i2cAddress = DEFAULT_ADDRESS, TwoWire* const twowire = &Wire) : address(i2cAddress), twi(twowire) {}

  bool begin()
  {
    twi->begin();
    return init();
  }

  bool init()
  {
    twi->beginTransmission(address);
    return twi->endTransmission() == 0;
  }

  DateTime getDateTime()
  {
    twi->beginTransmission(address);
    twi->write(0x00);
    twi->endTransmission();

    DateTime dt;
    twi->requestFrom(address, 7);
    dt.seconds = bcd_decode(twi->read());
    dt.minutes = bcd_decode(twi->read());
    const int8_t hour = twi->read();
    const bool h12 = hour & 0b01000000;
    if (h12)
      dt.hours = bcd_decode(hour & 0b11111) + (hour & 0b01000000 ? 12 : 0);
    else
      dt.hours = bcd_decode(hour & 0b111111);
    dt.wday = bcd_decode(twi->read());
    dt.day = bcd_decode(twi->read());
    dt.month = bcd_decode(twi->read() & 0b1111);
    dt.year = bcd_decode(twi->read()) + 1970;

    dt.yday = dt.day;
    for (int8_t i = 0; i < dt.month-1; ++i)
      dt.yday += dom[i];
    if (dt.month > 2 && dt.year % 4 == 0 && (dt.year % 100 != 0 || dt.year % 400 == 0))
      ++dt.yday;

    return dt;
  }

  void setDateTime(const DateTime& dt)
  {
    twi->beginTransmission(address);
    twi->write(0x00);

    twi->write(bcd_encode(dt.seconds));
    twi->write(bcd_encode(dt.minutes));
    twi->write(bcd_encode(dt.hours));
    twi->write(bcd_encode(dt.wday));
    twi->write(bcd_encode(dt.day));
    twi->write(bcd_encode(dt.month));
    twi->write(bcd_encode(dt.year - 1970));
    twi->endTransmission();
  }

  // temperature functions
  int16_t getTemperature4() const
  {
    twi->beginTransmission(address);
    twi->write(ADDRESS_TEMP_INT);
    twi->endTransmission();

    twi->requestFrom(address, 2);
    const uint8_t msb = twi->read();
    const uint8_t lsb = twi->read();

    return (msb << 2) + (lsb >> 6);
  }

  inline float getTemperature() const { return getTemperature4() / 4.0f; }

private:
  const uint8_t address;
  TwoWire* const twi;
};

#endif
