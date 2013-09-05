#ifndef _DEVIOLNACONTROLS_H_
#define _DEVIOLNACONTROLS_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 19/08/2011     Creation                                         */


#include <baciDevIO.h>
#include <IRA>
#include <ReceiverControl.h>

/**
 * This class is derived from template DevIO and it is used by the LNA controls  properties  of the  component: Vd_1, Id_2 and so on.....
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOLNAControls : public DevIO<ACS::doubleSeq>
{
public:

	/**
	 * Constructor
	 * @param core pointer to the boss core
	*/
	DevIOLNAControls(
            SRTLPBandCore* core,
            const IRA::ReceiverControl::FetValue& control,
            const DWORD ifs,
            const DWORD stage
    ) :  m_pCore(core), m_control(control), m_ifs(ifs), m_stage(stage)
	{
		AUTO_TRACE("DevIOLNAControls::DevIOLNAControls()");
        m_val.length(m_pCore->getFeeds());
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
	*/
	ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
        // In pCore I need a method to get the vector of values (An extension, so I can ereditate from ComponentCore)
		m_val=m_pCore->getStageValues(m_control, m_ifs, m_stage);
		timestamp=getTimeStamp();  // Completion time
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
	SRTLPBandCore* m_pCore;
    ACS::doubleSeq  m_val;
	IRA::ReceiverControl::FetValue m_control;
	DWORD m_ifs;
    DWORD m_stage;
};

#endif
