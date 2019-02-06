#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Encoder.h>

const uint8_t NUM_STEPS = 20;

const uint8_t PIN_DT = 3;
const uint8_t PIN_CLK = 2;
const uint8_t PIN_BUTTON = 10;

const uint8_t CODE_LENGTH = 3;
uint8_t code[CODE_LENGTH] = {17, 4, 10};

Encoder encoder(2, 3);

Adafruit_SSD1306 display(128, 32);

enum State
{
  INITIAL,
  CODE_INPUT,
  COMPLETE
} state;

uint8_t index = 0;

void setup()
{
  pinMode(PIN_DT, INPUT);
  pinMode(PIN_CLK, INPUT);
  pinMode(PIN_BUTTON, INPUT);

  digitalWrite(PIN_BUTTON, 1); // turn on internal pullup

  state = INITIAL;

  Wire.begin();

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
}

void screen()
{
  display.clearDisplay();

  switch (state)
  {
    case INITIAL:
      display.setCursor(0, 8);
      display.setTextSize(2);
      display.print("CALIBRATE");
    break;
    case CODE_INPUT:
      display.setCursor(0, 0);
      display.setTextSize(4);
      display.print(encoder.read() / 4);
    break;
    case COMPLETE:
      display.setCursor(0, 8);
      display.setTextSize(2);
      display.print("DISARMED");
    break;
  }

  display.display();
}

void loop()
{
  encoder.write((encoder.read() + NUM_STEPS * 4) % (NUM_STEPS * 4));
  if (!digitalRead(PIN_BUTTON))
  {
    while (!digitalRead(PIN_BUTTON));
    if (state == INITIAL)
    {
      encoder.write(0);
      state = CODE_INPUT;
      return;
    }

    if (code[index] != encoder.read() / 4)
    {
    }
    else
    {
      ++index;
      if (index >= CODE_LENGTH)
        state = COMPLETE;
    }
  }

  screen();
}