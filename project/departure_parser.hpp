#include "departures.hpp"
#include <ArduinoJson.h>
#include <Arduino.h>

bool parseFirstDeparture(Stream& input, Departure& result)
{
    JsonDocument document;

    DeserializationError error = deserializeJson(document, input);

    if (error) {
        return false;
    }

    JsonObject departure = document["departures"][0].as<JsonObject>();

    if (departure.isNull()) {
        return false;
    }

    JsonObject route = departure["route"].as<JsonObject>();

    if (!route["designation"].is<const char*>() ||
        !route["direction"].is<const char*>() ||
        !route["transport_mode"].is<const char*>() || 
        !departure["scheduled"].is<const char*>() ||
        !departure["canceled"].is<bool>()) {
        return false;
    }

    String line = route["designation"].as<const char*>();
    String destination = route["direction"].as<const char*>();
    String transportMode = route["transport_mode"].as<const char*>();

    String time = departure["scheduled"].as<const char*>();
    bool canceled = departure["canceled"].as<bool>();

    result = Departure{
        line,
        time,
        destination,
        "Status unknown",
        transportMode,
        canceled
    };

    return true;
}