#include "ReceiverControl.h"
#include <cstdlib>

template<class T>
std::string any2string(T i) {
    std::ostringstream buffer;
    buffer << i;
    return buffer.str();
}


int main(int argc, char *argv[])
{
    if(argc != 5) {
        cerr << "\nWrong number of parameters. ";
        cerr << "Usage:\n\tTestReceiverControl dewar_IP dewar_port LNA_IP LNA_port\n" << endl;
        return 1;
    }

    std::string dewar_IP = std::string(argv[1]);
    unsigned int dewar_port = atoi(argv[2]);
    std::string lna_IP = std::string(argv[3]);
    unsigned int lna_port = atoi(argv[4]);
    std::vector<BYTE> data;


    // Test the constructor 
    try {
        cout << "\nConnecting to the boards (LNA and deward)..." << endl;
        ReceiverControl rc = ReceiverControl(dewar_IP, dewar_port, lna_IP, lna_port);
        cout << "Connection" << " done!" << endl << endl;
    
        // Test the setCalibrationOn()
        cout << "Test setCalibrationOn()" << endl;
        rc.setCalibrationOn();
        cout << "Done!\n" << endl;

        // Test the setCalibrationOff()
        cout << "Test setCalibrationOff()" << endl;
        rc.setCalibrationOff();
        cout << "Done!\n" << endl;
    }
    catch(ReceiverControlEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }


    return 0;
}
