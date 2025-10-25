#include <ArduinoJson.h>
#include <WiFiManager.h>

#include "Config.h"
#include "Context.h"

String mqttServer;

void mqttReceivedCallback(char *topic, byte *payload, unsigned int length)
{
    String payloadString = "";
    for (int i = 0; i < length; i++)
        payloadString += (char)payload[i];

#if SERIAL_DEBUG
    DEBUG_PRINTLN("\n[MQTT-CALLBACK] Message arrived [" + String(topic) + "] ");
    DEBUG_PRINTLN("[MQTT-CALLBACK] Payload: " + payloadString);
#endif

    JsonDocument jsonDoc;

    if (deserializeJson(jsonDoc, payloadString))
    {
        jsonDoc.clear();
        return;
    }
}

void handleMqttConnection()
{
    IPAddress mqttServerIP = IPAddress(0, 0, 0, 0);
    uint16_t mqttServerPort = ctx.mqttServerPort;

    if (mqttServerIP.fromString(ctx.mqttServer))
        mqttClient.setServer(mqttServerIP, mqttServerPort);
    else
    {
        mqttServer = ctx.mqttServer;
        mqttClient.setServer(mqttServer.c_str(), mqttServerPort);
    }

    mqttClient.setBufferSize(MQTT_BUFFER_SIZE);
    mqttClient.setCallback(mqttReceivedCallback);
    ctx.mqttStatus = MqttStatus::CONNECTING;
}

void handleInitLoRa()
{
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    Serial.println("Iniciando receptor Heltec LoRa V2...");

    if (!LoRa.begin(LORA_BAND))
    {
        Serial.println("❌ Falló la inicialización del LoRa. Revisar cableado o frecuencia.");
        while (true)
            ;
    }

    LoRa.setSpreadingFactor(9);     // Entre 7 y 12
    LoRa.setSignalBandwidth(125E3); // 125 kHz
    LoRa.setCodingRate4(5);         // 4/5
    LoRa.enableCrc();

    Serial.println("✅ Receptor listo. Esperando mensajes...");
}

void handleInitPortal()
{
    digitalWrite(LED_BUILTIN, HIGH);
    DEBUG_PRINTLN("\nIniciando portal de configuración WiFi...");

    WiFi.mode(WIFI_STA);
    delay(1000);

    std::vector<const char *> menu = {"wifi", "wifinoscan", "restart", "exit"};

    wm.setMenu(menu);
    wm.setDebugOutput(SERIAL_DEBUG);

    wm.setTitle("Configuración WiFi");
    wm.setDarkMode(true);

    if (wm.startConfigPortal(DEFAULT_PORTAL_NAME, DEFAULT_PORTAL_PASSWORD))
    {
        DEBUG_PRINTLN("Portal de configuracion finalizado ✅");
    }

    ctx.WiFiSSID = wm.getWiFiSSID();
    ctx.WiFiPass = wm.getWiFiPass();

    DEBUG_PRINTLN("\nInformación de WiFi guardada:");
    DEBUG_PRINTLN(" - SSID: " + ctx.WiFiSSID);
    DEBUG_PRINTLN(" - PASS: " + ctx.WiFiPass);

    wm.stopConfigPortal();
    wm.disconnect();
    digitalWrite(LED_BUILTIN, LOW);
}

String csvToJson(const String &csvData)
{
    int size = 14;
    int partIndex = 0;
    bool inQuotes = false;
    String parts[size];
    String currentPart = "";

    for (size_t i = 0; i < csvData.length(); i++)
    {
        char c = csvData.charAt(i);

        if (c == '"')
        {
            inQuotes = !inQuotes;
        }
        else if (c == ',' && !inQuotes)
        {
            if (partIndex < size)
            {
                parts[partIndex++] = currentPart;
                currentPart = "";
            }
        }
        else
        {
            currentPart += c;
        }
    }

    if (partIndex < size)
    {
        parts[partIndex] = currentPart;
    }

    JsonDocument jsonDoc;

    jsonDoc["ts"] = parts[0].toInt();    // timestamp
    jsonDoc["ent"] = parts[1];           // empresa
    jsonDoc["mat"] = parts[2];           // material
    jsonDoc["dest"] = parts[3];          // destino
    jsonDoc["orig"] = parts[4];          // origen
    jsonDoc["qty"] = parts[5].toFloat(); // cantidad
    jsonDoc["val"] = parts[6].toInt();   // valor
    jsonDoc["drv"] = parts[7];           // conductor
    jsonDoc["pl"] = parts[8];            // placa
    jsonDoc["lp"] = parts[9];            // transportista
    jsonDoc["mach"] = parts[10];         // maquinaria
    jsonDoc["op"] = parts[11];           // operador
    jsonDoc["obs"] = parts[12];          // observaciones

    String output;
    JsonObject deviceJson = jsonDoc["device"].to<JsonObject>();
    deviceJson["id"] = ctx.deviceID; // dispositivo;
    deviceJson["from"] = parts[13];
    deviceJson["tx"] = "LoRa";
    deviceJson["rssi"] = ctx.lastPacketRssi;
    deviceJson["unit"] = "dbm";

    String jsonString;
    serializeJson(jsonDoc, jsonString);
    return jsonString;
}

void handleLoRaBridge()
{
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        digitalWrite(LED_BUILTIN, HIGH);

        String incoming;
        while (LoRa.available())
        {
            incoming += (char)LoRa.read();
        }
        incoming.trim();

        ctx.lastPacketRssi = LoRa.packetRssi();
        ctx.lastPacketSnr = LoRa.packetSnr();
        ctx.freqError = LoRa.packetFrequencyError();

        Serial.println("\n=============================");
        Serial.println("📩 Mensaje recibido:");
        Serial.println(incoming);
        Serial.print("\n📶 RSSI: ");
        Serial.println(ctx.lastPacketRssi);
        Serial.print("🔊 SNR: ");
        Serial.println(ctx.lastPacketSnr);
        Serial.print("⚙️ Error de frecuencia (Hz): ");
        Serial.println(ctx.freqError);
        Serial.println("-----------------------------");

        String json = csvToJson(incoming);
        if (json.length() > 0)
        {
            Serial.println("🧩 JSON reconstruido:");
            Serial.println(json);

            if (mqttClient.publish(ctx.mqttPublishDataTopic.c_str(), json.c_str()))
            {
                DEBUG_PRINTLN("\n✅ Mensaje publicado en MQTT.");
            }
            else
            {
                DEBUG_PRINTLN("\n❌ Error al publicar en MQTT.");
            }
        }
        else
        {
            Serial.println("⚠️ Error al convertir CSV → JSON");
        }
        Serial.println("=============================");
        digitalWrite(LED_BUILTIN, LOW);
    }
}