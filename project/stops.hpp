#pragma once
#include <stddef.h>

struct Stop { // Structure for stops
    const char* name;
    const char* id;
};

extern const Stop STOPS[];
extern const size_t STOP_COUNT;

extern Stop currentStop;

bool selectStop(size_t index);