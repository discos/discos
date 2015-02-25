#ifndef MEDMINORSERVOMODBUS_HPP
#define MEDMINORSERVOMODBUS_HPP

#include <boost/shared_ptr.hpp>
#include <boost/timer.hpp>

#include "IRA"

#include "ModbusChannel.hpp"
#include "MedMinorServoProtocol.hpp"

#define PROTOCOLTIMEOUT 2 * 10000000

class MedMinorServoModbus; //fwd decl.
typedef boost::shared_ptr<MedMinorServoModbus> MedMinorServoModbus_sp;

MedMinorServoModbus_sp 
get_med_modbus_channel(const char* server_ip, 
                       int server_port=MODBUS_DEFAULT_PORT); //throw ModbusError

class MedMinorServoModbus : public ModbusChannel
{
    public:
        MedMinorServoModbus(const char* server_ip, 
                      int server_port = MODBUS_DEFAULT_PORT); 
        /**
         * Read the actual status of the servo 
         * @throw ModbusTimeout: if timeout on modbus channel protocol
         */
        MEDMINORSERVOSTATUS read_status(); //throw ModbusError, ModbusTimeout
        /**
         * Write a structured command on the modbus channel
         * This method is not synchronized but must be executed mutually
         * whenever there is the possibility of someone requesting an ack
         * @param command: a structured command
         * @param wait_ack: wether to poll the server until ack is read
         * @throw ModbusTimeout: if timeout on modbus channel protocol or servo ack
         */
        void write_command(MEDMINORSERVOSETPOS& command, 
                           bool wait_ack = true); //throw ModbusError, ModbusTimeout
        void set_timeout(double timeout){ _timeout = timeout;};
        double get_timeout(){return _timeout;};
    private:
        double _timeout;
        //static uint64_t _command_index;
        MedMinorServoModbus(const ModbusChannel&);
        MedMinorServoModbus& operator=(const ModbusChannel&);
};

#endif

