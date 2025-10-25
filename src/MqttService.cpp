#include "MqttService.h"

MqttService::MqttService(PubSubClient *mqttClient, Context *context)
{
    _mqttClient = mqttClient;
    _ctx = context;
}

bool MqttService::reconnectToMqttServer()
{
    const String mqttUsername = _ctx->mqttUsername;
    const String mqttPassword = _ctx->mqttPassword;

    DEBUG_PRINT("[INFO-MQTT] Connecting to MQTT server with client ID " + _ctx->mqttClientId + " ...");

    bool connected = false;

    connected = _mqttClient->connect(
        _ctx->mqttClientId.c_str(),
        mqttUsername.c_str(),
        mqttPassword.c_str());

    if (connected)
    {
        _ctx->mqttStatus = MqttStatus::CONNECTED;
        DEBUG_PRINTLN(" ✅ SUCCESS");

        if (!_ctx->mqttSubscribeCommandTopic.isEmpty())
        {
            _mqttClient->subscribe(_ctx->mqttSubscribeCommandTopic.c_str());
            DEBUG_PRINTLN("[INFO-MQTT] Connected and subscribed to " + _ctx->mqttSubscribeCommandTopic);
        }
    }
    else
    {
        int mqttState = _mqttClient->state();
        DEBUG_PRINTLN(" ❌ FAILED: State = " + String(mqttState));
        DEBUG_PRINTLN("[ERROR-MQTT] Username: " + mqttUsername);

        if (mqttState < 0)
        {
            _ctx->mqttStatus = MqttStatus::CONNECTING;
            DEBUG_PRINTLN("[INFO-MQTT] Reconnecting...");
        }

        if (mqttState > 0)
        {
            _ctx->mqttStatus = MqttStatus::DISCONNECTED;
            DEBUG_PRINTLN("[INFO-MQTT] Disconnected");
        }
    }

    return connected;
}

void MqttService::mqttLoop(std::function<void()> beforeReconnect, std::function<void()> afterReconnect)
{
    static unsigned int mqttReconnectAttempt = MQTT_RECONNECT_ATTEMPT;
    static unsigned int lastReconnectAttempt = MQTT_RECONNECT_ATTEMPT + 1000;
    static unsigned int reconnectAttempt = 0;

    if (_ctx->mqttStatus == MqttStatus::DISCONNECTED)
        return;

    if (WiFi.status() != WL_CONNECTED)
        return;

    if (_mqttClient->connected())
    {
        _mqttClient->loop();
    }
    else
    {
        long now = millis();
        if (now - lastReconnectAttempt > mqttReconnectAttempt)
        {
            DEBUG_PRINTLN("\n[INFO-MQTT] MQTT Client disconnected, reconnecting... ");

            if (beforeReconnect)
                beforeReconnect();

            lastReconnectAttempt = now;
            if (this->reconnectToMqttServer())
            {
                reconnectAttempt = 0;
                lastReconnectAttempt = 0;
                mqttReconnectAttempt = MQTT_RECONNECT_ATTEMPT;
            }
            else
            {
                reconnectAttempt++;
                DEBUG_PRINTLN("[INFO-MQTT] Reconnect attempt " + String(reconnectAttempt));
                if (reconnectAttempt > 3)
                    mqttReconnectAttempt = MQTT_RECONNECT_ATTEMPT * 3;

                DEBUG_PRINTLN("[ERROR-MQTT] MQTT Reconnect attempt: " + String(reconnectAttempt) + " - " + String(mqttReconnectAttempt) + " ms");
            }

            if (afterReconnect)
                afterReconnect();
        }
    }
}

bool MqttService::sendDataByMQTT(const String &payload, const String &topic)
{
    if (topic == "")
        return false;

    if (!this->isConnectedToMQTTServer())
        return false;

    return _mqttClient->publish(topic.c_str(), payload.c_str());
}

bool MqttService::isConnectedToMQTTServer()
{
    return _mqttClient->connected();
}

void MqttService::updateInfo()
{
    if (_ctx->mqttStatus == MqttStatus::DISCONNECTED)
        _ctx->isMqttConnected = false;
    else
        _ctx->isMqttConnected = this->isConnectedToMQTTServer();
}