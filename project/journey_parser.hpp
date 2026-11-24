#pragma once

#include <Arduino.h>
#include "journeys.hpp"

bool parseJourneys(Stream& input, JourneyResults& result);