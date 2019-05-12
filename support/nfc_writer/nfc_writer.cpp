/**
 * NFC Tag writer for authorization module, code based on an MFRC522 example that was made public domain and is available from
 */
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t PIN_BUTTON = 3;

const uint8_t PIN_RESET = 9;
const uint8_t PIN_SELECT = 10;

Adafruit_SSD1306 display(128, 32);

MFRC522 mfrc522(PIN_SELECT, PIN_RESET);

MFRC522::MIFARE_Key key;

uint8_t dataBlock[16] = {0x00};

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(const uint8_t* const buffer, size_t len)
{
  for (uint8_t i = 0; i < len; ++i)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Initialize.
 */
void setup()
{
  pinMode(PIN_BUTTON, INPUT);

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextWrap(false);
  display.dim(1);

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (size_t i = 0; i < 6; ++i)
    key.keyByte[i] = 0xFF;

  render();
}

void render()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Tag #");
  display.print(dataBlock[15]);

  display.display();
}

/**
 * Main loop.
 */
void loop()
{
  if (digitalRead(PIN_BUTTON))
  {
    while (digitalRead(PIN_BUTTON));
    ++dataBlock[15];
    dataBlock[15] %= 24;
    render();
  }
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  // Show some details of the PICC (that is: the tag/card)
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

  // Check for compatibility
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI
    && piccType != MFRC522::PICC_TYPE_MIFARE_1K
    && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    return;
  }

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7
  const uint8_t sector = 1;
  const uint8_t blockAddr = 4;
  uint8_t trailerBlock = 7;
  MFRC522::StatusCode status;
  uint8_t buffer[18];
  uint8_t size = sizeof(buffer);

  // Authenticate using key A
  Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    return;
  }

  /*
  // Show the whole sector as it currently is
  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

  // Read data from the block
  Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
  dump_byte_array(buffer, 16); Serial.println();
  Serial.println();
  */

  // Authenticate using key B

  Serial.println(F("Authenticating again using key B..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    return;
  }

  // Write data to the block
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  dump_byte_array(dataBlock, 16); Serial.println();
  status = mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();

  // Read data from the block (again, should now be what we have written)
  Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
  dump_byte_array(buffer, 16); Serial.println();

  // Check that data in block is what we have written
  // by counting the number of bytes that are equal
  Serial.println(F("Checking result..."));
  byte count = 0;
  for (byte i = 0; i < 16; ++i)
  {
    // Compare buffer (= what we've read) with dataBlock (= what we've written)
    if (buffer[i] == dataBlock[i])
      ++count;
  }
  Serial.print(F("Number of bytes that match = ")); Serial.println(count);
  if (count == 16)
  {
    Serial.println(F("Success :-)"));
  }
  else
  {
    Serial.println(F("Failure, no match :-("));
    Serial.println(F("  perhaps the write didn't work properly..."));
  }
  Serial.println();

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}
