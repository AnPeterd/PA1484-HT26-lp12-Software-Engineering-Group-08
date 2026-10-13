#pragma once
#include <HTTPClient.h>
#include "stops.hpp"
#include "WiFi.hpp"

int8_t request_send(String stop_id);

bool stop_lookup (String user_input); // ONLY ASCII ALLOWED! (or I shoud write some url-refining function)

int8_t refresh_departures(String stop_id);