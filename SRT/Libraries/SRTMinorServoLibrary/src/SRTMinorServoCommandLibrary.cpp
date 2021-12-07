/**
 * SRTMinorServoCommandLibrary.cpp
 * 2021/12/07
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SRTMinorServoCommandLibrary.h"

std::string SRTMinorServoCommandLibrary::status(int servo_id)
{
    std::stringstream command;
    command << "status";
    if(servo_id >= 0)
    {
        command << "=" << servo_id;
    }
    command << std::endl;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::setup(std::string configuration)
{
    std::stringstream command;
    command << "setup=" << configuration << std::endl;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::stow(unsigned int servo_id, unsigned int stow_position)
{
    std::stringstream command;
    command << "stow=" << servo_id << "," << stow_position << std::endl;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::stop(unsigned int servo_id)
{
    std::stringstream command;
    command << "stop=" << servo_id << std::endl;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::preset(unsigned int servo_id, std::vector<double> coordinates)
{
    std::stringstream command;
    command << "preset=" << servo_id;
    for(unsigned int coordinate = 0; coordinate < coordinates.size(); coordinate++)
    {
        command << "," << coordinates[coordinate];
    }
    command << std::endl;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::programTrack(unsigned int servo_id, unsigned int trajectory_id, unsigned int point_id, std::vector<double> coordinates, double start_time)
{
    std::stringstream command;
    command << "programTrack=" << servo_id << "," << trajectory_id << "," << point_id << ",";
    if(start_time > 0)
    {
        command << std::fixed << std::setprecision(6) << start_time;
    }
    else
    {
        command << "*";
    }
    
    command.unsetf(std::ios_base::floatfield);
    for(unsigned int coordinate = 0; coordinate < coordinates.size(); coordinate++)
    {
        command << "," << coordinates[coordinate];
    }
    command << std::endl;
    return command.str();
}
