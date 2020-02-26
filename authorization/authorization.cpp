#include "shared/module.h"
#include "shared/util.h"

#include <MFRC522.h>
#include <SPI.h>


const Address addr = address::AUTHORIZATION;
const uint8_t PIN_DISARM_LED = 3;

const uint8_t PIN_ERROR_LED = 2;

const uint8_t PIN_RESET = 9;
const uint8_t PIN_SELECT = 10;

const uint8_t PSK[8] = {0x28, 0x54, 0x8E, 0xD1, 0x39, 0x32, 0xCA, 0x58};

uint32_t lastError = 0;

MFRC522 mfrc522(PIN_SELECT, PIN_RESET);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

uint8_t user;

void initialise()
{
  Serial.begin(9600);

  pinMode(PIN_ERROR_LED, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  // Prepare the default key, both A and B
  for (size_t i = 0; i < 6; ++i)
    key.keyByte[i] = 0xFF;
  reset();
}

void reset()
{
  Serial.print("reset");
  digitalWrite(PIN_ERROR_LED, 0);
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

  // stop any communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void idle()
{
  if (lastError + 500 > millis())
  {
    digitalWrite(PIN_ERROR_LED, 1);
    return;
  }
  else
  {
    digitalWrite(PIN_ERROR_LED, 0);
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

  // setup a buffer to store retrieved data in
  uint8_t buffer[18];
  uint8_t size = sizeof(buffer);

  status = mfrc522.MIFARE_Read(BLOCK, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    error();
    return;
  }

  // check that the tag contains our pre-shared key
  for (uint8_t i = 0; i < 8; ++i)
  {
    if (PSK[i] != buffer[i])
    {
      error();
      return;
    }
  }

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
}

void detonate()
{
}
