#include "bulkDataZMQCallback.h"

using namespace bulkdataZMQImpl;

BulkDataZMQCallback::BulkDataZMQCallback() {}

BulkDataZMQCallback::~BulkDataZMQCallback() {}

void BulkDataZMQCallback::printBuffer(const std::string message, const std::vector<uint8_t>& buffer) {
    std::cout << message;
    for (auto byte : buffer) {
        std::cout << (int)byte << " ";
    }
    std::cout << std::endl;
}