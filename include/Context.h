// #ifndef __CONTEXT_H__
// #define __CONTEXT_H__

// #include <Arduino.h>
// #include "Config.h"

// enum MqttStatus
// {
//     CONNECTED,
//     CONNECTING,
//     DISCONNECTED,
// };

// class Context
// {
// private:
// public:
//     float cpuTemperature = 0;
//     float usedRam = 0;

//     int wifiRSSI = 0;
//     int lastPacketRssi = 0;
//     int lastPacketSnr = 0;
//     int freqError = 0;

//     unsigned long uptimeInMinutes = 0;
//     unsigned long restartCounter = 0;

//     String deviceID = "";

//     // MQTT
//     MqttStatus mqttStatus = MqttStatus::DISCONNECTED;

//     String mqttServer = DEFAULT_SERVER_MQTT;
//     String mqttUsername = "";
//     String mqttPassword = "";
//     String mqttClientId = "LoRa_" + String(random(0xffff), HEX) + String(random(1, 9999));

//     String mqttPublishDataTopic = "apolo/data/test";
//     String mqttSubscribeCommandTopic = "apolo/command/test";

//     uint16_t mqttServerPort = DEFAULT_MQTT_SERVER_PORT;
//     bool isMqttConnected = false;

//     Context() {};
// };

// extern Context ctx;
// #endif // __CONTEXT_H__

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <Arduino.h>
#include "Config.h"

enum MqttStatus
{
    CONNECTED,
    CONNECTING,
    DISCONNECTED,
};

class Context
{
private:
    Context()
    {
    }
    Context(const Context &) = delete;

    Context &operator=(const Context &) = delete;
    static Context instance;

public:
    float cpuTemperature = 0;
    float usedRam = 0;

    int wifiRSSI = 0;
    int lastPacketRssi = 0;
    int lastPacketSnr = 0;
    int freqError = 0;

    unsigned long uptimeInMinutes = 0;
    unsigned long restartCounter = 0;

    String deviceID = "";

    // WIFI
    String WiFiSSID = "";
    String WiFiPass = "";

    // MQTT
    MqttStatus mqttStatus = MqttStatus::DISCONNECTED;

    String mqttServer = DEFAULT_SERVER_MQTT;
    String mqttUsername = "";
    String mqttPassword = "";
    String mqttClientId = "LoRa_" + String(random(0xffff), HEX) + String(random(1, 9999));

    String mqttPublishDataTopic = "apolo/data/test";
    String mqttSubscribeCommandTopic = "apolo/command/test";

    uint16_t mqttServerPort = DEFAULT_MQTT_SERVER_PORT;
    bool isMqttConnected = false;

    // CONTEXT
    static Context &getInstance()
    {
        return instance;
    }
};

extern Context &ctx;
#endif // __CONTEXT_H__