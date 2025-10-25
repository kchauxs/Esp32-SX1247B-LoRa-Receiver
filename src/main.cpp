#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>

#include <LoRa.h>
#include <ArduinoJson.h>
#include <TaskScheduler.h>
#include <WiFiManager.h>

#include "Config.h"
#include "Context.h"
#include "MqttService.h"
#include "Board.h"
#include "Storage.h"

WiFiManager wm;
Scheduler runner;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Context ctx;
Context Context::instance;
Context &ctx = Context::getInstance();

Board board(&ctx);
Storage storage(&ctx);
MqttService service(&mqttClient, &ctx);

#include "Task.hpp"
#include "Functions.hpp"

void setup()
{
  Serial.begin(115200);
  delay(2000);

  while (!Serial)
    ;

  board.initNativeLed();
  board.initButton();
  ctx.deviceID = board.getIdUnique();
  //------------------------------------------------------------------
  // LORA INIT
  //------------------------------------------------------------------
  handleInitLoRa();

  //------------------------------------------------------------------
  // PORTAL AUTO CONNECT
  //------------------------------------------------------------------
  if (digitalRead(PIN_BUTTON) == LOW)
  {
    handleInitPortal();
    storage.save();
    board.restartDevice();
    return;
  }
  else
  {
    storage.read();
    storage.printDebug();
    board.connectToWiFi(ctx.WiFiSSID.c_str(), ctx.WiFiPass.c_str());
    wm.~WiFiManager();
    delay(10);
  }

  //------------------------------------------------------------------
  // MQTT INIT
  //------------------------------------------------------------------
  handleMqttConnection();

  //------------------------------------------------------------------
  // TASK
  //------------------------------------------------------------------
  runner.init();
  runner.addTask(taskUpdateInfo);
#if SERIAL_DEBUG
  runner.addTask(taskLogs);
  taskLogs.enable();
#endif
  taskUpdateInfo.enable();
}

void loop()
{
  handleLoRaBridge();

  runner.execute();
  service.mqttLoop(
      []()
      {
        taskUpdateInfo.disable();
#if SERIAL_DEBUG
        taskLogs.disable();
#endif
        runner.execute();
      },
      []()
      {
        taskUpdateInfo.enable();
#if SERIAL_DEBUG
        taskLogs.enable();
#endif
      });
}
