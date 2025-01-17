#include "ZMQPublisher.hpp"
#include <thread>
#include <chrono>
#include <iostream>

int main()
{
    ZMQPublisher publisher("cpp");

    std::cout << "Started cpp ZMQPublisher on topic '" << publisher.topic << "'" << std::endl;

    while(true)
    {
        ZMQDictionary dictionary;
        dictionary["string"] = std::string("string value");
        char* c = strdup("char value");
        dictionary["charstring"] = c;
        dictionary["integer"] = 5;
        // This is a double
        dictionary["timestamp"] = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();

        publisher.publish(dictionary);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
