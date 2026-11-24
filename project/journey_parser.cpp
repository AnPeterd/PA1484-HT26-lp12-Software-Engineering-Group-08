#include "journey_parser.hpp"

#include <ArduinoJson.h>

static bool readRequiredText(
    JsonObject object,
    const char* key,
    String& result
) {
    if (!object[key].is<const char*>()) {
        return false;
    }

    result = object[key].as<const char*>();
    return !result.isEmpty();
}

static String readOptionalText(
    JsonObject object,
    const char* key
) {
    if (!object[key].is<const char*>()) {
        return "";
    }

    return String(object[key].as<const char*>());
}

static bool keepHoursAndMinutes(String& time) {
    if (time.length() < 5 || time.charAt(2) != ':') {
        return false;
    }

    time = time.substring(0, 5);
    return true;
}

static bool parseJourneyLeg(
    JsonObject legObject,
    JourneyLeg& result
) {
    JsonObject origin = legObject["Origin"].as<JsonObject>();
    JsonObject destination = legObject["Destination"].as<JsonObject>();

    String originName;
    String originTime;
    String destinationName;
    String destinationTime;
    String transportType;

    if (!readRequiredText(origin, "name", originName) ||
        !readRequiredText(origin, "time", originTime) ||
        !readRequiredText(destination, "name", destinationName) ||
        !readRequiredText(destination, "time", destinationTime) ||
        !readRequiredText(legObject, "type", transportType)) {
        return false;
    }

    if (!keepHoursAndMinutes(originTime) ||
        !keepHoursAndMinutes(destinationTime)) {
        return false;
    }

    String serviceName = readOptionalText(legObject, "name");
    String line = readOptionalText(legObject, "number");
    String direction = readOptionalText(legObject, "direction");

    if (serviceName.isEmpty()) {
        if (transportType.equalsIgnoreCase("WALK")) {
            serviceName = "Promenad";
        } else {
            serviceName = "OkÃ¤nd resa";
        }
    }

    result = JourneyLeg{
        originName,
        originTime,
        destinationName,
        destinationTime,
        transportType,
        serviceName,
        line,
        direction
    };

    return true;
}

static bool parseJourney(
    JsonObject tripObject,
    Journey& result
) {
    JsonObject origin = tripObject["Origin"].as<JsonObject>();
    JsonObject destination = tripObject["Destination"].as<JsonObject>();

    String originName;
    String originTime;
    String destinationName;
    String destinationTime;

    if (!readRequiredText(origin, "name", originName) ||
        !readRequiredText(origin, "time", originTime) ||
        !readRequiredText(destination, "name", destinationName) ||
        !readRequiredText(destination, "time", destinationTime)) {
        return false;
    }

    if (!keepHoursAndMinutes(originTime) ||
        !keepHoursAndMinutes(destinationTime)) {
        return false;
    }

    JsonArray legs =
        tripObject["LegList"]["Leg"].as<JsonArray>();

    if (legs.isNull()) {
        return false;
    }

    Journey parsedJourney{
        originName,
        originTime,
        destinationName,
        destinationTime
    };

    for (JsonVariant legValue : legs) {
        if (!legValue.is<JsonObject>()) {
            return false;
        }

        JourneyLeg parsedLeg;

        if (!parseJourneyLeg(
                legValue.as<JsonObject>(),
                parsedLeg
            )) {
            return false;
        }

        if (!parsedJourney.addLeg(parsedLeg)) {
            return false;
        }
    }

    if (parsedJourney.getLegCount() == 0) {
        return false;
    }

    result = parsedJourney;
    return true;
}

bool parseJourneys(Stream& input, JourneyResults& result) {
    JsonDocument filter;

    filter["Trip"][0]["Origin"]["name"] = true;
    filter["Trip"][0]["Origin"]["time"] = true;

    filter["Trip"][0]["Destination"]["name"] = true;
    filter["Trip"][0]["Destination"]["time"] = true;

    filter["Trip"][0]["LegList"]["Leg"][0]["Origin"]["name"] = true;
    filter["Trip"][0]["LegList"]["Leg"][0]["Origin"]["time"] = true;

    filter["Trip"][0]["LegList"]["Leg"][0]["Destination"]["name"] = true;
    filter["Trip"][0]["LegList"]["Leg"][0]["Destination"]["time"] = true;

    filter["Trip"][0]["LegList"]["Leg"][0]["type"] = true;
    filter["Trip"][0]["LegList"]["Leg"][0]["name"] = true;
    filter["Trip"][0]["LegList"]["Leg"][0]["number"] = true;
    filter["Trip"][0]["LegList"]["Leg"][0]["direction"] = true;

    JsonDocument document;

    DeserializationError error = deserializeJson(
        document,
        input,
        DeserializationOption::Filter(filter)
    );

    if (error) {
        return false;
    }

    if (!document["Trip"].is<JsonArray>()) {
        return false;
    }

    JsonArray trips = document["Trip"].as<JsonArray>();
    JourneyResults updatedResults;

    for (JsonVariant tripValue : trips) {
        if (updatedResults.getCount() >= 5) {
            break;
        }

        if (!tripValue.is<JsonObject>()) {
            return false;
        }

        Journey parsedJourney;

        if (!parseJourney(
                tripValue.as<JsonObject>(),
                parsedJourney
            )) {
            return false;
        }

        if (!updatedResults.addJourney(parsedJourney)) {
            return false;
        }
    }

    result = updatedResults;
    return true;
}