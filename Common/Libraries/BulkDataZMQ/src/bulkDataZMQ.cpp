#include "bulkDataZMQ.h"
#include <iostream>

using namespace bulkdataZMQImpl;

void ZmqSender::captureError() {
    last_error = zmq_strerror(zmq_errno());
}

bool ZmqSender::initialize(const std::string& endpoint) {
    context = zmq_ctx_new();
    if (!context) {
        captureError();
        return false;
    }
    socket = zmq_socket(context, ZMQ_PUB);
    if (!socket) {
        captureError();
        close();
        return false;
    }
    int rc = zmq_bind(socket, endpoint.c_str());
    if (rc != 0) {
        captureError();
        close();
        return false;
    }
    return true;
}

bool ZmqSender::send(const uint8_t* data, size_t size) {
    if (!socket) {
        last_error = "Socket is not initialized.";
        return false;
    }
    int rc = zmq_send(socket, data, size, 0);
    if (rc == -1) {
        captureError();
        return false;
    }        
    return true;
}

bool ZmqSender::send(const std::string& message) {
    return send(reinterpret_cast<const uint8_t*>(message.data()), message.size());
}

void ZmqSender::close() {
    if (socket) {
        zmq_close(socket);
        socket = nullptr;
    }
    if (context) {
        zmq_ctx_term(context);
        context = nullptr;
    }
}

bool ZmqReceiver::initialize(const std::string& endpoint, const std::string& topic) {
    context = zmq_ctx_new();
    if (!context) {
        captureError();
        return false;
    }
    socket = zmq_socket(context, ZMQ_SUB);
    if (!socket) {
        captureError();
        close();
        return false;
    }
    int rc = zmq_connect(socket, endpoint.c_str());
    if (rc != 0) {
        captureError();
        close();
        return false;
    }
    rc = zmq_setsockopt(socket, ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    if (rc != 0) {
        captureError();
        close();
        return false;
    }
    return true;
}

bool ZmqReceiver::receiveSync(std::vector<uint8_t>& out_buffer, long timeout_ms) {
    if (!socket) {
        captureError("Socket is not initialized.");
        return false;
    }
    zmq_pollitem_t items[] = { { socket, 0, ZMQ_POLLIN, 0 } };
    int rc = zmq_poll(items, 1, timeout_ms);
    if (rc == -1) {
        captureError(); // Actual ZMQ error
        return false; 
    }
    if (rc == 0) {
        return false;  // Timeout occurred (not treated as an error state)
    }
    if (items[0].revents & ZMQ_POLLIN) {
        return readMessageFromSocket(out_buffer);
    }
    return false;
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
        zmq_close(socket);
        socket = nullptr;
    }
    if (context) {
        zmq_ctx_term(context);
        context = nullptr;
    }
}

void ZmqReceiver::printBuffer(const std::string message, const std::vector<uint8_t>& buffer) {
    std::cout << message;
    for (auto byte : buffer) {
        std::cout << (int)byte << " ";
    }
    std::cout << std::endl;
    }

bool ZmqReceiver::readMessageFromSocket(std::vector<uint8_t>& out_buffer) {
    zmq_msg_t msg;
    if (zmq_msg_init(&msg) != 0) {
        captureError();
        return false;
    }
    int rc = zmq_msg_recv(&msg, socket, 0);
    if (rc == -1) {
        captureError();
        zmq_msg_close(&msg);
        return false;
    }
    size_t size = zmq_msg_size(&msg);
    uint8_t* data_ptr = static_cast<uint8_t*>(zmq_msg_data(&msg));
    out_buffer.assign(data_ptr, data_ptr + size);
    zmq_msg_close(&msg);
    return true;
}