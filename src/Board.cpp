#include "Board.h"

Board::Board(Context *ctx)
{
    _ctx = ctx;
}

void Board::initSerial(unsigned long baudrate)
{
    Serial.begin(baudrate);
}

void Board::initNativeLed()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void Board::initButton()
{
    pinMode(PIN_BUTTON, INPUT_PULLUP);
}

bool Board::isConnectedToWiFi()
{
    return WiFi.status() == WL_CONNECTED;
}

String Board::getIdUnique()
{
    uint64_t macAddress = ESP.getEfuseMac();
    uint32_t chip = (uint32_t)(macAddress >> 16);

    char idUnique[7];
    snprintf(idUnique, sizeof(idUnique), "%06X", chip);
    return "ESP32LORA" + (String)idUnique;
}

float Board::getFreeRam()
{
    uint32_t freeMem = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    uint32_t totalMem = heap_caps_get_total_size(MALLOC_CAP_8BIT);

    return ((float)freeMem / totalMem) * 100.0;
}

float Board::getUsedRam()
{
    uint32_t freeMem = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    uint32_t totalMem = heap_caps_get_total_size(MALLOC_CAP_8BIT);

    return ((float)(totalMem - freeMem) / totalMem) * 100.0;
}

int Board::getRSSIasQuality()
{
    if (!isConnectedToWiFi())
    {
        return 0;
    }

    int rssi = WiFi.RSSI();
    int quality = 0;

    if (rssi <= -100)
    {
        quality = 0;
    }
    else if (rssi >= -50)
    {
        quality = 100;
    }
    else
    {
        quality = map(rssi, -100, -50, 0, 100);
    }

    return quality;
}

float Board::getCPUTemperature()
{
    float _temperature = ((temprature_sens_read() - 32) / 1.8);
    if (isnan(_temperature) || _temperature <= 0)
        _temperature = 0;

    return _temperature;
}

unsigned long Board::getUptimeInMinutes()
{
    return (millis() / 60000);
}

unsigned long Board::getUptimeInSeconds()
{
    return (millis() / 1000);
}

void Board::updateInfo()
{
    _ctx->wifiRSSI = this->getRSSIasQuality();
    _ctx->cpuTemperature = this->getCPUTemperature();
    _ctx->uptimeInMinutes = this->getUptimeInMinutes();
    _ctx->usedRam = this->getUsedRam();
}

void Board::restartDevice(int waitTime)
{
    delay(waitTime);
    ESP.restart();
}