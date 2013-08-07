#ifndef CDATACOLLECTION_H_
#define CDATACOLLECTION_H_

/* ************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                                         */
/*                                                                                                                                                */
/* This code is under GNU General Public Licence (GPL).                                                    */
/*                                                                                                                                                */
/* Who                                                    	 When                      What                                      */
/* Andrea Orlati(aorlati@ira.inaf.it)          13/01/2009         Creation                                */

#include <AntennaBossC.h>
#include <String.h>
#include <GenericWeatherStationC.h> 
#include <ReceiversBossC.h>
#include <MinorServoBossC.h>
#include <SchedulerC.h>
#include <BackendsDefinitionsC.h>
#include <ManagmentDefinitionsC.h>
#include <DataReceiverS.h>
#include <vector>
#include <list>
#include <Definitions.h>
#include <Site.h>
#include <acsContainerServices.h>
#include "FitsWriter.h"
#include "Configuration.h"


// number of dumps in the cache
#define DUMP_CACHE_SIZE 1000 

#define BYTE2_TYPE unsigned short
#define BYTE4_TYPE float
#define BYTE8_TYPE double

namespace FitsWriter_private {

template <class SAMPLETYPE> void getChannelFromBuffer(const DWORD& index,
		const long& pol,const long& bins,char *& buffer,std::vector<SAMPLETYPE>& channel)  {
	unsigned size=sizeof(SAMPLETYPE);
	//char *p=buffer+(index*bins*pol*size);
	char *p=buffer;
	channel.clear();
	for(int j=0;j<pol;j++) {
		for (int k=0;k<bins;k++) {
			channel.push_back(*((SAMPLETYPE *)p));
			p+=size;
		}
	}
	buffer=p;
}

template <class SAMPLETYPE> void getChannelFromBuffer(const DWORD& index,
		const long& pol,const long& bins,char *& buffer,SAMPLETYPE* channel)  {
	unsigned size=sizeof(SAMPLETYPE);
	//char *p=buffer+(index*bins*pol*size);
	char *p=buffer;
	unsigned counter=0;
	for(int j=0;j<pol;j++) {
		for (int k=0;k<bins;k++) {
			channel[counter]=*((SAMPLETYPE *)p);
			p+=size;
			counter++;
		}
	}
	buffer=p;
}

void getTsysFromBuffer(char *& buffer,const DWORD& channels ,double *tsys);

/**
 * Used to store the dump from the backend, waiting for the thread to save them into the file
 */
class CDumpCollection {
public:
	CDumpCollection() { };
	/**
	 * Destructor. It clears all the dumps that eventually are still there.
	 */
	~CDumpCollection() {
		std::list<TRecord *>::iterator i;
		for (i=m_collection.begin();i!=m_collection.end();i++) {
			TRecord *rec=*i;
			delete [] (rec->memory);
			delete rec;
		}
		m_collection.clear();
	};
	/**
	 * Adds a new dump in the back of the stack (FIFO). If the stack reached its maximum size a false
	 * is returned and the dump is discarded
	 */
	bool pushDump(const ACS::Time& time,bool calOn,char *memory,char * buffer,bool tracking,const long& buffSize) {
		if (m_collection.size()>DUMP_CACHE_SIZE) return false;
		TRecord *rec=new TRecord;
		rec->calOn=calOn;
		rec->time=time;
		rec->buffer=buffer;
		rec->memory=memory; 
		rec->tracking=tracking;
		//memcpy(rec->buffer,buffer,buffSize);		
		rec->buffSize=buffSize;
		m_collection.push_back(rec);
		return true;
	}
	/**
	 * Pops the first dump of the stack. (FIFO). The returned buffer must be freed by the caller.
	 * @return true if the dump could be returned, otherwise if the stack is empty it returns false
	 */
	bool popDump(ACS::Time& time,bool& calOn,char *&memory,char *& buffer,bool& tracking,long& buffSize) {
		if (m_collection.empty()) return false;
		TRecord *rec=m_collection.front(); //get the first element
		time=rec->time;
		calOn=rec->calOn;
		buffer=rec->buffer;
		memory=rec->memory;
		buffSize=rec->buffSize;
		tracking=rec->tracking;
		delete rec;
		m_collection.pop_front(); // deletes the first element
		return true;
	}
	/**
	 * @return the time of the first element (first to come out) of the list. If the list is empty
	 * it returns a 0;
	 */
	ACS::Time getFirstTime() {
		if (m_collection.empty()) {
			return 0;
		}
		else {
			TRecord *rec=m_collection.front();
			return rec->time;
		}
	}
	/**
	 * @return the time of the last element (last to come out) of the list. if the list is empty it returns 0
	 */
	ACS::Time getLastTime() {
		if (m_collection.empty()) {
			return 0;
		}
		else {
			TRecord *rec=m_collection.back();
			return rec->time;
		}		
	}
private:
	typedef struct {
		ACS::Time time;
		bool calOn;
		char *buffer; //pointer inside memory buffer fromwhich the real data starts
		char *memory;
		long buffSize;
		bool tracking;
	} TRecord;
	std::list<TRecord *>m_collection;
	CDumpCollection(const CDumpCollection& src);
	void  operator=(const CDumpCollection&src);
};

/**
 * This class stores all the data that are needed by the fits. This class is thread unsafe so must be 
 * synchronized
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
  */
class CDataCollection {

public:
	/**
	 * Constructor
	 */
	CDataCollection();
	/**
	 * Destructor
	 */
	~CDataCollection();

	/**
	 * Change current scan information
	 * @param setup scan setup structure
	 * @param recording set in case of error, true if the method was called during recording
	 * @param inconsistent set in case of error, true if the method was called when sun scan headers were not expected
	 * @return false if the operation could not be done
	 */
	bool setScanSetup(const Management::TScanSetup& setup,bool& recording,bool& inconsistent);

	/**
	 * Change current sub scan information
	 * @param setup sub scan setup structure
	 * @param recording set in case of error, true if the method was called during recording
	 * @param inconsistent set in case of error, true if the method was called when sun scan headers were not expected
	 * @return false if the operation could not be done
	 */
	bool setSubScanSetup(const Management::TSubScanSetup& setup,bool& recording,bool& inconsistent);
	
	/**
	 * Set the scan stage to STOP
	 */
	bool stopScan();

	/**
	 * Get the name and path of the current file
	 * @param fileName of the file
	 * @param the exact path of the file in system disk (not including the file itself but including the trailing "/")
	 */
	void getFileName(IRA::CString& fileName,IRA::CString& fullPath);

	/**
	 * Get the full name of the current file
	 * @return the full path and the file name of the current file
	 */	
	IRA::CString getFileName();
	
	/**
	 * Get the name of the project
	 */
	const IRA::CString& getProjectName() const { return m_project; }
	
	/**
	 * get the identifier number of the next scan
	 */
	const long& getScanTag() const { return m_scanTag; }
	
	/**
	 * get default device identifier
	 */
	const long& getDeviceID() const { return m_deviceID; }

	/**
	 * get current subscan axis
	 */
	const Management::TScanAxis& getScanAxis() const { return m_scanAxis; }

	/**
	 * computes the subscan type (based on the ScanAxis) and returns it as string
	 * @return the value of the keyword subScantype
	 */
	IRA::CString getSubScanType() const;

	/**
	 * Get the name of the observer
	 */
	const IRA::CString& getObserverName() const { return m_observer; }
	
	/**
	 * Get the name of the schedule file
	 */
	const IRA::CString& getScheduleName() const { return m_scheduleName; }

	/**
	 * Get the identifier of the current scan
	 */
	const long& getScanID() const { return m_scanID; }

	/**
	 * Get the identifier of the current sub scan
	 */
	const long& getSubScanID() const { return m_subScanID; }

	/**
	 * Get the dimension of the sample in bytes. 
	 */  
	long getSampleSize() const { return m_mainH.sampleSize; }
	
	/**
	 * Get the number of bins given the section number. 
	 */
	long getSectionBins(const long& ch) const { return m_sectionH[ch].bins; }

	/**
	 * Get the number of sections. 
	 */
	long getSectionsNumber() const { return m_mainH.sections; }
	
	/**
	 * Get the millisecond of integration
	 */
	long getIntegrationTime() const { return m_mainH.integration; }
	
	/**
	 * Get the sum of the inputs of all the sections
	 */
	long getInputsNumber() const;
	
	/**
	 * Gets the inputs configuration from the backend
	 */
	void getInputsConfiguration(ACS::longSeq& sectionID,ACS::longSeq& feeds,ACS::longSeq& ifs,ACS::doubleSeq& freqs,ACS::doubleSeq& bws,ACS::doubleSeq& atts) const;
	
	/**
	 * Get number of expected streams coming from a section.
	 * @param section section identifier
	 */
	long getSectionStreamsNumber(const long& section) const { if (m_sectionH[section].polarization==Backends::BKND_FULL_STOKES) return 4; else return 1; }
	
	/**
	 * Saves the main headers coming from the backend
	 */
	void saveMainHeaders(Backends::TMainHeader const * h,Backends::TSectionHeader const * ch);
	
	/**
	 * Saves the feed header
	*/
	void saveFeedHeader(CFitsWriter::TFeedHeader *fH,const WORD& number);
	
	/**
	 * Saves the dumps incoming from the backend.
	 * @return true if the operation is succesful..otherwise false. The failure can be declared if the cache is full
	 * or because new data are coming before the previous file is finalized.
	 */
	bool saveDump(char * memory);
	
	/**
	 * @return the time stamp associated to the firt dump in the list. If no dump are present it returns a
	 * zero. 
	 */
	ACS::Time getFirstDumpTime();
	
	/**
	 * It will return the data of the next dump in the list. The returned buffer must be freed by the caller.
	 * @return  it will return a false if the list is empty or the corresponding time of the next dump is 
	 * greater than the <i>m_stopTime</i>. That means that the dump coming from  
	 * 
	 */
	bool getDump(ACS::Time& time,bool& calOn,char *& memory,char *& buffer,bool& tracking,long& buffSize);
	
	/**
	 * @return the reference to the main header. 
	 */
	const Backends::TMainHeader& getMainHeader() const { return m_mainH; }
	
	/**
	 * @return the pointer to the array of sections  header
	 */
	Backends::TSectionHeader * getSectionHeader() const { return m_sectionH; }
	
	/**
	 * @return the pointer to the array of the feed header
	 */
	CFitsWriter::TFeedHeader *getFeedHeader() const { return m_feeds; }
	
	/**
	 * @return the number of feeds, it is the dimension of the array returned with the call to 
	 * <i> getfeedHeader() </i>.
	 */
	const WORD& getFeedNumber() const { return m_feedNumber; }
	
	/**
	 * @return the current status of the component.
	 */
	const Management::TSystemStatus& getStatus() const { return m_status; }

	/**
	 * Sets the current status of the component
	 */
	void setStatus(const Management::TSystemStatus& st) { m_status=st; }
	
	/** 
	 * Sets the site information and the dut1 required by the component
	 */
	void setSite(const IRA::CSite& site,const double& dut1,const IRA::CString& siteName) {m_site=site; m_dut1=dut1; m_siteName=siteName; }
	
	/**
	 * allows to return back the information regarding the site
	 */ 
	void getSite(IRA::CSite& site,double& dut1,IRA::CString& siteName) const { site=m_site; dut1=m_dut1; siteName=m_siteName; }
	
	/**
	 * Gets the site information required by the component
	 */
	const IRA::CSite& getSite() const { return m_site; }
	
	/**
	 * Gets the dut1
	 */
	const double& getDut1() const { return m_dut1; }
	
	/**
	 * @return the combination of  <i>m_ready</i> flag( that means the headers have been saved)
	 */
	bool isReady() const { return m_ready; }
	
	/**
	 * @return the <i>m_running</i>  flag, that means the component is working and saving the data from the sender
	 */
	bool isRunning() const {return m_running; }
	
	/**
	 * @return the <i>m_stop</i> flag, that means the component id finalizing the file
	 */
	bool isStop() const { return m_stop; }
	
	/**
	 * @return the <i>m_start</i> flag, that means the component is opening the file
	 */
	bool isStart() const { return m_start; }
	
	/**
	 *  @return the <i>m_reset</i> flag, that means the component must perform the actions required for a reset
	 */
	bool isReset() const { return m_reset; }

	/**
	 * @return <i>m_scanHeaders</i> flag, that means the startScan() has been called
	 */
	bool isScanHeaderReady() const { return m_scanHeader; }

	/**
	 * @return <i>m_subScanHeaders</i> flag, that means the startsubScan() has been called
	 */
	bool isSubScanHeaderReady() const { return m_subScanHeader; }

	/**
	 * It puts the component into the stop stage
	 */
	void startStopStage(); 
	
	/**
	 * It puts the component into the running stage
	 */
	void startRunnigStage();
	
	/**
	 * If puts the component into the initial situation halting the stop stage
	 */
	void haltStopStage();
	
	/**
	 * acknowledge that the component has been reset
	 */
	void haltResetStage();

	/*
	 * It allows to set the current receiver code
	 */
	void setReceiverCode(const IRA::CString& recvCode) { m_receiverCode=recvCode; }
	
	/**
	 * @return the receiver code
	 */
	const IRA::CString& getReceiverCode() const {return m_receiverCode;	}

	/*
	 * It allows to read the sequence of identifiers of the backend sections
	 * @param sec used to return back the the array
	 */
	void getSectionsID(ACS::longSeq& sec) const { sec=m_sectionsID; }

	/*
	 * It allows to read the sequence of attenuations for each backend input
	 * @param att used to return back the the array
	*/
	void getBackendAttenuations(ACS::doubleSeq& att) const { att=m_backendAttenuations; }

	/*
	 * It allows to read the sequence of feeds identifiers for each backend input
	 * @param feed used to return back the the array
	*/
	void getFeedsID(ACS::longSeq& feed) const { feed=m_receiverFeedID; }

	/*
	 * It allows to read the sequence of ifs  identifiers for each backend input
	 * @param ifid used to return back the the array
	*/
	void getIFsID(ACS::longSeq& ifid) const { ifid=m_receiversIFID; }
	
	/*
	 * It allows to read the sequence of local oscillators
	 * @param lo used to return back the the array
	 */
	void getLocalOscillator(ACS::doubleSeq& lo) const { lo=m_localOscillator; }
	
	/*
	 * allows to read the sequence of IFs polarization
	 * @param pol used to return back the the array
	 */
	void getReceiverPolarization(ACS::longSeq& pol) const { pol=m_receiverPolarization; }
	
	void getServoAxisNames(ACS::stringSeq& names) const { names=m_servoAxisNames; }

	void getServoAxisUnits(ACS::stringSeq& units) const { units=m_servoAxisUnits; }

	/*
	 * @return the arrays that contains the local oscillator values.
	 * @param len used to return back the size of the local oscillator array
	 */
	void getCalibrationMarks(ACS::doubleSeq& mark) const { mark=m_calibrationMarks; }

	/**
	 * It allows to set the current source name and its salient parameters
	 */
	void setSource(const IRA::CString& source,const double& ra,const double& dec,const double& vlsr) { m_sourceName=source; m_sourceRa=ra; m_sourceDec=dec; m_sourceVlsr=vlsr; }
	
	/**
	 * @return the name of the source and its parameters
	 */
	void getSource(IRA::CString& source,double& ra,double& dec,double& vlsr) const { source=m_sourceName; ra=m_sourceRa; dec=m_sourceDec; vlsr=m_sourceVlsr; }
	
	/** 
	 * It allows to set the antenna position offsets as directly returned by the Antenna subsystem 
	*/ 
	void setAntennaOffsets(const double& azOff,const double& elOff,const double& raOff,const double& decOff,const double& lonOff,const double& latOff) { 
		m_azOff=azOff; m_elOff=elOff; m_raOff=raOff; m_decOff=decOff; m_lonOff=lonOff; m_latOff=latOff;
	}
	
	/**
	 * @return the antenna position offsets
	 */	
	void getAntennaOffsets(double& azOff,double& elOff,double& raOff,double& decOff,double& lonOff,double& latOff) const { 
		azOff=m_azOff; elOff=m_elOff; raOff=m_raOff; decOff=m_decOff; lonOff=m_lonOff; latOff=m_latOff;
	}

	/**
	 * Stores the list of values corresponding to the estimated source flux of the source,
	 * @param fl list of fluxes, one value is  given corresponding to each input, but just one for each section is taken
	 */
	void setSourceFlux(const ACS::doubleSeq& fl) { m_sourceFlux=fl; }
	void setSourceFlux() { m_sourceFlux.length(0); }

	/**
	 * It returns the estimated source flux, one for each section.
	 */
	void getSourceFlux(ACS::doubleSeq& fl) const { fl=m_sourceFlux; }

	/**
	 * allows to store the information about each of the servo system axis
	 */
	void setServoAxis(const ACS::stringSeq& name, const ACS::stringSeq& unit) {
		m_servoAxisNames=name;
		m_servoAxisUnits=unit;
	}

	/**
	 * allows to store the information about each of the backend inputs. The dimension of the sequences should be exactly the number returned by the method <i>getInputsNumber()</i>-
	 */
	void setInputsTable(const ACS::longSeq& sectionID,const ACS::longSeq& feed,const ACS::longSeq& ifs,const ACS::longSeq& pols,const ACS::doubleSeq& freq,const ACS::doubleSeq& bw,
			const ACS::doubleSeq& los,const ACS::doubleSeq& atts,const ACS::doubleSeq& mark)  {
		m_receiverPolarization=pols;
		m_receiversIFID=ifs;
		m_receiverFeedID=feed;
		m_calibrationMarks=mark;
		m_backendAttenuations=atts;
		m_skyFrequency=freq;
		m_skyBandwidth=bw;
		m_sectionsID=sectionID;
		m_localOscillator=los;
	}

	void setInputsTable()
	{
		m_receiverPolarization.length(0);
		m_receiversIFID.length(0);
		m_backendAttenuations.length(0);
		m_receiverFeedID.length(0);
		m_calibrationMarks.length(0);
		m_skyFrequency.length(0);
		m_skyBandwidth.length(0);
		m_sectionsID.length(0);
		m_localOscillator.length(0);
	}

	/**
	 * allows to set the sky frequencies. The input argument is expected to be of the same multiplicity as the number of inputs of the backend.
	 * The number of stored values corresponds to the number of sections
	 */
	/*void setSkyFrequency(const ACS::doubleSeq& freq);
	void setSkyFrequency() { m_skyFrequency.length(0); }*/

	/**
	 * @param freq used to return the number of frequencies, one value for each section
	 */
	void getSkyFrequency(ACS::doubleSeq& freq) const { freq=m_skyFrequency; }

	/**
	 * allows to set the sky bandwidth. The input argument is expected to be of the same multiplicity as the number of inputs of the backend.
	 * The number of stored values corresponds to the number of sections
	 */
	/*void setSkyBandwidth(const ACS::doubleSeq& bw);
	void setSkyBandwidth() { m_skyBandwidth.length(0); }*/

	/**
	 * @param freq used to return the number of bandwidths, one value for each section
	 */
	void getSkyBandwidth(ACS::doubleSeq& freq) const { freq=m_skyBandwidth; }

	/**
	 * it sets the meteo parameters
	 */
	void setMeteo(const double& hm,const double& temp,const double& press) {
		m_humidity=hm; 
		m_temperature=temp;
		m_pressure=press;
	}
	
	/**
	 * It reports back the meteo parameters value
	 */
	void getMeteo(double& hm,double& temp, double& press) const {
		hm=m_humidity;
		temp=m_temperature;
		press=m_pressure;
	}

	/**
	 * allows to set the current tracking flag from the telescope
	 */
	void setTelescopeTracking(const Management::TBoolean& tracking,const ACS::Time& time) {
		m_prevTelescopeTracking=m_telecopeTacking;
		m_telecopeTacking=(tracking==Management::MNG_TRUE);
		m_telescopeTrackingTime=time;
	}

	/**
	 * Set the flags so that the component can reset itself
	 */
	void forceReset();
		
private:
	/** the name of the file */
	IRA::CString m_fileName;
	/**
	 * the full path of the file in system disk
	 */
	IRA::CString m_fullPath;
	IRA::CString m_project;
	IRA::CString m_observer;
	/** the data block coming from the antenna */
	//Antenna::AntennaDataBlock m_antennaData;
	/** the data block coming from the antenna the previous one */
	//Antenna::AntennaDataBlock m_antennaDataPrev;
	/**
	 * Main header coming from the backend
	 */
	Backends::TMainHeader m_mainH;
	/**
	 * Section headers coming from the backend
	 */
	Backends::TSectionHeader *m_sectionH;
	/**
	 * This will stores the data damps as they are coming from the backend, waiting to be saved in the file
	 */
	FitsWriter_private::CDumpCollection m_dumpCollection;
	/**
	 * Reports about the status of the backend
	 */
	Management::TSystemStatus m_status;
	/**
	 * true if the headers of scan and subscan have been received respectively
	 */
	bool m_scanHeader;
	bool m_subScanHeader;
	/**
	 *  indicates if the component is saving...
	 */
	bool m_running;
	/**
	 * indicates that headers have been received
	 */
	bool m_ready;
	/**
	 * indicates that the file has to be closed
	 */
	bool m_stop;
	/**
	 * indicates that the file has to be opened
	 */
	bool m_start;
	/**
	 * a reset request has been received
	 */
	bool m_reset;
	/**
	 * stores the UT1-UT difference
	 */
	double m_dut1;
	/**
	 * information about the site
	 */
	CSite m_site;
	/**
	 * Name of the site
	 */
	IRA::CString m_siteName;
	/**
	 * Name of the currently observed source
	 */
	IRA::CString m_sourceName;
	double m_sourceRa,m_sourceDec,m_sourceVlsr;
	/**
	 * antenna position offsets
	 */
	double m_azOff,m_elOff,m_raOff,m_decOff,m_lonOff,m_latOff;
	/**
	 * array of structures that stores the data needed to fille the feed table
	 */
	CFitsWriter::TFeedHeader *m_feeds;
	/**
	 * Number of feed inside the table
	 */
	WORD m_feedNumber;
	/**
	 * Indicates the short name (code) of the receiver currently used
	 */
	IRA::CString m_receiverCode;
	/**
	 * Stores the name of the schedule file, currently running
	 */
	IRA::CString m_scheduleName;
	/**
	 * Scan tag
	 */
	long m_scanTag;
	/**
	 * default device identifier
	 */
	long m_deviceID;
	/**
	 * determine which is the scan axis
	 */
	Management::TScanAxis m_scanAxis;
	/**
	 * scan and subscan identifiers
	 */
	long m_scanID,m_subScanID;
	/**
	 * array that contains the start values of bands coming from the receiver
	*/	
	//ACS::doubleSeq m_recInitialFrequency;
	/**
	 * array that contains the values of the receiver band widths
	 */	
	//ACS::doubleSeq m_recBandWidth;
	/**
	 * array that contains the values of the local oscillator (one for each input defined by the backend)
	 */
	ACS::doubleSeq m_localOscillator;
	/**
	 * array that contains the values of receiver polarization (one for each input defined by the backend)
	 */	
	ACS::longSeq m_receiverPolarization;
	/**
	 * stores the value of the calibration marks, for each inputs from the backend
	 */
	ACS::doubleSeq m_calibrationMarks;
	/**
	 * stores the  estimated source fluxes, one for each section.
	 */
	ACS::doubleSeq m_sourceFlux;
	/**
	 * stores the initial frequencies of the bands observed (projected to sky) (one for each input defined by the backend)
	 */
	ACS::doubleSeq m_skyFrequency;
	/**
	 * stores the band width of the bands observed (projected to sky) (one for each input defined by the backend)
	 */
	ACS::doubleSeq m_skyBandwidth;
	/**
	 * stores the identifiers of the receiver IFs (one for each input defined by the backend)
	 */
	ACS::longSeq m_receiversIFID;
	/**
	 * Stores the attenuations of each backend input
	 */
	ACS::doubleSeq m_backendAttenuations;
	/**
	 * stores the identifier of the receiver feeds ((one for each input defined by the backend)
	 */
	ACS::longSeq m_receiverFeedID;
	/**
	 * Stores the identifier od the baclend section (one for each input)
	 */
	ACS::longSeq m_sectionsID;
	/**
	 * Stores the description or name of each involved axis of the servo system
	 */
	ACS::stringSeq m_servoAxisNames;
	/**
	 * Stores the measurment unit of each involved axis of the servo system
	 */
	ACS::stringSeq m_servoAxisUnits;
	/**
	 * relative humidity
	 */
	double m_humidity;
	/**
	 * temperature in degrees
	 */
	double m_temperature;
	/**
	 * pressure in millibars
	 */ 
	double m_pressure;
	/**
	 * this store if the telescope is currently tracking or not;
	 */
	bool m_telecopeTacking;
	/**
	 * This stored the time of the last update of the tracking flag
	 */
	ACS::Time m_telescopeTrackingTime;
	/**
	 * this stored the old telescope tracking flag
	 */
	bool m_prevTelescopeTracking;
};

};

#endif /*CDATACOLLECTION_H_*/
