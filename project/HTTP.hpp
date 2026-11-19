#pragma once
#include <HTTPClient.h>
#include "stops.hpp"
#include "WiFi.hpp"
#include "departures.hpp"

int8_t refresh_departures(String stop_id, StopData& result, const String& transport = "ALL");

int8_t route_refresh (String start_stop_id, String end_stop_id, StopData& result);

int8_t request_send(String url, StopData& result, const String& transport, StopData& result_route, int8_t request_type);