#include "ModbusChannel.h"

ModbusChannel_sp
get_modbus_channel(const char *server_ip, int server_port)
{
    ModbusChannel_sp _modbus_channel(new ModbusChannel(server_ip, server_port));
    _modbus_channel->connect();
    return _modbus_channel;
};

void
modbus_check_error(int rc)
{
    if( rc == -1)
    {
        if(errno == ETIMEDOUT){
            throw ModbusTimeout(modbus_strerror(errno));
        }else{
            throw ModbusError(modbus_strerror(errno));
        }
    }
};

ModbusChannel::ModbusChannel(const char* server_ip, int server_port) :
    _server_ip(std::string(server_ip)),
    _server_port(server_port),
    _is_connected(false),
    _modbus_context(NULL)
{
    int rc;
    this->_modbus_context = modbus_new_tcp(this->_server_ip.c_str(), this->_server_port);
    if(this->_modbus_context == NULL)
    {
        throw ModbusError(modbus_strerror(errno));
    }
    rc = modbus_set_error_recovery(this->_modbus_context,
                                   (modbus_error_recovery_mode)
                                   (MODBUS_ERROR_RECOVERY_PROTOCOL |
                                   MODBUS_ERROR_RECOVERY_LINK) );
    modbus_check_error(rc);
    timeval _response_timeout;
    _response_timeout.tv_sec = MODBUS_TIMEOUT;
    _response_timeout.tv_usec = 0;
    modbus_set_response_timeout(this->_modbus_context, &_response_timeout);
};

ModbusChannel::~ModbusChannel()
{
    this->disconnect();
    modbus_free(this->_modbus_context);
};

void
ModbusChannel::connect()
{
    boost::mutex::scoped_lock lock(_connection_guard);
    int rc;
    if(!(this->_is_connected))
    {
        rc = modbus_connect(this->_modbus_context);
        modbus_check_error(rc);
        this->_is_connected = true;
    }
};

void
ModbusChannel::disconnect()
{
    boost::mutex::scoped_lock lock(_connection_guard);
    if(this->_is_connected){
        modbus_close(this->_modbus_context);
        this->_is_connected = false;
    }
};

void 
ModbusChannel::read(int address, int size, unsigned char *dest)
{
    int rc;
    if(!(size % 2 == 0))
    {
        throw ModbusError("data transfer size must be even");
    }
    int _size = size / 2;
    if(!(address % 2 == 0))
    {
        throw ModbusError("destination address must be even");
    }
    int _address = address / 2;
    rc = modbus_read_registers(this->_modbus_context, _address, _size,
                                (uint16_t *)dest);
    modbus_check_error(rc);
};

void 
ModbusChannel::write(int address, int size, const unsigned char *source_buffer)
{
    int rc;
    if(!(size % 2 == 0))
    {
        throw ModbusError("data transfer size must be even");
    }
    int _size = size / 2;
    if(!(address % 2 == 0))
    {
        throw ModbusError("destination address must be even");
    }
    int _address = address / 2;
    rc = modbus_write_registers(this->_modbus_context, _address, _size,
                                (uint16_t *)source_buffer);
    modbus_check_error(rc);
};

