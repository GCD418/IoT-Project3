#pragma once
#include <ArduinoJson.h>

class JsonManager {
private:
    StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;
    StaticJsonDocument<JSON_OBJECT_SIZE(4)> outputDoc;
    char outputBuffer[128];

public:
    JsonManager() {}

    char* createStateReport(const char* propertyName, int value) {
        outputDoc.clear();
        outputDoc["state"]["reported"][propertyName] = value;
        serializeJson(outputDoc, outputBuffer);
        return outputBuffer;
    }

    char* createLockStateReport(const char* propertyName, String value) {
        outputDoc.clear();
        outputDoc["state"]["reported"][propertyName] = value;
        serializeJson(outputDoc, outputBuffer);
        return outputBuffer;
    }

    bool parseJson(byte* payload, unsigned int length) {
        DeserializationError err = deserializeJson(inputDoc, payload, length);
        return !err;
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(64)>& getInputDoc() {
        return inputDoc;
    }
};
