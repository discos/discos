#ifndef COMMANDSOCKET_H_
#define COMMANDSOCKET_H_


#include <IRA>
#include <BackendsErrors.h>
#include <ComponentErrors.h>
#include "Configuration.h"
#include "DBESMprotocol.h"

using namespace std;
/**
 * This class is inherited from the IRA::CSocket class. It takes charge of the management of the socket used to command the SRT ACU via the remote interface. 
  * if the remote side disconnects or a problem affects the comunication line, this class try to reconnect to the ACU until the communication is up again..
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
	void init(CConfiguration *config) throw (ComponentErrors::SocketErrorExImpl);
	
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
	 * This function can be used to send a command to the ACU.
	 * In case of problems it changes the component status accordingly. 
	 * @param cmd command to be sent
	 * @param len lenght in bytes of the command
	 * @throw TimeoutExImpl if the operation timeout before completing. 
	 * @thorw SocketErrorExImpl if the connection fails before completing. In that case the status is set to <i>Antenna::ACU_NOTCNTD</i>
	 */
	void sendCommand(std::string cmd,std::string* outBuff, int all) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl);
		
	void set_all(const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
	
	void set_mode(short b_addr, const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);	
		
	void set_att(short b_addr, short out_ch, double att_val); //throw (BackendsErrors::BackendsErrorsEx);

	void store_allmode(const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
	
	void delete_file(const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
	
	string get_status(short b_addr); //throw (BackendsErrors::BackendsErrorsEx);
	
	string get_comp(short b_addr); //throw (BackendsErrors::BackendsErrorsEx);
	
   string get_diag(short b_addr); //throw (BackendsErrors::BackendsErrorsEx);
   
   string get_cfg();
   
   string set_dbeatt(const char * out_dbe, const char * att_val);
   
   string get_dbeatt(const char * out_dbe);
			
	void parse_longSeq_response(string status_str, string start, string end, ACS::longSeq* vals); //throw (BackendsErrors::BackendsErrorsEx);
   
   void parse_doubleSeq_response(string status_str, string start, string end, ACS::doubleSeq* vals); //throw (BackendsErrors::BackendsErrorsEx);
   
   void parse_double_response(string status_str, string start, string end, double* val); //throw (BackendsErrors::BackendsErrorsEx);
   
   void CCommandSocket::parse_long_response(string status_str, string start, string end, long* val); //throw (BackendsErrors::BackendsErrorsEx)
   
   void parse_string_response(string status_str, string start, string end, string* val);
   
   string get_firm(short b_addr);
   
   string set_dbeamp(const char * out_dbe, const char * amp_val);
   
   string get_dbeamp(const char * out_dbe);

   string set_dbeeq(const char * out_dbe, const char * eq_val);
   
   string get_dbeeq(const char * out_dbe);

   string set_dbebpf(const char * out_dbe, const char * bpf_val);
   
   string get_dbebpf(const char * out_dbe);
      
protected:
	/**
	 * Automatically called by the framework as a result of a connection request. See super-class for more details.
	 * @param  ErrorCode connection operation result, zero means operation is succesful.
	*/
	void onConnect(int ErrorCode);
	

private:

	CConfiguration * m_pConfiguration;
	
	CDBESMCommand m_dbesm_command;

	bool m_bTimedout;
		
	/**
    * This member function is called to send a buffer to the DBESM.
	 * @param Msg pointer to the byte buffer that contains the message for the DBESM
	 * @param Len length of the buffer
	 * @param error It will contain the error description in case the operation fails.
	 * @return SUCCESS if the buffer was sent correctly, WOULDBLOCK if the send would block, FAIL in case of error, the parameter error is set accordingly.
	*/
	OperationResult sendBuffer(std::string Msg,CError& error);

   int receiveBuffer(std::string *Msg,CError& error, int all=0);
	
	void createSocket() throw (ComponentErrors::SocketErrorExImpl);
	
   
	void set_all_command(const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
	
	void set_mode_command(short b_addr, const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
	
   void set_att_command(short b_addr, short out_ch, double att_val); //throw (BackendsErrors::BackendsErrorsEx);
	
	void store_allmode_command(const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
	
	void delete_file_command(const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
	
	string get_status_command(short b_addr); //throw (BackendsErrors::BackendsErrorsEx);
	
   string get_comp_command(short b_addr); //throw (BackendsErrors::BackendsErrorsEx);	
	
	string get_diag_command(short b_addr); //throw (BackendsErrors::BackendsErrorsEx);
	
	string get_cfg_command();
	
	string set_dbeatt_command(const char * out_dbe, const char * att_val);
	
	string get_dbeatt_command(const char * out_dbe);
	
	string get_firm_command(short b_addr);
	
	string set_dbeamp_command(const char * out_dbe, const char * amp_val);
	
	string get_dbeamp_command(const char * out_dbe);	
	
	string set_dbeeq_command(const char * out_dbe, const char * eq_val);
	
	string get_dbeeq_command(const char * out_dbe);	
	
	string set_dbebpf_command(const char * out_dbe, const char * bpf_val);
	
	string get_dbebpf_command(const char * out_dbe);	
};

#endif /*COMMANDSOCKET_H_*/