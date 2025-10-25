#include <Arduino.h>
#include <vector>
#include <TaskScheduler.h>

#include "Config.h"
#include "Context.h"

#include "Board.h"
#include "MqttService.h"

void taskCallbackLogs();
void taskCallbackUpdateInfo();

Task taskLogs(TASK_SECOND *DEFAUL_LOOP_LOG_INTERVAL, TASK_FOREVER, &taskCallbackLogs);
Task taskUpdateInfo(TASK_SECOND *DEFAUL_LOOP_UPDATE_INFO_INTERVAL, TASK_FOREVER, &taskCallbackUpdateInfo);

void taskCallbackUpdateInfo()
{
    static unsigned long lastDisplayUpdate = 0;

    if (millis() - lastDisplayUpdate > 10000)
    {
        lastDisplayUpdate = millis();
        board.updateInfo();
        service.updateInfo();
    }
}

void taskCallbackLogs()
{
    // static bool firstLog = true;

    // if (firstLog)
    // {
    //     firstLog = false;
    //     return;
    // }
    static unsigned long lastDisplayUpdate = 0;

    if (millis() - lastDisplayUpdate > 15000)
    {
        lastDisplayUpdate = millis();
#if SERIAL_DEBUG
        DEBUG_PRINTLN("");
        DEBUG_PRINTLN("[LOG] ------------------------------------------------");
        DEBUG_PRINTLN("[LOG]\t SYSTEM INFO");
        DEBUG_PRINTLN("[LOG] ------------------------------------------------");
        DEBUG_PRINTLN("[LOG]\t Serial:                  " + board.getIdUnique());
        DEBUG_PRINTLN("[LOG]");
        DEBUG_PRINTLN("[LOG]\t RAM %:                   " + String(ctx.usedRam) + "%");
        DEBUG_PRINTLN("[LOG]\t CPU Temp:                " + String(ctx.cpuTemperature) + "°C");
        DEBUG_PRINTLN("[LOG]\t Uptime:                  " + String(ctx.uptimeInMinutes) + " min");
        DEBUG_PRINTLN("[LOG]");
        if (!board.isConnectedToWiFi())
        {
            DEBUG_PRINTLN("[LOG]\t WiFi:                    NOT❗");
        }
        else
        {
            DEBUG_PRINTLN("[LOG]\t WiFi:                    OK ✅");
            DEBUG_PRINTLN("[LOG]\t WiFi RSSI:               " + String(ctx.wifiRSSI) + "%");
        }
        DEBUG_PRINTLN("[LOG] ------------------------------------------------");
        DEBUG_PRINTLN("[LOG]\t LoRa");
        DEBUG_PRINTLN("[LOG] ------------------------------------------------");
        DEBUG_PRINTLN("[LOG]\t Last Packet RSSI               " + String(ctx.lastPacketRssi) + " dBm");
        DEBUG_PRINTLN("[LOG]\t Last Packet SNR                " + String(ctx.lastPacketSnr, 2) + " dB");
        DEBUG_PRINTLN("[LOG]\t Frequency Error                " + String(ctx.freqError) + " Hz");
        DEBUG_PRINTLN("[LOG] ------------------------------------------------");
        DEBUG_PRINTLN("[LOG]\t MQTT");
        DEBUG_PRINTLN("[LOG] ------------------------------------------------");
        DEBUG_PRINTLN("[LOG]\t MQTT:                    " + String(service.isConnectedToMQTTServer() ? "OK ✅" : "NOT❗"));
        DEBUG_PRINTLN("[LOG]\t MQTT Server:             " + ctx.mqttServer);
        DEBUG_PRINTLN("[LOG]\t MQTT Username:           " + ctx.mqttUsername);
        DEBUG_PRINTLN("[LOG]\t MQTT Client ID:          " + ctx.mqttClientId);
        DEBUG_PRINTLN("[LOG] ------------------------------------------------");
#endif
    }
}
