#pragma once

#include <Arduino.h>
#include "departures.hpp"

bool parseDepartures(Stream& input, ParsedStopData& result, const String& selectedTransport = "ALL");