/**
 * SRTMinorServoCommandLibrary.cpp
 * 2021/12/07
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SRTMinorServoCommandLibrary.h"

std::string SRTMinorServoCommandLibrary::status(std::string servo_id)
{
    std::stringstream command;
    command << "STATUS";
    if(servo_id != "")
    {
        command << "=" << servo_id;
    }
    command << "\r\n";
    return command.str();
}

std::string SRTMinorServoCommandLibrary::setup(std::string configuration)
{
    std::stringstream command;
    command << "SETUP=" << configuration << "\r\n";
    return command.str();
}

std::string SRTMinorServoCommandLibrary::stow(std::string servo_id, unsigned int stow_position)
{
    std::stringstream command;
    command << "STOW=" << servo_id << "," << stow_position << "\r\n";
    return command.str();
}

std::string SRTMinorServoCommandLibrary::stop(std::string servo_id)
{
    std::stringstream command;
    command << "STOP=" << servo_id << "\r\n";
    return command.str();
}

std::string SRTMinorServoCommandLibrary::preset(std::string servo_id, std::vector<double> coordinates)
{
    std::stringstream command;
    command << "PRESET=" << servo_id;
    for(unsigned int coordinate = 0; coordinate < coordinates.size(); coordinate++)
    {
        command << "," << coordinates[coordinate];
    }
    command << "\r\n";
    return command.str();
}

std::string SRTMinorServoCommandLibrary::programTrack(std::string servo_id, long unsigned int trajectory_id, unsigned int point_id, std::vector<double> coordinates, double start_time)
{
    std::stringstream command;
    command << "PROGRAMTRACK=" << servo_id << "," << trajectory_id << "," << point_id << ",";
    if(start_time > 0)
    {
        long unsigned int start_time_int = (long unsigned int)(start_time * 1000);
        command << start_time_int;
    }
    else
    {
        command << "*";
    }
    
    for(unsigned int coordinate = 0; coordinate < coordinates.size(); coordinate++)
    {
        command << "," << coordinates[coordinate];
    }
    command << "\r\n";
    return command.str();
}

std::string SRTMinorServoCommandLibrary::offset(std::string servo_id, std::vector<double> coordinates)
{
    std::stringstream command;
    command << "OFFSET=" << servo_id;
    for(unsigned int coordinate = 0; coordinate < coordinates.size(); coordinate++)
    {
        command << "," << coordinates[coordinate];
    }
    command << "\r\n";
    return command.str();
}

std::map<std::string, std::variant<int, double, std::string> > SRTMinorServoCommandLibrary::parseAnswer(std::string answer)
{
    std::map<std::string, std::variant<int, double, std::string> > args;

    std::stringstream ss(answer);
    std::string token;

    std::pair<std::string, std::string> output;
    std::getline(ss, token, ':');
    output.first = token;
    std::getline(ss, token, ',');
    output.second = token;
    args.insert(output);

    std::getline(ss, token, ',');
    args["TIMESTAMP"] = std::stod(token);

    while(std::getline(ss, token, '|'))
    {
        std::stringstream sss(token);
        std::string key, value;
        std::getline(sss, key, '=');
        std::getline(sss, value);
        args[key] = std::stoi(value);
    }

    return args;
}
