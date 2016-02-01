#ifndef MODBUSCHANNEL_HPP
#define MODBUSCHANNEL_HPP

#include <modbus/modbus.h>
#include <sys/time.h>
#include <string>
#include <cerrno>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#define MODBUS_DEFAULT_PORT MODBUS_TCP_DEFAULT_PORT
#define MODBUS_TIMEOUT 2

class ModbusChannel; //fwd declaration
typedef boost::shared_ptr<ModbusChannel> ModbusChannel_sp;


ModbusChannel_sp 
get_modbus_channel(const char* server_ip, 
                   int server_port=MODBUS_DEFAULT_PORT);

class ModbusError: public std::runtime_error
{
    public:
        ModbusError(const char *msg): std::runtime_error(std::string(msg)){};
};

class ModbusTimeout : public ModbusError
{
    public:
        ModbusTimeout(const char *msg): ModbusError(msg){};
};

/**
 * utility called for raising the proper errors during execution of
 * modbus methods
 */
void modbus_check_error(int rc);

class ModbusChannel
{
    public:
        /**
         * Ctor
         * @param ip: the ip of the modbus server
         * @param port: the modbus server port, defaults to 502 as specified by 
         *              modbus refence documentation
         */
        ModbusChannel(const char* server_ip, 
                      int server_port = MODBUS_DEFAULT_PORT);
        /**
         * destructor
         */
        virtual ~ModbusChannel();
        /**
         * Synchronized
         */
        void connect(); //throw ModbusError
        /**
         * Synchronized
         */
        void disconnect();
        inline bool is_connected() {return _is_connected;};
        /**
         * Read from the server the specified bytes into the dest_buffer. 
         * It verifies that the specified address is even and also
         * size is even so that it can use 16 bit words addressing as specified
         * by modbus library. If something goes wrong raises a ModbusError.
         * @param address: the address in bytes of the registers to be read
         * @param size: the size in bytes of the buffer to be read
         * @param dest_buffer: the binary content where data will be copied
         */
        void read(int address, int size, unsigned char *dest_buffer); //throw ModbusError
        /**
         * Write the content of source_buffer at the specified address in the
         * server. It verifies that the destination address is even and also
         * size is even so that it can use 16 bit words addressing as specified
         * by modbus library. If something goes wrong raises a ModbusError.
         * @param address: the address in bytes of the registers to be written
         * @param size: the size in bytes of the buffer to be written
         * @param source_buffer: the binary content to be copied into the server
         */
        void write(int address, int size, const unsigned char *source_buffer); //throw ModbusError
        /**
         * Template method used for directly unpacking the read value at
         * specified address, into the return type
         */
        template<typename T>
        T get(int address); //throw ModbusError
    private:
        /* class members */
        boost::mutex _connection_guard;
        std::string _server_ip;
        int _server_port;
        bool _is_connected;
        modbus_t *_modbus_context;

        /* We do not want this object to be copied so we prohibit copying
         * operations by definition , the _modbus_context is thread safe 
         * so the ModbusChannel can be shared among many threads
         */
        ModbusChannel(const ModbusChannel&);
        ModbusChannel& operator=(const ModbusChannel&);
};

template <typename T>
T ModbusChannel::get(int address)
{
    T return_value;
    this->read(address, sizeof(T), (unsigned char*)(&return_value));
    return return_value;
}


#endif

