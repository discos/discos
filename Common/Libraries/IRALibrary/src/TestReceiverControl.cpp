#include "ReceiverControl.h"
#include <cstdlib>

using namespace IRA;

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
    unsigned short feeds = 2; 
    std::vector<BYTE> data;


    // Test the constructor 
    try {
        cout << "\nConnecting to the boards (LNA and deward)..." << endl;
        ReceiverControl rc = ReceiverControl(dewar_IP, dewar_port, lna_IP, lna_port, feeds);
        cout << "Connection" << " done!" << endl << endl;
    
        // Test the setCalibrationOn()
        cout << "Test setCalibrationOn() with a reliable communication" << endl;
        rc.setCalibrationOn();
        cout << "Done!\n" << endl;

        // Test the isCalibrationOn()
        cout << "Test isCalibrationOn() with a reliable communication" << endl;
        cout << "Is the noise mark generator ON? " << (rc.isCalibrationOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setCalibrationOff()
        cout << "Test setCalibrationOff()" << endl;
        rc.setCalibrationOff();
        cout << "Done!\n" << endl;

        // Test the isCalibrationOn()
        cout << "Test isCalibrationOn() with a reliable communication" << endl;
        cout << "Is the noise mark generator ON? " << (rc.isCalibrationOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;
    
        // Test the setExtCalibrationOn()
        cout << "Test setExtCalibrationOn() with a reliable communication" << endl;
        rc.setExtCalibrationOn();
        cout << "Done!\n" << endl;

        // Test the isExtCalibrationOn()
        cout << "Test isExtCalibrationOn() with a reliable communication" << endl;
        cout << "Is the external noise mark generator command enabled? " << (rc.isExtCalibrationOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;
    
        // Test the setExtCalibrationOff()
        cout << "Test setExtCalibrationOff() with a reliable communication" << endl;
        rc.setExtCalibrationOff();
        cout << "Done!\n" << endl;

        // Test the isExtCalibrationOff()
        cout << "Test isExtCalibrationOn() with a reliable communication" << endl;
        cout << "Is the external noise mark generator command enabled? " << (rc.isExtCalibrationOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setReliableCommOff()
        cout << "Test setReliableCommOff()" << endl;
        rc.setReliableCommOff();
        cout << "Done!\n" << endl;

        // Test the vacuum() voltage value, without conversion
        cout << "Test vacuum()" << endl;
        cout << "Vacuum value before conversion [Volt]: " << rc.vacuum() << endl;
        cout << "Done!\n" << endl;

        // Test the vertexTemperature() voltage value, without conversion
        cout << "Test vertexTemperature()" << endl;
        cout << "Vertex temperature value before conversion [Volt]: " << rc.vertexTemperature() << endl;
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

        // Test the cryoTemperature(1)
        cout << "Test cryoTemperature(1) with a reliable communication" << endl;
        cout << "First Cryogenic Temperature value: " << rc.cryoTemperature(1, voltage2Kelvin) << endl;
        cout << "Done!\n" << endl;

        // Test the cryoTemperature(2)
        cout << "Test cryoTemperature(2) with a reliable communication" << endl;
        cout << "Second Cryogenic Temperature value: " << rc.cryoTemperature(2, voltage2Kelvin) << endl;
        cout << "Done!\n" << endl;

        // Test the cryoTemperature(3)
        cout << "Test cryoTemperature(3) with a reliable communication" << endl;
        cout << "Third Cryogenic Temperature value: " << rc.cryoTemperature(3, voltage2Kelvin) << endl;
        cout << "Done!\n" << endl;

        // Test the cryoTemperature(4)
        cout << "Test cryoTemperature(4) with a reliable communication" << endl;
        cout << "Fouth Cryogenic Temperature value: " << rc.cryoTemperature(4, voltage2Kelvin) << endl;
        cout << "Done!\n" << endl;

        // Test the isCoolHeadOn()
        cout << "Test isCoolHeadOn() with a reliable communication" << endl;
        cout << "Is the cool head ON? " << (rc.isCoolHeadOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setCoolHeadOn()
        cout << "Test setCoolHeadOn() with a reliable communication" << endl;
        rc.setCoolHeadOn();
        cout << "Done!\n" << endl;

        // Test the isCoolHeadOn()
        cout << "Test isCoolHeadOn() with a reliable communication" << endl;
        cout << "Is the cool head ON? " << (rc.isCoolHeadOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setCoolHeadOff()
        cout << "Test setCoolHeadOff() with a reliable communication" << endl;
        rc.setCoolHeadOff();
        cout << "Done!\n" << endl;

        // Test the isCoolHeadOn()
        cout << "Test isCoolHeadOn() with a reliable communication" << endl;
        cout << "Is the cool head ON? " << (rc.isCoolHeadOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the turnLeftLNAsOff()
        cout << "Test turnLeftLNAsOff() with a reliable communication" << endl;
        rc.turnLeftLNAsOff();
        cout << "Done!\n" << endl;

        sleep(5);

        // Test fetValues()
        cout << "Test fetValues() with a reliable communication" << endl;
        ReceiverControl::FetValues values = rc.fetValues(0, 4, currentConverter, voltageConverter);
        cout << "LNA left values of feed 0, stage 4: (VDL=" << values.VDL << ", IDL=" << values.IDL;
        cout << ", VGL=" << values.VGL << ")" << endl;
        cout << "LNA right values of feed 0, stage 4: (VDR=" << values.VDR << ", IDR=" << values.IDR;
        cout << ", VGR=" << values.VGR << ")" << endl;
        cout << endl;
        values = rc.fetValues(1, 2, currentConverter);
        cout << "LNA left values of feed 1, stage 2: (VDL=" << values.VDL << ", IDL=" << values.IDL;
        cout << ", VGL=" << values.VGL << ")" << endl;
        cout << "LNA right values of feed 1, stage 2: (VDR=" << values.VDR << ", IDR=" << values.IDR;
        cout << ", VGR=" << values.VGR << ")" << endl;
        cout << "Done!\n" << endl;

        sleep(5);

        // Test the turnLeftLNAsOn()
        cout << "Test turnLeftLNAsOn() with a reliable communication" << endl;
        rc.turnLeftLNAsOn();
        cout << "Done!\n" << endl;

        // Test fetValues()
        cout << "Test fetValues() with a reliable communication" << endl;
        values = rc.fetValues(0, 4, currentConverter, voltageConverter);
        cout << "LNA left values of feed 0, stage 4: (VDL=" << values.VDL << ", IDL=" << values.IDL;
        cout << ", VGL=" << values.VGL << ")" << endl;
        cout << "LNA right values of feed 0, stage 4: (VDR=" << values.VDR << ", IDR=" << values.IDR;
        cout << ", VGR=" << values.VGR << ")" << endl;
        cout << endl;
        values = rc.fetValues(1, 2, currentConverter);
        cout << "LNA left values of feed 1, stage 2: (VDL=" << values.VDL << ", IDL=" << values.IDL;
        cout << ", VGL=" << values.VGL << ")" << endl;
        cout << "LNA right values of feed 1, stage 2: (VDR=" << values.VDR << ", IDR=" << values.IDR;
        cout << ", VGR=" << values.VGR << ")" << endl;
        cout << "Done!\n" << endl;

        sleep(3);

        // Test stageValues()
        cout << "Test stageValues() with a reliable communication" << endl;
        ReceiverControl::StageValues svalues = rc.stageValues(ReceiverControl::DRAIN_VOLTAGE, 4, voltageConverter);
        cout << "Drain voltages for the stage number 4:\n" << endl;
        cout << "\tLeft channel:\n";
        std::vector<double>::iterator liter = (svalues.left_channel).begin();
        std::vector<double>::iterator lend = (svalues.left_channel).end();
        for(int idx=0; liter != lend; liter++, idx++) 
            cout << "Item " << idx << ": " << *liter << endl;
        cout << "\tRight channel:\n";
        std::vector<double>::iterator riter = (svalues.right_channel).begin();
        std::vector<double>::iterator rend = (svalues.right_channel).end();
        for(int idx = 0; riter != rend; riter++, idx++) 
            cout << "Item " << idx << ": " << *riter << endl;
        cout << "Done!\n" << endl;

        // Test the isCalibrationOn()
        cout << "Test isCalibrationOn() with a reliable communication" << endl;
        cout << "Is the noise mark generator ON? " << (rc.isCalibrationOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumSensorOff()
        cout << "Test setVacuumSensorOff() with a reliable communication" << endl;
        rc.setVacuumSensorOff();
        cout << "Done!\n" << endl;

        // Test the isVacuumSensorOn()
        cout << "Test isVacuumSensorOn() with a reliable communication" << endl;
        cout << "Is the vacuum sensor ON? " << (rc.isVacuumSensorOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumSensorOn()
        cout << "Test setVacuumSensorOn() with a reliable communication" << endl;
        rc.setVacuumSensorOn();
        cout << "Done!\n" << endl;

        // Test the isVacuumSensorOn()
        cout << "Test isVacuumSensorOn() with a reliable communication" << endl;
        cout << "Is the vacuum sensor ON? " << (rc.isVacuumSensorOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumPumpOff()
        cout << "Test setVacuumPumpOff() with a reliable communication" << endl;
        rc.setVacuumPumpOff();
        cout << "Done!\n" << endl;

        // Test the isVacuumPumpOn()
        cout << "Test isVacuumPumpOn() with a reliable communication" << endl;
        cout << "Is the vacuum pump ON? " << (rc.isVacuumPumpOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumPumpOn()
        cout << "Test setVacuumPumpOn() with a reliable communication" << endl;
        rc.setVacuumPumpOn();
        cout << "Done!\n" << endl;

        // Test the isVacuumPumpOn()
        cout << "Test isVacuumPumpOn() with a reliable communication" << endl;
        cout << "Is the vacuum pump ON? " << (rc.isVacuumPumpOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the hasVacuumPumpFault()
        cout << "Test hasVacuumPumpFault() with a reliable communication" << endl;
        cout << "Has the vacuum pump a fault? " << (rc.hasVacuumPumpFault() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumValveOff()
        cout << "Test setVacuumValveOff() with a reliable communication" << endl;
        rc.setVacuumValveOff();
        cout << "Done!\n" << endl;

        // Test the isVacuumValveOn()
        cout << "Test isVacuumValveOn() with a reliable communication" << endl;
        cout << "Is the vacuum valve ON? " << (rc.isVacuumValveOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVacuumValveOn()
        cout << "Test setVacuumValveOn() with a reliable communication" << endl;
        rc.setVacuumValveOn();
        cout << "Done!\n" << endl;

        // Test the isVacuumValveOn()
        cout << "Test isVacuumValveOn() with a reliable communication" << endl;
        cout << "Is the vacuum valve ON? " << (rc.isVacuumValveOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isRemoteOn()
        cout << "Test isRemoteOn() with a reliable communication" << endl;
        cout << "Is the remote command enable? " << (rc.isRemoteOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the selectLO1()
        cout << "Test selectLO1() with a reliable communication" << endl;
        rc.selectLO1();
        cout << "Done!\n" << endl;

        // Test the isLO1Selected()
        cout << "Test isLO1Selected() with a reliable communication" << endl;
        cout << "Is the LO1 selected? " << (rc.isLO1Selected() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isLO2Selected()
        cout << "Test isLO2Selected() with a reliable communication" << endl;
        cout << "Is the LO2 selected? " << (rc.isLO2Selected() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the selectLO2()
        cout << "Test selectLO2() with a reliable communication" << endl;
        rc.selectLO2();
        cout << "Done!\n" << endl;

        // Test the isLO2Selected()
        cout << "Test isLO2Selected() with a reliable communication" << endl;
        cout << "Is the LO2 selected? " << (rc.isLO2Selected() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isLO1Selected()
        cout << "Test isLO1Selected() with a reliable communication" << endl;
        cout << "Is the LO1 selected? " << (rc.isLO1Selected() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isLO2Locked()
        cout << "Test isLO2Locked() with a reliable communication" << endl;
        cout << "Is the LO2 locked? " << (rc.isLO2Locked() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setSingleDishMode()
        cout << "Test setSingleDishMode() with a reliable communication" << endl;
        rc.setSingleDishMode();
        cout << "Done!\n" << endl;

        // Test the isSingleDishModeOn()
        cout << "Test isSingleDishModeOn() with a reliable communication" << endl;
        cout << "Is the single dish mode active? " << (rc.isSingleDishModeOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isVLBIModeOn()
        cout << "Test isVLBIModeOn() with a reliable communication" << endl;
        cout << "Is the VLBI mode active? " << (rc.isVLBIModeOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the setVLBIMode()
        cout << "Test setVLBIMode() with a reliable communication" << endl;
        rc.setVLBIMode();
        cout << "Done!\n" << endl;

        // Test the isVLBIModeOn()
        cout << "Test isVLBIModeOn() with a reliable communication" << endl;
        cout << "Is the VLBI mode active? " << (rc.isVLBIModeOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isSingleDishModeOn()
        cout << "Test isSingleDishModeOn() with a reliable communication" << endl;
        cout << "Is the single dish mode active? " << (rc.isSingleDishModeOn() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isLNABoardConnectionOK()
        cout << "Test isLNABoardConnectionOK() with a reliable communication" << endl;
        cout << "Is the connection to the LNA board OK? " << \
             (rc.isLNABoardConnectionOK() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isDewarBoardConnectionOK()
        cout << "Test isDewarBoardConnectionOK() with a reliable communication" << endl;
        cout << "Is the connection to the dewar board OK? " << \
             (rc.isDewarBoardConnectionOK() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test closeConnection()
        cout << "Test closeConnection() with a reliable communication" << endl;
        rc.closeConnection();
        cout << "Done!\n" << endl;

        // Test the isLNABoardConnectionOK()
        cout << "Test isLNABoardConnectionOK() with a reliable communication" << endl;
        cout << "Is the connection to the LNA board OK? " << \
             (rc.isLNABoardConnectionOK() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        // Test the isDewarBoardConnectionOK()
        cout << "Test isDewarBoardConnectionOK() with a reliable communication" << endl;
        cout << "Is the connection to the dewar board OK? " << \
             (rc.isDewarBoardConnectionOK() == true ? "yes" : "no") << endl;
        cout << "Done!\n" << endl;

        try {
            // Test the vacuum() voltage value, without conversion
            cout << "Test vacuum()" << endl;
            cout << "Vacuum value before conversion [Volt]: " << rc.vacuum() << endl;
            cout << "Done!\n" << endl;
        }
        catch(ReceiverControlEx& ex) {
            cout << ex.what() << endl;
        }

        try {
            // Test fetValues()
            cout << "Test fetValues() with a reliable communication" << endl;
            values = rc.fetValues(0, 4, currentConverter, voltageConverter);
            cout << "LNA left values of feed 0, stage 4: (VDL=" << values.VDL << ", IDL=" << values.IDL;
            cout << ", VGL=" << values.VGL << ")" << endl;
            cout << "LNA right values of feed 0, stage 4: (VDR=" << values.VDR << ", IDR=" << values.IDR;
            cout << ", VGR=" << values.VGR << ")" << endl;
            cout << endl;
            values = rc.fetValues(1, 2, currentConverter);
            cout << "LNA left values of feed 1, stage 2: (VDL=" << values.VDL << ", IDL=" << values.IDL;
            cout << ", VGL=" << values.VGL << ")" << endl;
            cout << "LNA right values of feed 1, stage 2: (VDR=" << values.VDR << ", IDR=" << values.IDR;
            cout << ", VGR=" << values.VGR << ")" << endl;
            cout << "Done!\n" << endl;
        }
        catch(ReceiverControlEx& ex) {
            cout << ex.what() << endl;
        }

        // Test openConnection()
        cout << "Test openConnection() with a reliable communication" << endl;
        rc.openConnection();
        cout << "Done!\n" << endl;

        // Test the cryoTemperature(1)
        cout << "Test cryoTemperature(1) with a reliable communication" << endl;
        cout << "First Cryogenic Temperature value: " << rc.cryoTemperature(1, voltage2Kelvin) << endl;
        cout << "Done!\n" << endl;

        // Test the vacuum() voltage value, without conversion
        cout << "Test vacuum()" << endl;
        cout << "Vacuum value before conversion [Volt]: " << rc.vacuum() << endl;
        cout << "Done!\n" << endl;

        // Test the vacuum()
        cout << "Test vacuum() with a reliable communication" << endl;
        cout << "Vacuum value: " << rc.vacuum(voltage2mbar) << endl;
        cout << "Done!\n" << endl;
    }
    catch(ReceiverControlEx& ex) {
        cout << ex.what() << endl;
        return 1;
    }


    return 0;
}
