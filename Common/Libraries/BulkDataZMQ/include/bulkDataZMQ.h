#ifndef _BULKDATAZMQ_H
#define _BULKDATAZMQ_H

#include <zmq.h>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

namespace bulkdataZMQImpl {

/**
 * @class ZmqSender
 * @brief A wrapper class for ZeroMQ PUB socket operations.
 */
class ZmqSender {
protected:
    void* context; ///< ZeroMQ context pointer
    void* socket;  ///< ZeroMQ socket pointer
    std::string last_error; ///< Stores the last error message encountered

    /**
     * @brief Helper to capture the last ZeroMQ error message.
     */
    void captureError(); 

public:
    /**
     * @brief Constructor for ZmqSender.
     */
    ZmqSender() : context(nullptr), socket(nullptr) {}

    /**
     * @brief Destructor for ZmqSender, ensures resources are closed.
     */
    ~ZmqSender() {
        close();
    }

    /**
     * @brief Retrieve the most recent error message.
     * @return A string containing the last error description.
     */
    std::string getLastError() const {
        return last_error;
    }

    /**
     * @brief Checks if the sender is initialized.
     * @return true if context and socket are set, false otherwise.
     */
    bool isInitialized() const {
        return context != nullptr && socket != nullptr;
    }

    /**
     * @brief Initializes the ZeroMQ context and binds the PUB socket to an endpoint.
     * @param endpoint The connection string (e.g., "tcp: / / *:5555").
     * @return true if initialization and binding succeeded, false otherwise.
     */
    bool initialize(const std::string& endpoint);

    /**
     * @brief Sends raw binary data over the socket.
     * @param data Pointer to the data buffer.
     * @param size Size of the data in bytes.
     * @return true if the message was sent successfully, false otherwise.
     */
    bool send(const uint8_t* data, size_t size);

    /**
     * @brief Sends a string message over the socket.
     * @param message The string to be sent.
     * @return true if the message was sent successfully, false otherwise.
     */
    bool send(const std::string& message);

    /**
     * @brief Closes the ZeroMQ socket and terminates the context.
     */
    void close();

};

/**
 * @class ZmqReceiver
 * @brief A wrapper class for ZeroMQ SUB socket operations, supporting both synchronous and asynchronous reception.
 */
class ZmqReceiver {
protected:
    void* context; ///< ZeroMQ context pointer
    void* socket;  ///< ZeroMQ socket pointer
    
    std::thread async_thread; ///< Thread handle for asynchronous reception
    std::atomic<bool> is_running; ///< Atomic flag to control the async thread lifecycle

    mutable std::mutex error_mutex; ///< Mutex to protect access to last_error
    std::string last_error; ///< Stores the last error message encountered

    /**
     * @brief Thread-safe helper to capture ZeroMQ errors or set custom error messages.
     * @param custom_msg Optional custom error string. If empty, zmq_strerror is used.
     */
    void captureError(const std::string& custom_msg = "") {
        std::lock_guard<std::mutex> lock(error_mutex);
        if (!custom_msg.empty()) {
            last_error = custom_msg;
        } else {
            int err = zmq_errno();
            last_error = zmq_strerror(err);
        }
    }
    /**
     * @brief Utility function to print the contents of a buffer in a human-readable format.
     * @param buffer The data buffer to print. 
     */
    void printBuffer(const std::string message, const std::vector<uint8_t>& buffer);
    
public:
    /**
     * @brief Constructor for ZmqReceiver.
     */
    ZmqReceiver() : context(nullptr), socket(nullptr), is_running(false) {}

    /**
     * @brief Destructor for ZmqReceiver, ensures threads are stopped and resources are closed.
     */
    ~ZmqReceiver() {
        stopAsync();
        close();
    }

    /**
     * @brief Thread-safe retrieval of the most recent error message.
     * @return A string containing the last error description.
     */
    std::string getLastError() const {
        std::lock_guard<std::mutex> lock(error_mutex);
        return last_error;
    }

    /**
     * @brief Checks if the receiver is initialized.
     * @return true if context and socket are set, false otherwise.
     */
    bool isInitialized() const {
        return context != nullptr && socket != nullptr;
    }

    /**
     * @brief Initializes the ZeroMQ context and connects the SUB socket to an endpoint.
     * @param endpoint The connection string (e.g., "tcp://127.0.0.1:5555").
     * @param topic The topic string to subscribe to (defaults to all).
     * @return true if initialization, connection, and subscription succeeded, false otherwise.
     */
    bool initialize(const std::string& endpoint, const std::string& topic = "");

    /**
     * @brief Synchronously polls and receives a message from the socket.
     * @param out_buffer Vector to be populated with the received data.
     * @param timeout_ms Maximum time to wait for a message in milliseconds.
     * @return true if a message was received, false on timeout or error.
     */
    bool receiveSync(std::vector<uint8_t>& out_buffer, long timeout_ms);

    /**
     * @brief Starts a background thread that continuously receives messages and invokes a callback.
     * @param callback A function to be called with the received data buffer.
     * @return true if the thread started successfully, false if already running or on error.
     */
    bool startAsync(std::function<void(const std::vector<uint8_t>&)> callback);

    /**
     * @brief Signals the asynchronous thread to stop and joins it.
     */
    void stopAsync();

    /**
     * @brief Closes the ZeroMQ socket and terminates the context.
     */
    void close();

private:
    /**
     * @brief Internal helper to read a message from the ZMQ socket into a vector.
     * @param out_buffer The destination vector.
     * @return true if the read was successful.
     */
    bool readMessageFromSocket(std::vector<uint8_t>& out_buffer);
};

}
#endif /*!_BULKDATAZMQ_H*/

