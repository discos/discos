#ifndef __SRTMINORSERVOSOCKET_H__
#define __SRTMINORSERVOSOCKET_H__

/**
 * SRTMinorServoSocket.h
 * 2023/02/23
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <mutex>
#include <IRA>
#include <ComponentErrors.h>
#include "SRTMinorServoCommandLibrary.h"

#define TIMEOUT 0.1

using namespace IRA;

class SRTMinorServoSocket: public IRA::CSocket
{
// Declare the SRTMinorServoSocketTest class as friend in order for it to have access to destroyInstance for testing purposes
friend class SRTMinorServoSocketTest;
friend class SRPProgramTrackTest;
friend class DerotatorProgramTrackTest;
friend class CombinedProgramTrackTest;
friend class ReadStatusOnlyTest;

public:
    /**
     * Calls the constructor and returns the singleton socket instance
     * @param ip_address the IP address to which the socket will connect
     * @param port the port to which the socket will connect
     * @param timeout the timeout, in seconds, for the communication to be considered failed
     * @return the singleton socket instance, connected to the given IP address and port, by reference
     * @throw ComponentErrors::SocketErrorExImpl when the user calls this method a second time with different IP address and port arguments
     */
    static SRTMinorServoSocket& getInstance(std::string ip_address, int port, double timeout=TIMEOUT);

    /**
     * Returns the previously generated singleton socket instance
     * @return the singleton socket instance, by reference
     * @throw ComponentErrors::SocketErrorExImpl when the user calls this method when the instance has not been generated yet
     */
    static SRTMinorServoSocket& getInstance();

    /**
     * Sends a command on the socket and returns the received answer, if any
     * @param command the command to be sent over the socket
     * @return the received answer to the given command
     */
    SRTMinorServoAnswerMap sendCommand(std::string command);

    /**
     * Copy constructor operator disabled by default
     */
    SRTMinorServoSocket(SRTMinorServoSocket const&) = delete;

    /**
     * Copy assignment operator disabled by default
     */
    void operator=(SRTMinorServoSocket const&) = delete;

protected:
    /*
     * Force the destruction of the singleton socket object. Only used in tests
     */
    static void destroyInstance();

private:
    /*
     * Constructor method. Generates the singleton socket instance
     * @param ip_address the IP address to which the socket will connect
     * @param port the port to which the socket will connect
     * @param timeout the timeout, in seconds, for the communication to be considered failed
     */
    SRTMinorServoSocket(std::string ip_address, int port, double timeout);

    /*
     * Destructor method. Closes the socket upon destruction
     */
    ~SRTMinorServoSocket();

    /*
     * Instance of the socket. By default it gets initialized to a null pointer
     */
    inline static SRTMinorServoSocket* m_instance = nullptr;

    /*
     * IP address and port of the socket. Being object members their values only exist when a singleton socket object is created correctly
     */
    std::string m_address;
    int m_port;

    /*
     * Timeout for communication operations
     */
    double m_timeout;

    /*
     * Mutex object used to syncronize communications and prevent collisions between multiple threads
     */
    std::mutex m_mutex;

    /*
     * Library mutex, used only to synchronize the getInstance methods
     */
    static std::mutex c_mutex;

    /*
     * Socket status enumerator
     */
    enum socket_status { READY, NOTRDY, BROKEN, TOUT, BUSY } m_socket_status;

    /*
     * Socket error variable. This stores an error condition in case it arises
     */
    IRA::CError m_error;
};

#endif
