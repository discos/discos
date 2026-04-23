#include <iostream>
#include <thread>
#include <chrono>
#include "bulkDataZMQPubSub.h"

using namespace bulkdataZMQImpl;

#define FLOW_NUMBER 1

class TestCallback : public BulkDataZMQCallback {
public:
    int cbStart(ACE_Message_Block * userParam_p = 0) override {
        if (userParam_p == nullptr || userParam_p->length() == 0) {
            std::cout << "Callback Start: FAIL" << std::endl;
            return 0;
        }
        else if (userParam_p->rd_ptr() == nullptr) {
            std::cout << "Callback Start: FAIL - Null data pointer" << std::endl;
            return 0;
        }
        std::cout << "Callback Start: PASS" << std::endl; 
        //printBuffer("User Param: ", std::vector<uint8_t>(userParam_p->rd_ptr(), userParam_p->rd_ptr() + userParam_p->length()));
        return 0;
    };
    int cbReceive(ACE_Message_Block * frame_p) override {
        if (frame_p == nullptr || frame_p->length() == 0) {
            std::cout << "Callback Receive: FAIL" << std::endl;
            return 0;
        }
        else if (frame_p->rd_ptr() == nullptr) {
            std::cout << "Callback Receive: FAIL - Null data pointer" << std::endl;
            return 0;
        }
        std::cout << "Callback Receive: PASS" << std::endl;

        //printBuffer("Received Frame: ", std::vector<uint8_t>(frame_p->rd_ptr(), frame_p->rd_ptr() + frame_p->length()));
        return 0;
    };
    int cbStop() override {
        std::cout << "Callback Stop: PASS" << std::endl;
        return 0;
    };
}; 

int main() {
    BulkDataZMQPublisher publisher;
    BulkDataZMQSubscriber subscriberSync,subscriberAsync;
    TZMQConfig conf;
    conf.endPoint = "tcp://127.0.0.1:5555";
    conf.tcpNoDelay = true;
    conf.cpuAffinity = false;
    conf.flowNumber = FLOW_NUMBER;
    conf.backendName = "TestBackend";
    TestCallback callback;
    std::vector<uint8_t> startMsg= {100, 110, 120, 130, 140};
    std::vector<uint8_t> testData = {1, 2, 3, 4, 5};
    std::vector<uint8_t> testData2 = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    
    // Setup
    publisher.initialize(conf);
    subscriberSync.initialize(conf, nullptr);
    subscriberAsync.initialize(conf, &callback);

    // Ensure subscriber is ready before publishing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    bool allPassed = true;
    
    // Test PublisherInitialization
    if (publisher.isInitialized()) {
        std::cout << "PublisherInitialization: PASS" << std::endl;
    } else {
        std::cout << "PublisherInitialization: FAIL" << std::endl;
        allPassed = false;
    }
    
    // Test SubscriberInitialization
    if (subscriberSync.isInitialized()) {
        std::cout << "SyncSubscriberInitialization: PASS" << std::endl;
    } else {
        std::cout << "SyncSubscriberInitialization: FAIL" << std::endl;
        allPassed = false;
    }

    if (subscriberAsync.isInitialized()) {
        std::cout << "AsyncSubscriberInitialization: PASS" << std::endl;
    } else {
        std::cout << "AsyncSubscriberInitialization: FAIL" << std::endl;
        allPassed = false;
    }
    
    // Test PublishData
    //test startSend
   
    try {
        publisher.startSend(FLOW_NUMBER, (const char *)startMsg.data(), startMsg.size());
        std::cout << "PublishStartSend: PASS" << std::endl;
    } catch (const ACSBulkDataError::AVSendFrameErrorExImpl& e) {
        std::cout << "PublishStartSend: FAIL" <<  std::endl;
        allPassed = false;
    }   
    //test sendData with ACE_Message_Block
    try {
        publisher.sendData(FLOW_NUMBER, (const char *)testData.data(), testData.size());
        std::cout << "PublishData: PASS" << std::endl;
    } catch (const ACSBulkDataError::AVSendFrameErrorExImpl& e) {
        std::cout << "PublishData: FAIL" <<  std::endl;
        allPassed = false;
    }
    std::thread pubThread([&publisher, testData2]() {
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            publisher.sendData(FLOW_NUMBER, (const char *)testData2.data(),testData2.size());
        } catch (const ACSBulkDataError::AVSendFrameErrorExImpl& e) {
            // Exception in thread: could log, but for test, ignore
        }
    });
    
    std::vector<uint8_t> receivedData;
    bool received = subscriberSync.receive(receivedData, 500); //this should receive the startSend message first
    /*cout << "Received Data: ";
    for (auto byte : receivedData) {
        cout << (int)byte << " ";
    }
    cout << endl;*/
    if (received && startMsg == receivedData) {
        std::cout << "Header message received: PASS" << std::endl;
    } else {
        std::cout << "Header message received: FAIL" << std::endl;
        allPassed = false;
    }
    received = subscriberSync.receive(receivedData, 500);
    /*cout << "Received Data: ";
    for (auto byte : receivedData) {
        cout << (int)byte << " ";
    }
    cout << endl;*/
    if (received && testData == receivedData) {
        std::cout << "Data message 1 received: PASS" << std::endl;
    } else {
        std::cout << "Data message 1 received: FAIL" << std::endl;
        allPassed = false;
    }
    received = subscriberSync.receive(receivedData, 500);
    /*cout << "Received Data: ";
    for (auto byte : receivedData) {
        cout << (int)byte << " ";
    }
    cout << endl;*/
    if (received && testData2 == receivedData) {
        std::cout << "Data message 2 received: PASS" << std::endl;
    } else {
        std::cout << "Data message 2 received: FAIL" << std::endl;
        allPassed = false;
    }
    pubThread.join();

    try {
        publisher.stopSend(FLOW_NUMBER);
        std::cout << "PublishClose: PASS" << std::endl;
    } catch (const ACSBulkDataError::AVSendFrameErrorExImpl& e) {
        std::cout << "PublishClose: FAIL" <<  std::endl;
        allPassed = false;
    }
    
    
    // Teardown
    publisher.close();
    subscriberSync.close();
    subscriberAsync.close();
    
    return allPassed ? 0 : 1;
}

