#include "MedMinorServoModbus.hpp"
#include <IRA>

//uint64_t MedMinorServoModbus::_command_index = 0;

MedMinorServoModbus::MedMinorServoModbus(const char* server_ip,
                                         int server_port) :
    ModbusChannel(server_ip, server_port),
    _timeout(PROTOCOLTIMEOUT)
{};

MedMinorServoModbus_sp
get_med_modbus_channel(const char *server_ip, int server_port)
{
    MedMinorServoModbus_sp _modbus_channel(new MedMinorServoModbus(server_ip, server_port));
    _modbus_channel->connect();
    return _modbus_channel;
};

MEDMINORSERVOSTATUS
MedMinorServoModbus::read_status()
{
    MEDMINORSERVOSTATUS status;
    /*read((int)MINORSERVO_STATUS_ADDR,
         (int)sizeof(MEDMINORSERVOSTATUS),
         (unsigned char *)&status);*/
    status = get<MEDMINORSERVOSTATUS>((int)(MINORSERVO_STATUS_ADDR));
    return status;
};

void
MedMinorServoModbus::write_command(MEDMINORSERVOSETPOS& command, 
                                   bool wait_ack)
{
    //command.ID = ++_command_index;
    MEDMINORSERVOSETPOSANSWER answer;
    read((int)SETPOSCMD_ANS_ADDR,
         (int)sizeof(MEDMINORSERVOSETPOSANSWER),
         (unsigned char *)&answer);
    // DEFAULT VALUE FOR FS COMPATIBILITY CHECK IN PLC CODE
    command.conf = 2; 
    command.ID = answer.ID + 1;
    write((int)MINORSERVO_SETPOS_ADDR, 
          (int)sizeof(MEDMINORSERVOSETPOS),
          (unsigned char *)&command);
    if(wait_ack)
    {
        IRA::CTimer _timer;
        do{
            if(_timer.elapsed() > _timeout)
                throw ModbusTimeout("error reading back command ID");
            read((int)SETPOSCMD_ANS_ADDR,
                 (int)sizeof(MEDMINORSERVOSETPOSANSWER),
                 (unsigned char *)&answer);
        }while(answer.ID != command.ID);
    }
};

