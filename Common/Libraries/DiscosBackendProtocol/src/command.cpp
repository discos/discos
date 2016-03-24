#include "command.hpp"

using namespace DiscosBackend;

Message
Command::setConfiguration(string conf)
{
    Message command(REQUEST, "set-configuration");
    command.add_argument<string>(conf);
    return command;
}

Message
Command::setIntegration(int integration)
{
    Message command(REQUEST, "set-integration");
    command.add_argument<int>(integration);
    return command;
}

Message
Command::start(unsigned long long utctimestamp)
{
    Message command(REQUEST, "start");
    if(utctimestamp > 0)
        command.add_argument<unsigned long long>(utctimestamp);
    return command;
}

Message
Command::stop(unsigned long long utctimestamp)
{
    Message command(REQUEST, "stop");
    if(utctimestamp > 0)
        command.add_argument<unsigned long long>(utctimestamp);
    return command;
}

Message 
Command::setSection(long input, double freq, double bandwidth, 
                    long feed, long pol, double sampleRate, 
                    long bins)
{
    Message command(REQUEST, "set-section");
    command.add_argument<long>(input);
    command.add_argument<double>(freq);
    command.add_argument<double>(bandwidth);
    command.add_argument<long>(feed);
    command.add_argument<long>(pol);
    command.add_argument<double>(sampleRate);
    command.add_argument<long>(bins);
    return command;
}

Message
Command::calOn(long interleave)
{
    Message command(REQUEST, "cal-on");
    if(interleave > 0)
        command.add_argument<long>(interleave);
    return command;
}

Message
Command::setFilename(string filename)
{
    Message command(REQUEST, "set-filename");
    command.add_argument<string>(filename);
    return command;
}

