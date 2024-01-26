#include "SRTMinorServoSocket.h"

namespace MinorServo
{
    class SRTMinorServoTestingSocket: public SRTMinorServoSocket
    {
        /**
         * This class is a friend class of SRTMinorServoSocket. It can be used for testing purposes without altering the behavior of the original class.
         * Instructions on how to use this class inside tests:
         * 1) Declare the test class with a forward declaration
         * 2) Define the FRIEND_CLASS_DECLARATION macro
         * 3) Include this header file
         * 4) Finally implement the test class
         * Example:
         *
         * ...
         * class TestClass;
         * #define FRIEND_CLASS_DECLARATION friend class ::TestClass;
         * #include "SRTMinorServoTestingSocket.h"
         *
         * class TestClass : public ::testing::Test
         * {
         * ...
         */

        /**
         * This is a macro that each test file will have to define prior to including this header file.
         * It can be a single or multiple lines each one containing a friend class declaration.
         */
        FRIEND_CLASS_DECLARATION;
    public:
        /**
         * Ovverride of the original SRTMinorServoSocket getInstance methods. Before doing anything, they set the c_testing variable of the original class to true.
         * This changes how the exceptions are thrown.
         * In non testing purposes, exceptions are thrown as MinorServoErrorsEx
         * In testing purposes, exceptions are thrown as they are (ExImpl)
         * This change in behavior allow for better testing and debugging purposes, while still allowing the socket to throw the base MinorServoErrorsEx when not used in testing purposes.
         * This allow us to avoid catching exceptions in the component, since MinorServoErrorsEx are already captured by CORBA
         */
        static SRTMinorServoSocket& getInstance(std::string ip_address, int port, double timeout=SOCKET_TIMEOUT)
        {
            SRTMinorServoSocket::c_testing = true;
            return SRTMinorServoSocket::getInstance(ip_address, port, timeout);
        }
        static SRTMinorServoSocket& getInstance()
        {
            SRTMinorServoSocket::c_testing = true;
            return SRTMinorServoSocket::getInstance();
        }

        /**
         * This method explicitly destroys the singleton socket instance.
         * Each test in the same test file is executed under the same process, therefore a singleton instance that does not get destroyed will still exist in the next executed tests.
         * We want to test a new instance of the socket every time, therefore the existence of this method is critical.
         */
        static void destroyInstance()
        {
            if(SRTMinorServoSocket::m_instance != nullptr)
            {
                delete SRTMinorServoSocket::m_instance;
                SRTMinorServoSocket::m_instance = nullptr;
            }
        }
    };
}
