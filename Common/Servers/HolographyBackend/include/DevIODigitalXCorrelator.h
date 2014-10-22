
#include <baciDevIO.h>

using name space IRA;


class DevIODigitalXCorrelator : public DevIO<ACS::doubleSeq 

{

public:
	/**
	Constructor
	*/
	
	DevIODigitalXCorrelator() {
		AUTO_TRACE("DevIODigitalXCorrelator::DevIODigitalXCorrelator()";

					
	} ;
	/**
	Constructor
	*/

	virtual ~DevIODigitalXCorrelator() {
		AUTO_TRACE("DevIODigitalXCorrelator::DevIODigitalXCorrelator()";
		
	} ;
	virtual bool   initializeValue() { return true; }

	ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) 
	{

		
		
			
	
	}
	
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/ 
	void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}



}

