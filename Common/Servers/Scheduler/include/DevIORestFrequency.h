#ifndef _DEVIORESTFREQUENCY_H_
#define _DEVIORESTFREQUENCY_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  12/06/2014     Creation                                         */


#include <baciDevIO.h>
#include <IRA>


using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the rest Frequency property  of the Scheduler
 * component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIORestFrequency : public DevIO<ACS::doubleSeq>
{
public:

	/**
	 * Constructor
	 * @param
	*/
	DevIORestFrequency(CCore* core) :  m_core(core)
	{
		AUTO_TRACE("DevIORestFrequency::DevIORestFrequency()");
	}

	/**
	 * Destructor
	*/
	~DevIORestFrequency()
	{
		ACS_TRACE("DevIORestFrequency::~DevIORestFrequency()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIORestFrequency::initializeValue()");
		return false;
	}

	ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		m_core->getRestFrequency(m_val);
		return m_val;
	}

	void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}

private:
	CCore *m_core;
	ACS::doubleSeq m_val;
};

#endif /*_DEVIORESTFREQUENCY_H_*/
