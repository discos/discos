/**
 * GAIABoardCommandLibrary.cpp
 * 2022/02/25
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "GAIABoardCommandLibrary.h"

std::string GAIABoardCommandLibrary::idn()
{
    std::stringstream command;
    command << HEADER << "IDN?" << TAIL;
    return command.str();
}

std::string GAIABoardCommandLibrary::enable(unsigned int channel)
{
    std::stringstream command;
    command << HEADER << "ENABLE " << channel << TAIL;
    return command.str();
}

std::string GAIABoardCommandLibrary::getvg(unsigned int channel)
{
    std::stringstream command;
    command << HEADER << "GETVG " << channel << TAIL;
    return command.str();
}

std::string GAIABoardCommandLibrary::getvd(unsigned int channel)
{
    std::stringstream command;
    command << HEADER << "GETVD " << channel << TAIL;
    return command.str();
}

std::string GAIABoardCommandLibrary::getid(unsigned int channel)
{
    std::stringstream command;
    command << HEADER << "GETID " << channel << TAIL;
    return command.str();
}

std::string GAIABoardCommandLibrary::getref(unsigned int channel)
{
    std::stringstream command;
    command << HEADER << "GETREF " << channel << TAIL;
    return command.str();
}

std::string GAIABoardCommandLibrary::getemp(unsigned int channel)
{
    std::stringstream command;
    command << HEADER << "GETEMP " << channel << TAIL;
    return command.str();
}

std::string GAIABoardCommandLibrary::name()
{
    std::stringstream command;
    command << HEADER << "NAME?" << TAIL;
    return command.str();
}
