#pragma once

#include <Arduino.h>

class JourneyLeg {
private:
    String originName;
    String originTime;
    String destinationName;
    String destinationTime;
    String transportType;
    String serviceName;
    String line;
    String direction;

public:
    JourneyLeg(
        const String& originNameValue = "",
        const String& originTimeValue = "",
        const String& destinationNameValue = "",
        const String& destinationTimeValue = "",
        const String& transportTypeValue = "",
        const String& serviceNameValue = "",
        const String& lineValue = "",
        const String& directionValue = ""
    )
        : originName{originNameValue},
          originTime{originTimeValue},
          destinationName{destinationNameValue},
          destinationTime{destinationTimeValue},
          transportType{transportTypeValue},
          serviceName{serviceNameValue},
          line{lineValue},
          direction{directionValue}
    {}

    const String& getOriginName() const { return originName; }
    const String& getOriginTime() const { return originTime; }

    const String& getDestinationName() const { return destinationName; }
    const String& getDestinationTime() const { return destinationTime; }

    const String& getTransportType() const { return transportType; }
    const String& getServiceName() const { return serviceName; }
    const String& getLine() const { return line; }
    const String& getDirection() const { return direction; }

    bool isWalking() const {
        return transportType.equalsIgnoreCase("WALK");
    }
};

class Journey {
private:
    static const int MAX_LEGS = 8;

    String originName;
    String originTime;
    String destinationName;
    String destinationTime;

    JourneyLeg legs[MAX_LEGS] = {};
    int legCount = 0;

public:
    Journey(
        const String& originNameValue = "",
        const String& originTimeValue = "",
        const String& destinationNameValue = "",
        const String& destinationTimeValue = ""
    )
        : originName{originNameValue},
          originTime{originTimeValue},
          destinationName{destinationNameValue},
          destinationTime{destinationTimeValue}
    {}

    const String& getOriginName() const { return originName; }
    const String& getOriginTime() const { return originTime; }

    const String& getDestinationName() const { return destinationName; }
    const String& getDestinationTime() const { return destinationTime; }

    int getLegCount() const { return legCount; }

    bool addLeg(const JourneyLeg& leg) {
        if (legCount >= MAX_LEGS) {
            return false;
        }

        legs[legCount] = leg;
        ++legCount;
        return true;
    }

    const JourneyLeg* getLeg(int index) const {
        if (index < 0 || index >= legCount) {
            return nullptr;
        }

        return &legs[index];
    }
};

class JourneyResults {
private:
    static const int MAX_JOURNEYS = 5;

    Journey journeys[MAX_JOURNEYS] = {};
    int count = 0;

public:
    int getCount() const { return count; }

    bool addJourney(const Journey& journey) {
        if (count >= MAX_JOURNEYS) { return false; }

        journeys[count] = journey;
        ++count;
        return true;
    }

    const Journey* getJourney(int index) const {
        if (index < 0 || index >= count) { return nullptr; }

        return &journeys[index];
    }
};