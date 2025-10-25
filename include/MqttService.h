#ifndef __MQTT_SERVICE_H__
#define __MQTT_SERVICE_H__

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include "Context.h"
#include "Config.h"

class MqttService
{
private:
    Context *_ctx;
    PubSubClient *_mqttClient;

public:
    MqttService(PubSubClient *mqttClient, Context *context);

    bool isConnectedToMQTTServer();
    bool reconnectToMqttServer();
    bool sendDataByMQTT(const String &payload, const String &topic);

    void updateInfo();
    void mqttLoop(std::function<void()> beforeReconnect, std::function<void()> afterReconnect);
};

extern MqttService service;

#endif // __MQTT_SERVICE_H__