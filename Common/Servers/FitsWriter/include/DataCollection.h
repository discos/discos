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
//#include "FitsWriter.h"
#include "Configuration.h"


// number of dumps in the cache
// 30000 corresponds to 30 seconds with 1 milliseconds dumps
#define DUMP_CACHE_SIZE 30000


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
		flushAll();
	};
	void flushAll() {
		std::list<TRecord *>::iterator i;
		for (i=m_collection.begin();i!=m_collection.end();i++) {
			TRecord *rec=*i;
			delete [] (rec->memory);
			delete rec;
		}
		m_collection.clear();
	}
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
	/**
	 * @return true if the stack is empty
	 */
	bool isEmpty() { return m_collection.empty(); }
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
 * This class stores all the raw data that are needed by the fits.
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
	void getFileName(IRA::CString& fileName,IRA::CString& fullPath) const;

	/**
	 * Get the full name of the current file
	 * @return the full path and the file name of the current file
	 */	
	IRA::CString getFileName() const;

	/**
	 * Get the full name of the current summary file
	 * @return the full path and the file name of the current file
	 */
	IRA::CString getSummaryFileName() const;

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
	long getInputsNumber();
	
	/**
	 * Gets the inputs configuration from the backend
	 */
	void getInputsConfiguration(ACS::longSeq& sectionID,ACS::longSeq& feeds,ACS::longSeq& ifs,ACS::doubleSeq& freqs,ACS::doubleSeq& bws,ACS::doubleSeq& atts);
	
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
	 * Saves the dumps incoming from the backend.
	 * @return true if the operation is successful..otherwise false. The failure can be declared if the cache is full
	 * or because new data are coming before the previous file is finalized.
	 */
	bool saveDump(char * memory);
	
	/**
	 * @return the time stamp associated to the first dump in the list. If no dump are present it returns a
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
	 * @return the current status of the component.
	 */
	const Management::TSystemStatus& getStatus() const { return m_status; }

	/**
	 * Sets the current status of the component, almost atomic
	 */
	void setStatus(const Management::TSystemStatus& st) { /*baci::ThreadSyncGuard guard(&m_mutex); */m_status=st; }
	
	/** 
	 * Sets the site information and the dut1 required by the component
	 */
	void setSite(const IRA::CSite& site,const double& dut1,const IRA::CString& siteName) { 	baci::ThreadSyncGuard guard(&m_mutex); m_site=site; m_dut1=dut1; m_siteName=siteName; }
	
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

	bool isWriteSummary() const { return m_writeSummary; }

	void closeSummary() { m_writeSummary=false; }

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

	/**
	 * it sets the meteo parameters
	 */
	void setMeteo(const double& hm,const double& temp,const double& press) {
		baci::ThreadSyncGuard guard(&m_mutex);
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
		baci::ThreadSyncGuard guard(&m_mutex);
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
	bool m_writeSummary;
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
	
	/**
	 * Thread sync mutex
	 */
	BACIMutex m_mutex;
};

};

#endif /*CDATACOLLECTION_H_*/
