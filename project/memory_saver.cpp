#include "memory_saver.hpp";

bool MemorySaver::begin() {
    if (initialized) {
        return true;
    }

    initialized = storage.begin("app-settings", false); //open namespace for writing (works as map)
    return initialized;
}

uint32_t MemorySaver::loadUInt(const char* key, uint32_t defaultValue) {
    if (!initialized) {
        return defaultValue;
    }
    else {
        return storage.getUInt(key, defaultValue);
    }
}

bool MemorySaver::saveUInt(const char* key, uint32_t value) {
    if (!initialized) {
        return false;
    }
    else {
        return (storage.putUInt(key, value) > 0);
    }
}

// This returns Arduino string from NVS. If const char* is needed - just use .c_str().
String MemorySaver::loadString(const char* key, const char* defaultValue) {
    if (!initialized) {
        return defaultValue;
    }
    else {
        return storage.getString(key, defaultValue);
    }
}

// putString natively supports c strings
bool MemorySaver::saveString(const char* key, const char* value) {
    if (!initialized) {
        return false;
    }
    else {
        return (storage.putString(key, value) > 0);
    }
}

// putString natively supports Arduino Strings
bool MemorySaver::saveString(const char* key, const String& value) {
    if (!initialized) {
        return false;
    }
    else {
        return (storage.putString(key, value) > 0);
    }
}

// funktion overload for std::string
bool MemorySaver::saveString(const char* key, const std::string& value) {
    if (!initialized) {
        return false;
    }
    else {
        return (storage.putString(key, value.c_str()) > 0);
    }
}

void MemorySaver::end() {
    if (initialized) {
        storage.end();
        initialized = false;
    }
}
