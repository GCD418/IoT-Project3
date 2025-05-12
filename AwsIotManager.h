#pragma once
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "JsonManager.h"
#include <ArduinoJson.h>
#include "ServoMotor.h"

class AwsIotManager {
private:
    WiFiClientSecure wiFiClient;
    PubSubClient client;
    JsonManager jsonManager;
    
    const char* awsEndpoint;
    int awsPort;
    const char* clientId;
    const char* updateTopic;
    const char* updateDeltaTopic;
    
    const char* rootCaCert;
    const char* clientCert;
    const char* privateKey;
    
    ServoMotor servoMotor;
    byte lockState = 0;
    //0 = closed, 1 = open
    
    void messageCallback(char* topic, byte* payload, unsigned int length) {
        String message;
        for (int i = 0; i < length; i++) message += String((char) payload[i]);
        Serial.println("Message from topic " + String(topic) + ":" + message);
        
        if (jsonManager.parseJson(payload, length)) {
            if (String(topic) == updateDeltaTopic) {
                // lockState = jsonManager.getValue<int8_t>("state.lockState"); //como no voy a lograr que mi template funcione?. Ya estoy muy viejo para estas cosas
                lockState = jsonManager.getInputDoc()["state"]["lockState"];
                Serial.println("Servo state updated to: " + String(lockState));
                updatelockState();
            }
        }
    }
    
    void reconnect() {
        while (!client.connected()) {
            Serial.print("Attempting MQTT connection to AWS IoT...");
            if (client.connect(clientId)) {
                Serial.println("connected");
                client.subscribe(updateDeltaTopic);
                Serial.println("Subscribed to " + String(updateDeltaTopic));
                delay(100);
                reportLockState();
            } else {
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println(" trying again in 5 seconds");
                delay(5000);
            }
        }
    }
    
public:
    AwsIotManager(
        const char* endpoint,
        int port,
        const char* id,
        const char* updateTopicPath,
        const char* deltaTopicPath
    ) : client(wiFiClient), awsEndpoint(endpoint), awsPort(port), clientId(id),
        updateTopic(updateTopicPath), updateDeltaTopic(deltaTopicPath), servoMotor(13) {}
    
    void setup(const char* rootCa, const char* cert, const char* privKey) {
        rootCaCert = rootCa;
        clientCert = cert;
        privateKey = privKey;
        
        wiFiClient.setCACert(rootCaCert);
        wiFiClient.setCertificate(clientCert);
        wiFiClient.setPrivateKey(privateKey);
        
        client.setServer(awsEndpoint, awsPort);
        client.setCallback([this](char* topic, byte* payload, unsigned int length) {
            messageCallback(topic, payload, length);
        });
    }
    
    void loop() {
        if (!client.connected()) {
            reconnect();
        }
        client.loop();
    }
    
    void reportLockState() {
        char* buffer = jsonManager.createStateReport("lockState", lockState);
        client.publish(updateTopic, buffer);
    }
    
    void updatelockState() {
        if (lockState) servoMotor.open();
        else servoMotor.close();
        reportLockState();
    }
    
    void reportUser(String user) {
        char* buffer = jsonManager.createLockStateReport("lastUser", user);
        client.publish(updateTopic, buffer);
    }
};
