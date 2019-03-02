#include <Arduino.h>

// outputs
const uint8_t LED_R = 5;
const uint8_t LED_G = 4;
const uint8_t LED_Y = 3;
const uint8_t LED_B = 2;
const uint8_t LEDS[] = {LED_R, LED_G, LED_Y, LED_B};

// inputs
const uint8_t PIN_R = 9;
const uint8_t PIN_G = 8;
const uint8_t PIN_Y = 7;
const uint8_t PIN_B = 6;
const uint8_t PINS[] = {PIN_R, PIN_G, PIN_Y, PIN_B};

const uint8_t PIN_SEED = 0;

const uint8_t MAX_CODE_LENGTH = 4;
uint8_t code[MAX_CODE_LENGTH] = {0};
uint8_t current = 0; // current button we're pressing for
uint8_t progress = 0; // our total progress (i.e., how many to flash)

const uint32_t FLASH_TIME = 500;
const uint32_t PAUSE_TIME = 500;
const uint32_t REPEAT_TIME = 2500;

enum State
{
  INITIAL,
  SHOWING,
  WAITING,
  COMPLETE
};

State state;
uint32_t start;

void generateCode()
{
  for (uint8_t i = 0; i < MAX_CODE_LENGTH; ++i)
    code[i] = random(4);
}

void setup()
{
  Serial.begin(9600);

  // setup pins
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_B, OUTPUT);

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

  // generate our random code
  generateCode();

  Serial.print(code[0]);
  Serial.print(code[1]);
  Serial.print(code[2]);
  Serial.println(code[3]);

  state = INITIAL;
  start = millis();
}

void loop()
{
  switch (state)
  {
    case INITIAL:
      digitalWrite(LEDS[code[0]], (millis() % (FLASH_TIME + REPEAT_TIME)) < FLASH_TIME);
    break;
    case SHOWING:
    {
      // turn the LEDs off
      for (uint8_t i = 0; i < 4; ++i)
        digitalWrite(LEDS[i], 0);

      const uint32_t period = PAUSE_TIME + (progress+1) * (FLASH_TIME + PAUSE_TIME);
      const uint32_t now = millis() - start;
      if (now > period) // if we're done, go to WAITING
      {
        state = WAITING;
        return;
      }

      // calculate which one to show
      uint8_t show = MAX_CODE_LENGTH;
      if (now < PAUSE_TIME)
        show = 4;
      else if (now < PAUSE_TIME + FLASH_TIME)
        show = 0;
      else if (now > FLASH_TIME + 2 * PAUSE_TIME &&
               now < FLASH_TIME + 2 * PAUSE_TIME + FLASH_TIME)
        show = 1;
      else if (now > 2 * (FLASH_TIME + PAUSE_TIME) + PAUSE_TIME &&
               now < 3 * (FLASH_TIME + PAUSE_TIME))
        show = 2;
      else if (now > 3 * (FLASH_TIME + PAUSE_TIME) + PAUSE_TIME &&
               now < 4 * (FLASH_TIME + PAUSE_TIME))
        show = 3;

      if (show < MAX_CODE_LENGTH)
        digitalWrite(LEDS[code[show]], 1);
    }
    break;
    case WAITING:
      // just waiting for user input, nothing to do
    break;
    case COMPLETE:
      for (uint8_t i = 0; i < 4; ++i)
        digitalWrite(LEDS[i], 0);
    return;
  }

  for (uint8_t i = 0; i < 4; ++i)
  {
    if (digitalRead(PINS[i]))
    {
      while (digitalRead(PINS[i]));
      if (code[current] != i)
      {
        Serial.println("STRIKE");
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
        }
        else
        {
          ++current;
        }
      }
      state = SHOWING;
      Serial.print("progress: ");
      Serial.println(progress);
      Serial.print("current: ");
      Serial.println(current);
    }
  }
  if (progress == MAX_CODE_LENGTH)
  {
    Serial.println("DISARMED");
    state = COMPLETE;
  }
}
