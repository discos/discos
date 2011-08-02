/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __RECEIVER_CONTROL__H
#define __RECEIVER_CONTROL__H

#include <string>
#include <vector>
#include <bitset>
#include <IRA>
#include "Definitions.h"
#include "MicroControllerBoard.h"


namespace IRA {

class ReceiverControlEx {

public:
   ReceiverControlEx(const std::string& msg) : m_msg(msg) {}
  ~ReceiverControlEx() {}
   std::string what() const { return(m_msg); }

private:
   const std::string m_msg;
};


struct FetValues {
    double VDL; // Drain Voltage, left channel  [V]
    double IDL; // Drain Current, left channel  [mA]
    double VGL; // Gate Voltage, left channel   [V]
    double VDR; // Drain Voltage, right channel [V]
    double IDR; // Drain Current, right channel [mA]
    double VGR; // Gate Voltage, right channel  [V]
};


/**
 * The left_channel member stores all the left channel values of a specific
 * fet quantity (VD, ID or VG), and the right one stores the values for
 * the right channel.
 */
struct StageValues {
    std::vector<double> left_channel;
    std::vector<double> right_channel;
};


enum FetValue {DRAIN_VOLTAGE, DRAIN_CURRENT, GATE_VOLTAGE};


/** 
 * This class performs a high level library to comunicate via TCP/IP
 * to receivers controlled by the board designed in Medicina (BO, Italy) 
 * by Franco Fiocchi <f.fiocchi@ira.inaf.it>
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * INAF, Osservatorio Astronomico di Cagliari
 * <p>There are two board for each receiver: the first one allows to
 * communicate to the dewar either reading the status bits and voltage values
 * or writing some configurations. The second one supplies the power to 
 * the feed LNAs (Low Noise Amplifier), and also allows to read the values
 * of VDs (drain voltage), IDs (drain current) and VGs (gate voltage).</p>
 * <p>The user of this class just have to know its interface without to see
 * which board perform the request or other low level details.</p>
 * <p>Once you instantiate the ReceiverControl, you can execute the
 * following operations:</p>
 * <ul>
 *     <li>unsigned short numberOfFeeds(): return the number of feeds</li>
 *     <li>double vacuum(double (*converter)(double)): return the vacuum value inside the dewar</li>
 *     <li>double cryoTemperature1(double (*converter)(double)): return the first cryogenic temperature value</li>
 *     <li>double cryoTemperature2(double (*converter)(double)): return the second cryogenic temperature value</li>
 *     <li>double cryoTemperature3(double (*converter)(double)): return the third cryogenic temperature value</li>
 *     <li>double cryoTemperature4(double (*converter)(double)): return the fourth cryogenic temperature value</li>
 *     <li>void setCoolHeadOn(): set to ON the cool head</li>
 *     <li>void setCoolHeadOff(): set to OFF the cool head</li>
 *     <li>bool isCoolHeadOn(): return true if the cool head is ON</li>
 *     <li>void setVacuumSensorOn(): set to ON the vacuum sensor</li>
 *     <li>void setVacuumSensorOff(): set to OFF the vacuum sensor</li>
 *     <li>bool isVacuumSensorOn(): return true if the vacuum sensor is ON</li>
 *     <li>void setVacuumPumpOn(): set to ON the vacuum pump</li>
 *     <li>void setVacuumPumpOff(): set to OFF the vacuum pump</li>
 *     <li>bool isVacuumPumpOn(): return true if the vacuum pump is ON</li>
 *     <li>void setVacuumValveOn(): open the valve IMMEDIATELY</li>
 *     <li>void setVacuumValveOff(): disable the valve</li>
 *     <li>bool isVacuumValveOn(): return true if the vacuum valve is opened</li>
 *     <li>void setCalibrationOn(): set the noise mark to ON</li>
 *     <li>void setCalibrationOff(): set the noise mark to OFF</li>
 *     <li>bool isCalibrationOn(): is the noise mark generator set to ON?</li>
 *     <li>bool isRemoteOn(): return true if the remote command is enable</li>
 *     <li>void selectLO1(): select the Local Oscillator 1</li>
 *     <li>bool isLO1Selected(): return true if the first Local Oscillator (LO1) is selected</li>
 *     <li>void selectLO2(): select the Local Oscillator 2</li>
 *     <li>bool isLO2Selected(): return true if the second Local Oscillator (LO2) is selected</li>
 *     <li>bool isLO2Locked(): return true if the second Local Oscillator (LO2) is locked</li>
 *     <li>void setSingleDishMode(): set the single dish mode to ON</li>
 *     <li>bool isSingleDishModeOn(): is the single dish mode set to ON?</li>
 *     <li>void setVLBIMode(): set the VLBI mode</li>
 *     <li>bool isVLBIModeOn(): is the VLBI mode set to ON?</li>
 *     <li>void setReliableCommOn(): set the reliable communication to/from the 
 *     board to ON</li>
 *     <li>void setReliableCommOff(): set the reliable communication to/from the 
 *     board to OFF</li>
 *     <li>bool isReliableCommOn(): return true if the communication to the
 *     board is set to be reliable</li>
 *     <li>bool isLNABoardConnectionOK(): return true if the connection to the LNA
 *     board is OK</li>
 *     <li>bool isDewarBoardConnectionOK(): return true if the connection to the 
 *     dewar board is OK</li>
 *     <li>FetValues fetValues(unsigned short feed_number, unsigned short stage_number,
 *     double (*currentConverter)(double voltage), double (*voltageConverter)(double voltage).
 *     return the FetValues (VDL, IDL, VGL, VDR, IDR and VGR) of the LNA of the feed `feed_number`, 
 *     and stage `stage_id`</li>
 *     <li>StageValues stageValues(FetValue quantity, unsigned short stage_number, 
 *     double (*converter)(double voltage)=NULL)
 *     return the StageValues for a given fet ``quantity`` and ``stage_number``. The StageValues
 *     is a struct of two members std::vector<double>, one member for the left channel and one for 
 *     the right one. That members contain the related quantities of all the feeds, that is an 
 *     item of the std::vector<double> is the quantity value of a feed, for the stage requested.</li>
 *     <li>void turnLeftLNAsOn(): turn the the left LNAs ON</li>
 *     <li>void turnLeftLNAsOff(): turn the the left LNAs OFF</li>
 *     <li>void turnRightLNAsOn(): turn the the right LNAs ON</li>
 *     <li>void turnRightLNAsOff(): turn the the right LNAs OFF</li>
 * </ul>
 * 
 */
class ReceiverControl {

public:

	/** 
	 * Constructor
	 * @param dewar_ip the IP address of the dewar control board
	 * @param dewar_port the port of the dewar control board
	 * @param lna_ip the IP address of the LNA control board
	 * @param lna_port the port of the LNA control board
     * @param number_of_feeds number of feeds (default 1)
     * @params dewar_maddr the dewar board master address (default 0x7D)
     * @params dewar_saddr the dewar board slave address (default 0x7F)
     * @params lna_maddr the LNA board master address (default 0x7D)
     * @params lna_saddr the LNA board slave address (default 0x7F)
     * @param reliable_comm when it is true then the communication
     * to the board is reliable because there is a checksum field in
     * both request and answer. It is true to default.
     * @throw ReceiverControlEx
	*/
    ReceiverControl(
            const std::string dewar_ip,
            const unsigned short dewar_port, 
            const std::string lna_ip, 
            const unsigned short lna_port, 
            const unsigned short number_of_feeds=1,
            const BYTE dewar_madd=0x7C, // Dewar board master address
            const BYTE dewar_sadd=0x7D, // Dewar board slave address
            const BYTE lna_madd=0x7C,   // LNA board master address
            const BYTE lna_sadd=0x7D,   // LNA board slave address
            bool reliable_comm=true,
            const unsigned int guard_time=250000  // 250000 us == 0.25 seconds
    ) throw (ReceiverControlEx);

    
    /** Destructor */
    ~ReceiverControl();

    /** Return the number of feeds of the receiver */
    unsigned short numberOfFeeds() { return m_number_of_feeds; }


    /** Set the noise mark generator to ON 
     *  @throw ReceiverControlEx
     */
    void setCalibrationOn() throw (ReceiverControlEx);


    /** Set the noise mark generator to OFF
     *  @throw ReceiverControlEx
     */
    void setCalibrationOff() throw (ReceiverControlEx);


    /** Is the noise mark generator set to ON?
     *  @return true if the noise mark generator is set to ON
     *  @throw ReceiverControlEx
     */
    bool isCalibrationOn() throw (ReceiverControlEx);


    /** Set the reliable communication to/from the board to ON */
    void setReliableCommOn() { m_reliable_comm = true; }


    /** Set the reliable communication to/from the board to OFF */
    void setReliableCommOff() { m_reliable_comm = false; }


    /** return true if the communication to the board is set to be reliable */
    bool isReliableCommOn() { return m_reliable_comm; }


    /** Return the vacuum value inside the dewar in mbar
     *
     *  @param converter pointer to the function that performs the conversion from
     *  voltage to vacumm unit [mbar]; default value is NULL, and in this case the value
     *  returned by vacuum is the voltage value (the value before conversion).
     *  @return the vacuum inside the dewar in mbar if converter != NULL, the value in voltage
     *  (before conversion) otherwise.
     *  @throw ReceiverControlEx
     */
    double vacuum(double (*converter)(double voltage) = NULL) throw (ReceiverControlEx);


    /** Return the first cryogenic temperature
     *
     *  @param converter pointer to the function that performs the conversion from
     *  voltage to Kelvin; default value is NULL, and in this case the value
     *  returned by cryoTemperature1 is the voltage value (the value before conversion).
     *  @return the first cryogenic temperature in Kelvin if converter != NULL, the value in voltage
     *  (before conversion) otherwise.
     *  @throw ReceiverControlEx
     */
    double cryoTemperature1(double (*converter)(double voltage) = NULL) throw (ReceiverControlEx);


    /** Return the second cryogenic temperature
     *
     *  @param converter pointer to the function that performs the conversion from
     *  voltage to Kelvin; default value is NULL, and in this case the value
     *  returned by cryoTemperature2 is the voltage value (the value before conversion).
     *  @return the second cryogenic temperature in Kelvin if converter != NULL, the value in voltage
     *  (before conversion) otherwise.
     *  @throw ReceiverControlEx
     */
    double cryoTemperature2(double (*converter)(double voltage) = NULL) throw (ReceiverControlEx);


    /** Return the third cryogenic temperature
     *
     *  @param converter pointer to the function that performs the conversion from
     *  voltage to Kelvin; default value is NULL, and in this case the value
     *  returned by cryoTemperature3 is the voltage value (the value before conversion).
     *  @return the third cryogenic temperature in Kelvin if converter != NULL, the value in voltage
     *  (before conversion) otherwise.
     *  @throw ReceiverControlEx
     */
    double cryoTemperature3(double (*converter)(double voltage) = NULL) throw (ReceiverControlEx);


    /** Return the fourth cryogenic temperature
     *
     *  @param converter pointer to the function that performs the conversion from
     *  voltage to Kelvin; default value is NULL, and in this case the value
     *  returned by cryoTemperature4 is the voltage value (the value before conversion).
     *  @return the fourth cryogenic temperature in Kelvin if converter != NULL, the value in voltage
     *  (before conversion) otherwise.
     *  @throw ReceiverControlEx
     */
    double cryoTemperature4(double (*converter)(double voltage) = NULL) throw (ReceiverControlEx);

    
    /** Set to ON the cool head 
     *  @throw ReceiverControlEx
     */
    void setCoolHeadOn() throw (ReceiverControlEx);

    
    /** Set to OFF the cool head 
     *  @throw ReceiverControlEx
     */
    void setCoolHeadOff() throw (ReceiverControlEx);


    /** Is the cool head ON?
     *  @return true if the cool head is ON
     *  @throw ReceiverControlEx
     */
    bool isCoolHeadOn() throw (ReceiverControlEx);


    /** Set to ON the vacuum sensor
     *  @throw ReceiverControlEx
     */
    void setVacuumSensorOn() throw (ReceiverControlEx);


    /** Set to OFF the vacuum sensor
     *  @throw ReceiverControlEx
     */
    void setVacuumSensorOff() throw (ReceiverControlEx);


    /** Is the vacuum sensor ON?
     *  @return true if the vacuum sensor is ON 
     *  @throw ReceiverControlEx
     */
    bool isVacuumSensorOn() throw (ReceiverControlEx);


    /** Set to ON the vacuum pump
     *  @throw ReceiverControlEx
     */
    void setVacuumPumpOn() throw (ReceiverControlEx);


    /** Set to OFF the vacuum pump
     *  @throw ReceiverControlEx
     */
    void setVacuumPumpOff() throw (ReceiverControlEx);


    /** Is the vacuum pump ON?
     *  @return true if the vacuum pump is ON
     *  @throw ReceiverControlEx
     */
    bool isVacuumPumpOn() throw (ReceiverControlEx);


    /** Set to ON the vacuum valve
     *  @throw ReceiverControlEx
     */
    void setVacuumValveOn() throw (ReceiverControlEx);


    /** Set to OFF the vacuum valve
     *  @throw ReceiverControlEx
     */
    void setVacuumValveOff() throw (ReceiverControlEx);


    /** Is the vacuum valve ON?
     *  @return true if the vacuum valve is ON
     *  @throw ReceiverControlEx
     */
    bool isVacuumValveOn() throw (ReceiverControlEx);


    /** Is the remote command enable?
     *  @return true if the remote command is enable
     *  @throw ReceiverControlEx
     */
    bool isRemoteOn() throw (ReceiverControlEx);


    /** Select the first local oscillator (LO1)
     *  @throw ReceiverControlEx
     */
    void selectLO1() throw (ReceiverControlEx);


    /** Is LO1 selected?
     *  @return true if the first local oscillator (LO1) is selected
     *  @throw ReceiverControlEx
     */
    bool isLO1Selected() throw (ReceiverControlEx);


    /** Select the second local oscillator (LO2)
     *  @throw ReceiverControlEx
     */
    void selectLO2() throw (ReceiverControlEx);


    /** Is LO2 selected?
     *  @return true if the second local oscillator (LO2) is selected
     *  @throw ReceiverControlEx
     */
    bool isLO2Selected() throw (ReceiverControlEx);


    /** Is LO2 locked?
     *  @return true if the second local oscillator (LO2) is locked
     *  @throw ReceiverControlEx
     */
    bool isLO2Locked() throw (ReceiverControlEx);


    /** Set the single dish mode to ON. The VLBI mode will be turn OFF
     *  @throw ReceiverControlEx
     */
    void setSingleDishMode() throw (ReceiverControlEx);


    /** Is the single dish mode set to ON?
     *  @return true if the single dish mode is active
     *  @throw ReceiverControlEx
     */
    bool isSingleDishModeOn() throw (ReceiverControlEx);


    /** Set the VLBI mode to ON. The SD mode will be turn OFF
     *  @throw ReceiverControlEx
     */
    void setVLBIMode() throw (ReceiverControlEx);


    /** Is the VLBI mode set to ON?
     *  @return true if the VLBI mode is active
     *  @throw ReceiverControlEx
     */
    bool isVLBIModeOn() throw (ReceiverControlEx);

    
    /** return the FetValues (VDL, IDL, VGR, VDR, IDR and VGR) 
     *  of the LNA of the feed `feed_number` and stage `stage_number`. The letter L means
     *  that the value is referred to the left channel, the R if for the right one.
     *  @param feed_number the ID code of the feed (from 0 to 15)
     *  @param stage_number the stage number (from 1 to 5)
     *  @param currentConverter pointer to the function that performs the conversion from
     *  voltage to mA; default value is NULL, and in this case the value
     *  of ID is the voltage value (the value before conversion).
     *  @param voltageConverter pointer to the function that performs the conversion from
     *  voltage to voltage; default value is NULL, and in this case the values
     *  of VD and VG are the voltage values before the conversion.
     *  @return the low cryogenic temperature in Kelvin if converter != NULL, the value in voltage
     *  (before conversion) otherwise.
     *  @return the FetValues, a struct of three double members: VD [V], ID [mA] and VG [V] after
     *  the conversion if the function pointers are not NULL, the values before conversion (voltage)
     *  if the pointers are NULL.
     *  @throw ReceiverControlEx
     */
    FetValues fetValues(
            unsigned short feed_number, 
            unsigned short stage_number,
            double (*currentConverter)(double voltage) = NULL,
            double (*voltageConverter)(double voltage) = NULL
    ) throw (ReceiverControlEx);


    /** Return for each feed and channel the fet quantity value of a given stage. For
     *  instance, if you want to get the VD values of all the feeds related to the amplifier stage N,
     *  you must call the method like so: stageValues(DRAIN_VOLTAGE, N).
     *
     *  @param quantity a FetValue: DRAIN_VOLTAGE, DRAIN_CURRENT or GATE_CURRENT 
     *  @param stage_number the stage number (from 1 to 5)
     *  @param converter pointer to the function that performs the conversion from
     *  voltage to the right unit or just with a scale factor; default value is NULL, and in this 
     *  case the value returned is without conversion.
     *  @return the StageValues for a given fet ``quantity`` and ``stage_number``. The StageValues
     *  is a struct of two members std::vector<double>, one member for the left channel and one for 
     *  the right one. That members contain the related quantities of all the feeds, that is an 
     *  item of the std::vector<double> is the quantity value of a feed, for the stage requested.
     *  @throw ReceiverControlEx
     */
     StageValues stageValues(
             FetValue quantity, 
             unsigned short stage_number, 
             double (*converter)(double voltage)=NULL
    ) throw (ReceiverControlEx);


    /** Turn the LNAs of the left channels ON
     *  @throw ReceiverControlEx
     */
    void turnLeftLNAsOn() throw (ReceiverControlEx);


    /** Turn the LNAs of the left channels OFF
     *  @throw ReceiverControlEx
     */
    void turnLeftLNAsOff() throw (ReceiverControlEx);


    /** Turn the LNAs of the right channels ON
     *  @throw ReceiverControlEx
     */
    void turnRightLNAsOn() throw (ReceiverControlEx);


    /** Turn the LNAs of the right channels OFF
     *  @throw ReceiverControlEx
     */
    void turnRightLNAsOff() throw (ReceiverControlEx);


    /** Perform a TCP connection socket to the boards
     *  @throw ReceiverControlEx
     */
    void openConnection(void) throw (ReceiverControlEx);


    /** Close the TCP connection sockets to the boards */
    void closeConnection(void);


    /** Is the connection to the LNA board OK?
     *  @return true if the connection to the LNA board is OK
     */
    bool isLNABoardConnectionOK();


    /** Is the connection to the dewar board OK?
     *  @return true if the connection to the dewar board is OK
     */
    bool isDewarBoardConnectionOK();


private:
    
	/** Send the request to the board and receive the answer
     * @param board_ptr pointer to the board (dewar of LNA board)
	 * @param command the command code of the request
	 * @param len the number of parameters to send
	 * @param ... the parameters
     * @return a vector of parameters, empty if there are not some
     * @throw MicroControllerBoardEx
	*/
    std::vector<BYTE> makeRequest(MicroControllerBoard *board_ptr, const BYTE command, size_t len, ...) throw(MicroControllerBoardEx);

    
	/** Return the requested value computed from a vector<BYTE> of parameters
	 * @param parameters a vector of BYTEs to join to compute the final value
     * @param RAW_INDEX the index of the PORT (f.i. AD24) from which read the data
     * @return a dobule value computed from a vector<BYTE> of parameters
	*/
    double get_value(const std::vector<BYTE> parameters, const size_t RAW_INDEX);


    /** The IP address of the dewar board */
    const std::string m_dewar_ip;


    /** The port address of the dewar board */
    const unsigned short m_dewar_port; 
    

    /** The IP address of the LNA board */
    const std::string m_lna_ip; 


    /** The port address of the LNA board */
    const unsigned short m_lna_port; 
    

    /** Number of feeds of the receiver */
    const unsigned short m_number_of_feeds;


    /** The master address of the dewar board */
    const BYTE m_dewar_madd;


    /** The slave address of the dewar board */
    const BYTE m_dewar_sadd;


    /** The master address of the LNA board */
    const BYTE m_lna_madd;


    /** The slave address of the LNA board */
    const BYTE m_lna_sadd;


    /** If m_reliable_comm is true then a checksum byte is added to the request
     *  and to the answer during the communication to and from the board.
     *  So m_reliable_comm == true means we have a reliable communication.
     */
    bool m_reliable_comm;


    /** Dewar MicroControllerBoard pointer */
    MicroControllerBoard *m_dewar_board_ptr;


    /** LNA MicroControllerBoard pointer */
    MicroControllerBoard *m_lna_board_ptr;


    /** Sleep time from a SET_DATA and a GET_DATA, needed to stabilize the output */
    const unsigned int m_guard_time; 

};

}

#endif
