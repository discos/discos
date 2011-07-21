#include "ReceiverControl.h"
#include <cstdlib>

template<class T>
std::string any2string(T i) {
    std::ostringstream buffer;
    buffer << i;
    return buffer.str();
}

/************************ CONVERSION FUNCTIONS **************************/
// Convert the voltage value of the vacuum to mbar
double voltage2mbar(double voltage) { return(pow(10, 1.5 * voltage - 12)); }

// Convert the voltage value of the temperatures to Kelvin
double voltage2Kelvin(double voltage) 
{ 
    return voltage < 1.12 ? \
        (660.549422889947 * pow(voltage, 6)) - (2552.334255456860 * pow(voltage, 5)) + (3742.529989384570 * pow(voltage, 4)) \
        - (2672.656926956470 * pow(voltage, 3)) + (947.905578508975 * pow(voltage, 2)) - 558.351002849576 * voltage + 519.607622398508 \
                 : \
        (865.747519105672 * pow(voltage, 6)) - (7271.931957100480 * pow(voltage, 5)) + (24930.666241800500 * pow(voltage, 4)) \
        - (44623.988512320400 * pow(voltage, 3)) + (43962.922216886600 * pow(voltage, 2)) - 22642.245121997700 * voltage + 4808.631312836750;
}

// Convert the ID voltage value to the mA value
double currentConverter(double voltage) { return(10 * voltage); }

// Convert the VD and VG voltage values using a right scale factor
double voltageConverter(double voltage) { return(voltage); }
/************************** END CONVERSIONS ****************************/


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
        cout << "Test setCalibrationOn() with a reliable communication" << endl;
        rc.setCalibrationOn();
        cout << "Done!\n" << endl;

        // Test the isCalibrationOn()
        cout << "Test isCalibrationOn() with a reliable communication" << endl;
        cout << "It the noise mark generator ON? " << (rc.isCalibrationOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setReliableCommOff()
        cout << "Test setReliableCommOff()" << endl;
        rc.setReliableCommOff();
        cout << "Done!\n" << endl;
    
        // Test the setCalibrationOn()
        cout << "Test setCalibrationOn()" << endl;
        rc.setCalibrationOn();
        cout << "Done!\n" << endl;

        // Test the setCalibrationOff()
        cout << "Test setCalibrationOff()" << endl;
        rc.setCalibrationOff();
        cout << "Done!\n" << endl;

        // Test the isCalibrationOn()
        cout << "Test isCalibrationOn() with a reliable communication" << endl;
        cout << "It the noise mark generator ON? " << (rc.isCalibrationOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the vacuum() voltage value, without conversion
        cout << "Test vacuum()" << endl;
        cout << "Vacuum value before conversion [Volt]: " << rc.vacuum() << endl;
        cout << "Done!\n" << endl;

        // Test the setReliableCommOn()
        cout << "Test setReliableCommOn()" << endl;
        rc.setReliableCommOn();
        cout << "Done!\n" << endl;

        // Test the setCalibrationOff()
        cout << "Test setCalibrationOff() with a reliable communication" << endl;
        rc.setCalibrationOff();
        cout << "Done!\n" << endl;

        // Test the vacuum()
        cout << "Test vacuum() with a reliable communication" << endl;
        cout << "Vacuum value: " << rc.vacuum(voltage2mbar) << endl;
        cout << "Done!\n" << endl;

        // Test the lowTemperature()
        cout << "Test lowTemperature() with a reliable communication" << endl;
        cout << "Low Cryogenic Temperature value: " << rc.lowTemperature(voltage2Kelvin) << endl;
        cout << "Done!\n" << endl;

        // Test the highTemperature()
        cout << "Test highTemperature() with a reliable communication" << endl;
        cout << "High Cryogenic Temperature value: " << rc.highTemperature(voltage2Kelvin) << endl;
        cout << "Done!\n" << endl;

        // Test the isCoolHeadOn()
        cout << "Test isCoolHeadOn() with a reliable communication" << endl;
        cout << "It the cool head ON? " << (rc.isCoolHeadOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setCoolHeadOn()
        cout << "Test setCoolHeadOn() with a reliable communication" << endl;
        rc.setCoolHeadOn();
        cout << "Done!\n" << endl;

        // Test the isCoolHeadOn()
        cout << "Test isCoolHeadOn() with a reliable communication" << endl;
        cout << "It the cool head ON? " << (rc.isCoolHeadOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setCoolHeadOff()
        cout << "Test setCoolHeadOff() with a reliable communication" << endl;
        rc.setCoolHeadOff();
        cout << "Done!\n" << endl;

        // Test the isCoolHeadOn()
        cout << "Test isCoolHeadOn() with a reliable communication" << endl;
        cout << "It the cool head ON? " << (rc.isCoolHeadOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test lna()
        cout << "Test lna() with a reliable communication" << endl;
        FetValues values = rc.lna(0, 4, currentConverter, voltageConverter);
        cout << "LNA left values of feed 0, stage 4: (VDL=" << values.VDL << ", IDL=" << values.IDL;
        cout << ", VGL=" << values.VGL << ")" << endl;
        cout << "LNA right values of feed 0, stage 4: (VDR=" << values.VDR << ", IDR=" << values.IDR;
        cout << ", VGR=" << values.VGR << ")" << endl;
        cout << endl;
        values = rc.lna(1, 2, currentConverter);
        cout << "LNA left values of feed 1, stage 2: (VDL=" << values.VDL << ", IDL=" << values.IDL;
        cout << ", VGL=" << values.VGL << ")" << endl;
        cout << "LNA right values of feed 1, stage 2: (VDR=" << values.VDR << ", IDR=" << values.IDR;
        cout << ", VGR=" << values.VGR << ")" << endl;
        cout << "Done!\n" << endl;

        // Test the isCalibrationOn()
        cout << "Test isCalibrationOn() with a reliable communication" << endl;
        cout << "It the noise mark generator ON? " << (rc.isCalibrationOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumSensorOff()
        cout << "Test setVacuumSensorOff() with a reliable communication" << endl;
        rc.setVacuumSensorOff();
        cout << "Done!\n" << endl;

        // Test the isVacuumSensorOn()
        cout << "Test isVacuumSensorOn() with a reliable communication" << endl;
        cout << "It the vacuum sensor ON? " << (rc.isVacuumSensorOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumSensorOn()
        cout << "Test setVacuumSensorOn() with a reliable communication" << endl;
        rc.setVacuumSensorOn();
        cout << "Done!\n" << endl;

        // Test the isVacuumSensorOn()
        cout << "Test isVacuumSensorOn() with a reliable communication" << endl;
        cout << "It the vacuum sensor ON? " << (rc.isVacuumSensorOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumPumpOff()
        cout << "Test setVacuumPumpOff() with a reliable communication" << endl;
        rc.setVacuumPumpOff();
        cout << "Done!\n" << endl;

        // Test the isVacuumPumpOn()
        cout << "Test isVacuumPumpOn() with a reliable communication" << endl;
        cout << "It the vacuum pump ON? " << (rc.isVacuumPumpOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumPumpOn()
        cout << "Test setVacuumPumpOn() with a reliable communication" << endl;
        rc.setVacuumPumpOn();
        cout << "Done!\n" << endl;

        // Test the isVacuumPumpOn()
        cout << "Test isVacuumPumpOn() with a reliable communication" << endl;
        cout << "It the vacuum pump ON? " << (rc.isVacuumPumpOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumValveOff()
        cout << "Test setVacuumValveOff() with a reliable communication" << endl;
        rc.setVacuumValveOff();
        cout << "Done!\n" << endl;

        // Test the isVacuumValveOn()
        cout << "Test isVacuumValveOn() with a reliable communication" << endl;
        cout << "It the vacuum valve ON? " << (rc.isVacuumValveOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumValveOn()
        cout << "Test setVacuumValveOn() with a reliable communication" << endl;
        rc.setVacuumValveOn();
        cout << "Done!\n" << endl;

        // Test the isVacuumValveOn()
        cout << "Test isVacuumValveOn() with a reliable communication" << endl;
        cout << "It the vacuum valve ON? " << (rc.isVacuumValveOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isRemoteOn()
        cout << "Test isRemoteOn() with a reliable communication" << endl;
        cout << "It the remote command enable? " << (rc.isRemoteOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

    }
    catch(ReceiverControlEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }


    return 0;
}
