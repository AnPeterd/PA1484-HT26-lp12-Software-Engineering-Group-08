#pragma once
#include <Arduino.h>
#include <Preferences.h>

class MemorySaver {
    public:
        bool begin();
        
        uint32_t loadUInt(const char* key, uint32_t defaultValue = 0);
        bool saveUInt(const char* key, uint32_t value);
        
        String loadString(const char* key, const char* defaultValue = "");
        bool saveString(const char* key, const char* value);

        void end();
    
    private:
        Preferences storage;
        bool initialized = false;
};
