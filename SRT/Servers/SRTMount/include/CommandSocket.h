#ifndef COMMANDSOCKET_H_
#define COMMANDSOCKET_H_


/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id:  $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/09/2009      Creation                                         */


#include <IRA>
#include <AntennaErrors.h>
#include <ComponentErrors.h>
#include "CommonData.h"
#include "Configuration.h"
#include "ACUProtocol.h"

/**
 * This class is inherited from the IRA::CSocket class. It takes charge of the management of the socket used to command the SRT ACU via the remote interface. 
  * if the remote side disconnects or a problem affects the comunication line, this class try to reconnect to the ACU until the communication is up again..
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CCommandSocket: public IRA::CSocket
{
public:
	/**
	 * Constructor.
	*/
	CCommandSocket();
	/**
	 * Destructor
	*/
	virtual ~CCommandSocket();
	
	/**
     * This member function is used to enstablish and configure the communication channel to the ACU. 
	 * This must be the first call before using any other function of this class.
	 * @param config pointer to the component configuration data structure
	 * @param data pointer to the ACU data structure
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c IRALibraryResource>
	*/
	void init(CConfiguration *config,IRA::CSecureArea<CCommonData> *data) throw (ComponentErrors::SocketErrorExImpl);
	
	/**
	 * Called to close the socket and free everything.
	 */
	void cleanUp();
	
	/**
	 * Force the socket to connect to the ACU. If the operation fails an exception is thrown, if the operation could not succesfully complete immediately 
	 * the command line state is set to <i>ACU::ACU_CNTDING</i>. It must be called by an external watchdog in order to keep the connection enstablished.
	 * This is not synchronized because this method acts on the socket only by accessing the mutex for the <i>CCommonData</i> class as the background thread of this socket @sa <i>onconnect()</i> 
	 */
	void connectSocket() throw (ComponentErrors::SocketErrorExImpl);
		
	/**
	 * This method can be used to command the telescope to fixed position. Range check are performed before commanding, if a value is beyond the hardware limits of the telescope,
	 * this value is forced inside. The azimuth coordinate commanded effectively to the ACU depands on the current position and on cable wrapping considerations.   
	 * @param az azimuth position in degrees.
	 * @param el elevation positions in degrees. 
	 * @throw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @throw AntennaErrors::AntennaBusyExImpl
	 * @throw AntennaErrors::NakExImpl
	 * @throw AntennaErrors::OperationNotPermittedExImpl
	 */
	void preset(const double& az,const double& el) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,
			AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl,AntennaErrors::OperationNotPermittedExImpl);
	
	
	void programTrack(const double& az,const double& el,const ACS::Time& time,bool clear) throw (AntennaErrors::NakExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,
			ComponentErrors::TimeoutExImpl,AntennaErrors::OperationNotPermittedExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl);
	
	/**
	 * This function can be used to command new axis mode to the ACU. Even if the methods allows for different mode for elevation and azimuth it fails if the modes are not the same.
	 * the accepted modes are <i>Antenna::ACU_RATE</i>, <i>Antenna::ACU_PRESET</i> and <i>Antenna::ACU_PROGRAMTRACK</i>. 
	 * @param azMode this argument selects the mode for the azimuth
	 * @param elmode this argument selects the mode for elevation 
	 * @thorw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @thorw AntennaErrors::AntennaBusyExImpl
	 * @thorw AntennaErrors::NakExImpl
	 * @thorw ComponentErrors::ValidationErrorExImpl
	 */
	void changeMode(const Antenna::TCommonModes& azMode,const Antenna::TCommonModes& elMode) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,
			AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl,ComponentErrors::ValidationErrorExImpl);

	/**
	 * This function can be used to stop all movement and other activities of the mount.  
	 * @thorw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @thorw AntennaErrors::AntennaBusyExImpl
	 * @thorw AntennaErrors::NakExImpl
	*/	
	void stop() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * This function can be used to deactivate the telescope axis, a sort of standby.  
	 * @thorw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @thorw AntennaErrors::AntennaBusyExImpl
	 * @thorw AntennaErrors::NakExImpl
	*/		
	void deactivate() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl);

	/**
	 * This function can be used to explicitly activate the telescope axis, generally speaking it should not be necessary because any other action that requires an active axis will take care of
	 * issuing an activate command before.
	 * @thorw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @thorw AntennaErrors::AntennaBusyExImpl
	 * @thorw AntennaErrors::NakExImpl
	*/
	void activate() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * This function can be used to reset the error condition on the axis.  
	 * @thorw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @thorw AntennaErrors::AntennaBusyExImpl
	 * @thorw AntennaErrors::NakExImpl
	*/		
	void resetErrors() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * This function can be called in order to command new position offsets for both axis of the mount. Offsets are considered only if the currently configured mode is <i>ACU_PRESET</i>
	 * or <i>ACU_PROGRAMTRACK</i>.
	 * @param azOff azimuth offset in degrees
	 * @param elOff elevation offset in degtrees
	 * @thorw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @thorw AntennaErrors::AntennaBusyExImpl
	 * @thorw AntennaErrors::NakExImpl
	 * @throw AntennaErrors::OperationNotPermittedExImpl
	 */
	void setOffsets(const double& azOff,const double& elOff) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,
			AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl,AntennaErrors::OperationNotPermittedExImpl);

	/**
	 * This function can be called in order to synchronize the mount servo system
	 * @param time reference time in 100ns units
	 * @thorw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @thorw AntennaErrors::AntennaBusyExImpl
	 * @thorw AntennaErrors::NakExImpl
	 */	
	void setTime(const ACS::Time& time) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,
			AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl);

	/**
	 * This function can be called in order to set a new time offset for the program track mode
	 * @param seconds number of seconds and fraction to be used as new offset
	 * @thorw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @thorw AntennaErrors::AntennaBusyExImpl
	 * @thorw AntennaErrors::NakExImpl
	 */	
	void setTimeOffset(const double& seconds) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,
			AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * This function allows to move the telescope by providing constant rates for both axis. Range check are done, if rates are commanded beyond the limits, they are downgraded accordingly, 
	 * in order to reflect the hardware limits of the telescope. The component must not be  <i>>ACU_BUSY</i> and the proper mode must be configured (<i>ACU_RATE</i>).
	 * @param azRate rate to be adopted for azimuth (degrees per second)
	 * @param elRate rate to be adopted for the elevation (degrees per second)
	 * @thorw ComponentErrors::TimeoutExImpl
	 * @thorw ComponentErrors::SocketErrorExImpl
	 * @thorw AntennaErrors::ConnectionExImpl
	 * @thorw AntennaErrors::AntennaBusyExImpl
	 * @thorw AntennaErrors::NakExImpl
	 * @thorw AntennaErrors::OperationNotPermittedExImpl 
	 */ 
	void rates(const double& azRate,const double& elRate) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,
			AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl,AntennaErrors::OperationNotPermittedExImpl);
	
	/**
	 * Set the commanded sector to be a specific one.
	 * @param section new azimuth section to be used for the next track point.
	 */
	void forceSector(const Antenna::TSections& section);
	
	/**
	 * Sends the Antenna to survival position.
	 *  @throw ComponentErrors::TimeoutExImpl
	 *  @throw ComponentErrors::SocketErrorExImpl
	 *  @throw AntennaErrors::ConnectionExImpl
	 *  @throw AntennaErrors::AntennaBusyExImpl
	 *  @throw AntennaErrors::NakExImpl
	 */
	void stow() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl);

	/**
	 *  Extracts the stowpins
	 *  @throw ComponentErrors::TimeoutExImpl
	 *  @throw ComponentErrors::SocketErrorExImpl
	 *  @throw AntennaErrors::ConnectionExImpl
	 *  @throw AntennaErrors::AntennaBusyExImpl
	 *  @throw AntennaErrors::NakExImpl
	 */
	void unstow() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * Checks if the telescope is stowed, if so it also clear the busy state of the control line. 
	 * @param startTime time the stow procedure begun
	 * @param timeout true if the timeout has to be thrown
	 * @param stopped true if the stow procedure has been stopped by external action
	 * @param stowed true if the stow procedure was successful 
	 * @return the remaining time before the time out
	 */
	ACS::TimeInterval checkIsStowed(const ACS::Time& startTime,bool& timeout,bool& stopped,bool& stowed);

	/**
	 * Checks if the telescope is unstowed, if so it also clear the busy state of the control line. 
	 * @param startTime time the unstow procedure begun
	 * @param timeout true if the timeout has to be thrown
	 * @param stopped true if the stow procedure has been stopped by external action
	 * @param unstowed true if the unstow procedure was successful 
	 * @return the remaining time before the time out
	 */
	ACS::TimeInterval checkIsUnstowed(const ACS::Time& startTime,bool& timeout,bool& stopped,bool& unstowed);
		
protected:
	/**
	 * Automatically called by the framework as a result of a connection request. See super-class for more details.
	 * @param  ErrorCode connection operation result, zero means operation is succesful.
	*/
	void onConnect(int ErrorCode);
	
private:
	
	template <class X> class autoArray {
	public:
		autoArray(): m_message(NULL) {}
		~autoArray() { if (m_message!=NULL) delete [] m_message; }
		operator X *&() { return m_message; }
		operator const X *() const { return m_message; }
		X operator[](WORD pos) const { return m_message[pos]; }
		X& operator[](WORD pos) { return m_message[pos]; }
	private:
		X* m_message;
	};

	CConfiguration * m_pConfiguration;
	IRA::CSecureArea<CCommonData> * m_pData;
	
	CACUProtocol m_protocol;

	bool m_bTimedout;
	
	/**
	 * This mutex is used to sync external calls to the public methods of the class. The requirements is that just one call at the time must be served if the methos requires to
	 * send a message to the ACU. the reason for that is that we need to wait to known the result of the previous command to the ACU that comes form the status socket.
	 */
	ACE_Recursive_Thread_Mutex m_syncMutex;
	
	/**
	 * Stores the program track points until the minimum number has been reached
	*/
	CACUProtocol::TProgramTrackPoint m_ptTable[CACUProtocol::PROGRAMTRACK_TABLE_MINIMUM_LENGTH];

	/**
	 * size of the program track points
	 */ 
	WORD m_ptSize;
	
	ACS::Time m_lastScanEpoch;
	
	/**
	 * This methods will check is the axis of the telescope are active, if not it will activate them. 
	 * An active axis has the control loop closed, the servo amplifier enabled and the brakes open.
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw ComponentErrors::SocketErrorExImpl
	 * @throw AntennaErrors::NakExImpl
	 */
	void activate_command() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);

	/**
	 * This methods will check is the axis of the telescope are active, in that case it will deactivate them. 
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw ComponentErrors::SocketErrorExImpl
	 * @throw AntennaErrors::NakExImpl
	 */	
	void deactivate_command() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);

	/**
	 * This methods will send the telescope to survival position 
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw ComponentErrors::SocketErrorExImpl
	 * @throw AntennaErrors::NakExImpl
	 */	
	void stow_command() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * This methods will extract the telescope from survival position. 
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw ComponentErrors::SocketErrorExImpl
	 * @throw AntennaErrors::NakExImpl
	 */		
	void unstow_command() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * This methods will command a new operating mode to the ACU 
	 * @param mode this argument is the mode to be commanded 
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw ComponentErrors::SocketErrorExImpl
	 * @throw AntennaErrors::NakExImpl
	 */	
	void changeMode_command(const Antenna::TCommonModes& mode) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);

	/**
	 * This method will command the offsets to be applied to both axis.
	 * @param azOff azimuth offset (degrees)
	 * @param elOff elevation offset (degrees)
	 */ 
	void setOffsets_command(const double& azOff,const double& elOff) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * This method will command the stop mode to the ACU. when this command is received the axis movement stops but they still remain active.
	 */
	void stop_command() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * This method will command the reset errors message to the ACU. 
	 */	
	void resetErrors_command() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * This method will allows to sync the servo system.
	 * @param time reference time
	 */
	void setTime_command(const ACS::Time& time) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * this method allows to configure a new time offset in the ACU
	 * @param seconds number of seconds and fraction to be loaded as time offset
	 */
	void setTimeOffset_command(const double& seconds) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * this method allows to sed a preset position to the ACU.
	 * @param az azimuith position in degrees
	 * @param el elevation position in degrees
	 */
	void preset_command(const double& az,const double& el) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl);
	
	/**
	 * Creates and setup the socket in charge of sending commands to the ACU. 
	 */ 
	void createSocket() throw (ComponentErrors::SocketErrorExImpl);
	
	/**
    * This member function is called to send a buffer to the ACU.
	 * @param Msg pointer to the byte buffer that contains the message for the ACU
	 * @param Len length of the buffer
	 * @param error It will contain the error description in case the operation fails.
	 * @return SUCCESS if the buffer was sent correctly, WOULDBLOCK if the send would block, FAIL in case of error, the parameter error is set accordingly.
	*/
	OperationResult sendBuffer(const BYTE *Msg,const WORD& Len,CError& error);
	
	/**
	 * This methos will cycle to check is the ACU responded to the last command sent. There three possible scenarios:
	 * @arg the ACU responded and the operation was succesfull
	 * @arg the ACU did not responded yet (in that case a timeout policy is implemented)
	 * @arg the ACU responded but the operation was not completed correctly.
	 * @param commandTime marks the epoch the command message was sent to the ACU, used to apply the timeout mechanism.
	 * @throw NakExImpl if an error was repoerted by the ACU
	 * @throw TimeoutExImpl if the ACU does not report back the answer within the expected time
	 */
	void waitAck(const ACS::Time& commandTime) throw (AntennaErrors::NakExImpl,ComponentErrors::TimeoutExImpl);
		
	/**
	 * This function can be used to send a command to the ACU.
	 * In case of problems it changes the component status accordingly. 
	 * @param cmd command to be sent
	 * @param len lenght in bytes of the command
	 * @throw TimeoutExImpl if the operation timeout before completing. 
	 * @thorw SocketErrorExImpl if the connection fails before completing. In that case the status is set to <i>Antenna::ACU_NOTCNTD</i>
	 */
	void sendCommand(const BYTE *cmd,const WORD& len) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl);
	
	/**
	 * This function checks if the connection is ok. 
	 * @return true if the connection is ok, false is the connection is not available.
	 */
	bool checkConnection();
	
	/**
	 * This method check if the Antenna is busy with long jobs and could not accept new commands.
	 * @return true if the antenna is busy
	 */
	bool checkIsBusy();
	
};





#endif /*COMMANDSOCKET_H_*/
