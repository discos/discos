#ifndef _DEVIOLNACONTROLS_H_
#define _DEVIOLNACONTROLS_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(andrea.orlati@inaf.it) 10/08/2023      Creation                                         */


#include <baciDevIO.h>
#include <IRA>
#include <ReceiverControl.h>

/**
 * This class is derived from template DevIO and it is used by the LNA controls  properties  of the  component: Vd_1, Id_2 and so on.....
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOLNAControls : public DevIO<CORBA::Double>
{
public:

	/**
	 * Constructor
	 * @param core pointer to the boss core
	*/
	DevIOLNAControls(CComponentCore* core,const IRA::ReceiverControl::FetValue& control,const DWORD& ifs) :  m_pCore(core), m_control(control), m_ifs(ifs)
	{
		AUTO_TRACE("DevIOLNAControls::DevIOLNAControls()");
	}

	/**
	 * Destructor
	*/
	~DevIOLNAControls()
	{
		ACS_TRACE("DevIOLNAControls::~DevIOLNAControls()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIOLNAControls::initializeValue()");
		return true; // initialize with the default in order to avoid the alarm system when the component start and the value has not been read at least once
	}

	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	 * @throw ACSErr::ACSbaseExImpl
	*/
	CORBA::Double read(ACS::Time& timestamp)
	{
		m_val=m_pCore->getFetValue(m_control,m_ifs);
		timestamp=getTimeStamp();  //Completion time
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	 * @throw ACSErr::ACSbaseExImpl
	*/
	void write(const CORBA::Double& value, ACS::Time& timestamp)
	{
		timestamp=getTimeStamp();
		return;
	}

private:
	CComponentCore* m_pCore;
	double  m_val;
	IRA::ReceiverControl::FetValue m_control;
	DWORD m_ifs;
};

#endif
