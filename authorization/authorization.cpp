#include "shared/module.h"
#include "shared/util.h"

#include <MFRC522.h>
#include <SPI.h>

const uint8_t PIN_ADDRESS_SELECT = A0;
const uint8_t PIN_DISARM_LED = 3;

const uint8_t PIN_LED_1 = 8;
const uint8_t PIN_LED_2 = 7;
const uint8_t PIN_LED_3 = 6;
const uint8_t PIN_LED_4 = 5;

const uint8_t PIN_BUZZER = 4;

const uint8_t PIN_RESET = 9;
const uint8_t PIN_SELECT = 10;

const uint8_t PSK[8] = {0x28, 0x54, 0x8E, 0xD1, 0x39, 0x32, 0xCA, 0x58};

uint32_t lastError = 0;
uint32_t lastBuzz = 0;

MFRC522 mfrc522(PIN_SELECT, PIN_RESET);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

uint8_t user;

void initialise()
{
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_LED_4, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  // Prepare the default key, both A and B
  for (size_t i = 0; i < 6; ++i)
    key.keyByte[i] = 0xFF;
  reset();
}

void reset()
{
  digitalWrite(PIN_LED_1, 0);
  digitalWrite(PIN_LED_2, 0);
  digitalWrite(PIN_LED_3, 0);
  digitalWrite(PIN_LED_4, 0);
}


void onIndicators()
{
  const uint8_t digit = indicators.numerical % 10;
  const uint8_t tens = (indicators.numerical / 10) % 10;
  user = indicators.strikes == 0 ? digit : tens;

  if (state == ModuleState::INITIALISATION)
    state = ModuleState::READY;
}

void arm()
{
}

const uint8_t BLOCK = 4;
const uint8_t TRAILER = 7;

void error()
{
  lastError = millis();

  tone(PIN_BUZZER, 440);

  // stop any communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void config()
{
}

void idle()
{
  if (lastError + 500 <= millis())
  {
    digitalWrite(PIN_LED_1, 0);
    digitalWrite(PIN_LED_2, 0);
    digitalWrite(PIN_LED_3, 0);
    digitalWrite(PIN_LED_4, 0);
  }
  if (lastBuzz + 500 <= millis())
  {
    noTone(PIN_BUZZER);
  }

  // if no tag present, return -- this prevents us from processing the same tag twice
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  // select the presented tag
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  // check the type, we only support MiFare 1k
  if (mfrc522.PICC_GetType(mfrc522.uid.sak) != MFRC522::PICC_TYPE_MIFARE_1K)
  {
    error();
    return;
  }
  digitalWrite(PIN_LED_1, 1);
  delay(50);

  // authenticate
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    error();
    return;
  }
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, TRAILER, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    error();
    return;
  }
  digitalWrite(PIN_LED_2, 1);
  delay(50);

  // setup a buffer to store retrieved data in
  uint8_t buffer[18];
  uint8_t size = sizeof(buffer);

  status = mfrc522.MIFARE_Read(BLOCK, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    error();
    return;
  }
  digitalWrite(PIN_LED_3, 1);
  delay(50);

  // check that the tag contains our pre-shared key
  for (uint8_t i = 0; i < 8; ++i)
  {
    if (PSK[i] != buffer[i])
    {
      error();
      return;
    }
  }
  digitalWrite(PIN_LED_4, 1);
  delay(50);

  // test
  if (buffer[15] == user)
    disarm();
  else
    strike();

  // stop communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void defuse()
{
  noTone(PIN_BUZZER);
  digitalWrite(PIN_LED_1, 0);
  digitalWrite(PIN_LED_2, 0);
  digitalWrite(PIN_LED_3, 0);
  digitalWrite(PIN_LED_4, 0);
}

void detonate()
{
  noTone(PIN_BUZZER);
}
