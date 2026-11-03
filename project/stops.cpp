#include "stops.hpp"

const Stop STOPS[] = {
    {"Campus Gräsvik", "740032188"},
    {"Karlskrona Centralstation", "740000230"},
    {"Karlskrona centrum", "740023372"},
    {"Kungsplan", "740020510"},
    {"Bergåsa station", "740001537"},
    {"Karlskrona Amiralen", "740031800"},
    {"Torskors", "740011646"},
    {"NKT Arena Karlskrona", "740032192"},
    {"Lyckeby centrum", "740000663"},
    {"Sjukhuset huvudentrén", "740024365"}
};

const size_t STOP_COUNT = sizeof(STOPS) / sizeof(STOPS[0]);

Stop currentStop = STOPS[0];

bool selectStop(size_t index){
    if(index>=STOP_COUNT){ return false; }
    currentStop = STOPS[index];
    return true; 
}