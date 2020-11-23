#ifndef RECEIVER_CONTROL_C_BAND
#define RECEIVER_CONTROL_C_BAND

#include <ReceiverControl.h>

/**
 * @brief Receiveri Controll child class
 * @details We need to extend some functionalities as C Hi/LOW receiver selection
 * 
 */
class ReceiverControlCBand : public IRA::Receviercontrol {

public:

    /** 
	 * Constructor
	 * @param dewar_ip the IP address of the dewar control board
	 * @param dewar_port the port of the dewar control board
	 * @param lna_ip the IP address of the LNA control board
	 * @param lna_port the port of the LNA control board
     * @param guard_time the time in useconds we need to wait between a request and an
     * @param number_of_feeds number of feeds (default 1)
	 * @param switch_ip the IP address of the switches control board
	 * @param switch_port the port of the switches control board
     * @params dewar_maddr the dewar board master address (default 0x7C)
     * @params dewar_saddr the dewar board slave address (default 0x7F)
     * @params lna_maddr the LNA board master address (default 0x7C)
     * @params lna_saddr the LNA board slave address (default 0x7F)
     * @params switch_maddr the switches board master address (default 0x7C)
     * @params switch_saddr the switches board slave address (default 0x7F)
     * @param reliable_comm when it is true then the communication
     * to the board is reliable because there is a checksum field in
     * both request and answer. It is true to default.
     * answer to the LNA board
     * @throw ReceiverControlEx
	*/
    ReceiverControlCBand(
            const std::string dewar_ip,
            const unsigned short dewar_port, 
            const std::string lna_ip, 
            const unsigned short lna_port, 
            const unsigned int guard_time=250000,  // 250000 us == 0.25 seconds
            const unsigned short number_of_feeds=1,
            const std::string switch_ip="", 
            const unsigned short switch_port=0, 
            const BYTE dewar_madd=0x7C, // Dewar board master address
            const BYTE dewar_sadd=0x7F, // Dewar board slave address - We use broadcast address
            const BYTE lna_madd=0x7C,   // LNA board master address
            const BYTE lna_sadd=0x7F,   // LNA board slave address - We use broadcast address
            const BYTE switch_madd=0x7C,   // Switch board master address
            const BYTE switch_sadd=0x7F,   // Switch board slave address - We use broadcast address
            bool reliable_comm=false
    ) throw (ReceiverControlEx);

    /**
     * @brief Destroy the Receiver Control object
     * 
     */
    ~ReceiverControlCBand();

    
    /** Set HI C Receiver
     *  @param data_type the type of the data; the default type is 1 bit
     *  @param port_type the port type; the default port is the Digital IO
     *  @param port_number the port number; the default port number is 11
     *  @param value the value to set; the default value is 0x01
     *  @throw ReceiverControlEx
     */
    void setReceiverHigh(
            const BYTE data_type=MCB_CMD_DATA_TYPE_B01, 
            const BYTE port_type=MCB_PORT_TYPE_DIO, 
            const BYTE port_number=MCB_PORT_NUMBER_01, 
            const BYTE value=0x01
    ) throw (ReceiverControlEx);

    /** Set LOW C Receiver
     *  @param data_type the type of the data; the default type is 1 bit
     *  @param port_type the port type; the default port is the Digital IO
     *  @param port_number the port number; the default port number is 11
     *  @param value the value to set; the default value is 0x01
     *  @throw ReceiverControlEx
     */
    void setReceiverLow(
            const BYTE data_type=MCB_CMD_DATA_TYPE_B01, 
            const BYTE port_type=MCB_PORT_TYPE_DIO, 
            const BYTE port_number=MCB_PORT_NUMBER_01, 
            const BYTE value=0x00
    ) throw (ReceiverControlEx);

private:


};


#endif