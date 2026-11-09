#include "departure_parser.hpp"

#include <ArduinoJson.h>
#include <algorithm>
#include <vector>

static bool earlierDeparture(
    const Departure& first,
    const Departure& second
) {
    return first.getTime().compareTo(second.getTime()) < 0;
}

bool parseDepartures(Stream& input, StopData& result, const String& selectedTransport)
{

    String filter = selectedTransport;
    filter.trim();
    filter.toUpperCase();

    //Names used in TrafiklabAPI
    if (filter == "ALL TRANSPORT TYPES")
        filter = "ALL";

    if (filter == "FERRY")
        filter = "BOAT";

    if (filter != "ALL" &&
        filter != "BUS" &&
        filter != "TRAIN" &&
        filter != "BOAT")
    {
        return false;
    }


    JsonDocument document;

    DeserializationError error = deserializeJson(document, input);

    if (error) {return false;}

    if (!document["departures"].is<JsonArray>()) {return false;}

    JsonArray departures = document["departures"].as<JsonArray>();

    std::vector<Departure> parsedDepartures;

    for (JsonVariant entry : departures) {
        if (!entry.is<JsonObject>()) {return false;}

        JsonObject departure = entry.as<JsonObject>();
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
        if(filter != "ALL" && !transportMode.equalsIgnoreCase(filter)){ continue; }

        String time = departure["scheduled"].as<const char*>();
        bool canceled = departure["canceled"].as<bool>();

        if (time.isEmpty()) {return false;}

        bool hasRealtime = departure["is_realtime"] | false;
        bool usingRealtime = false;

        if (hasRealtime &&
            departure["realtime"].is<const char*>()) {
            String realtime =
                departure["realtime"].as<const char*>();

            if (!realtime.isEmpty()) {
                time = realtime;
                usingRealtime = true;
            }
        }

        String status = "Status unknown";

        if (canceled) {
            status = "Canceled";
        } else if (usingRealtime &&
            departure["delay"].is<int>()) {
            int delaySeconds = departure["delay"].as<int>();

            if (delaySeconds > 0) {status = "Delayed";}
            else if (delaySeconds < 0) { status = "Early";} 
            else {status = "On time";}
        }

        Departure item{
            line,
            time,
            destination,
            status,
            transportMode,
            canceled
        };

        parsedDepartures.push_back(item);
    }

    std::stable_sort( parsedDepartures.begin(), parsedDepartures.end(), earlierDeparture);

    StopData updatedStop{result.getName(), result.getStopId()};

    for (const Departure& departure : parsedDepartures) {
        if (!updatedStop.addDeparture(departure)) { break;}
    }

    result = updatedStop;

    return true;
}