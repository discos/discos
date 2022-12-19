#ifndef _GAIABOARDCOMMANDLIBRARY_H
#define _GAIABOARDCOMMANDLIBRARY_H

/**
 * GAIABoardCommandLibrary.h
 * 2022/02/25
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <sstream>

#define HEADER  "#"
#define TAIL    "\n"

/**
 * GAIA Board Command Library
 *
 * This class features static functions used to build commands to be sent to the GAIA Boards
 */
class GAIABoardCommandLibrary
{
public:
    /**
     * Builds the command used to ask some info about the GAIA Board and its firmware
     * @return the composed message
     */
    static std::string idn();

    /**
     * Builds the command used to turn on the Low Noise Amplifiers
     * @param channel the channel the command will be sent to
     * @return the composed message
     */
    static std::string enable(unsigned int channel);

    /**
     * Builds the command used to retrieve the gate tension
     * @param channel the channel the command will be sent to
     * @return the composed message
     */
    static std::string getvg(unsigned int channel);

    /**
     * Builds the command used to retrieve the drain tension
     * @param channel the channel the command will be sent to
     * @return the composed message
     */
    static std::string getvd(unsigned int channel);

    /**
     * Builds the command used to retrieve the drain current draw
     * @param channel the channel the command will be sent to
     * @return the composed message
     */
    static std::string getid(unsigned int channel);

    /**
     * Builds the command used to retrieve the reference tensions
     * @param channel the channel the command will be sent to
     * @return the composed message
     */
    static std::string getref(unsigned int channel);

    /**
     * Builds the command used to retrieve the temperature of the analog to digital converter
     * @param channel the channel the command will be sent to
     * @return the composed message
     */
    static std::string getemp(unsigned int channel);

    /**
     * Builds the command used to retrieve the name of the GAIA Board
     * @return the composed message
     */
    static std::string name();
};

#endif
