#include <Arduino.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#include <TM1637Display.h>

#include <Adafruit_GFX.h>
#include <XTronical_ST7735.h>

#include "shared/address.h"
#include "shared/config.h"
#include "shared/message.h"

#include "ds3231.h"

#include "settings.h"

// analog pins
const uint8_t PIN_SEED = A0; // for seeding rng

const uint8_t PIN_CONFIG = A2;
const uint8_t PIN_BACK = A3;

// digital pins
const uint8_t PIN_CLK = 2;
const uint8_t PIN_DIO = 3;

const uint8_t PIN_START = 4;
const uint8_t PIN_RESET = 5;

const uint8_t PIN_SD_CS = 8;

const uint8_t PIN_BUZZER_DISARM = 6;
const uint8_t PIN_BUZZER_STRIKE = 7;

const int8_t TFT_CS = 10;
const int8_t TFT_RST = -1;
const int8_t TFT_DC = 9;

// colour stuff for the TFT
constexpr uint16_t colour(const float r, const float g, const float b);
constexpr uint16_t COLOUR_BLACK   = 0x0000;
constexpr uint16_t COLOUR_BLUE    = 0x001F;
constexpr uint16_t COLOUR_RED     = 0xF800;
constexpr uint16_t COLOUR_GREEN   = 0x07E0;
constexpr uint16_t COLOUR_CYAN    = 0x07FF;
constexpr uint16_t COLOUR_MAGENTA = 0xF81F;
constexpr uint16_t COLOUR_YELLOW  = 0xFFE0;
constexpr uint16_t COLOUR_WHITE   = 0xFFFF;

BaseState state;

// timing
uint32_t start;
uint32_t end;
uint32_t now;
uint32_t lastStrike;
uint32_t lastDisarm;
uint32_t STRIKE_BUZZ_TIME = 300;
uint32_t DISARM_BUZZ_TIME = 200;

Address modules[address::NUM_MODULES];
Indicators indicators;

uint8_t& strikes = indicators.strikes;
uint8_t& moduleCount = indicators.modules;
uint8_t& disarmed = indicators.disarmed;


DS3231 rtc;
TM1637Display countdown(PIN_CLK, PIN_DIO);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

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

void renderState(const bool force = false)
{
  static BaseState lastState = BaseState::EXPLODED;

  if (!force && lastState == state)
    return;

  int16_t x1, y1;
  uint16_t x2, y2;

  const char* str = "ERROR";
  switch (state)
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

  tft.fillRect(0, 64, 128, 96, COLOUR_BLACK);
  tft.setTextColor(COLOUR_RED);
  tft.getTextBounds(str, 0, 0, &x1, &y1, &x2, &y2);
  tft.setCursor((128 - x2) / 2, 64);
  tft.print(str);
  lastState = state;
}

void displayCountdown()
{
  const int32_t remaining = max((start + settings::time_allowed) - (end ? end : now), 0);

  uint32_t m = (remaining / 1000) / 60;
  uint32_t s = (remaining / 1000) % 60;

  countdown.showNumberDecEx(m * 100 + s, remaining / 500 & 1 ? 0b01000000 : 0, true);
}

void renderSerial() {
  int16_t x1, y1;
  uint16_t x2, y2;

  tft.fillRect(0, 64, 128, 96, COLOUR_BLACK);
  tft.getTextBounds(indicators.serial, 0, 0, &x1, &y1, &x2, &y2);
  tft.setCursor((128 - x2) / 2, 64);
  tft.setTextColor(COLOUR_YELLOW);
  tft.print(indicators.serial);
}

void render(const bool force = false)
{
  static uint8_t lastSeconds = 61;
  static uint8_t lastDay = 32;

  int16_t x1, y1;
  uint16_t x2, y2;

  if (state == BaseState::INITIALISATION || state == BaseState::READY)
  {
    renderState(force);
  }

  const DateTime dt = rtc.getDateTime();
  if (force || dt.seconds != lastSeconds)
  {
    char str[8];
    sprintf(str, "%02d:%02d:%02d", dt.hours, dt.minutes, dt.seconds);

    tft.fillRect(0, 18, 128, 34, COLOUR_BLACK);
    tft.setTextColor(COLOUR_WHITE);
    tft.getTextBounds(str, 0, 0, &x1, &y1, &x2, &y2);
    tft.setTextSize(2);
    tft.setCursor((128 - x2) / 2, 18);
    tft.print(str);
    lastSeconds = dt.seconds;
  }
  if (force || dt.day != lastDay)
  {
    char str[10];
    sprintf(str, "%04d-%02d-%02d", dt.year, dt.month, dt.day);

    tft.getTextBounds(str, 0, 0, &x1, &y1, &x2, &y2);
    tft.fillRect(0, 0, 128, 16, COLOUR_BLACK);
    tft.setCursor((128 - x2) / 2, 2);
    tft.print(str);
    lastDay = dt.day;
  }
}

void generate()
{
  for (uint8_t i = 0; i < SERIAL_LENGTH; ++i)
  {
    const uint8_t c = random(46);
    if (c < 26)
      indicators.serial[i] = 'a' + c;
    else if (c < 46)
      indicators.serial[i] = '0' + (c - 26) % 10;
    else
      indicators.serial[i] = '?';
  }
  indicators.serial[SERIAL_LENGTH] = 0;
}

void indicate()
{
  Message msg;
  msg.opcode = OpCode::INDICATORS;

  memcpy(msg.data, &indicators, sizeof(indicators));

  broadcast(msg);
}

void reset()
{
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

  indicators.numerical = random(100);
  indicators.binary = random(16);

  // scan for modules on the bus
  // delay to ensure they're all powered up
  delay(250);

  scan();

  Message rmsg(OpCode::RESET);
  broadcast(rmsg);

  state = BaseState::INITIALISATION;
  // write to the screen once, since loop will block writes until all modules are ready

  displayCountdown();
  render();

  // transmit info to all
  indicate();

  // TODO: Send configuration to all modules
}


void setup()
{
  countdown.setBrightness(0x01);

  randomSeed(analogRead(PIN_SEED));

  pinMode(PIN_CONFIG, INPUT);
  pinMode(PIN_BACK, INPUT);

  pinMode(PIN_BUZZER_STRIKE, OUTPUT);
  pinMode(PIN_BUZZER_DISARM, OUTPUT);
  pinMode(PIN_START, INPUT);
  pinMode(PIN_RESET, INPUT);

  Wire.begin();

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(COLOUR_BLACK);
  tft.setCursor(16, 16);

  if (!SD.begin(PIN_SD_CS))
  {
    tft.print("SD failed");
    while (1);
  }

  if (!rtc.init())
  {
    tft.print("RTC failed");
    while (1);
  }

  // make sure storage is setup correctly
  if (!SD.exists("/logs"))
    SD.mkdir("/logs");


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

  if (state == BaseState::READY)
  {
    if (digitalRead(PIN_START))
    {
      start = now;
      state = BaseState::ARMED;

      Message tmsg(OpCode::ARM);
      broadcast(reinterpret_cast<uint8_t*>(&tmsg), sizeof(tmsg));

      renderSerial();
    }
    else if (digitalRead(PIN_CONFIG))
    {
      state = BaseState::CONFIG;
      tft.fillScreen(COLOUR_BLACK);
    }
  }

  if (state == BaseState::CONFIG)
  {
    // PIN_START is up
    // PIN_RESET is down
    if (digitalRead(PIN_START))
    {
      rtc.setDateTime(DateTime(rtc.getDateTime().now() + 1));
    }
    else if (digitalRead(PIN_RESET))
    {
      rtc.setDateTime(DateTime(rtc.getDateTime().now() - 1));
    }
    else if (digitalRead(PIN_BACK))
    {
      state = BaseState::INITIALISATION;
      render(true);
    }

    return;
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

  if (state != BaseState::INITIALISATION && state != BaseState::READY)
    displayCountdown();
  render();
}
