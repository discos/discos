#ifndef MEDMINORSERVOCONTROL_HPP
#define MEDMINORSERVOCONTROL_HPP

#include <string.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <modbus/modbus.h>

#include <acsThread.h>

#include <IRA>

#include "PositionQueue.hpp"
#include "MedMinorServoProtocol.hpp"
#include "MedMinorServoConstants.hpp"
#include "MedMinorServoGeometry.hpp"
#include "MedMinorServoModbus.hpp"

class ServoPositionError: public std::runtime_error
{
    public:
        ServoPositionError(const char *msg): std::runtime_error(std::string(msg)){};
};

class ServoTimeoutError: public std::runtime_error
{
    public:
        ServoTimeoutError(const char *msg): std::runtime_error(std::string(msg)){};
};

class ServoConnectionError: public std::runtime_error
{
    public:
        ServoConnectionError(const char *msg): std::runtime_error(std::string(msg)){};
};

class MedMinorServoControl;
typedef boost::shared_ptr<MedMinorServoControl> MedMinorServoControl_sp;

MedMinorServoControl_sp get_servo_control(
                     const char* server_ip = "192.168.51.30", 
                     const int server_port = MODBUS_TCP_DEFAULT_PORT,
                     double position_buffer_length = 15.0,
                     double position_refresh_time = 0.002,
                     double elevation_refresh_time = 0.1); 

/**
 * Issues command to the minor servo implementing the protocol defined in 
 * MedMinorServoProtcol and interacting via Modbus using the ModbusChannel 
 */
class MedMinorServoControl
{
    public:
        /**
         * Ctor
         */
        MedMinorServoControl(const char* server_ip = "192.168.51.30", 
                             const int server_port = MODBUS_TCP_DEFAULT_PORT,
                             double position_buffer_length = 300.0,
                             double position_refresh_time = 0.1,
                             double elevation_refresh_time = 0.1); //throw ServoConnectionError, ServoTimeoutError
        virtual ~MedMinorServoControl();
        /**
        * Command a new position to the minor servo system
        * @param position: the new position
        */
        void set_position(const MedMinorServoPosition& position); //throw(MinorServoAxisLimitError, ServoTimeoutError)

        /**
        * Command a new position to the minor servo system
        * @param position: the new position
        * @param speed: the constant speed of all axes in mm/sec.
        */
        void set_position_with_speed(const MedMinorServoPosition& position,
                                     double speed); //throw(MinorServoAxisLimitError, ServoTimeoutError)

        /**
        * Command a new position to the minor servo system to be reached in
        * a given time. This will calculate the speed of each axis and set it
        * during the movement.
        * @param position: the new position
        * @param time: the time it will take to reach the commanded position in
        * seconds
        */
        void set_position_with_time(const MedMinorServoPosition& position,
                                    double time_offset); //throw(MinorServoAxisLimitError, ServoTimeoutError)

        /**
        * Issue again the last commanded position
        */
        void set_last_position();
        /**
         * return the last read position 
         */
        MedMinorServoPosition get_position();
        /**
         * return the minor servo position at the specified time
         */
        MedMinorServoPosition get_position_at(const ACS::Time& time);
        MedMinorServoPosition get_commanded_position(){return _commanded_position;};
        /**
         * return the offset between commanded and actual position
         */
        MedMinorServoPosition get_position_offset();
        /**
         * check if commanded position has been reached
         * @return: True if delta_position < offset_error
         */
        bool is_tracking();
        /**
         * Performs a duty cycle of the PLC control
         * 1) deactivate axes
         * 2) activate axes
         * 3) wait for ack request
         * 4) sends ack
         * This should act as a reset action and is our only hope for trying to
         * resolve some error status.
         */
        void reset();
        /**
         * Ask a new position to the minor servo PLC via the modbus
         * interface and stores it in its own queue for successive
         * interpolation. This method is thread safe and can be continuosly
         * called by position monitoring thread.
         * @return the obtained position
         */
         MedMinorServoPosition update_position();
        //void start_monitoring_position();
        //void stop_monitoring_position();
        void connect();// throw (ServoConnectionError);
        void disconnect();
        bool is_connected();
    private:
        std::string _server_ip, _servo_name;
        int _server_port;
        double _position_buffer_length, _position_refresh_time, _elevation_refresh_time;
        MedMinorServoModbus_sp _modbus;
        boost::mutex _command_guard;
        boost::mutex _tracking_offset_guard;
        boost::recursive_mutex _read_guard;
        PositionQueue _position_queue;
        MEDMINORSERVOSETPOS _commanded_status;
        MedMinorServoPosition _commanded_position;
        MedMinorServoPosition _primary_offset_error, _secondary_offset_error;
        MedMinorServoPosition _system_offset, _user_offset, _tracking_offset;
        MedMinorServoPosition _zero_position;
        bool _is_elevation_tracking, _can_track_elevation, _is_connected;
        /**
         * Verifies wether the servo is available for operations or
         * is actually controlled by fieldsystem
         */
        bool _can_operate();
        /**
         * Flush commands to the servo server
         */
        void _send_commanded_status(bool wait_ack=true);
        /**
         * Get actual status informations from the servo server
         */
        MedMinorServoPosition _read_status();
        /**
         * If not connected throw ServoConnectionError
         */
        void _check_connection();
};

#endif

