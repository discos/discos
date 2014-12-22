#include <iostream>
#include <cmath>
#include "MedMinorServoControl.hpp"

MedMinorServoControl::MedMinorServoControl(const char* server_ip, 
                                           const int server_port,
                                           double position_buffer_length,
                                           double position_refresh_time,
                                           double elevation_refresh_time ) :
                                           _server_ip(server_ip),
                                           _server_port(server_port),
                                           _position_buffer_length(position_buffer_length),
                                           _position_refresh_time(position_refresh_time),
                                           _elevation_refresh_time(elevation_refresh_time),
                                           _position_queue((int)(position_buffer_length/position_refresh_time)),
                                           _is_elevation_tracking(false)
{
    //initialize offsets
    _primary_offset_error = MedMinorServoGeometry::get_primary_tolerance();
    _secondary_offset_error = MedMinorServoGeometry::get_secondary_tolerance();
    try{
        //initialize modbus channel for communication
        _modbus = get_med_modbus_channel(_server_ip.c_str(), _server_port);
    }catch(ModbusError const & _connection_error){
        throw ServoConnectionError(_connection_error.what()); 
    }
    try{
        connect();
        reset();
        //initilize actual position and position queue
        _read_status();
    }catch(ModbusTimeout const & _timeout_error){
        throw ServoTimeoutError(_timeout_error.what());
    }catch(ModbusError const & _modbus_error){
        throw ServoConnectionError(_modbus_error.what());
    }
}

MedMinorServoControl::~MedMinorServoControl()
{
    disconnect();
}

void
MedMinorServoControl::connect()
{
    _modbus->connect();
    CUSTOM_LOG(LM_FULL_INFO,
               "MinorServo::MedMinorServoControl::connect",
               (LM_DEBUG, "connected to server %s on port %d", 
                _server_ip.c_str(),
               _server_port));
    if(_can_operate())
        return;
    else{
        disconnect();
        throw ServoConnectionError("Minor Servo is in use by fieldsystem");
    }
}

void
MedMinorServoControl::disconnect()
{
    _commanded_status.escs = 0; //release control now fieldsystem can operate
    _send_commanded_status();
    _modbus->disconnect();
    CUSTOM_LOG(LM_FULL_INFO,
               "MinorServo::MedMinorServoControl::disconnect",
               (LM_DEBUG, "disconnected from server %s on port %d", 
                _server_ip.c_str(),
               _server_port));
}

void 
MedMinorServoControl::set_position(const MedMinorServoPosition& position)
{
    boost::mutex::scoped_lock lock(_command_guard);
    try{
        _commanded_status = MedMinorServoGeometry::positionToAxes(position);
    }catch(MinorServoGeometryError _geometry_error){
        throw ServoPositionError(_geometry_error.what());
    }catch(MinorServoAxisLimitError _axis_limit_error){
        throw ServoPositionError(_axis_limit_error.what());
    }
    _commanded_position = position;
    _commanded_status.enable = 1;
    _commanded_status.acknowledge = 0;
    _send_commanded_status();
    CUSTOM_LOG(LM_FULL_INFO,
               "MinorServo::MedMinorServoControl::set_position",
               (LM_DEBUG, "set new position"));
}

void 
MedMinorServoControl::set_position_with_speed(const MedMinorServoPosition& position,
                                              double speed)
{
    boost::mutex::scoped_lock lock(_command_guard);
    try{
        _commanded_status = MedMinorServoGeometry::positionToAxes(position);
    }catch(MinorServoGeometryError _geometry_error){
        throw ServoPositionError(_geometry_error.what());
    }catch(MinorServoAxisLimitError _axis_limit_error){
        throw ServoPositionError(_axis_limit_error.what());
    }
    _commanded_position = position;
    _commanded_status.enable = 1;
    _commanded_status.acknowledge = 0;
    _commanded_status.speed_enable = 1;
    _commanded_status.vel_x_yp = speed;
    _commanded_status.vel_y_zp = speed;
    _commanded_status.vel_z1 = speed;
    _commanded_status.vel_z2 = speed;
    _commanded_status.vel_z3 = speed;
    
    _send_commanded_status();
    CUSTOM_LOG(LM_FULL_INFO,
               "MinorServo::MedMinorServoControl::set_position_with_speed",
               (LM_DEBUG, "set new position"));
}

void 
MedMinorServoControl::set_position_with_time(const MedMinorServoPosition& position,
                                             double time_offset)
{
    boost::mutex::scoped_lock lock(_command_guard);
    try{
        _commanded_status = MedMinorServoGeometry::positionToAxes(position);
    }catch(MinorServoGeometryError _geometry_error){
        throw ServoPositionError(_geometry_error.what());
    }catch(MinorServoAxisLimitError _axis_limit_error){
        throw ServoPositionError(_axis_limit_error.what());
    }
    MedMinorServoPosition _actual_position = update_position();
    MEDMINORSERVOSETPOS _actual_status = MedMinorServoGeometry::positionToAxes(_actual_position);
    _commanded_position = position;
    _commanded_status.enable = 1;
    _commanded_status.acknowledge = 0;
    _commanded_status.speed_enable = 1;

    _commanded_status.vel_x_yp = std::abs(_actual_status.pos_x_yp -
                                _commanded_status.pos_x_yp) / time_offset;
    _commanded_status.vel_y_zp = std::abs(_actual_status.pos_y_zp -
                                _commanded_status.pos_y_zp) / time_offset;
    if(position.mode == MED_MINOR_SERVO_SECONDARY)
    {
        _commanded_status.vel_z1 = std::abs(_actual_status.pos_z1 -
                                _commanded_status.pos_z1) / time_offset;
        _commanded_status.vel_z2 = std::abs(_actual_status.pos_z2 -
                                _commanded_status.pos_z2) / time_offset;
        _commanded_status.vel_z3 = std::abs(_actual_status.pos_z3 -
                                _commanded_status.pos_z3) / time_offset;
    }
    _send_commanded_status();
    _send_commanded_status();
    CUSTOM_LOG(LM_FULL_INFO,
               "MinorServo::MedMinorServoControl::set_position_with_time",
               (LM_DEBUG, "set new position"));
}

void 
MedMinorServoControl::set_last_position()
{
    boost::mutex::scoped_lock lock(_command_guard);
    /*try{
        _commanded_status = MedMinorServoGeometry::positionToAxes(_commanded_position);
    }catch(MinorServoGeometryError _geometry_error){
        throw ServoPositionError(_geometry_error.what());
    }catch(MinorServoAxisLimitError _axis_limit_error){
        throw ServoPositionError(_axis_limit_error.what());
    }
    _commanded_status.enable = 1;
    _commanded_status.acknowledge = 0;*/
    _send_commanded_status();
    _send_commanded_status();
    CUSTOM_LOG(LM_FULL_INFO,
               "MinorServo::MedMinorServoControl::set_last_position",
               (LM_DEBUG, "set new position"));
}

MedMinorServoPosition
MedMinorServoControl::get_position()
{
    return _position_queue.get_position();
}

MedMinorServoPosition
MedMinorServoControl::get_position_at(const ACS::Time& time)
{
    return _position_queue.get_position(time);
}

MedMinorServoPosition
MedMinorServoControl::get_position_offset()
{
    //At time 0 _commanded_position is all zeros
    MedMinorServoPosition position = get_position();
    position.mode = MED_MINOR_SERVO_OFFSET;
    MedMinorServoPosition delta = position - _commanded_position;
    delta.mode = MED_MINOR_SERVO_OFFSET;
    return MedMinorServoGeometry::abs(delta);
}

bool
MedMinorServoControl::is_tracking()
{
    MedMinorServoPosition actual_position = _position_queue.get_position();
    if((actual_position.mode != MED_MINOR_SERVO_PRIMARY) &&
       (actual_position.mode != MED_MINOR_SERVO_SECONDARY))
        return false;
    MedMinorServoPosition offset = get_position_offset();
    if(_commanded_position.mode == MED_MINOR_SERVO_PRIMARY)
    {
        if((offset.y < _primary_offset_error.y) &&
           (offset.z < _primary_offset_error.z))
            return true;
        else
            return false;
    }else{
        if((offset.x < _secondary_offset_error.x) &&
           (offset.y < _secondary_offset_error.y) &&
           (offset.z < _secondary_offset_error.z) &&
           (offset.theta_x < _secondary_offset_error.theta_x) &&
           (offset.theta_y < _secondary_offset_error.theta_y))
            return true;
        else
            return false;
    }
}

void 
MedMinorServoControl::reset()
{
    MedMinorServoPosition actual_position;
    boost::mutex::scoped_lock lock(_command_guard);
    _commanded_status.escs = 1;
    _commanded_status.enable = 0;
    _commanded_status.acknowledge = 0;
    _send_commanded_status(true);
    IRA::CTimer _timer;
    do{
        actual_position = update_position();
        if(_timer.elapsed() > 5 * 10000000)
            throw ServoTimeoutError("timeout on disabling axes during reset");
    }while((actual_position.mode != MED_MINOR_SERVO_SR_BLOCK) &&
          (actual_position.mode != MED_MINOR_SERVO_PFR_BLOCK) &&
          (actual_position.mode != MED_MINOR_SERVO_SYSTEM_BLOCK));
    _commanded_status.enable = 1;
    _send_commanded_status(true);
    _timer.reset();
    do{
        actual_position = _read_status();
        if(_timer.elapsed() > 5 * 10000000)
            throw ServoTimeoutError("timeout on waiting ack request during reset");
    }while(actual_position.mode != MED_MINOR_SERVO_INTERLOCK);
    _commanded_status.acknowledge = 1;
    _send_commanded_status(true);
    _timer.reset();
    do{
        actual_position = _read_status();
        if(_timer.elapsed() > 5 * 10000000)
            throw ServoTimeoutError("timeout on waiting ack reply during reset");
    }while(actual_position.mode == MED_MINOR_SERVO_STATUS_INTERLOCK);
    _commanded_status.acknowledge = 0;
    _send_commanded_status(true);
    _timer.reset();
    do{
        actual_position = _read_status();
        if(_timer.elapsed() > 5 * 10000000)
            throw ServoTimeoutError("timeout on waiting success code during reset");
    }while(!(actual_position.is_success_position())); //error status
    _send_commanded_status();
    CUSTOM_LOG(LM_FULL_INFO,
               "MinorServo::MedMinorServoControl::reset",
               (LM_DEBUG, "control reset"));
}

MedMinorServoPosition
MedMinorServoControl::update_position()
{
    //this method was meant to incapsulate _read_status while providing
    //synchronization logics. Turns out there's no need to synchronize so it's
    //still here just because of previous decisions.
    return _read_status();
    CUSTOM_LOG(LM_FULL_INFO,
               "MinorServo::MedMinorServoControl::update_position",
               (LM_DEBUG, "position updated"));
}

MedMinorServoPosition
MedMinorServoControl::_read_status()
{
    IRA::CTimer _timer;
    MEDMINORSERVOSTATUS actual_status;
    MedMinorServoPosition actual_position;
    bool valid_position = true;
    do{
        if(_timer.elapsed() > 5 * 10000000)
            throw ServoTimeoutError("timeout on read_status, cannot find valid position");
        try{
            actual_status = _modbus->read_status();
            if(actual_status.escs == 2){
                disconnect();
                throw ServoConnectionError("Minor Servo is in use by fieldsystem");
            }
            actual_position = MedMinorServoGeometry::positionFromAxes(actual_status);
            valid_position = _position_queue.push(actual_position);
        }catch(ModbusError const & _modbus_error){
            //TODO: log warning
            // we have a modbus error, we can try again until timeout
            valid_position = false;
        }catch(MinorServoGeometryError const & _geometry_error){
            throw ServoPositionError(_geometry_error.what());
        }
    }while(!valid_position);
    return actual_position;
}

bool
MedMinorServoControl::_can_operate()
{
    MEDMINORSERVOSTATUS actual_status;
    actual_status = _modbus->read_status();
    if(actual_status.escs == 2)
        return false;
    else
        return true;
}

void
MedMinorServoControl::_send_commanded_status(bool wait_ack)
{
    //_commanded_status.escs = 1;
    _commanded_status.time = MedMinorServoTime::ACSToServoNow();
    try{
        _modbus->write_command(_commanded_status, wait_ack);
    }catch(ModbusTimeout const & _timeout_error){
        throw ServoTimeoutError(_timeout_error.what());
    }catch(ModbusError const & _modbus_error){
        throw ServoConnectionError(_modbus_error.what());
    }
    //_read_status();
}

MedMinorServoControl_sp 
get_servo_control(const char* server_ip, 
                  const int server_port,
                  double position_buffer_length,
                  double position_refresh_time,
                  double elevation_refresh_time )
{
    MedMinorServoControl_sp _tmp(new MedMinorServoControl(server_ip, 
                                                          server_port,
                                                          position_buffer_length,
                                                          position_refresh_time,
                                                          elevation_refresh_time )
                                    );
    return _tmp;
}
