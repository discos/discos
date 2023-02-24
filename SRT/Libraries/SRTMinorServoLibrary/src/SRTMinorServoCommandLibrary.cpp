/**
 * SRTMinorServoCommandLibrary.cpp
 * 2021/12/07
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <iostream>
#include <algorithm>
#include "SRTMinorServoCommandLibrary.h"

std::string SRTMinorServoCommandLibrary::status(std::string servo_id)
{
    std::stringstream command;
    command << "STATUS";
    if(servo_id != "")
    {
        command << "=" << servo_id;
    }
    command << CLOSER;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::setup(std::string configuration)
{
    std::stringstream command;
    command << "SETUP=" << configuration << CLOSER;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::stow(std::string servo_id, unsigned int stow_position)
{
    std::stringstream command;
    command << "STOW=" << servo_id << "," << stow_position << CLOSER;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::stop(std::string servo_id)
{
    std::stringstream command;
    command << "STOP=" << servo_id << CLOSER;
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
    command << CLOSER;
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
    command << CLOSER;
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
    command << CLOSER;
    return command.str();
}

std::map<std::string, std::variant<int, double, std::string> > SRTMinorServoCommandLibrary::parseAnswer(std::string answer)
{
    // First thing first, standardize the separators
    std::replace(answer.begin(), answer.end(), ':', '=');
    std::replace(answer.begin(), answer.end(), '|', ',');

    // Create the dictionary
    std::map<std::string, std::variant<int, double, std::string> > args;

    std::stringstream ss(answer);
    std::string token;

    try
    {
        // Loop through the tokens
        while(std::getline(ss, token, ','))
        {
            std::stringstream sss(token);
            std::string key, value;
            std::getline(sss, key, '=');
            std::getline(sss, value);

            // No value, should be the timestamp
            if(value.empty())
            {
                if(args.find("TIMESTAMP") != args.end())    // Timestamp already found, some other value is missing its key
                    throw std::invalid_argument(std::string("Missing key for value " + value));

                value = key;
                key = "TIMESTAMP";
            }

            if(key == "OUTPUT")
            {
                if(value != "GOOD" && value != "BAD")
                    throw std::invalid_argument(std::string("Unrecognized OUTPUT value: " + value));

                args[key] = value;
            }
            else if(key == "TIMESTAMP")
            {
                size_t last_char;
                args[key] = std::stod(value, &last_char);
                if(last_char != value.size())
                    throw std::invalid_argument(std::string("Wrong TIMESTAMP value: " + value));
            }
            else
            {
                size_t last_char;
                args[key] = std::stoi(value, &last_char);
                if(last_char != value.size())
                    args[key] = std::stod(value);
            }
        }

        if(args.find("OUTPUT") == args.end())
            throw std::invalid_argument(std::string("Missing OUTPUT value!"));
        else if(args.find("TIMESTAMP") == args.end())
            throw std::invalid_argument(std::string("Missing TIMESTAMP value!"));
    }
    catch(const std::invalid_argument& e)
    {
        // If we are not able to convert any of the values to the correct type,
        // or if OUTPUT and/or TIMESTAMP is missing, we send back an empty dictionary.
        // It will be the caller's duty to understand that something was wrong with the answer.
        args.clear();
    }

    return args;
}
