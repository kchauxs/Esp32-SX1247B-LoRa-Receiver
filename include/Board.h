#ifndef __BOARD_H__
#define __BOARD_H__

#include <Arduino.h>
#include "esp_adc_cal.h"
#include <ArduinoJson.h>
#include <Wire.h>

#include "Config.h"
#include "Context.h"
#include <WiFi.h>

class Board
{
private:
    Context *_ctx;

public:
    Board(Context *ctx);
    void initSerial(unsigned long baudrate);
    void initNativeLed();
    void initButton();

    float getFreeRam();
    float getUsedRam();
    float getCPUTemperature();
    String getIdUnique();
    unsigned long getUptimeInMinutes();
    unsigned long getUptimeInSeconds();
    void restartDevice(int waitTime = 3300);

    bool isConnectedToWiFi();
    int getRSSIasQuality();
    void updateInfo();
};
#ifdef __cplusplus
extern "C"
{
#endif
    uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

extern Board board;
#endif // __BOARD_H__