#ifndef CDATACOLLECTION_H_
#define CDATACOLLECTION_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: DataCollection.h,v 1.5 2011-02-28 13:09:19 c.migoni Exp $      */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  23/11/2010      Creation              */

#include <AntennaBossC.h>
#include <String.h>
#include <SchedulerC.h>
#include <BackendsDefinitionsC.h>
#include <ManagmentDefinitionsC.h>
#include <vector>
#include <list>
#include <Definitions.h>
#include <acsContainerServices.h>
#include "Configuration.h"


// number of dumps in the cache
#define DUMP_CACHE_SIZE 1000 

namespace CalibrationTool_private {

template <class SAMPLETYPE> void getChannelFromBuffer(const DWORD& index,
		const long& pol,const long& bins,char *& buffer,std::vector<SAMPLETYPE>& channel)  {
	unsigned size=sizeof(SAMPLETYPE);
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
 * This class stores all the data that are needed by the calibration file.
 * This class is thread unsafe so must be synchronized.
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
	 * Change the name of the save file. 
	 * @param name new file name
	 * @return false if the operation could not be done
	 */
	bool setFileName(const IRA::CString& name);
	
	/**
	 * Get the name if the current file 
	 */
	const IRA::CString& getFileName() const { return m_fileName; }
	
	/**
	 * Set the name of the currently running project
	 */
	void setProjectName(const IRA::CString& projectName) { m_projectName=projectName; }
	
	/**
	 * Get the name of the project
	 */
	const IRA::CString& getProjectName() const { return m_projectName; }
	
	/**
	 * Sets the identifier number of the next scan
	 */
	void setScanId(const long& id) { m_scanId=id; }
	
	/**
	 * get the identifer number of the next scan
	 */
	const long& getScanId() const { return m_scanId; }

	/**
	 * Sets the identifier number of the device
	 */
	void setDevice(const long& deviceID) { m_deviceID=deviceID; }
	
	/**
	 * get the identifer number of the device
	 */
	const long& getDevice() const { return m_deviceID; }
	
	/**
	 * Set the current observer name
	 */
	void setObserverName(const IRA::CString& observer) { m_observer=observer; }
	
	/**
	 * Get the name of the observer
	 */
	const IRA::CString& getObserverName() const { return m_observer; }
	
	/**
	 * Set the current source name
	 */
	void setSourceName(const IRA::CString& sourceName) { m_sourceName=sourceName; }
	
	/**
	 * Get the name of the source name
	 */
	const IRA::CString& getSourceName() const { return m_sourceName; }
	
	/**
	 * Get the millisecond of integration
	 */
	long getIntegrationTime() const { return m_mainH.integration; }
	
	/**
	 * Get the sum of the inputs of all the sections
	 */
	long getInputsNumber() const;
	
	/**
	 * Saves the main headers coming from the backend
	 */
	void saveMainHeaders(Backends::TMainHeader const * h,Backends::TSectionHeader const * ch);
	
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
	 * @return the current status of the component.
	 */
	const Management::TSystemStatus& getStatus() const { return m_status; }

	/**
	 * Sets the current status of the component
	 */
	void setStatus(const Management::TSystemStatus& st) { m_status=st; }
	
	/**
	 * @sets the scan axis value  of the component.
	 */
	void setScanAxis(Management::TScanAxis& scanAxis) {m_scanAxis=scanAxis;}

	/**
	 * @return the scan axis value  of the component.
	 */
	const Management::TScanAxis& getScanAxis() const { return m_scanAxis; }

	/**
	 * @return the <i>m_ready</i> flag, that means the headers have been saved
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
	 * @return the <i>m_start</i> flag, tham means the component is opening the file
	 */
	bool isStart() const { return m_start; }
	
	/**
	 * It puts the component into the stop stage
	 */
	void startStopStage(); 
	
	/**
	 * It puts the component into the runnig stage
	 */
	void startRunnigStage();
	
	/**
	 * If puts the component into the initial situation halting the stop stage
	 */
	void haltStopStage();
	
	/**
	 * allows to set the current tracking flag from the telescope
	 */
	void setTelescopeTracking(const Management::TBoolean& tracking,const ACS::Time& time) {
		m_prevTelescopeTracking=m_telecopeTacking;
		m_telecopeTacking=(tracking==Management::MNG_TRUE);
		m_telescopeTrackingTime=time;
	}

    /**
     * allows to get HPBW value after gaussian fitting
     */
    const double& getHPBW() {return m_HPBW;}

    /**
     * sets HPBW value after gaussian fitting
     */
    void setHPBW(double HPBW) {m_HPBW=HPBW;}

    /**
     * allows to get amplitude (peak temperature) value after gaussian fitting
     */
    const double& getAmplitude() {return m_amplitude;}

    /**
     * sets amplitude (peak temperature) value after gaussian fitting
     */
    void setAmplitude(double amplitude) {m_amplitude=amplitude;}

    /**
     * allows to get peak offset value after gaussian fitting
     */
    const double& getPeakOffset() {return m_peakOffset;}

    /**
     * sets peak offset value after gaussian fitting
     */
    void setPeakOffset(double peakOffset) {m_peakOffset=peakOffset;}

    /**
     * allows to get offset value after gaussian fitting
     */
    const double& getOffset() {return m_offSet;}

    /**
     * sets offset value after gaussian fitting
     */
    void setOffset(double offset) {m_offSet=offset;}

    /**
     * allows to get slope value after gaussian fitting
     */
    const double& getSlope() {return m_slope;}

    /**
     * sets slope value after gaussian fitting
     */
    void setSlope(double slope) {m_slope=slope;}

    /**
     * allows to get source flux value
     */
    const double& getSourceFlux() {return m_sourceFlux;}

    /**
     * sets source flux value
     */
    void setSourceFlux(double sourceFlux) {m_sourceFlux=sourceFlux;}

    /**
     * allows to get single dataY value
     */
    const double& getDataY() {return m_dataY;}
		
    /**
     * allows to get single dataX value
     */
    const double& getDataX() {return m_dataX;}
		
    /**
     * allows to get arrayDataY value
     */
    const ACS::doubleSeq& getArrayDataY() {return m_arrayDataY;}
		
    /**
     * allows to get arrayDataX value
     */
    const ACS::doubleSeq& getArrayDataX() {return m_arrayDataX;}
		
    /**
     * sets single dataY value, tsys value
     */
    void setDataY(double dataY) {m_dataY=dataY;}
		
    /**
     * sets single dataX value, axis coordinate
     */
    void setDataX(double dataX) {m_dataX=dataX;}
		
    /**
     * sets arrayDataY value
     */
    void setArrayDataY(ACS::doubleSeq arrayDataY) {m_arrayDataY=arrayDataY;}
		
    /**
     * sets arrayDataX value
     */
    void setArrayDataX(ACS::doubleSeq arrayDataX) {m_arrayDataX=arrayDataX;}
		
private:
	/** the name of the file */
	IRA::CString m_fileName;
	IRA::CString m_projectName;
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
	CalibrationTool_private::CDumpCollection m_dumpCollection;
	/**
	 * Reports about the status of the backend
	 */
	Management::TSystemStatus m_status;
	/**
	 * Reports about the scan axis
	 */
	Management::TScanAxis m_scanAxis;
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
	 * Name of the currently observed source
	 */
	IRA::CString m_sourceName;
	/**
	 * Scan identifier number
	 */
	long m_scanId;
	/**
	 * Device number
	 */
	long m_deviceID;
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

    double m_dataY, m_dataX;
	ACS::doubleSeq m_arrayDataY, m_arrayDataX;
    double m_HPBW, m_amplitude, m_peakOffset, m_offSet, m_slope, m_sourceFlux;
};

};

#endif /*CDATACOLLECTION_H_*/
