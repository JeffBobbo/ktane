#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "shared/address.h"
#include "shared/config.h"
#include "shared/message.h"

// analog pins
const uint8_t PIN_SEED = A0; // for seeding rng

// digital pins
const uint8_t PIN_START = 5;
const uint8_t PIN_RESET = 3;

// game state, and strikes
BaseState state;
uint8_t strikes;
uint8_t MAX_STRIKES = 3;

// timing
const uint32_t TIME_ALLOWED = 3UL * 60UL * 1000UL;
uint32_t start;
uint32_t end;
uint32_t now;
uint32_t lastStrike;
uint32_t STRIKE_TIME = 1000;

Address modules[address::NUM_MODULES];
size_t moduleCount;
char version[VERSION_LENGTH+1];

Adafruit_SSD1306 display(128, 64);

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
  display.setCursor((128 - x2) / 2, 16);
  display.print(str);
}

void display_timeRemaining()
{
  const uint32_t remaining = (start + TIME_ALLOWED) - (end ? end : now);
  int16_t x1, y1;
  uint16_t x2, y2;

  String str;
  uint32_t m = (remaining / 1000) / 60;
  uint32_t s = (remaining / 1000) % 60;
  uint32_t cs = (remaining / 10) % 100;
  if (m < 10)
    str += '0';
  str += m;
  str += ':';
  if (s < 10)
    str += '0';
  str += s;
  str += '.';
  if (cs < 10)
    str += '0';
  str += cs;

  display.setTextSize(2);
  display.getTextBounds(str.c_str(), 0, 0, &x1, &y1, &x2, &y2);
  display.setCursor((128 - x2) / 2, 32);
  display.print(str.c_str());
}

void display_strikes()
{
  char str[MAX_STRIKES+1] = {0};
  for (uint8_t i = 0; i < strikes; ++i)
    str[i] = 'X';
  for (uint8_t i = strikes; i < MAX_STRIKES; ++i)
    str[i] = ' ';

  int16_t x1, y1;
  uint16_t x2, y2;

  display.setTextSize(2);
  display.getTextBounds(str, 0, 0, &x1, &y1, &x2, &y2);
  display.setCursor(128 - x2, 48);
  display.print(str);
}


size_t broadcast(uint8_t data)
{
  for (uint8_t i = 0; i < sizeof(modules); ++i)
  {
    Wire.beginTransmission(modules[i]);
    Wire.write(data);
    Wire.endTransmission();
  }
  return 1;
}

size_t broadcast(const uint8_t* const data, const size_t len)
{
  for (size_t i = 0; i < moduleCount; ++i)
  {
    Wire.beginTransmission(modules[i]);
    Wire.write(data, len);
    Wire.endTransmission();
  }
  return len;
}

template <typename T>
size_t broadcast(T& t)
{
  return broadcast(reinterpret_cast<uint8_t*>(&t), sizeof(t));
}

void scan()
{
  for (size_t i = 0; i < address::NUM_MODULES; ++i)
  {
    Wire.beginTransmission(address::modules[i]);
    if (Wire.endTransmission() == 0)
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

  display.setTextSize(2);
  display.getTextBounds(version, 0, 0, &x1, &y1, &x2, &y2);
  display.setCursor((128 - x2) / 2, 0);
  display.print(version);

  display_state();

  if (state != BaseState::READY)
  {
    display_timeRemaining();
    display_strikes();

    if (lastStrike + STRIKE_TIME > millis())
    {
      display.getTextBounds("STRIKE", 0, 0, &x1, &y1, &x2, &y2);
      display.setCursor((128 - x2) / 2, 48);
      display.print("STRIKE");
    }
  }

  display.setTextSize(2);
  display.setCursor(0, 48);
  display.print(moduleCount);

  display.display();
}

void generate()
{
  for (uint8_t i = 0; i < VERSION_LENGTH; ++i)
  {
    const uint8_t c = random(46);
    if (c < 26)
      version[i] = 'a' + c;
    else if (c < 46)
      version[i] = '0' + (c - 26) % 10;
    else
      version[i] = '?';
  }
  version[VERSION_LENGTH] = 0;
}

void reset()
{
  Message rmsg(OpCode::RESET);
  broadcast(rmsg);

  end = 0;
  now = 0;
  strikes = 0;
  lastStrike = 0;

  memset(modules, 0, address::NUM_MODULES);
  moduleCount = 0;
  memset(version, 0, VERSION_LENGTH+1);

  state = BaseState::INITIALISATION;
  // generate random version hash
  generate();

  // scan for modules on the bus
  // delay to ensure they're all powered up
  delay(250);
  scan();

  // write to the screen once, since loop will block writes until all modules are ready
  screen();

  // transmit version info to all
  Message vmsg(OpCode::VERSION);
  strncpy(reinterpret_cast<char*>(vmsg.data), version, VERSION_LENGTH+1);
  broadcast(vmsg);
}

void setup()
{
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
    if (start + TIME_ALLOWED < now || strikes >= MAX_STRIKES)
    {
      state = BaseState::EXPLODED;
      end = now;
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
      }
      if (address::isNeedy(modules[i]) || status.state == ModuleState::DISARMED)
        ++numDisarmed;
    }
    if (numDisarmed == moduleCount)
    {
      state = BaseState::DEFUSED;
      end = now;

      Message tmsg(OpCode::DEFUSED);
      broadcast(reinterpret_cast<uint8_t*>(&tmsg), sizeof(tmsg));
    }
  }

  if (state == BaseState::DEFUSED || state == BaseState::EXPLODED)
  {
    if (digitalRead(PIN_RESET))
    {
      reset();
      return;
    }
  }

  screen();
}
