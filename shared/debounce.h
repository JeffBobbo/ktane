#ifndef DEBOUNCE_H_INCLUDE
#define DEBOUNCE_H_INCLUDE

#include <Arduino.h>

class Debounce
{
public:
  Debounce(const uint8_t p, const bool low = false) : pin(p), active_low(low), state(0) {}

  void init()
  {
    pinMode(pin, INPUT);
  }

  inline uint8_t getPin() const { return pin; }

  void update() {
    state = state << 1 | (active_low ? !digitalRead(pin) : digitalRead(pin));
  }

  bool is_pressed()
  {
    const uint8_t MASK = 0b11000111;
    if ((state & MASK) == 0b00000111)
    {
      state = 0b11111111;
      return true;
    }
    return false;
  }

  bool is_released()
  {
    const uint8_t MASK = 0b11000111;
    if ((state & MASK) == 0b11000000)
    {
      state = 0b00000000;
      return true;
    }
    return false;
  }

  uint8_t is_down() const
  {
    return state == 0b11111111;
  }
  uint8_t is_up() const
  {
    return state == 0b00000000;
  }

private:
  const uint8_t pin;
  const bool active_low;
  uint8_t state;
};


class Debounce16
{
public:
  Debounce16(const uint8_t p, const bool low = false) : pin(p), active_low(low), state(0) {}

  void init()
  {
    pinMode(pin, INPUT);
  }

  inline uint8_t getPin() const { return pin; }

  void update() {
    state = state << 1 | (active_low ? !digitalRead(pin) : digitalRead(pin));
  }

  bool is_pressed()
  {
    const uint16_t MASK = 0b1100000000011111;
    if ((state & MASK) == 0b0000000000011111)
    {
      state = 0b1111111111111111;
      return true;
    }
    return false;
  }

  bool is_released()
  {
    const uint16_t MASK = 0b1100000000011111;
    if ((state & MASK) == 0b1111000000000000)
    {
      state = 0b0000000000000000;
      return true;
    }
    return false;
  }

  bool is_down() const
  {
    return state == 0b1111111111111111;
  }
  bool is_up() const
  {
    return state == 0b0000000000000000;
  }

private:
  const uint8_t pin;
  const bool active_low;
  uint16_t state;
};

#endif
