/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 13/11/2014     Creation                                         */

#ifndef DEVIODEROTATORPOSITION_H_
#define DEVIODEROTATORPOSITION_H_

#include <baciDevIO.h>
#include <IRA>

/**
 * This class is derived from template DevIO and it is used by the DerotatorPosition property of the receiverBoss component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIODerotatorPosition : public DevIO<CORBA::Double>
{
public:

	/**
	 * Constructor
	 * @param core pointer to the boss core
	*/
	DevIODerotatorPosition(CRecvBossCore* core) : m_pCore(core)
	{
		AUTO_TRACE("DevIODerotatorPosition::DevIODerotatorPosition()");
	}

	/**
	 * Destructor
	*/
	~DevIODerotatorPosition()
	{
		ACS_TRACE("DevIODerotatorPosition::~DevIODerotatorPosition()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIODerotatorPosition::DevIODerotatorPosition()");
		return false;
	}

	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	*/
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();  //compilation time
		m_val=(CORBA::Double)m_pCore->getDerotatorPosition(timestamp);
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/
	void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}

private:
	CRecvBossCore* m_pCore;
	CORBA::Double m_val;
};

#endif /* DEVIODEROTATORPOSITION_H_ */
