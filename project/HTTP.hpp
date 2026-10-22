#pragma once
#include <HTTPClient.h>
#include "stops.hpp"
#include "WiFi.hpp"
#include "departures.hpp"

int8_t request_send(String stop_id, StopData& result);


int8_t refresh_departures(String stop_id, StopData& result);