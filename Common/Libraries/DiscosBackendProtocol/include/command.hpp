#ifndef DISCOS_BACKEND_COMMANDS_HPP
#define DISCOS_BACKEND_COMMANDS_HPP

#include "protocol.hpp"
#include <string>

using namespace std;

namespace DiscosBackend{

    class Command
    {
        public:
            static Message status(){return Message(REQUEST, "status");};
            static Message version(){return Message(REQUEST, "version");};
            static Message getConfiguration(){return Message(REQUEST, "get-configuration");};
            static Message setConfiguration(string conf);
            static Message getIntegration(){return Message(REQUEST, "get-integration");};
            static Message setIntegration(int integration);
            static Message getTpi(double frequency = 0, double bandwidth = 0);
            static Message getTp0(){return Message(REQUEST, "get-tp0");};
            static Message time(){return Message(REQUEST, "time");};
            static Message start(unsigned long long utctimestamp = 0);
            static Message stop(unsigned long long utctimestamp = 0);
            static Message setSection(long, double, double, long, long, double, long);
            static Message calOn(long interleave = 0);
            static Message setFilename(string filename);
            static Message convertData(){return Message(REQUEST, "convert-data");};
            static Message getRms(){return Message(REQUEST, "get-rms");};
            static Message getDDCoffset(){return Message(REQUEST, "get-ddcoffset");};
            static Message setEnable(int feed1, int feed2);
            static Message endSchedule(){return Message(REQUEST, "end-schedule");};
            static Message backendPark(){return Message(REQUEST, "backend-park");};
            static Message setShift(int shift);
    }; //class Command
}; //namespace DiscosBackend

#endif

