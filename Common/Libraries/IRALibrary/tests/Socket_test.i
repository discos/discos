#include <IRA>

#include <stdlib.h>

namespace IRALibraryTest {

class IRALibrary_Socket : public ::testing::Test {

public:

    ::testing::AssertionResult sendWithoutConnection() {

        const BYTE *msg = (const BYTE *)("enniomorricone");
        size_t len = strlen((const char *)(msg));
        IRA::CError error;
        IRA::CSocket socket;
        socket.Create(error);
        socket.setSockMode(error, IRA::CSocket::BLOCKINGTIMEO, 3000000, 3000000);
        // Note: there is no call to socket.Connect()
        if(socket.Send(error, (const void *)(msg), len) == IRA::CSocket::FAIL)
            return ::testing::AssertionSuccess();
        else
            return ::testing::AssertionFailure();
    }

protected:
    static void TearDownTestCase() {
    }

    static void SetUpTestCase() {
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

};

}
