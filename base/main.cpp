#include <Arduino.h>
#include <Wire.h>

#include <TM1637Display.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "shared/address.h"
#include "shared/config.h"
#include "shared/message.h"

#include "settings.h"

// analog pins
const uint8_t PIN_SEED = A0; // for seeding rng

// digital pins
const uint8_t PIN_CLK = 2;
const uint8_t PIN_DIO = 3;

const uint8_t PIN_START = 4;
const uint8_t PIN_RESET = 5;

TM1637Display countdown(PIN_CLK, PIN_DIO);

const uint8_t PIN_BUZZER_STRIKE = 11;
const uint8_t PIN_BUZZER_DISARM = 13;

// game state, and strikes
BaseState state;
uint8_t strikes;

// timing
uint32_t start;
uint32_t end;
uint32_t now;
uint32_t lastStrike;
uint32_t lastDisarm;
uint32_t STRIKE_BUZZ_TIME = 300;
uint32_t DISARM_BUZZ_TIME = 200;

Address modules[address::NUM_MODULES];
size_t moduleCount;
size_t disarmed;
char serial[SERIAL_LENGTH+1];

uint8_t bindicator;
uint8_t nindicator;

Adafruit_SSD1306 display(128, 32);

void display_state()
{
  int16_t x1, y1;
  uint16_t x2, y2;

  const char* str = "ERROR";
  switch (::state)
  {
    case BaseState::INITIALISATION:
      str = "INIT";
    break;
    case BaseState::READY:
      str = "READY";
    break;
    case BaseState::ARMED:
      str = "ARMED";
    break;
    case BaseState::DEFUSED:
      str = "DEFUSED";
    break;
    case BaseState::EXPLODED:
      str = "EXPLODED";
    break;
  }

  display.getTextBounds(str, 0, 0, &x1, &y1, &x2, &y2);
  display.setCursor((128 - x2) / 2, (32 -y2) / 2);
  display.print(str);
}

void display_timeRemaining()
{
  const int32_t remaining = max((start + settings::time_allowed) - (end ? end : now), 0);

  uint32_t m = (remaining / 1000) / 60;
  uint32_t s = (remaining / 1000) % 60;

  countdown.showNumberDecEx(m * 100 + s, remaining / 500 & 1 ? 0b01000000 : 0, true);
}

size_t broadcast(const uint8_t* const data, const size_t len)
{
  for (size_t i = 0; i < moduleCount; ++i)
  {
    Wire.beginTransmission(modules[i]);
    Wire.write(data, len);
    Wire.endTransmission();
  }

  Wire.beginTransmission(address::INDICATORS);
  Wire.write(data, len);
  Wire.endTransmission();

  Wire.beginTransmission(address::LID),
  Wire.write(data, len);
  Wire.endTransmission();

  return len;
}

template <typename T>
size_t broadcast(T& t)
{
  return broadcast(reinterpret_cast<uint8_t*>(&t), sizeof(t));
}

bool detect(const uint8_t address)
{
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

void scan()
{
  for (size_t i = 0; i < address::NUM_MODULES; ++i)
  {
    if (detect(address::modules[i]))
      modules[moduleCount++] = address::modules[i];
  }
}

Status report(const Address address)
{
  Wire.requestFrom(address, sizeof(Status));

  Status rpt;
  for (size_t i = 0; i < sizeof(rpt); ++i)
    reinterpret_cast<uint8_t*>(&rpt)[i] = Wire.read();

  return rpt;
}

void screen()
{
  display.clearDisplay();

  int16_t x1, y1;
  uint16_t x2, y2;

  if (state != BaseState::INITIALISATION && state != BaseState::READY)
  {
    display.getTextBounds(serial, 0, 0, &x1, &y1, &x2, &y2);
    display.setCursor((128 - x2) / 2, (32 - y2) / 2);
    display.print(serial);

    display_timeRemaining();
  }
  else
  {
    display_state();
  }

  display.display();
}

void generate()
{
  for (uint8_t i = 0; i < SERIAL_LENGTH; ++i)
  {
    const uint8_t c = random(46);
    if (c < 26)
      serial[i] = 'a' + c;
    else if (c < 46)
      serial[i] = '0' + (c - 26) % 10;
    else
      serial[i] = '?';
  }
  serial[SERIAL_LENGTH] = 0;
}

void indicate()
{
  Message msg;
  msg.opcode = OpCode::INDICATORS;

  Indicators indicators;
  indicators.numerical = nindicator;
  indicators.binary = bindicator;
  indicators.strikes = strikes;
  indicators.modules = moduleCount;
  indicators.disarmed = disarmed;
  strncpy(reinterpret_cast<char*>(indicators.serial), serial, SERIAL_LENGTH+1);

  memcpy(msg.data, &indicators, sizeof(indicators));

  broadcast(msg);
}

void reset()
{
  Message rmsg(OpCode::RESET);
  broadcast(rmsg);

  end = 0;
  now = 0;
  start = 0;
  strikes = 0;
  lastStrike = 0;
  lastDisarm = 0;

  memset(modules, 0, address::NUM_MODULES);
  moduleCount = 0;
  // generate random serial hash
  generate();

  bindicator = random(16);
  nindicator = random(100);

  // scan for modules on the bus
  // delay to ensure they're all powered up
  delay(250);

  scan();

  // write to the screen once, since loop will block writes until all modules are ready
  screen();

  state = BaseState::INITIALISATION;

  // transmit info to all
  indicate();

  // TODO: Send configuration to all modules
}


void setup()
{
  countdown.setBrightness(0x01);

  pinMode(PIN_BUZZER_STRIKE, OUTPUT);
  pinMode(PIN_BUZZER_DISARM, OUTPUT);
  pinMode(PIN_START, INPUT);
  pinMode(PIN_RESET, INPUT);

  randomSeed(analogRead(PIN_SEED));


  Wire.begin();

  // setup display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    while (1)
    {
      digitalWrite(LED_BUILTIN, 1);
      delay(250);
      digitalWrite(LED_BUILTIN, 0);
      delay(250);
    }
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextWrap(false);
  display.setTextSize(2);
  display.dim(1);

  reset();
}

void loop()
{
  now = millis();

  if (state == BaseState::INITIALISATION)
  {
    bool allReady = false;
    while (!allReady)
    {
      allReady = true;
      for (size_t i = 0; i < moduleCount; ++i)
      {
        Status status = report(modules[i]);
        if (status.state == ModuleState::INITIALISATION)
        {
          allReady = false;
          break;
        }
        delay(10);
      }
      // small delay so we don't lock up all the slaves in their callbacks
      delay(10);
    }
    state = BaseState::READY;
    return;
  }

  if (state == BaseState::READY && digitalRead(PIN_START))
  {
    start = now;
    state = BaseState::ARMED;

    Message tmsg(OpCode::ARM);
    broadcast(reinterpret_cast<uint8_t*>(&tmsg), sizeof(tmsg));
  }


  if (state == BaseState::ARMED)
  {
    digitalWrite(PIN_BUZZER_STRIKE, lastStrike + STRIKE_BUZZ_TIME > millis());
    digitalWrite(PIN_BUZZER_DISARM, lastDisarm + DISARM_BUZZ_TIME > millis());

    if ((start + settings::time_allowed) < now || strikes >= settings::max_strikes)
    {
      state = BaseState::EXPLODED;
      end = now;
      digitalWrite(PIN_BUZZER_STRIKE, 0);
      digitalWrite(PIN_BUZZER_DISARM, 0);
      Message tmsg(OpCode::EXPLODED);
      broadcast(reinterpret_cast<uint8_t*>(&tmsg), sizeof(tmsg));
    }
    size_t numDisarmed = 0;
    for (size_t i = 0; i < moduleCount; ++i)
    {
      Status status = report(modules[i]);
      if (status.strikes)
      {
        strikes += status.strikes;
        lastStrike = now;
        indicate();
      }
      if (address::isNeedy(modules[i]) || status.state == ModuleState::DISARMED)
        ++numDisarmed;
    }
    if (disarmed != numDisarmed && numDisarmed != 0)
    {
      lastDisarm = now;
      disarmed = numDisarmed;
    }
    if (numDisarmed == moduleCount)
    {
      state = BaseState::DEFUSED;
      end = now;
      digitalWrite(PIN_BUZZER_STRIKE, 0);
      digitalWrite(PIN_BUZZER_DISARM, 0);

      Message tmsg(OpCode::DEFUSED);
      broadcast(reinterpret_cast<uint8_t*>(&tmsg), sizeof(tmsg));
    }
  }

  if (state == BaseState::READY || state == BaseState::DEFUSED || state == BaseState::EXPLODED)
  {
    if (digitalRead(PIN_RESET))
    {
      reset();
      return;
    }
  }

  screen();
}
