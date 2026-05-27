/* Scheduler calls
sender->connect(receiver)       *
sender->sendHeader()            getSender()->startSend(FLOW_NUMBER,buffer,len); 
recv->setScanLayout()           //
recv->startScan()               //
sender->sendData()              backend Implementation dependent -> restart sending thread: *getSender->sendData(flow,ACE_Message_Block)
----- stop
sender->sendStop()              backend Implementation dependent -> suspend sending thread *getSender->stopSend(flow)
----- terminate
sender->sendStop()              backend Implementation dependent -> suspend sending thread
recv->stopScan()                //
recv->reset()                   //
sender->terminate()             backend Implementation dependent
sender->disconnect()            *
*/
#include <iostream>
#include <chrono>
#include "bulkDataZMQ.h"

using namespace bulkdataZMQImpl;

int main() {
    const std::string endpoint = "tcp://127.0.0.1:5555";

    // 1. Setup Sender
    ZmqSender sender;
    if (!sender.initialize(endpoint)) {
        std::cerr << "Failed to initialize sender. ZMQ Error: " << zmq_strerror(zmq_errno()) << "\n";
        return 1;
    }

    // 2. Setup Receiver (Async Mode)
    ZmqReceiver async_receiver;
    if (!async_receiver.initialize(endpoint)) {
        std::cerr << "Failed to initialize async receiver.\n";
        return 1;
    }

    auto onMessageReceived = [](const std::vector<uint8_t>& data) {
        std::string msg(data.begin(), data.end());
        std::cout << "[Async Callback] Received: " << msg << "\n";
    };

    if (!async_receiver.startAsync(onMessageReceived)) {
        std::cerr << "Failed to start async thread.\n";
        return 1;
    }

    // Give ZeroMQ a moment to connect the subscriber (PUB-SUB slow joiner syndrome)
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 3. Send data
    std::cout << "Sending messages...\n";
    sender.send("Hello from Sender!");
    sender.send("Message number 2");
    sender.send("!!!!!!!!!!!!!!!!!!!!!!!");

    // Let the async thread process the messages before exiting
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Cleanup happens automatically in destructors, 
    // but you can call .close() or .stopAsync() explicitly if you prefer.
    std::cout << "Shutting down...\n";
    
    return 0;
}