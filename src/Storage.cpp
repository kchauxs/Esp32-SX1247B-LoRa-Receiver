#include "Storage.h"

Storage::Storage(Context *context, String partitionName)
{
    _partitionName = partitionName;
    _ctx = context;
}

void Storage::reset()
{
    _ctx->WiFiSSID = "";
    _ctx->WiFiPass = "";
    this->save();
}

bool Storage::read()
{
    if (!_preferences.begin(_partitionName.c_str(), false))
    {
        this->reset();
        return false;
    }

    _ctx->WiFiSSID = _preferences.getString("ssid", "");
    _ctx->WiFiPass = _preferences.getString("pass", "");
    _preferences.end();

    if (_ctx->WiFiSSID.isEmpty() || _ctx->WiFiPass.isEmpty())
    {
#if SERIAL_DEBUG
        DEBUG_PRINTLN("[STORAGE] Credenciales vacías. Reiniciando valores.");
#endif
        this->reset();
        return false;
    }

    return true;
}

bool Storage::save()
{
    if (!_preferences.begin(_partitionName.c_str(), false))
        return false;

    _preferences.putString("ssid", _ctx->WiFiSSID);
    _preferences.putString("pass", _ctx->WiFiPass);
    _preferences.end();
    return true;
}

void Storage::printDebug()
{
#if SERIAL_DEBUG
    DEBUG_PRINTLN("\n[STORAGE] Datos leídos desde Storage:");
    DEBUG_PRINTLN(" - SSID: " + _ctx->WiFiSSID);
    DEBUG_PRINTLN(" - PASS: " + _ctx->WiFiPass);
#endif
}
