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
    ): line{lineValue}, time{timeValue}, destination{destinationValue}, status{statusValue}, transportMode{transportModeValue}, canceled{canceledValue}
    {}

    const String& getLine() { return line; }
    const String& getTime() { return time; }
    const String& getDestination() { return destination; }
    const String& getStatus() { return status; }
    const String& getTransportMode() { return transportMode; }
    bool isCanceled() { return canceled; }
};

class StopData{

    private: 
        String name;
        Departure departures[5] = {};
        int count = 0;

    public: 
        StopData(const String& stopName) : name{stopName} {}

};