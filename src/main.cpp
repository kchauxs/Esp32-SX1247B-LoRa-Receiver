#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <AutoConnect.h>
#include <TaskScheduler.h>

#include "Config.h"
#include "Context.h"
#include "MqttService.h"
#include "Board.h"

Scheduler runner;
WebServer server;
AutoConnect portal(server);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Context ctx;
Context Context::instance;
Context &ctx = Context::getInstance();

Board board(&ctx);
MqttService service(&mqttClient, &ctx);

#include "Task.hpp"
#include "Functions.hpp"

void setup()
{
  Serial.begin(115200);
  delay(2000);

  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);
  ctx.deviceID = board.getIdUnique();
  //------------------------------------------------------------------
  // LORA INIT
  //------------------------------------------------------------------
  initLoRa();

  //------------------------------------------------------------------
  // PORTAL AUTO CONNECT
  //------------------------------------------------------------------
  initPortalAutoConnect();

  //------------------------------------------------------------------
  // MQTT INIT
  //------------------------------------------------------------------
  handleMqttConnection();

  //------------------------------------------------------------------
  // TASK
  //------------------------------------------------------------------
  //   runner.init();
  //   runner.addTask(taskUpdateInfo);
  // #if SERIAL_DEBUG
  //   runner.addTask(taskLogs);
  //   taskLogs.enable();
  // #endif
  //   taskUpdateInfo.enable();
}

void loop()
{
  handleLoRaBridge();

  // runner.execute();
  portal.handleClient();
  service.mqttLoop(
      []()
      {
        //         taskUpdateInfo.disable();
        // #if SERIAL_DEBUG
        //         taskLogs.disable();
        // #endif
        //         runner.execute();
      },
      []()
      {
        //         taskUpdateInfo.enable();
        // #if SERIAL_DEBUG
        //         taskLogs.enable();
        // #endif
      });
  // taskCallbackLogs();
  // taskCallbackUpdateInfo();
}
