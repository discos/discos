#include "MicroControllerBoard.h"
#include <cstdlib>

template<class T>
std::string any2string(T i) {
    std::ostringstream buffer;
    buffer << i;
    return buffer.str();
}


int main(int argc, char *argv[])
{
    if(argc != 3) {
        cerr << "\nWrong number of parameters. Usage:\n\tTestMicrocontrollerBoard IP port\n" << endl;
        return 1;
    }

    std::string IP = std::string(argv[1]);
    unsigned int port = atoi(argv[2]);
    std::vector<BYTE> data;

    MicroControllerBoard mcb = MicroControllerBoard(IP, port);

    // Test openConnection
    try {
        cout << "\nConnecting..." << endl;
        mcb.openConnection();
        cout << "Connection to " << mcb.getIP() << ":" << mcb.getPort() << " done!" << endl << endl;
    }
    catch(MicroControllerBoardEx& ex)  {
        cout << ex.what() << endl;
        return 1;
    }
    
    // Test socket status
    cout << "Socket status: ";
    switch(mcb.getConnectionStatus()) {
        case 0:
            cout << "Not Created";
            break;
        case 1:
            cout << "Ready";
            break;
        case 2:
            cout << "Accepting";
            break;
        case 3:
            cout << "Connecting";
            break;
        default:
            cout << "Unknown";
    }
    cout << endl << endl;

    // Test the MCB_CMD_INQUIRY extended and short commands
    try {
        cout << "Test MCB_CMD_INQUIRY......" << endl;
        mcb.send(MCB_CMD_INQUIRY);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Test MCB_CMD_INQUIRY SHORT......" << endl;
        mcb.send(MCB_CMD_INQUIRY | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_RESET extended and short commands
    try {
        cout << "Testing MCB_CMD_RESET......" << endl;
        mcb.send(MCB_CMD_RESET);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_RESET SHORT......" << endl;
        mcb.send(MCB_CMD_RESET | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_VERSION extended and short commands
    try {
        cout << "Testing MCB_CMD_VERSION......" << endl;
        mcb.send(MCB_CMD_VERSION);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_VERSION SHORT......" << endl;
        mcb.send(MCB_CMD_VERSION | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_SAVE extended and short commands
    try {
        cout << "Testing MCB_CMD_SAVE......" << endl;
        mcb.send(MCB_CMD_SAVE);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_SAVE SHORT......" << endl;
        mcb.send(MCB_CMD_SAVE | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_RESTORE extended and short commands
    try {
        cout << "Testing MCB_CMD_RESTORE......" << endl;
        mcb.send(MCB_CMD_RESTORE);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_RESTORE SHORT......" << endl;
        mcb.send(MCB_CMD_RESTORE | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_GET_ADDR extended and short commands
    try {
        cout << "Testing MCB_CMD_GET_ADDR......" << endl;
        mcb.send(MCB_CMD_GET_ADDR);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_GET_ADDR SHORT......" << endl;
        mcb.send(MCB_CMD_GET_ADDR | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_SET_ADDR extended and short commands
    try {
        cout << "Testing MCB_CMD_SET_ADDR......" << endl;
        mcb.send(MCB_CMD_SET_ADDR);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_SET_ADDR SHORT......" << endl;
        mcb.send(MCB_CMD_SET_ADDR | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_GET_TIME extended and short commands
    try {
        cout << "Testing MCB_CMD_GET_TIME......" << endl;
        mcb.send(MCB_CMD_GET_TIME);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_GET_TIME SHORT......" << endl;
        mcb.send(MCB_CMD_GET_TIME | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_SET_TIME extended and short commands
    try {
        cout << "Testing MCB_CMD_SET_TIME......" << endl;
        mcb.send(MCB_CMD_SET_TIME);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_SET_TIME SHORT......" << endl;
        mcb.send(MCB_CMD_SET_TIME | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_GET_FRAME extended and short commands
    try {
        cout << "Testing MCB_CMD_GET_FRAME......" << endl;
        mcb.send(MCB_CMD_GET_FRAME);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_GET_FRAME SHORT......" << endl;
        mcb.send(MCB_CMD_GET_FRAME | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_SET_FRAME extended and short commands
    try {
        cout << "Testing MCB_CMD_SET_FRAME......" << endl;
        mcb.send(MCB_CMD_SET_FRAME);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_SET_FRAME SHORT......" << endl;
        mcb.send(MCB_CMD_SET_FRAME | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_GET_PORT extended and short commands
    try {
        cout << "Testing MCB_CMD_GET_PORT......" << endl;
        mcb.send(MCB_CMD_GET_PORT);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_GET_PORT SHORT......" << endl;
        mcb.send(MCB_CMD_GET_PORT | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_SET_PORT extended and short commands
    try {
        cout << "Testing MCB_CMD_SET_PORT......" << endl;
        mcb.send(MCB_CMD_SET_PORT);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_SET_PORT SHORT......" << endl;
        mcb.send(MCB_CMD_SET_PORT | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_GET_DATA extended and short commands
    try {
        cout << "Testing MCB_CMD_GET_DATA......" << endl;
        mcb.send(MCB_CMD_GET_DATA);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
        cout << "Testing MCB_CMD_GET_DATA SHORT......" << endl;
        mcb.send(MCB_CMD_GET_DATA | MCB_CMD_TYPE_NOCHECKSUM);
        data = mcb.receive();
        if(!data.empty())
            for(std::vector<BYTE>::iterator iter=data.begin(); iter != data.end(); iter++)
                cout << hex << int(*iter) << " " << flush;
        cout << "Done!\n" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test the MCB_CMD_SET_DATA command
    try {
        cout << "Testing MCB_CMD_SET_DATA......" << endl;
        std::vector<BYTE> params;
        params.push_back(0x01);
        params.push_back(0x02);
        params.push_back(0x03);
        mcb.send(0x4F, params);
        mcb.receive();
        cout << "Done!" << endl;
    }
    catch(MicroControllerBoardEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }

    // Test closeConnection
    mcb.closeConnection();

    return 0;
}
