#include "bulkDataZMQ.h"
#include <iostream>

using namespace bulkdataZMQImpl;

bool ZmqSender::initialize(const std::string& endpoint) {
    try {
        context = ZMQLibrary::ZMQContext::getInstance();
        socket = std::make_shared<zmq::socket_t>(*context, zmq::socket_type::pub);
        socket->bind(endpoint);
        return true;
    } catch (const zmq::error_t& e) {
        captureError(e.what());
        socket.reset();
        context.reset();
        return false;
    }
}

bool ZmqSender::send(const uint8_t* data, size_t size) {
    if (!socket) {
        last_error = "Socket is not initialized.";
        return false;
    }
    try {
        socket->send(zmq::const_buffer(data, size), zmq::send_flags::none);
        return true;
    } catch (const zmq::error_t& e) {
        captureError(e.what());
        return false;
    }
}

bool ZmqSender::send(const std::string& message) {
    return send(reinterpret_cast<const uint8_t*>(message.data()), message.size());
}

void ZmqSender::close() {
    if (socket) {
        socket->close();
        socket.reset();
    }
    context.reset();
}

bool ZmqReceiver::initialize(const std::string& endpoint, const std::string& topic) {
    try {
        context = ZMQLibrary::ZMQContext::getInstance();
        socket = std::make_shared<zmq::socket_t>(*context, zmq::socket_type::sub);
        socket->connect(endpoint);
        socket->set(zmq::sockopt::subscribe, topic);
        return true;
    } catch (const zmq::error_t& e) {
        captureError(e.what());
        socket.reset();
        context.reset();
        return false;
    }
}

bool ZmqReceiver::receiveSync(std::vector<uint8_t>& out_buffer, long timeout_ms) {
    if (!socket) {
        captureError("Socket is not initialized.");
        return false;
    }
    try {
        zmq::pollitem_t items[] = { { static_cast<void*>(*socket), 0, ZMQ_POLLIN, 0 } };
        int rc = zmq::poll(items, 1, std::chrono::milliseconds(timeout_ms));
        if (rc == 0) {
            return false;
        }
        if (items[0].revents & ZMQ_POLLIN) {
            return readMessageFromSocket(out_buffer);
        }
        return false;
    } catch (const zmq::error_t& e) {
        captureError(e.what());
        return false;
    }
}

bool ZmqReceiver::startAsync(std::function<void(const std::vector<uint8_t>&)> callback) {
    if (!socket) {
        captureError("Socket is not initialized.");
        return false;
    }
    if (is_running.load()) {
        captureError("Async thread is already running.");
        return false;
    }
    is_running.store(true);
    try {
        async_thread = std::thread([this, callback]() {
            std::vector<uint8_t> buffer;
            while (is_running.load()) {
                if (receiveSync(buffer, 100)) {
                    //printBuffer("ZMQ receiver - Async Received: ", buffer);
                    callback(buffer);
                }
            }
        });
    } catch (const std::exception& e) {
        is_running.store(false);
        captureError(std::string("Failed to spawn thread: ") + e.what());
        return false;
    } catch (...) {
        is_running.store(false);
        captureError("Unknown error occurred while spawning thread.");
        return false;
    }
    return true;
}

void ZmqReceiver::stopAsync() {
    if (is_running.load()) {
        is_running.store(false);
        if (async_thread.joinable()) {
            async_thread.join();
        }
    }
}

/**
 * @brief Closes the ZeroMQ socket and terminates the context.
 */
void ZmqReceiver::close() {
    stopAsync();
    if (socket) {
        socket->close();
        socket.reset();
    }
    context.reset();
}

void ZmqReceiver::printBuffer(const std::string message, const std::vector<uint8_t>& buffer) {
    std::cout << message;
    for (auto byte : buffer) {
        std::cout << (int)byte << " ";
    }
    std::cout << std::endl;
}

bool ZmqReceiver::readMessageFromSocket(std::vector<uint8_t>& out_buffer) {
    try {
        zmq::message_t msg;
        auto result = socket->recv(msg, zmq::recv_flags::none);
        if (!result) {
            captureError("recv returned no data (non-blocking EAGAIN).");
            return false;
        }
        out_buffer.assign(
            static_cast<uint8_t*>(msg.data()),
            static_cast<uint8_t*>(msg.data()) + msg.size()
        );
        return true;
    } catch (const zmq::error_t& e) {
        captureError(e.what());
        return false;
    }
}
