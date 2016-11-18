/* **************************************************************************************************** */
/* DISCOS Project                                                                                       */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 16/11/2016     Creation                                         */

#ifndef TONE_H_
#define TONE_H_

#include <IRA>
#include <ComponentErrors.h>

/**
 * This class allows to communicate to the HIPB/Ethernet device in order to turn on and off the phase cal tone
 * (activate Antenna Unit).
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
  */
class CTone {
public:
	/**
	 * @pram timeo
	 */
	CTone(const IRA::CString& ip,const long& port,const long& timeo);
	~CTone();
	void init() throw (ComponentErrors::IRALibraryResourceExImpl);
	void turnOn() throw (ComponentErrors::IRALibraryResourceExImpl);
	void turnOff() throw (ComponentErrors::IRALibraryResourceExImpl);
private:
	IRA::CString m_address;
	long m_port;
	long m_timeout;
	IRA::CSocket m_socket;
};

#endif /* TONE_H_ */
