#ifndef _SRTMINORSERVOCOMMANDLIBRARY_H
#define _SRTMINORSERVOCOMMANDLIBRARY_H

/**
 * SRTMinorServoCommandLibrary.h
 * 2021/12/07
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <sstream>
#include <vector>
#include <map>
#include <variant>

#define CLOSER std::string("\r\n")

using SRTMinorServoAnswerMap = std::map<std::string, std::variant<long, double, std::string> >;

/**
 * SRT Minor Servo Command Library
 *
 * This class features static functions used to build commands to be sent to the PON minor servo control unit of the Sardinia Radio Telescope
 */
class SRTMinorServoCommandLibrary
{
public:
    /**
     * Builds the command used to ask the status of the MSCU or, eventually, a single servo
     * @param servo_id the ID string of the eventual single servo to retrieve the status
     * @return the composed message
     */
    static std::string status(std::string servo_id = "");

    /**
     * Builds the command used to configure the telescope for an observation
     * @param configuration the desired observing configuration
     * @return the composed message
     */
    static std::string setup(std::string configuration);

    /*
     * Builds the command used to stow a single servo system to a given stow position
     * @param servo_id the ID string of the single servo to be stowed
     * @param stow_position the position to which the servo have to stow to
     * @return the composed message
     */
    static std::string stow(std::string servo_id, unsigned int stow_position = 1);

    /*
     * Builds the command used to stop a single servo system
     * @param servo_id the ID string of the single servo to be stopped
     * @return the composed message
     */
    static std::string stop(std::string servo_id);

    /*
     * Builds the command used to move a single servo to a given set of coordinates
     * @param servo_id the ID string of the single servo to be moved
     * @param coordinates a vector containing the N coordinates to be sent to the servo
     * @return the composed message
     */
    static std::string preset(std::string servo_id, std::vector<double> coordinates);

    /*
     * Builds the command used to provide a single tracking set of coordinates to a single servo
     * @param servo_id the ID string of the single servo to send the command to
     * @param trajectory_id the ID number of the trajectory the given set of coordinates belongs to
     * @param point_id the ID number of the given set of coordinates inside the trajectory
     * @param coordinates a vector containing the N coordinates the servo have to move to at the given time
     * @param start_time only mandatory for the first point in the trajectory, a double representing the UNIX epoch of the starting instant of the trajectory
     * @return the composed message
     */
    static std::string programTrack(std::string servo_id, long unsigned int trajectory_id, unsigned int point_id, std::vector<double> coordinates, double start_time = 0);

    /*
     * Builds the command used to send a set of offset coordinates
     * @param servo_id the ID string of the single servo to send the offsets to
     * @param coordinates a vector containing the N offsets to be added the servo coordinates
     * @return the composed message
     */
    static std::string offset(std::string servo_id, std::vector<double> coordinates);

    /*
     * Parses the received answer by splitting it and synamically populating a std::map
     * @param answer the string containing the answer received from the VBrain proxy
     * @return a std::map (dictionary) containing the answer splitted into keys and values. The keys are always std::string, the values can either be int, double or std::string.
     */
    static SRTMinorServoAnswerMap parseAnswer(std::string answer);
};

#endif
