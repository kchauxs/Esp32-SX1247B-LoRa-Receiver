#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <Arduino.h>
#include <Preferences.h>
#include "Context.h"

class Storage
{
private:
    String _partitionName;
    Preferences _preferences;
    Context *_ctx;

public:
    Storage(Context *ctx, String name = "storage");

    bool save();
    bool read();
    void reset();
    void printDebug();
};

extern Storage storage;

#endif // __STORAGE_H__