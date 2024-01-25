#ifndef __SRTMINORSERVOSOCKET_H__
#define __SRTMINORSERVOSOCKET_H__

/**
 * SRTMinorServoSocket.h
 * 2023/02/23
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <IRA>
#include <mutex>
#include <ComponentErrors.h>
#include <MinorServoErrors.h>
#include <optional>
#include "SRTMinorServoCommandLibrary.h"


#define SOCKET_TIMEOUT 0.1
#define CONFIG_DOMAIN   "alma/"
#define CONFIG_DIRNAME  "/MINORSERVO/Socket"


class SRTMinorServoSocketTest;
class SRPProgramTrackTest;
class DerotatorProgramTrackTest;
class CombinedProgramTrackTest;
class ReadStatusOnlyTest;


namespace MinorServo
{
    class SRTMinorServoSocket: public IRA::CSocket
    {
        /**
         * This class implements a singleton socket. The singleton pattern was necessary to provide each servo system component communication capabilities with the Leonardo system.
         * As long as all the said components run on the same container this will only be instanced once.
         */

        /**
         * Declare the testing classes as friend classes in order for them to have access to destroyInstance for testing purposes
         */
        friend class ::SRTMinorServoSocketTest;
        friend class ::SRPProgramTrackTest;
        friend class ::DerotatorProgramTrackTest;
        friend class ::CombinedProgramTrackTest;
        friend class ::ReadStatusOnlyTest;

    public:
        /**
         * Calls the constructor and returns the singleton socket instance
         * @param ip_address the IP address to which the socket will connect
         * @param port the port to which the socket will connect
         * @param timeout the timeout, in seconds, for the communication to be considered failed
         * @throw MinorServoErrors::CommunicationErrorExImpl when the user calls this method a second time with different IP address and port arguments
         * @return the singleton socket instance, eventually connected to the given IP address and port, by reference
         */
        static SRTMinorServoSocket& getInstance(std::string ip_address, int port, double timeout=SOCKET_TIMEOUT);

        /**
         * Returns the previously generated singleton socket instance
         * @throw MinorServoErrors::CommunicationErrorExImpl when the user calls this method when the instance has not been generated yet
         * @return the singleton socket instance, by reference
         */
        static SRTMinorServoSocket& getInstance();

        /**
         * Sends a command on the socket and returns the received answer, if any
         * @param command the command to be sent over the socket
         * @param map, optional SRTMinorServoAnswerMap object. If provided, the 'map' argument content gets updated with the newly received answer
         * @throw MinorServoErrors::CommunicationErrorExImpl when the operation of sending or receiving fails unexpectedly
         * @return the received answer to the given command
         */
        SRTMinorServoAnswerMap sendCommand(std::string command, std::optional<std::reference_wrapper<SRTMinorServoAnswerMap>> map = {});

        /**
         * Copy constructor operator disabled by default
         */
        SRTMinorServoSocket(SRTMinorServoSocket const&) = delete;

        /**
         * Copy assignment operator disabled by default
         */
        void operator=(SRTMinorServoSocket const&) = delete;

        /**
         * Method to check if socket is connected
         * @return true if the socket is connected, false otherwise
         */
        const bool isConnected() const;

    private:
        /**
         * Constructor method. Generates the singleton socket instance
         * @param ip_address the IP address to which the socket will connect
         * @param port the port to which the socket will connect
         * @param timeout the timeout, in seconds, for the communication to be considered failed
         */
        SRTMinorServoSocket(std::string ip_address, int port, double timeout);

        /**
         * Destructor method. Closes the socket upon destruction
         */
        ~SRTMinorServoSocket();

        /**
         * Force the destruction of the singleton socket object. Only used in tests
         */
        static void destroyInstance();

        /**
         * Connection function. It gets called every time the socket gets disconnected
         * throw MinorServoErrors::CommunicationErrorExImpl when the connection attempt fails
         */
        void connect();

        /**
         * Instance of the socket. By default it gets initialized to a null pointer
         */
        inline static SRTMinorServoSocket* m_instance = nullptr;

        /**
         * IP address and port of the socket. Being object members their values only exist when a singleton socket object is created correctly
         */
        std::string m_ip_address;
        int m_port;

        /**
         * Timeout for communication operations
         */
        double m_timeout;

        /**
         * Mutex object used to syncronize communications and prevent collisions between multiple threads
         */
        std::mutex m_mutex;

        /**
         * Library mutex, used only to synchronize the getInstance methods
         */
        static std::mutex c_mutex;

        /**
         * Socket status enumerator
         */
        enum socket_status
        {
            NOTREADY,
            TIMEOUT,
            READY
        } m_socket_status;

        /**
         * Socket error variable. This stores an error condition in case it arises
         */
        IRA::CError m_error;
    };


    class SRTMinorServoSocketConfiguration
    {
        /**
         * This class implements a singleton socket configuration object. The singleton pattern was necessary to provide each servo system component access to the singleton socket parameters.
         * As long as all the said components run on the same container this will only be instanced once.
         * The first component to instantiate the singleton configuration will also instantiate the socket, opening communications.
         */
    public:
        /**
         * Calls the constructor and returns the singleton socket configuration instance
         * @param containerServices, the container services necessary to read the configuration from the CDB
         * @return the singleton socket configuration instance
         */
        static SRTMinorServoSocketConfiguration& getInstance(maci::ContainerServices* containerServices);

        /**
         * Copy constructor operator disabled by default
         */
        SRTMinorServoSocketConfiguration(SRTMinorServoSocketConfiguration const&) = delete;

        /**
         * Copy assignment operator disabled by default
         */
        void operator=(SRTMinorServoSocketConfiguration const&) = delete;

        /**
         * IP address and port of the socket configuration
         */
        std::string m_ip_address;
        int m_port;

        /**
         * Timeout for communication operations
         */
        double m_timeout;

    private:
        /**
         * Constructor method
         * @param containerServices, the container services necessary to read the configuration from the CDB
         * @return the socket configuration object
         */
        SRTMinorServoSocketConfiguration(maci::ContainerServices* containerServices);

        /**
         * Destructor method
         */
        ~SRTMinorServoSocketConfiguration();

        /**
         * Instance of the socket configuration object. By default it gets initialized to a null pointer
         */
        inline static SRTMinorServoSocketConfiguration* m_instance = nullptr;
    };
}

#endif
