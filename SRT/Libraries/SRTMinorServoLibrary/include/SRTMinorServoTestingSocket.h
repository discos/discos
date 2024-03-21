#include "SRTMinorServoSocket.h"

namespace MinorServo
{
    class SRTMinorServoTestingSocket: public SRTMinorServoSocket
    {
        /**
         * This class is a friend class of SRTMinorServoSocket. It can be used for testing purposes without altering the behavior of the original class.
         */
    public:
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
