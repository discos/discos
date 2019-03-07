#ifndef _DEVIOBANDWIDTH_H_
#define _DEVIOBANDWIDTH_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 10/08/2011     Creation                                         */


#include <baciDevIO.h>
#include <IRA>

/**
 * This class is derived from template DevIO and it is used by the bandWidth property of the  component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOBandWidth : public DevIO<ACS::doubleSeq>
{
public:

	/**
	 * Constructor
	 * @param core pointer to the boss core
	*/
	DevIOBandWidth(CComponentCore* core) :  m_pCore(core)
	{
		AUTO_TRACE("DevIOBandWidth::DevIOBandWidth()");
	}

	/**
	 * Destructor
	*/
	~DevIOBandWidth()
	{
		ACS_TRACE("DevIOBandWidth::~DevIOBandWidth()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIOBandWidth::DevIOBandWidth()");
		return false;
	}

	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	*/
	ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		m_pCore->getBandwidth(m_val);
		timestamp=getTimeStamp();  //Completion time
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/
	void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}

private:
	CComponentCore* m_pCore;
	ACS::doubleSeq m_val;
};

#endif
