#include <Arduino.h>
#include <Wire.h>

#include <TM1637Display.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "shared/address.h"
#include "shared/config.h"
#include "shared/message.h"

// analog pins
const uint8_t PIN_SEED = A0; // for seeding rng

// digital pins
const uint8_t PIN_CLK = 3;
const uint8_t PIN_DIO = 2;

const uint8_t PIN_START = 4;
const uint8_t PIN_RESET = 5;

TM1637Display countdown(PIN_CLK, PIN_DIO);

//const uint8_t PIN_CLOCK = 11;
//const uint8_t PIN_LATCH = 10;
//const uint8_t PIN_DATA  = 9;

//const uint8_t PIN_STRIKE_0 = 8;
//const uint8_t PIN_STRIKE_1 = 7;
//const uint8_t PIN_STRIKE_2 = 6;

// const uint8_t PIN_DIGIT_3 = 5;
// const uint8_t PIN_DIGIT_2 = 2;
// const uint8_t PIN_DIGIT_1 = 3;
// const uint8_t PIN_DIGIT_0 = 4;
// const uint8_t PIN_DIGITS[] = {
//   PIN_DIGIT_3,
//   PIN_DIGIT_2,
//   PIN_DIGIT_1,
//   PIN_DIGIT_0
// };
 const uint8_t PIN_BUZZER = 11;
 const uint8_t PIN_RELAY = 6;

//const uint8_t SEG_TABLE[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

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
  const uint32_t remaining = (start + TIME_ALLOWED) - (end ? end : now);

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



  if (state != BaseState::READY)
  {
    display.setTextSize(2);
    display.getTextBounds(version, 0, 0, &x1, &y1, &x2, &y2);
    display.setCursor((128 - x2) / 2, (32 - y2) / 2);
    display.print(version);

    display_timeRemaining();

    digitalWrite(PIN_BUZZER, lastStrike + STRIKE_TIME > millis());
  }
  else
  {
    display_state();
  }

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

void indicate()
{
  Indicators indicators;
  indicators.numerical = nindicator;
  indicators.binary = bindicator;
  indicators.strikes = strikes;
  indicators.state = state;

  Wire.beginTransmission(address::INDICATORS);
  Wire.write(reinterpret_cast<uint8_t*>(&indicators), sizeof(indicators));
  Wire.endTransmission();
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

  bindicator = random(15);
  nindicator = random(9);

  // scan for modules on the bus
  // delay to ensure they're all powered up
  delay(250);
  scan();

  // write to the screen once, since loop will block writes until all modules are ready
  screen();
  indicate();

  // transmit version info to all
  Message vmsg(OpCode::VERSION);
  strncpy(reinterpret_cast<char*>(vmsg.data), version, VERSION_LENGTH+1);
  broadcast(vmsg);
}

void setup()
{
  countdown.setBrightness(0x01);

  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
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
    indicate();
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
