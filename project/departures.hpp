#pragma once
#include <string>

class Bus{

    private: 
        std::string busLine;
        std::string time;
        std::string destination;
        std::string delays;

    public:

    Bus(std::string bL="", std::string time="", std::string des="", std::string del="") : 
    busLine{bL}, time{time}, destination{des}, delays{del} {}

};


class BusStop{

    private:
        std::string name;
        Bus allOngoingBuses[5] = {};
        int count;

    public: 

        BusStop(std::string name) : name{name} 
        {count=0;}

};