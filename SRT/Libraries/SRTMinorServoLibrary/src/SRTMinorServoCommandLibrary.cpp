/**
 * SRTMinorServoCommandLibrary.cpp
 * 2021/12/07
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <iostream>
#include <iomanip>
#include <algorithm>
#include "SRTMinorServoCommandLibrary.h"

using namespace MinorServo;

std::string SRTMinorServoCommandLibrary::status(const std::string servo_id)
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

std::string SRTMinorServoCommandLibrary::setup(const std::string& configuration)
{
    std::stringstream command;
    command << "SETUP=" << configuration << CLOSER;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::stow(const std::string& servo_id, const unsigned int stow_position)
{
    std::stringstream command;
    command << "STOW=" << servo_id << "," << stow_position << CLOSER;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::stop(const std::string& servo_id)
{
    std::stringstream command;
    command << "STOP=" << servo_id << CLOSER;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::preset(const std::string& servo_id, const std::vector<double>& coordinates)
{
    std::stringstream command;
    command << "PRESET=" << servo_id;
    for(unsigned int coordinate = 0; coordinate < coordinates.size(); coordinate++)
    {
        command << "," << std::fixed << std::setprecision(6) << coordinates[coordinate];
    }
    command << CLOSER;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::programTrack(const std::string& servo_id, const unsigned long& trajectory_id, const unsigned long& point_id, const std::vector<double>& coordinates, const double start_time)
{
    std::stringstream command;
    command << "PROGRAMTRACK=" << servo_id << "," << trajectory_id << "," << point_id << ",";
    if(start_time > 0)
    {
        command << std::fixed << std::setprecision(6) << start_time;
    }
    else
    {
        command << "*";
    }
    
    for(unsigned int coordinate = 0; coordinate < coordinates.size(); coordinate++)
    {
        command << "," << std::fixed << std::setprecision(6) << coordinates[coordinate];
    }
    command << CLOSER;
    return command.str();
}

std::string SRTMinorServoCommandLibrary::offset(const std::string& servo_id, const std::vector<double>& coordinates)
{
    std::stringstream command;
    command << "OFFSET=" << servo_id;
    for(unsigned int coordinate = 0; coordinate < coordinates.size(); coordinate++)
    {
        command << "," << std::fixed << std::setprecision(6) << coordinates[coordinate];
    }
    command << CLOSER;
    return command.str();
}

SRTMinorServoAnswerMap SRTMinorServoCommandLibrary::parseAnswer(const std::string& original_answer)
{
    // First thing first, standardize the separators and remove the newline/carriage return characters
    std::string answer(original_answer);
    std::replace(answer.begin(), answer.end(), ':', '=');
    std::replace(answer.begin(), answer.end(), '|', ',');
    answer.erase(std::remove(answer.begin(), answer.end(), '\n'), answer.end());
    answer.erase(std::remove(answer.begin(), answer.end(), '\r'), answer.end());

    // Create the dictionary
    SRTMinorServoAnswerMap args;

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

            // No value, could be the timestamp
            if(value.empty())
            {
                if(args.contains("TIMESTAMP"))    // Timestamp already found, some other value is missing
                {
                    throw std::invalid_argument(std::string("Missing key for value " + value));
                }

                value = key;
                key = "TIMESTAMP";
            }

            if(key == "OUTPUT")
            {
                if(value != "GOOD" && value != "BAD")
                {
                    throw std::invalid_argument(std::string("Unrecognized OUTPUT value: " + value));
                }

                args.put(key, value);
            }
            else if(key == "TIMESTAMP")
            {
                size_t last_char;
                args.put(key, std::stod(value, &last_char));
                if(last_char != value.size())
                {
                    throw std::invalid_argument(std::string("Wrong TIMESTAMP value: " + value));
                }
            }
            else
            {
                size_t last_char;
                args.put(key, std::stol(value, &last_char));
                if(last_char != value.size())
                {
                    args.put(key, std::stod(value));
                }
            }
        }

        if(!args.contains("OUTPUT"))
        {
            throw std::invalid_argument(std::string("Missing OUTPUT value!"));
        }
        else if(args.checkOutput() && !args.contains("TIMESTAMP"))
        {
            throw std::invalid_argument(std::string("Missing TIMESTAMP value!"));
        }
    }
    catch(const std::invalid_argument& ex)
    {
        // If we are not able to convert any of the values to the correct type,
        // or if OUTPUT and/or TIMESTAMP is missing, we send back an empty dictionary.
        // It will be the caller's duty to understand that something was wrong with the answer.
        args.clear();
    }

    return args;
}
