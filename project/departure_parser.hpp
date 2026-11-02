#pragma once

#include <Arduino.h>
#include "departures.hpp"

bool parseDepartures(Stream& input, StopData& result, const String& selectedTransport = "ALL");