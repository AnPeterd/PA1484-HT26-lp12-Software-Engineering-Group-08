#pragma once
#include <Arduino.h>

class Departure {
private:
    String line;
    String time;
    String destination;
    String status;
    String transportMode;
    bool canceled;

public:
    Departure(
        const String& lineValue = "",
        const String& timeValue = "",
        const String& destinationValue = "",
        const String& statusValue = "",
        const String& transportModeValue = "",
        bool canceledValue = false
    ) : line{lineValue}, time{timeValue}, destination{destinationValue}, status{statusValue}, transportMode{transportModeValue}, canceled{canceledValue}{}

    const String& getLine() const { return line; }

    const String& getTime() const { return time; }

    const String& getDestination() const { return destination; }

    const String& getStatus() const { return status; }

    const String& getTransportMode() const { return transportMode;}

    bool isCanceled() const { return canceled; }
};

class StopData {
private:
    String name;
    Departure departures[5] = {};
    int count = 0;

public:
    StopData(const String& stopName) : name{stopName}{}

    const String& getName() const { return name; }

    int getCount() const { return count; }

    bool addDeparture(const Departure& departure) {
        if (count >= 5) { return false; }

        departures[count] = departure;
        ++count;

        return true;
    }

    const Departure* getDeparture(int index) const {
        if (index < 0 || index >= count) { return nullptr; }

        return &departures[index];
    }
};