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
#include "MicroControllerBoard.h"

const unsigned int GUARD_TIME = 250000; // 250000 us == 0.25 seconds

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
 *     <li>double lowTemperature(double (*converter)(double)): return the low cryogenic temperature value</li>
 *     <li>double highTemperature(double (*converter)(double)): return the high cryogenic temperature value</li>
 *     <li>void setCoolHeadOn(): set to ON the cool head</li>
 *     <li>void setCoolHeadOff(): set to OFF the cool head</li>
 *     <li>bool isCoolHeadOn(): return true if the cool head is ON</li>
 *     <li>void setVacuumSensorOn(): set to ON the vacuum sensor</li>
 *     <li>void setVacuumSensorOff(): set to OFF the vacuum sensor</li>
 *     <li>bool isVacuumSensorOn(): return true if the vacuum sensor is ON</li>
 *     <li>bool isRemoteEnable(): return true if the remote command is enabled</li>
 *     <li>void setVacuumPumpOn(): set to ON the vacuum pump</li>
 *     <li>void setVacuumPumpOff(): set to OFF the vacuum pump</li>
 *     <li>bool isVacuumPumpOn(): return true if the vacuum pump is ON</li>
 *     <li>void setVacuumValveOn(): open the valve IMMEDIATELY</li>
 *     <li>void setVacuumValveOff(): disable the valve</li>
 *     <li>bool isVacuumValveOn(): return true if the vacuum valve is opened</li>
 *     <li>void setCalibrationOn(): set the noise mark to ON</li>
 *     <li>void setCalibrationOff(): set the noise mark to OFF</li>
 *     <li>bool isCalibrationOn(): is the noise mark generator sets to ON?</li>
 *     <li>void setRemoteOn(): enable the remote command</li>
 *     <li>void setRemoteOff(): disable the remote command</li>
 *     <li>bool isRemoteEnable(): return true if the remote command is enable</li>
 *     <li>void selectLO1(): select the Local Oscillator 1</li>
 *     <li>void selectLO2(): select the Local Oscillator 2</li>
 *     <li>void setReliableCommOn(): set the reliable communication to/from the 
 *     board to ON</li>
 *     <li>void setReliableCommOff(): set the reliable communication to/from the 
 *     board to OFF</li>
 *     <li>bool isReliableCommOn(): return true if the communication to the
 *     board is sets to be reliable</li>
 *     <li>bool isLNABoardConnectionOK(): return true if the connection to the LNA
 *     board is OK</li>
 *     <li>bool isDewarBoardConnectionOK(): return true if the connection to the 
 *     dewar board is OK</li>
 *     <li>FetValues lna(unsigned short feed_number, unsigned short stage_number): 
 *     return the FetValues (VDL, IDL, VGL, VDR, IDR and VGR) of the LNA of the feed `feed_number`, 
       and stage `stage_id`</li>
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
            bool reliable_comm=true
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


    /** Is the noise mark generator sets to ON?
     *  @return true if the noise mark generator is set to ON
     *  @throw ReceiverControlEx
     */
    bool isCalibrationOn() throw (ReceiverControlEx);


    /** Set the reliable communication to/from the board to ON */
    void setReliableCommOn() { m_reliable_comm = true; }


    /** Set the reliable communication to/from the board to OFF */
    void setReliableCommOff() { m_reliable_comm = false; }


    /** return true if the communication to the board is sets to be reliable */
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


    /** Return the low cryogenic temperature value
     *
     *  @param converter pointer to the function that performs the conversion from
     *  voltage to Kelvin; default value is NULL, and in this case the value
     *  returned by lowTemperature is the voltage value (the value before conversion).
     *  @return the low cryogenic temperature in Kelvin if converter != NULL, the value in voltage
     *  (before conversion) otherwise.
     *  @throw ReceiverControlEx
     */
    double lowTemperature(double (*converter)(double voltage) = NULL) throw (ReceiverControlEx);


    /** Return the high cryogenic temperature value
     *
     *  @param converter pointer to the function that performs the conversion from
     *  voltage to Kelvin; default value is NULL, and in this case the value
     *  returned by highTemperature is the voltage value (the value before conversion).
     *  @return the high cryogenic temperature in Kelvin if converter != NULL, the value in voltage
     *  (before conversion) otherwise.
     *  @throw ReceiverControlEx
     */
    double highTemperature(double (*converter)(double voltage) = NULL) throw (ReceiverControlEx);

    
    /** Set to ON the cool head 
     *  @throw ReceiverControlEx
     */
    void setCoolHeadOn() throw (ReceiverControlEx);

    
    /** Set to OFF the cool head 
     *  @throw ReceiverControlEx
     */
    void setCoolHeadOff() throw (ReceiverControlEx);


    /** Is the cool head opened?
     *  @return true if the cool head is opened
     *  @throw ReceiverControlEx
     */
    bool isCoolHeadOn() throw (ReceiverControlEx);

    
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
    FetValues lna(
            unsigned short feed_number, 
            unsigned short stage_number,
            double (*currentConverter)(double voltage) = NULL,
            double (*voltageConverter)(double voltage) = NULL
    ) throw (ReceiverControlEx);


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


    /** Number of feeds of the receiver */
    const unsigned short m_number_of_feeds;


    /** If m_reliable_comm is true then a checksum byte is added to the request
     *  and to the answer during the communication to and from the board.
     *  So m_reliable_comm == true means we have a reliable communication.
     */
    bool m_reliable_comm;


    /** Dewar MicroControllerBoard pointer */
    MicroControllerBoard *m_dewar_board_ptr;


    /** LNA MicroControllerBoard pointer */
    MicroControllerBoard *m_lna_board_ptr;

};



#endif
