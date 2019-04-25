#include <Arduino.h>
#include <Wire.h>

#include "shared/address.h"
#include "shared/config.h"
#include "shared/message.h"
#include "shared/util.h"

// outputs
const uint8_t PIN_DISARMED_LED = 10;
const uint8_t LED_R = 5;
const uint8_t LED_G = 4;
const uint8_t LED_Y = 3;
const uint8_t LED_B = 2;
const uint8_t LEDS[] = {LED_R, LED_G, LED_Y, LED_B};

const uint8_t PIN_BUZZER = 12;
const uint16_t TONES[] = {587, 659, 698, 784};

// inputs
const uint8_t PIN_R = 9;
const uint8_t PIN_G = 8;
const uint8_t PIN_Y = 7;
const uint8_t PIN_B = 6;
const uint8_t PINS[] = {PIN_R, PIN_G, PIN_Y, PIN_B};

const uint8_t PIN_SEED = 0;

char version[VERSION_LENGTH+1] = {0};

const uint8_t MAX_CODE_LENGTH = 4;
uint8_t code[MAX_CODE_LENGTH] = {0};
uint8_t current; // current button we're pressing for
uint8_t progress; // our total progress (i.e., how many to flash)

const uint32_t FLASH_TIME = 500;
const uint32_t PAUSE_TIME = 500;
const uint32_t REPEAT_TIME = 5000;

enum State
{
  INITIAL,
  SHOWING,
  WAITING
};

State state;
uint32_t start;

Status status;

void generateCode()
{
  for (uint8_t i = 0; i < MAX_CODE_LENGTH; ++i)
    code[i] = random(4);
}

uint8_t mapCode(const uint8_t value)
{
  if (util::countNumbers(version) > 4)
    return (value + 1) % 4;
  return (value + 3) % 4;
}

void reset()
{
  if (status.state != ModuleState::READY)
    status.state = ModuleState::INITIALISATION;
  status.strikes = 0;

  current = 0;
  progress = 0;

  generateCode();
  state = SHOWING;
}

void receiveEvent(int count)
{
  if (count == 0)
    return;

  Message msg;
  for (int i = 0; i < count; ++i)
    reinterpret_cast<uint8_t*>(&msg)[i] = Wire.read();

  switch (msg.opcode)
  {
    case OpCode::ARM:
      status.state = ModuleState::ARMED;
      start = millis() + 2500;
    break;
    case OpCode::DEFUSED:
    case OpCode::EXPLODED:
      status.state = ModuleState::STOP;
    break;
    case OpCode::RESET:
      reset();
    break;
    case OpCode::VERSION:
      strncpy(version, reinterpret_cast<char*>(msg.data), VERSION_LENGTH+1);
      status.state = ModuleState::READY;
    default:
    break;
  }
}

void requestEvent()
{
  Wire.write(reinterpret_cast<uint8_t*>(&status), sizeof(status));
  status.strikes = 0;
}

void setup()
{
  // setup pins
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(LED_R, 0);
  digitalWrite(LED_G, 0);
  digitalWrite(LED_Y, 0);
  digitalWrite(LED_B, 0);

  pinMode(PIN_R, INPUT);
  pinMode(PIN_G, INPUT);
  pinMode(PIN_Y, INPUT);
  pinMode(PIN_B, INPUT);

  // seed
  randomSeed(analogRead(PIN_SEED));

  Wire.begin(address::SIMON);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  reset();
}

void loop()
{
  if (status.state != ModuleState::ARMED)
    return;

  digitalWrite(PIN_DISARMED_LED, 1);
  switch (state)
  {
    case SHOWING:
    {
      const uint32_t period = (progress+1) * (FLASH_TIME + PAUSE_TIME) + REPEAT_TIME;

      const uint32_t now = (millis() - start) % period;
      if (now < period - REPEAT_TIME)
      {
        const uint32_t led = now / (FLASH_TIME + PAUSE_TIME);

        if (now % (FLASH_TIME + PAUSE_TIME) < FLASH_TIME)
        {
          digitalWrite(LEDS[code[led]], 1);
          tone(PIN_BUZZER, TONES[code[led]]);
        }
        else
        {
          // turn the LEDs off
          for (uint8_t i = 0; i < 4; ++i)
            digitalWrite(LEDS[i], 0);
          noTone(PIN_BUZZER);
        }
      }
    }
    break;
    case WAITING:
      // just waiting for user input, nothing to do
    break;
  }

  for (uint8_t i = 0; i < 4; ++i)
  {
    if (digitalRead(PINS[i]))
    {
      while (digitalRead(PINS[i]));
      state = WAITING;
      if (code[current] != mapCode(i))
      {
        status.strikes = 1;
        start = millis();
        current = 0;
      }
      else
      {
        if (current == progress)
        {
          current = 0;
          ++progress;
          start = millis();
          state = SHOWING;
        }
        else
        {
          ++current;
        }
      }
    }
  }
  if (progress == MAX_CODE_LENGTH)
  {
    status.state = ModuleState::DISARMED;
    for (uint8_t i = 0; i < 4; ++i)
      digitalWrite(LEDS[i], 0);
    digitalWrite(PIN_DISARMED_LED, 0);
  }
}
