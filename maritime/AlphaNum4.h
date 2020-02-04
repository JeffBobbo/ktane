/*************************************************** 
  Backpack library for 4 digit 14 segment display
  Adapted from Adafruit's library to run with SoftWire 
 ****************************************************/
#ifndef ALPHANUM_H_INCLUDE
#define ALPHANUM_H_INCLUDE

#include <SoftWire.h>

#define LED_ON 1
#define LED_OFF 0

#define LED_RED 1
#define LED_YELLOW 2
#define LED_GREEN 3
 


#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

#define HT16K33_CMD_BRIGHTNESS 0xE0

#define SEVENSEG_DIGITS 5


class AlphaNum4
{
public:
  AlphaNum4(const uint8_t sda, const uint8_t scl);

  void begin(uint8_t _addr);
  void setBrightness(uint8_t b);
  void blinkRate(uint8_t b);
  void writeDisplay();
  void clear();

  uint16_t displaybuffer[8]; 

  void init(uint8_t a);

  void writeDigitRaw(uint8_t n, uint16_t bitmask);
  void writeDigitAscii(uint8_t n, uint8_t ascii, bool dot = false);

private:
  const uint8_t pin_sda;
  const uint8_t pin_scl;
  uint8_t i2c_addr;
  SoftWire sw;
  uint8_t rxbuf[32];
  uint8_t txbuf[32];
};

#endif
