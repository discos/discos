#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when           What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  12/12/2008     Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  15/10/2011      Added support for auxiliary file: scan layout                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  08/04/2013     support for the new SKYDIP scan type.*/

#include <IRA>
#include <iostream>
#include <fstream>
#include <acstimeDurationHelper.h>
#include <vector>
#include <AntennaDefinitionsC.h>
#include <ReceiversDefinitionsC.h>
#include <MinorServoBossC.h>
#include <AntennaBossC.h>
#include <ReceiversBossC.h>
#include "Configuration.h"

#define _SCHED_NULLTARGET "NULL"
#define SEPARATOR '\t'

/**
 * This namespace contains all the classes and symbol that are needed to parse and execute the schedules
 */
namespace Schedule {

class CParser;

/**
 * This class quickly maps a subscan (defined by its salient parameters) to the correct subscan configurations structures used to
 * command the subscan to the telescope.
 */
class CSubScanBinder {
public:
	CSubScanBinder(Antenna::TTrackingParameters * const primary,Antenna::TTrackingParameters * const secondary,
	  MinorServo::MinorServoScan * const servo,Receivers::TReceiversParameters * const receievers,CConfiguration* config=NULL);
	CSubScanBinder(CConfiguration* config=NULL,bool dispose=true);
	~CSubScanBinder();
	void lonOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration);
	void latOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration);
	void addOffsets(const double& lonOff,const double& latOff,const Antenna::TCoordinateFrame& frame);
	void OTF(const IRA::CString& target,
			const double& lon1,const double& lat1,const double& lon2,const double& lat2,
			const Antenna::TCoordinateFrame& coordFrame,const Antenna::TsubScanGeometry& geometry,
			const Antenna::TCoordinateFrame& subScanFrame,const Antenna::TsubScanDescription& description,
			const Antenna::TsubScanDirection& direction,const ACS::TimeInterval& subScanDuration);

	void OTFC(const Antenna::TCoordinateFrame& coordFrame,const Antenna::TsubScanGeometry& geometry,
		const Antenna::TCoordinateFrame& subScanFrame,const Antenna::TsubScanDirection& direction,
		const double& span,const ACS::TimeInterval& subScanDuration,const Antenna::TTrackingParameters * const sec);

	void skydip(const double& lat1,const double& lat2,const ACS::TimeInterval& duration,
			const Antenna::TTrackingParameters * const sec);

	void peaker(const IRA::CString& axis,const double& span,const ACS::TimeInterval& duration,const Antenna::TTrackingParameters * const sec);

	void track(const char *targetName);
	void moon();
	void sidereal(const char * targetName,const double& ra,const double& dec,const Antenna::TSystemEquinox& eq,const Antenna::TSections& section);
	void goTo(const double& az,const double& el);

	Antenna::TTrackingParameters * getPrimary() const { return m_primary; }
	Antenna::TTrackingParameters * getSecondary() const { return m_secondary;}
	MinorServo::MinorServoScan * getServo() const { return m_servo; }
	Receivers::TReceiversParameters *getReceivers() const { return m_receivers;}


private:
	Antenna::TTrackingParameters *m_primary;
	Antenna::TTrackingParameters *m_secondary;
	MinorServo::MinorServoScan *m_servo;
	Receivers::TReceiversParameters *m_receivers;
	bool m_own;
	CConfiguration* m_config;
	void init();
	void addSecondaryAntennaTrack(const Antenna::TTrackingParameters * const sec);
	void copyPrimaryAntenaTrack(const Antenna::TTrackingParameters * const prim);

};

/**
 * This is the base class that exposes a pure virtual method that is used by the parser to read the input file.
 * All classes that are specific parsing unit must inherit from this class. In order to read another schedule the user must create another
 * object, there is no way for the same object to read more than one schedule at a time.
 */ 
class CBaseSchedule {
public:
	/**
	 * Constructor
	 * @param path path to the file, the final "/" must be included
	 * @param file file name to parse
	 */
	CBaseSchedule(const IRA::CString& path,const IRA::CString& fileName);
	
	/**
	 * Destructor
	 */
	virtual ~CBaseSchedule();
	
	/** 
	 * Called to ask the object to read the schedule file
	 * @param check true if the method has also to check the schedule for consistency
	 * @return the result of the operation, true the operation is successful
	 */
	virtual bool readAll(bool check);
	
	/**
	 * @return the number of lines in the schedule
	 */
	const DWORD& getTotalLines() const  { return m_lines; }
	
	/**
	 * @return the name of the file of the schedule
	 */
	const IRA::CString& getFileName() const { return m_fileName; }
	
	/**
	 * @return the name (no extension) of the file of the schedule
	 */
	const IRA::CString& getBaseName() const { return m_baseName;}

	/**
	 * @return the message relative to the last error
	 */
	const IRA::CString& getLastError() const { return m_lastError; }

	/**
	 * sets the pointer to the configuration object
	 */
	void setConfiguration(CConfiguration* config) { m_config=config; }

	/**
	 * return the reference to the configuration object
	 */
	CConfiguration* getConfiguration() const { return m_config; }
protected:
	friend class CParser;
	/**
	 * number of schedule lines
	*/
	DWORD m_lines;
	/**
	 * file name
	 */
	IRA::CString m_fileName;
	/**
	 * path to the file
	 */
	IRA::CString m_filePath;
	/**
	 * base name of the file
	 */
	IRA::CString m_baseName;
	/**
	 * Reports the message relative to the  last error
	 */ 
	IRA::CString m_lastError;
	/**
	 * Parser class
	 */
	CParser *m_parser;
	/**
	 * Pointer to the configuration object
	 */
	CConfiguration *m_config;
	/**
	 * This pure virtual method is called to check is the schedule is consistent. In particular
	 * to check if all references to other file are all satisfied.
	 * @param line it returns the line number at which the inconsistency has been found
	 * @param errMsg description of the error if the check fails
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line,IRA::CString& errMsg)=0;
	
	/**
	 * This pure virtual method is called by the parser in order to parse single line
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @param errorMsg message i case an error is found
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg)=0;
};


class CLayoutList : public virtual CBaseSchedule {
public:
	/**
	 * Constructor
	 * @param path path to the file, the final "/" must be included
	 * @param file file name to parse
	 */
	CLayoutList(const IRA::CString& path,const IRA::CString& fileName);

	/**
	 * Destructor
	 */
	virtual ~CLayoutList();

	/**
	 * This method is called to get a scan layout given its name
	 * @param layoutName name of the layout
	 * @param layout sequence of string containing one layout line each.
	 * @return false if the procedure could not be found.
	 */
	virtual bool getScanLayout(const IRA::CString& layoutName,ACS::stringSeq& proc);

	/**
	 * Checks the existence of a layout
	 * @param layoutName name to be checked against the layout file
	 * @return true if the layout exists, false if it doesn't.
	 */
	virtual bool checkLayout(const IRA::CString& layoutName);

protected:

	struct TRecord {
		DWORD line;
		IRA::CString layoutName;
		std::vector<IRA::CString> layout;
	};

	typedef std::vector<TRecord *> TLayout;
	typedef TLayout::iterator TIterator;
	TLayout m_layout;

	bool m_started;
	TRecord * m_currentRecord;

	/**
	 * This method is called to check if the schedule is consistent.
	 * @param line it returns the line number at which the inconsistency has been found
	 * @param errMsg description of the error if the check fails	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line,IRA::CString& errMsg);

	/**
	 * This method is called by the parser in order to read what a file contains
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @param errorMsg message in case an error is found
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg);
};

class CProcedureList: public virtual CBaseSchedule
{
public:
	/**
	 * Constructor
	 * @param path path to the file, the final "/" must be included
	 * @param file file name to parse
	 */
	CProcedureList(const IRA::CString& path,const IRA::CString& fileName);
		
	/**
	 * Destructor
	 */
	virtual ~CProcedureList();

	/**
	 * This method is called to get a procedure.
	 * @param conf the name of the procedure.
	 * @param proc vector of string containing one command each. if the array is empty, no configuration is required.
	 * @param args number of arguments required by the configuration procedure
	 * @return false if the procedure could not be found.
	 */
	virtual bool getProcedure(const IRA::CString& conf,std::vector<IRA::CString>& proc,WORD& args);
	
	/**
	 * This method is called to get a procedure.
	 * @param conf the name of the procedure.
	 * @param proc sequence of string containing one command each. if the string sequence is empty, no
	 *                configuration is required.
	 * @param args number of arguments required by the configuration procedure
	 * @return false if the procedure could not be found.
	 */	
	virtual bool getProcedure(const IRA::CString& conf,ACS::stringSeq& proc,WORD& args);
	
	/**
	 * This method is called to get the procedure at a given position
	 * @param pos position of the procedure
	 * @param conf name of the procedure
	 * @param args number of arguments required by the configuration procedure
	 * @param proc sequence of string containing one command each.
	 */
	virtual bool getProcedure(const WORD& pos,IRA::CString& conf,ACS::stringSeq& proc,WORD& args);

	/**
	 * Checks the procedure, if the procedure exists the number of arguments is checked to coherent .
	 * @param conf procedure to be checked
	 * @param args number of expected arguments
	 * @return true if the procedure exists and the number of argument is ok, or if the procedure does not exist. False otherwise
	 */
	virtual bool checkProcedure(const IRA::CString& conf,WORD args);
	
protected:
	
	struct TRecord {
		DWORD line;
		IRA::CString procName;
		std::vector<IRA::CString> proc;
		WORD args; // number of arguments required by the procedure
	};
	
	bool m_started;
	TRecord * m_currentRecord;
	
	typedef std::vector<TRecord *> TProcedure;
	typedef TProcedure::iterator TIterator;
	TProcedure m_procedure;
	
	/**
	 * This pure virtual method is called to check if the schedule is consistent.
	 * @param line it returns the line number at which the inconsistency has been found
	 * @param errMsg description of the error if the check fails
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line,IRA::CString& errMsg);
	
	/**
	 * This method is called by the parser in order to read the configuration file of a main schedule
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @param errorMsg message in case an error is found
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg);

	/**
	 * Called to extract the number of expected arguments from the name of the procedure
	 * @param orig original name of the procedure
	 * @param proc name of the procedure without the arguments indication
	 * @param args number of arguments
	 * @param errorMsg error message in case the operation has failed
	 * @return false it the
	 */
	virtual bool extractArgument(const IRA::CString& orig,IRA::CString& proc,WORD& args,IRA::CString& errorMsg);
};

class CBackendList: public virtual CBaseSchedule
{
public:
	/**
	 * Constructor
	 * @param path path to the file, the final "/" must be included
	 * @param file file name to parse
	 */
	CBackendList(const IRA::CString& path,const IRA::CString& fileName);
		
	/**
	 * Destructor
	 */
	virtual ~CBackendList();
	
	/**
	 * This method is called to get a backend configuration procedure.
	 * @param name name of the backend configuration procedure.
	 * @param backend name of the backend that the procedure refers to
	 * @param proc sequence of string containing one command each. if the string sequence is empty, no
	 *                configuration is required.
	 * @return false if the configuration could not be found. 
	 */
	virtual bool getBackend(const IRA::CString& name,IRA::CString& backend,std::vector<IRA::CString>& proc);
	
	/**
	 * Checks the existence of a backend configuration procedure
	 * @param name backend configuration we are checking
	 * @return true if the scan exists, false if it doesn't.
	 */
	virtual bool checkBackend(const IRA::CString& name);

protected:
	struct TRecord {
		DWORD line;
		IRA::CString procName;
		IRA::CString target;
		std::vector<IRA::CString> proc;
	};
	
	bool m_started;
	TRecord * m_currentRecord;
	
	typedef std::vector<TRecord *> TBackend;
	typedef TBackend::iterator TIterator;
	TBackend m_backend;
	
	/**
	 * This pure virtual method is called to check if the schedule is consistent.
	 * @param line it returns the line number at which the inconsistency has been found
	 * @param errMsg description of the error if the check fails
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line,IRA::CString& errMsg);
	
	/**
	 * This method is called by the parser in order to read the backend file of a main schedule
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @param errorMsg message in case an error is found
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg);
	
};

class CScanList: public virtual CBaseSchedule
{
public:
	struct TRecord {
		DWORD line;
		DWORD id;
		Management::TScanTypes type;
		void *primaryParameters;
		void *secondaryParameters;
		void *servoParameters;
		void *receieversParsmeters;
		//IRA::CString target;
	};
	/**
	 * Constructor
	 * @param path path to the file, the final "/" must be included
	 * @param file file name to parse
	 */
	CScanList(const IRA::CString& path,const IRA::CString& fileName);
		
	/**
	 * Destructor
	 */
	virtual ~CScanList();
	
	/**
	 * This method is called to get the scan with a specific identifier.
	 * @param id identifier of the scan we are asking for
	 * @param type returns the type of the scan we are dealing with
	 * @param prim pointer to the buffer that stores the primary scan parameters, the number and the type of parameters could vary from scna type
	 *                to scan type.
	 * @param sec pointer to the secondary scan parameters
	 * @param servo pointer to the servo scan parameters
	 * @param recv poiner to the receievers scan parameters
	 * @return false if the scan could not be found. 
	 */
	virtual bool getScan(const DWORD&id,Management::TScanTypes& type,void *& prim,void *& sec,void *& servo,void *& recv);
	
	/**
	 * This is the wrapper function of the previuos one, used to get scan data using a structure.
	 * @param id identifier of the scan we are asking for 
	 * @param rec structure that stored the scan parameters
	 * @return false if the scan could not be found.
	 */ 
	virtual bool getScan(const DWORD& id,TRecord& rec);
	
	/**
	 * Checks the existence of a scan by its identifier
	 * @param id identifier of the scan we are checking
	 * @return true if the scan exists, false if it doesn't.
	 */
	virtual bool checkScan(const DWORD& id);
	
private:
	typedef std::vector<TRecord *> TSchedule;
	typedef TSchedule::iterator TIterator;
	TSchedule m_schedule;
	/**
	 * translate a string into a scan type defined in the Management package
	 * @return false if the translation was not possible
	 */
	/*bool string2ScanType(const IRA::CString& val,Management::TScanTypes& type);*/
	
	bool parsePeaker(const IRA::CString& line,DWORD& id,IRA::CString& errMsg,CSubScanBinder& binder);

	/**
	 * Reset the basic fields of tracking parameters structure.
	 * @param scan pointer to the structure
	 */
	void resetTrackingParameters(Antenna::TTrackingParameters *scan);
	
	/**
	 * parse the list of parameters of an OTF.
	 * @param val line to parse
	 * @param scan structure containing the OTF parameters
	 * @param id numeral identifier of the scan
	 * @param errMsg error specification string in case of unsuccessful operation
	 * @return the result of the parse
	 */
	bool parseOTF(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg);

	/**
	 * parse the list of parameters of an OTFC scan.
	 * @param val line to parse
	 * @param scan structure containing the OTF parameters
	 * @param scanSec structure containing the parameters of the secondary scan which is the scan used to compute the center of the scan
	 * @param id numeral identifier of the scan
	 * @param errMsg error specification string in case of unsuccessful operation
	 * @return the result of the parse
	 */	
	bool parseOTFC(const IRA::CString& val,Antenna::TTrackingParameters *scan,Antenna::TTrackingParameters *secScan,DWORD& id,IRA::CString& errMsg);

	/**
	 * parse the list of parameters of a SKYDIP scan.
	 * @param val line to parse
	 * @param scan structure containing the OTF parameters
	 * @param scanSec structure containing the parameters of the secondary scan which is the scan used to compute the center of the scan
	 * @param id numeral identifier of the scan
	 * @param errMsg error specification string in case of unsuccessful operation
	 * @return the result of the parse
	 */
	bool parseSKYDIP(const IRA::CString& val,Antenna::TTrackingParameters *scan,Antenna::TTrackingParameters *secScan,DWORD& id,IRA::CString& errMsg);

	/**
	 * Parse the list of parameters of sidereal tracking.
	 * @param val line to parse
	 * @param otf structure containing the ORF parameters
	 * @param id numeral identifier of the scan
	 * @param errMsg error specification string in case of unsuccessful operation
	 * @return the result of the parse
	 */
	bool parseSidereal(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg);
	
	/**
	 * Parse the list of parameters for moon tracking
	 * @param val line to parse
	 * @param otf structure containing the ORF parameters
	 * @param id numeral identifier of the scan
	 * @param errMsg error specification string in case of unsuccessful operation
	 * @return the result of the parse
	 */
	bool parseMoon(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg);
	
	/**
	 * This method is called by the parser in order to read the scan list of a main schedule
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @param errorMsg message in case an error is found
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg);
	
	/**
	 * This pure virtual method is called to check is the schedule is consistent. In particular
	 * it checks that the scan identifiers are unique and not duplicated.
	 * @param line it returns the line number at which the inconsistency has been found
	 * @param errMsg description of the error if the check fails
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line,IRA::CString& errMsg);
};

/**
 * This class is in charge to load, parse the scheduler's schedule file.
 */
class CParser {
public:
	/**
	 * Constructor
	 */
	CParser();
	
	/**
	 * Destructor
	 */
	virtual ~CParser();
	
	/**
	 * This method will open the schedule file.
	 * @param unit reference to a class that know how to parse the line of the schedule file.
	 * @return false if the file could not be opened, for example because the file does not exist or the user does not retain the
	 *               priviledges to open it.
	 */ 
	bool open(const IRA::CString& fileName);
	
	/**
	 * This method force the current file to be closed
	 */
	void close();
	
	/**
	 * This method will parse the opened schedule file.
	 * @param unit reference to a class that know how to parse the line of the schedule file.
	 * @param if the function returns without errors it represent the number of lines in the schedule; otherwise it is the line at which
	 *               the parsing stopped due to an error
	 * @param message if an error is found
	 * @return true if the parsing is completed succesfully, otherwise it returns false.
	 */
	bool parse(CBaseSchedule* unit,DWORD& line,IRA::CString& errorMsg);
	
private:
	std::ifstream m_file;
};

}; // end namespace

/**
 * This class is able to interpret and  handle a main schedule
 */
class CSchedule : public virtual Schedule::CBaseSchedule {
public:
	enum TScheduleMode {
		LST,
		SEQ,
		TIMETAGGED
	};
	/**
	 * This record describes the single scan
	 */
	struct TRecord {
		DWORD line;						           /*!< line number of the schedule file at which the subscan starts*/
		DWORD scanLine;                          /*< line number of the schedule file at which the scan starts */
		DWORD subscanid;                    /*!< subscan identifier */
		DWORD scanid;					          /*!< scan identifier */
		DWORD counter;		        		  /*!< scan counter */
		ACS::TimeInterval lst;                /*!< in case, the start local sidereal time */
		ACS::Time ut;                               /*!< the start time as  UT time */
		double duration;                     /*!< duration of the scan */
		IRA::CString layout;					/*!< name of macro that defines the scan layout, the name must be present in the layoutfile */
		DWORD scan;                             /*!< index to the scan list that identifies the scan parameters */
		IRA::CString preScan;              /*!< prescan procedure name */
		WORD preScanArgs;               /*!< number of prescan procedure expected arguments */
		bool preScanBlocking;         /*!< true if the prescan procedure as to be blocking */
		IRA::CString postScan;            /*!< post scan procedure name */
		WORD postScanArgs;            /*!< number of postcan procedure expected arguments */
		bool postScanBlocking;       /*!< true if the postscan procedure has to be blocking */
		IRA::CString backendProc;    /*!< name of the configuration procedure of the backend */
		IRA::CString writerInstance; /*!< name of the component in charge of receiving the data from the backend */
		IRA::CString suffix                    /*!< token appended to the name of the file */;
		bool rewind;							/*!< this is a runtime attribute, it will be set to true when the current subscan is the first one located by the <i>getScan_LST</i> or <i>getScan_SEQ</i>  methods after the schedule was rewound*/
	};
	/**
	 * Constructor
	 * @param path path to the file, the final "/" must be included
	 * @param file file name to parse
	 */
	CSchedule(const IRA::CString& path,const IRA::CString& fileName);

	/**
	 * Destructor
	 */
	virtual ~CSchedule();
	
	/** 
	 * Called to ask the object to read the schedule file and the correlated files
	 * @return check true to force a check of the schedule
	 * @return the result of the operation, true the operation is suiccesful
	 */
	virtual bool readAll(bool check);
	
	/**
	 * This  method returns true if the schdule is complete, for example if all the required fields or sections
	 * are reported in the schedule file
	 */
	virtual bool isComplete();
	
	/**
	 * @return the number of lines of the schedule
	*/
	DWORD getSubScansNumber() const { return m_schedule.size(); }
	
	/**
	 * @return the mode of the schedule
	 */
	const TScheduleMode& getSchedMode() const { return m_mode; }
	
	/**
	 * @return the number of repetitions
	 */
	const long& getSchedReps() const { return m_repetitions; }
	
	/**
	 * @return the local sidereal time when the schedule is supposed to start
	 */
	const ACS::TimeInterval& getSchedStartTime() const { return m_startLST; }
	
	/**
	 * @return the start number used to enumerate the scans. Negative means the number was not provided so the scan enumeration is not done
	 */
	long getScanTag() { long tmp=m_scanTag; if (m_scanTag>0) m_scanTag++; return tmp; }
	
	/**
	 * Called to retrieve the initialization procedure
	 * @param procName name of the procedure, included the parameter list
	 */
	bool getInitProc(IRA::CString& procName);
	
	/**
	 * Used to retrieve the desired  subscan number.
	 * @param counter subscan counter
	 * @param rec stucture that stores all the subscan parameters
	 * @return true if the subscan can be returned
	 */
	bool getSubScan(const DWORD& counter,TRecord& rec);
	
	/**
	 * Can be called to know the subscan counter given the scanid and the subscanid.
	 * @param scanid identifier of the scan
	 * @param subscanid identifer of the subscan
	 * @return the corresponding subscan counter, if the couple scanid/subscanid does not exists, the first subscan that matches scanid is returned; if nothing is matched  the first subscan is returned (counter=1)
	 */
	DWORD getSubScanCounter(const DWORD& scanid,const DWORD& subscanid);

	/**
	 * Can be called to know the subscan counter given the string that codes the scanid and subscanid.
	 * @param scan it codes the subscan, the format is expected to be scanid_subscanid
	 * @return the corresponding subscan counter, if the couple scanid/subscanid does not exists, the first subscan that matches scanid is returned; if nothing is matched  the first subscan is returned (counter=1)
	 */
	DWORD getSubScanCounter(const IRA::CString& scan);

	/**
	 * Called to get the scan and subscan identifier starting from the subscan counter
	 * @param counter subscan counter
	 * @param scanID corresponding scan identifier
	 * @param subScanID corresponding sub scan identifier
	 */
	void getIdentifiers(const DWORD& counter,DWORD& scanID,DWORD& subScanID);

	/**
	 * Called to get the scan and subscan identifier (as a string) starting from the subscan counter
	 * @param counter subscan counter
	 * @return the corresponding scan
	 */
	IRA::CString getIdentifiers(const DWORD& counter);

	/**
	 * Used to retrive the next scan number of the schedule. 
	 * @param counter subscan counter inside the schedule (input/output)
	 * @param scanid scan identifier(output)
	 * @param subscanid subscan identifier (output)
	 * @param duration duration of the returned schedule line (output)
	 * @param scan identifier of the scan (inside the scan list) of the current line (output)
	 * @param pre identifier of the procedure inside the procedure file to be executed before that scan (output)
	 * @param preBlocking this flag is true if the end of the pre scan procedure must be waited for (output)
	 * @param preArgs number of expected parameter of the pre scan procedure
	 * @param post identifier of the procedure inside the procedure file to be executed after that scan (output)
	 * @param postBlocking this flag is true if the end of the post scan procedure must be waited for (output)
	 * @param postArgs number of expected parameter of the post scan procedure
	 * @param bckProc identifier of the backend configuration procedure inside the backend file (output)
	 * @param wrtInstance name of the instance of the storage to be used (output) 
	 * @param suffix string that will be appended at the end of the file name(output)
	 * @param layout name of the scan layout definition (output)
	 * @param rewind is the scan the first one after a schedule rewind? (output)
	 * @return true if the line can be returned 
	 */
	bool getSubScan_SEQ(DWORD& counter,DWORD& scanid,DWORD& subscanid,double& duration,DWORD& scan,IRA::CString& pre,bool& preBlocking,WORD& preArgs,IRA::CString& post,bool& postBlocking,
			WORD& postArgs,IRA::CString& bckProc,IRA::CString& wrtInstance,IRA::CString& suffix,IRA::CString& layout,bool &rewind);
	
	/**
	 * Used to retrive the desired scan number. This method is a wrapper of the previuos method used to retrieve the informations using a structure.
	 * @param rec stucture that stores all the scan parameters
	 * @param rewound true if the schedule had to be rewound in order to return the next subscan
	 * @return true if the line can be returned 
	 */	
	bool getSubScan_SEQ(TRecord& rec);
	
	/**
	 * Used to retrive the desired  line based on start LST. This method will returns the information of the scan
	 * that has the start lst immediately after the lst provided. The parameter <i>id</i> is also used. if it is non zero
	 * the search is started from the line number which is greater than <i>id</i>.
	 * @param lst local sidereal time used for the search (input). It returns the effective lst of the located scan (output) 
	 * @param counter scan counter  (input). If non zero the search is started from that subscan. It returns the effective line number of the located scan (output).
	 * @param scanid scan identifier(output)
	 * @param subscanid subscan identifier (output)
	 * @param duration duration of the returned schedule line (output)
	 * @param scan identifier of the scan (inside the scan list) of the current line (output)
	 * @param pre identifier of the procedure inside the procedure file to be executed before that scan (output)
	 * @param preBlocking this flag is true if the end of the pre scan procedure must be waited for (output)
	 * @param preArgs number of expected parameter of the pre scan procedure
	 * @param post identifier of the procedure inside the procedure file to be executed after that scan (output)
	 * @param postBlocking this flag is true if the end of the post scan procedure must be waited for (output)
	 * @param postArgs number of expected parameter of the post scan procedure
	 * @param bck identifier of the backend conf procedure inside the backend file (output)
	 * @param wrtInstance name of the instance of the storage to be used (output)
	 * @param suffix string that will be appended at the end of the file name (output)
	 * @param layout  name of the scan layout definition (output)
	 * @param rewind is the scan the first one after a schedule rewind? (output)
	 * @return true if the line can be returned 
	 */
	bool getSubScan_LST(ACS::TimeInterval& lst,DWORD& counter,DWORD&scanid,DWORD& subscanid,double& duration,DWORD& scan,
			IRA::CString& pre,bool& preBlocking,WORD& preArgs,IRA::CString& post,bool& postBlocking,WORD& postArgs,IRA::CString& bckProc,IRA::CString& wrtInstance,IRA::CString& suffix,IRA::CString& layout,bool& rewind);

	/**
	 * Used to retrive the desired  line based on start LST. This method is a wrapper of the previuos method used to retrieve the informations using a structure.
	 * @param rec stucture that stores all the scan parameters
	 * @return true if the line can be returned 
	 */
	bool getSubScan_LST(TRecord& rec);

	/**
	 * @return the pointer to the scan list
	 */
	Schedule::CScanList *getScanList() const { return m_scanListUnit; }
	
	/**
	 * @return the pointer to the preScan procedures
	 */
	Schedule::CProcedureList *getPreScanProcedureList() const { return m_preScanUnit; }

	/**
	 * @return the pointer to the postScan procedures
	 */
	Schedule::CProcedureList *getPostScanProcedureList() const { return m_postScanUnit; }
	
	/**
	 * @return the pointer to the backend list
	 */
	Schedule::CBackendList *getBackendList() const { return m_backendListUnit; }
	
	/**
	 * @return the pointer to the layout  list, it could be null
	 */
	Schedule::CLayoutList *getLayoutList() const { return m_layoutListUnit; }

	/**
	 * @return the name of the project
	 */
	const IRA::CString& getProjectName() const { return m_projectName; }
	
	/**
	 * @return the name of the observer
	 */ 
	const IRA::CString& getObserverName() const { return m_observer; }

	/**
	 * used to retrieve the elevation limit configured by the schedule in radians.
	 * @param lower return back the lower  value of the range, a negative means use system default
	 * @param upper return back the upper value of the range, a negative means use system default
	 */
	void getElevationLimits(double&lower, double&upper) const { lower=m_lowerElevationLimit; upper=m_upperElevationLimit; }
private:
	typedef std::vector<TRecord *> TSchedule;
	typedef TSchedule::iterator TIterator;
	IRA::CString m_projectName;
	IRA::CString m_observer;
	IRA::CString m_scanList;
	IRA::CString m_configList;
	IRA::CString m_backendList;
	IRA::CString m_layoutFile;
	Schedule::CScanList *m_scanListUnit;
	Schedule::CProcedureList *m_preScanUnit;
	Schedule::CProcedureList *m_postScanUnit;
	Schedule::CBackendList *m_backendListUnit;
	Schedule::CLayoutList *m_layoutListUnit;
	TScheduleMode m_mode;
	long m_repetitions;
	double m_lowerElevationLimit,m_upperElevationLimit;
	ACS::TimeInterval m_startLST;
	long m_scanTag;
	bool m_modeDone;
	IRA::CString m_initProc;
	WORD m_initProcArgs;
	TSchedule m_schedule;
	struct {
		bool valid;
		DWORD id;
		DWORD line;
		IRA::CString backendProc;
		IRA::CString writerInstance;
		IRA::CString suffix;
		IRA::CString layout;
	} m_currentScanDef;



	bool parseLST(const IRA::CString& val,ACS::TimeInterval& lst);
	bool parseUT(const IRA::CString& val,ACS::Time& ut);
	
	/**
	 * @param proc procedure
	 * @return the number of arguments of a procedure
	 */
	WORD getProcedureArgs(const IRA::CString& proc);

	/**
	 * it checks if the procedure must be executed asynchronously or not. The async character is also cut from the name.
	 */
	bool isSync(char *procName);
	
	/**
	 * This pure virtual method is called to check is the schedule is consistent. In particular
	 * it checks that the scan identifiers is present in the scanlist, and the configuration procedures are in the configuration file
	 * @param line it returns the line number at which the inconsistency has been found	 *
	 * @param errMsg description of the error if the check fails
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line,IRA::CString& errMsg);
	
	/**
	 * This method is called by the parser in order to read a main schedule line
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @param errorMsg message in an error is found
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg);
	
	/**
	 * Called by <i>parseLine()</i> i order to parse the body of the schedule which the part that contains the scan/subscans definition
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @param errorMsg message in case an error is found
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseScans(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg);

};

#endif /*SCHEDULE_H_*/
