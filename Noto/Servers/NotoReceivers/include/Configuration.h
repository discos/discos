#ifndef CCONFIGURATION_H
#define CCONFIGURATION_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 02/08/2011     Creation                                         */


#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>
#include <ReceiversDefinitionsC.h>

/**
 * This class implements the component configuration. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
  */
class CConfiguration {
public:

	typedef struct {
		double skyFrequency;
		double markValue;
		Receivers::TPolarization polarization;
	} TMarkValue;

	typedef struct {
		double frequency;
		double outputPower;
	} TLOValue;

	typedef struct {
		double frequency;
		double taper;
	} TTaperValue;

	typedef struct {
		WORD code;
		double xOffset;
		double yOffset;
		double relativePower;
	} TFeedValue;

	/**
	 * Default constructor
	 */
	CConfiguration();

	/**
	 * Destructor
	 */
	~CConfiguration();

	/**
	 * @return the IP address of the board that controls the dewar
	 */
	inline const IRA::CString& getHPIBIPAddress() const { return m_HPIBIPAddress; }

	/**
	 * @return the IP port of the board that controls the dewar
	 */
	inline const WORD& getHPIBPort() const { return m_HPIBPort; }

	/**
	 * @return the instance of the local oscillator component  that the receiver will use to drive the set its LO
	 */
	inline const IRA::CString& getLocalOscillatorInstance() const { return m_localOscillatorInstance; }

	/**
	 * @return the interface of the focus selector component that the receiver will use to drive the set its LO
	 */
	inline const IRA::CString& getFocusSelctorInterface() const { return m_fsInterface; }

	/**
	 * Allows to get the table of mark values relative to left polarization
	 * @param freq vector containing the frequency value of the mark table. It must be freed by caller.
	 * @param markValue vector of the value of the calibration diode. It must be freed by caller.
	 * @param len used to return the length of the mark values array
	 * @return the size of the output vectors
	 */
	DWORD getLeftMarkTable(double *& freq,double *& markValuel) const;

	/**
	 * Allows to get the table of mark values relative to left polarization
	 * @param freq vector containing the frequency value of the mark table. It must be freed by caller.
	 * @param markValue vector of the value of the calibration diode. It must be freed by caller.
	 * @param len used to return the length of the mark values array
	 * @return the size of the output vectors
	 */
	DWORD getRightMarkTable(double *& freq,double *& markValue) const;

	/**
	 * @param freq vector with the synthesizer frequencies. It must be freed by caller.
	 * @param power corresponding powers for the frequencies vector. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getSynthesizerTable(double *& freq,double *& power) const;

	/**
	 * @param freq vector with the frequencies. It must be freed by caller.
	 * @param taper corresponding taper  for the frequencies vector. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getTaperTable(double * &freq,double *&taper) const;

	/**
	 * @param code feed identification codes. It must be freed by caller.
	 * @param xOffset displacement of the feed with respect to the central one along x axis. It must be freed by caller.
	  *@param yOffset displacement of the feed with respect to the central one along y axis. It must be freed by caller.
	  *@param relativePower expected percentage of variation of gain with respect to the central one. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getFeedInfo(WORD *& code,double *& xOffset,double *& yOffset,double *& relativePower) const;

	/**
	 * @return mnemonic of the working mode of the receiver
	 */
	inline const IRA::CString& getSetupMode() const { return m_mode; }

	/**
	 * @return the lower limit of the RF coming from the receiver (MHz)
	 */
	inline double const * const  getRFMin() const { return m_RFMin; }

	/**
	 * @return the upper  limit of the RF coming from the receiver (MHz)
	 */
	inline double const * const  getRFMax() const { return m_RFMax; }

	/**
	 * @return the start frequency of the IF coming from the receiver (MHz)
	 */
	inline double const * const  getIFMin() const { return m_IFMin; }

	/**
	 * @return the bandwidth of the IF coming from the receiver (MHz)
	 */
	inline double const * const  getIFBandwidth() const{ return  m_IFBandwidth; }

	/**
	 * @return the number of IF chains for each feed
	 */
	inline const  DWORD& getIFs() const { return m_IFs; }

	/**
	 * @return the pointer to the polarizations array, the first element correspond to the first IF and so on.....
	 */
	inline Receivers::TPolarization const * const getPolarizations() const { return m_polarizations; }

	/**
	 * @return the number of feeds
	 */
	inline const  DWORD& getFeeds() const { return m_feeds; }

	/**
	 * @return the default frequency for the synthesizer  (MHz)
	 */
	inline double const * const  getDefaultLO()  const { return m_defaultLO; }

	/**
	 * @return the value of the fixed synthesizer used for the second conversion (MHz)
	 */
	inline double const * const  getFixedLO2() const { return m_fixedLO2; }

	/**
	 * @return lower limit for the synthesizer tuning (MHz)
	 */
	inline double const * const  getLOMin() const { return  m_LOMin; }

	/**
	 * @return upper limit for  the  synthesizer tuning (MHz)
	 */
	inline double const * const  getLOMax() const { return  m_LOMax; }

	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccess
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl);

	/**
    * This member function is used to configure the current receiver, the static information is red from the CDB.
	 * @throw ComponentErrors::CDBAccess
	 * @param Services pointer to the container services object
	 * @param conf configuration to be loaded
	*/
	void loadConf(maci::ContainerServices *Services,const IRA::CString& conf) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl);

	
private:
	IRA::CString m_HPIBIPAddress;
	WORD m_HPIBPort;
	DDWORD m_watchDogResponseTime;
	DDWORD m_watchDogSleepTime;
	IRA::CString m_localOscillatorInstance;
	IRA::CString m_fsInterface;

	IRA::CString m_mode;
	double *m_RFMin;
	double *m_RFMax;
	double *m_IFMin;
	double *m_IFBandwidth;
	DWORD m_IFs;
	Receivers::TPolarization *m_polarizations;
	DWORD m_feeds;
	double *m_defaultLO;
	double *m_fixedLO2;
	double *m_LOMin;
	double *m_LOMax;

	IRA::CDBTable *m_markTable;
	IRA::CDBTable *m_loTable;
	IRA::CDBTable *m_taperTable;
	IRA::CDBTable *m_feedsTable;

	TMarkValue *m_markVector;
	DWORD m_markVectorLen;
	TLOValue * m_loVector;
	DWORD m_loVectorLen;
	TTaperValue * m_taperVector;
	DWORD m_taperVectorLen;
	TFeedValue * m_feedVector; // length given by m_feeds
	void freeAll();
};


#endif
