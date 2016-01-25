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
            static Message getTpi(){return Message(REQUEST, "get-tpi");};
            static Message getTp0(){return Message(REQUEST, "get-tp0");};
            static Message time(){return Message(REQUEST, "time");};
            static Message start(unsigned long long utctimestamp = 0);
            static Message stop(unsigned long long utctimestamp = 0);
            static Message setSection(long, double, double, long, long, double, long);
            static Message calOn(long interleave = 0);
            static Message setFilename(string filename);
            static Message doConversion(){return Message(REQUEST, "do-conversion");};
    }; //class Command
}; //namespace DiscosBackend

#endif

