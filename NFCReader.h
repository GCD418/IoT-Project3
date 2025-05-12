#pragma once
#include <Wire.h>
#include <Adafruit_PN532.h>

class NFCReader
{
private:
    Adafruit_PN532 nfc;
    int sdaPin;
    int sclPin;

public:
    NFCReader(int sda, int scl)
        : sdaPin(sda), sclPin(scl), nfc(sda, scl) {}

    void begin()
    {
        Wire.begin(sdaPin, sclPin);

        Serial.println("Initializing PN532...");
        nfc.begin();

        uint32_t versiondata = nfc.getFirmwareVersion();
        if (!versiondata)
        {
            Serial.println("Didn't find PN532 module");
            // while (1);
        }

        Serial.print("Found PN532 with firmware version: ");
        Serial.println(versiondata, HEX);

        nfc.SAMConfig();
        Serial.println("Waiting for NFC card ...");
    }

    bool scanForCard(uint8_t *uid, uint8_t *uidLength)
    {
        uint8_t success;
        success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, uidLength, 50);
        return success;
    }

    void printCardDetails(uint8_t *uid, uint8_t uidLength)
    {
        Serial.println("Found an NFC tag!");

        Serial.print("UID Value: ");
        for (uint8_t i = 0; i < uidLength; i++)
        {
            Serial.print(" 0x");
            Serial.print(uid[i], HEX);
        }
        Serial.println("");
    }

    String getUidAsString(uint8_t *uid, uint8_t uidLength)
    {
        String uidStr = "";
        for (uint8_t i = 0; i < uidLength; i++)
        {
            char buf[4];
            sprintf(buf, "%02X", uid[i]);
            uidStr += buf;
        }
        return uidStr;
    }

    bool isConnected()
    {
        uint32_t versiondata = nfc.getFirmwareVersion();
        return (versiondata != 0);
    }

    void reconnect()
    {
        Serial.println("Attempting to reconnect to NFC reader...");
        nfc.begin();

        uint32_t versiondata = nfc.getFirmwareVersion();
        if (!versiondata)
        {
            Serial.println("Reconnection to PN532 failed");
            return;
        }

        Serial.print("Reconnected to PN532 with firmware version: ");
        Serial.println(versiondata, HEX);
        nfc.SAMConfig();
        Serial.println("NFC reader ready again");
    }
};
