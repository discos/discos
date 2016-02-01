/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __SENSORSOCKET_H__
#define __SENSORSOCKET_H__

#include <IRA>
#include "acstime.h" 
#include "utils.h"
#include <ComponentErrors.h>

#define BUFF_LEN 5         // Buffer length: request and answer have the same length
#define WORD_LEN 3         // Message byte numbers containing the sensor position
#define CHECKSUM_INDEX 3   // Index of byte in which containing the checksum
#define FB_INDEX 1         // Index of the first byte containing the sensor position

const BYTE STS = 0x02;     // Start of message (Start message To the Sensor)
const BYTE ETS = 0x03;     // Stop of message (End of message To the Sensor)
const BYTE SA = 0x0B;      // Sensor Address encoding, corresponding to sensor serial address "0"
const BYTE PR = 0x04;      // This encoding means a position request
const BYTE AR = 0x05;      // This encoding means an address request


using namespace IRA;

class sensorSocket: public CSocket {

public:

    /**
     * Derotator socket constructor.
     * @arg IP IP address of position sensor
     * @arg port port number of position sensor
     * @arg timeout timeout
     * @arg PS_CF conversion factor from lenght unit (step) to angle unit (degree)
     * @arg PS_REFERENCE zero position of sensor
     * @arg ICD_REFERENCE zero position of ICD
     */
    sensorSocket(CString IP, DWORD port, DWORD timeout, double PS_CF, long REFERENCE_POSITION, double ICD_REFERENCE_POSITION);
    

    virtual ~sensorSocket();

    
   /**
    * Create a socket and make a connection to it 
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void Init() throw (ComponentErrors::SocketErrorExImpl) ;

    
    /**
    * Receive a buffer from the position sensor
    * @param msg pointer to the byte buffer that will contain the message from the sensor
    * @param len number of bytes to be read
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void receiveBuffer(BYTE * msg, WORD len) throw (ComponentErrors::SocketErrorExImpl) ;


    /// Return the sensor position as degree angle
    /**
     * This member function returns the sensor position as degree angle.
     * To get a sensor position I send a PR (Position Request).
     * The response from the sensor contains the position in its three middle
     * bytes. These bytes are converted from binary to decimal and after converted from
     * length unit (step) to angle unit (degree). The Conversion Factor (PS_CF) is compute as follow:
     * 
     * \f[
     *         $$sensor\_length - reference = \alpha^{rad}\cdot R$$
     * \f]
     * 
     * <ul>   
     *   <li><em>sensor_length</em> is the position (in steps) we get from the sensor</li>
     *   <li><em>reference</em> is the zero position of the sensor (688671 steps)</li>
     *   <li><em>R</em> is the radius of derotator (\f$D = 2\cdot R = 68052\,steps\f$) and \f$C = \pi\cdot D\f$ 
     *       is the circumference</li>
     *   <li>\f$\alpha^{r}\f$ is the angle (in radiant) between the sensor position  and the reference</li>
     *   <li>1 step = 0.01 mm</li>
     * </ul>
     *
     * \f[
     *         $$\alpha^{r} = 2\cdot(sensor\_length - reference)/D$$
     * \f]
     * \f[
     *         $$\alpha^\circ = \frac{180}{\pi}\cdot\alpha^{r}$$
     * \f]
     * \f[
     *         $$\alpha^\circ = PS\_CF\cdot(sensor\_length - reference)$$
     * \f]
     *
     * where:
     *   
     * \f[
     *         $$PS\_CF = 360/(\pi\cdot D) = 360/C = 0.001683884$$
     * \f]
     *
     *   Since we want the rotation way of derotator is the same of engine rotation way, we change
     *   the sign of the previous equation:
     *
     * \f[
     *         $$\alpha^\circ = -PS\_CF(sensor\_length - reference)$$
     * \f]
     *
     * @return sensor position as degree angle
     * @throw ComponentErrors::SocketErrorExImpl
     */
    double getPosition() throw (ComponentErrors::SocketErrorExImpl);
    
    
    /**
     * Return the sensor position (double) in the user's reference system
     *
     * @return sensor position (double) in the user's reference system
     * @throw CORBA::SystemException
     */
    double getURSPosition();
    

    /** @var connection status */
    enum soStat { READY, NOTRDY, BROKEN, TOUT, BUSY } m_soStat;   
    

protected:

    /**
     * Computes the checksum byte and set it on buffer word.
     * It makes the arithmetic byte sum
     * @param buf the buffer
     */
    void set_checksum(BYTE* buf);
    

    /** 
    * Recovers the socket functionality.
    * In case of broken socket(err=0), close and try to reopen it
    * on timeout  (err=-2) check the LAN/485 converter functionality sending a dummy buffer
    * on fail (err=-3),  close and try to reopen the socket
    * @return 0 on succesfull reconnection
    * -1 on USD tout, and USD's buffer clear
    * -2 socket tout 
    * -3 socket fail
    */
    int reConnect(int);
    

    /**
    *   flush the input buffer of USD's sending 8 dummy bytes
    *   @returns:
    *   0 for socket success write 
    *   -x on socket error
    */
    int clearUSDBuffer();

        
private:

    double m_sensor_position;
    
    /** @var contains error information */
    CError m_Error;
    
    /** @var indicates when the monitor data buffer has been updated the last time */
    TIMEVALUE m_MonitorTime;
    
    /** @var sensor ip address */    
    const CString m_PS_IP;
    
    /** @var sensor listening port */
    const DWORD m_PS_PORT;
   
    /** @var timeout */
    const DWORD m_PS_TIMEO;

    /** @var sensor conversion factor from step position to angle position */
    const double m_PS_CF;

    /** @var sensor zero position */
    const long m_PS_REFERENCE;

    /** @var icd zero position */
    const double m_ICD_REFERENCE;
};

#endif
