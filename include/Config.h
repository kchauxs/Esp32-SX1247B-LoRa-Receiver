#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Arduino.h>

//----------------------------------------------
// DEVICE CONFIG
//----------------------------------------------
#define SERIAL_DEBUG !false
//----------------------------------------------

#if SERIAL_DEBUG
#define DEBUG_PRINT(msg) Serial.print(msg)
#define DEBUG_PRINTLN(msg) Serial.println(msg)
#else
#define DEBUG_PRINT(msg)   // No hacer nada si SERIAL_DEBUG es false
#define DEBUG_PRINTLN(msg) // No hacer nada si SERIAL_DEBUG es false
#endif

#define PIN_BUTTON 0

//----------------------------------------------
// PORTAL CONFIG
//----------------------------------------------
#define DEFAULT_PORTAL_PASSWORD "12345678"
#define DEFAULT_PORTAL_NAME "LoRa-Gatewa"

//----------------------------------------------
// TASK
//----------------------------------------------
#define DEFAUL_LOOP_LOG_INTERVAL 15         // seconds
#define DEFAUL_LOOP_UPDATE_INFO_INTERVAL 20 // seconds

//----------------------------------------------
// MQTT
//----------------------------------------------
#define DEFAULT_SERVER_MQTT "broker.emqx.io"
#define DEFAULT_MQTT_SERVER_PORT 1883

#define MQTT_BUFFER_SIZE 8192
#define MQTT_RECONNECT_ATTEMPT 55000
// Status Messages
#define MIN_STATUS_MESSAGES_INTERVAL 30
#define MAX_STATUS_MESSAGES_INTERVAL 3600 // 1 hour

// ----------------------------------------------
// loRa
// ----------------------------------------------
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 26
#define LORA_BAND 433E6

#endif // __CONFIG_H__