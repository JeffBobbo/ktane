#include <Arduino.h>
#include <Wire.h>

#include "shared/module.h"
#include "shared/util.h"

#include <MFRC522.h>
#include <SPI.h>


const Address addr = address::AUTHORIZATION;
const uint8_t PIN_DISARM_LED = 3;

const uint8_t PIN_ERROR_LED = 2;

const uint8_t PIN_RESET = 9; // NFC tag reader reset pin
const uint8_t PIN_SELECT = 10; // NFC slave select line

uint32_t lastError = 0;

MFRC522 mfrc522(PIN_SELECT, PIN_RESET);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

uint8_t user;

void initialise()
{
  pinMode(PIN_ERROR_LED, OUTPUT);


  SPI.begin();
  mfrc522.PCD_Init();

  // Prepare the default key, both A and B
  for (size_t i = 0; i < 6; ++i)
    key.keyByte[i] = 0xFF;

}

void reset()
{
  digitalWrite(PIN_ERROR_LED, 0);
}


void onIndicators()
{
  // replace this to be done from information
  if (state == ModuleState::INITIALISATION)
  {
    user = 6;
    state = ModuleState::READY;
  }
}

void arm()
{
}

// not used, slightly concerned?
//const uint8_t SECTOR = 1;
const uint8_t BLOCK = 4;
const uint8_t TRAILER = 7;

void error()
{
  lastError = millis();

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
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

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

  // Check for compatibility
  if (/*piccType != MFRC522::PICC_TYPE_MIFARE_MINI
    && */piccType != MFRC522::PICC_TYPE_MIFARE_1K
    //&& piccType != MFRC522::PICC_TYPE_MIFARE_4K
  )
  {
    error();
    return;
  }

  MFRC522::StatusCode status;
  uint8_t buffer[18];
  uint8_t size = sizeof(buffer);

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7

  // Authenticate using key A
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    error();
    return;
  }

  // Authenticate using key B
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, TRAILER, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    error();
    return;
  }

  // Read data from the block (again, should now be what we have written)
  status = mfrc522.MIFARE_Read(BLOCK, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    error();
    return;
  }

  if (buffer[15] == user)
    disarm();
  else
    strike();


  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

void defuse()
{
}

void detonate()
{
}
