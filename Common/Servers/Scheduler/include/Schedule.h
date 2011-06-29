#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: Schedule.h,v 1.16 2011-06-21 16:39:52 a.orlati Exp $										                                                            */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when           What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  12/12/2008     Creation                                          */

#include <IRA>
#include <iostream>
#include <fstream>
#include <acstimeDurationHelper.h>
#include <vector>
#include <AntennaDefinitionsS.h>
#include <AntennaBossS.h>

#define _SCHED_NULLTARGET "NULL"

/**
 * This namespace containes all the classes and symbol that are needed to parse and execute the schuedules
 */
namespace Schedule {

class CParser;

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
	 * @return the result of the operation, true the operation is succesful
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
	 * @return the message relative to the last error
	 */
	const IRA::CString& getLastError() const { return m_lastError; }
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
	 * Reports the message relative to the  last error
	 */ 
	IRA::CString m_lastError;
	/**
	 * Parser class
	 */
	CParser *m_parser;
	/**
	 * This pure virtual method is called to check is the schedule is consistent. In particular
	 * to check if all references to other file are all satisfied.
	 * @param line it returns the line number at which the inconsistency has been found
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line)=0;
	
	/**
	 * This pure virtual method is called by the parser in order to parse single line
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber)=0;
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
	 * @param proc vector of string containing one command each. if the array is empty, no
	 *                configuration is required.
	 * @return false if the procedure could not be found.
	 */
	virtual bool getProcedure(const IRA::CString& conf,std::vector<IRA::CString>& proc);
	
	/**
	 * This method is called to get a procedure.
	 * @param conf the name of the procedure.
	 * @param proc sequence of string containing one command each. if the string sequence is empty, no
	 *                configuration is required.
	 * @return false if the procedure could not be found.
	 */	
	virtual bool getProcedure(const IRA::CString& conf,ACS::stringSeq& proc);
	
	/**
	 * Checks the existance of a procedure 
	 * @param conf configuration we are checking
	 * @return true if the scan exists, false if it doesn't.
	 */
	virtual bool checkProcedure(const IRA::CString& conf);
	
protected:
	
	struct TRecord {
		DWORD line;
		IRA::CString procName;
		std::vector<IRA::CString> proc;
	};
	
	bool m_started;
	TRecord * m_currentRecord;
	
	typedef std::vector<TRecord *> TProcedure;
	typedef TProcedure::iterator TIterator;
	TProcedure m_procedure;
	
	/**
	 * This pure virtual method is called to check if the schedule is consistent.
	 * @param line it returns the line number at which the inconsistency has been found
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line);
	
	/**
	 * This method is called by the parser in order to read the configuration file of a main schedule
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed	 
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber);
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
	 * Checks the existance of a backend configuration procedure 
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
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line);
	
	/**
	 * This method is called by the parser in order to read the backend file of a main schedule
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber);
	
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
	 * @return false if the scan could not be found. 
	 */
	virtual bool getScan(const DWORD&id,Management::TScanTypes& type,void *& prim,void *& sec/*,IRA::CString& target*/);
	
	/**
	 * This is the wrapper function of the previuos one, used to get scan data using a structure.
	 * @param id identifier of the scan we are asking for 
	 * @param rec structure that stored the scan parameters
	 * @return false if the scan could not be found.
	 */ 
	virtual bool getScan(const DWORD& id,TRecord& rec);
	
	/**
	 * Checks the existance of a scan by its identifier
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
	bool string2ScanType(const IRA::CString& val,Management::TScanTypes& type);
	
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
	 */
	bool parseOTF(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id);

	/**
	 * parse the list of parameters of an OTFC scan.
	 * @param val line to parse
	 * @param scan structure containing the OTF parameters
	 * @param scanSec structure containing the parameters of the secondary scan which is the scan used to compute the center of the scan
	 * @param id numeral identifier of the scan
	 */	
	bool parseOTFC(const IRA::CString& val,Antenna::TTrackingParameters *scan,Antenna::TTrackingParameters *secScan,DWORD& id);
	
	/**
	 * Parse the list of parameters of sidereal tracking.
	 * @param val line to parse
	 * @param otf structure containing the ORF parameters
	 * @param id numeral identifier of the scan
	 */
	bool parseSidereal(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id);
	
	/**
	 * Parse the list of parameters for moon tracking
	 * @param val line to parse
	 * @param otf structure containing the ORF parameters
	 * @param id numeral identifier of the scan
	 */
	bool parseMoon(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id);
	
	/**
	 * This method is called by the parser in order to read the scan list of a main schedule
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber);
	
	/**
	 * This pure virtual method is called to check is the schedule is consistent. In particular
	 * it checks that the scan identifiers are unique and not duplicated.
	 * @param line it returns the line number at which the inconsistency has been found
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line);
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
	 * @return true if the parsing is completed succesfully, otherwise it returns false.
	 */
	bool parse(CBaseSchedule* unit,DWORD& line);
	
private:
	std::ifstream m_file;
};

}; // end namespace

/**
 * This class is able to interpret and the hadle a main schedule
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
		DWORD line;
		DWORD id;
		ACS::TimeInterval lst;
		ACS::Time ut;
		double duration;
		DWORD scan;
		IRA::CString preScan;
		bool preScanBlocking;
		IRA::CString postScan;
		bool postScanBlocking;
		IRA::CString backendProc;
		IRA::CString writerInstance;
		IRA::CString suffix;
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
	DWORD getSchedLines() const { return m_schedule.size(); }
	
	/**
	 * @return the mode of the schedule
	 */
	const TScheduleMode& getSchedMode() const { return m_mode; }
	
	/**
	 * @return the number of repetitions
	 */
	const long& getScanReps() const { return m_repetitions; }
	
	/**
	 * @return the local sidereal time when the schedule is supposed to start
	 */
	const ACS::TimeInterval& getSchedStartTime() const { return m_startLST; }
	
	/**
	 * @return the start number used to enumarate the scans. Negative means the number was not provided to the scan enumeration is not done
	 */
	long getScanId() { long tmp=m_scanId; if (m_scanId>0) m_scanId++; return tmp; }
	
	/**
	 * Called to retrieve the initialization procedure
	 * @param procName name of the procedure
	 * @param proc sequence of strings that stores the procedure, one command each
	 */
	bool getInitProc(IRA::CString& procName,ACS::stringSeq& proc);
	
	/**
	 * Used to retrive the desired  line number of the schedule.
	 * @param id line identifier or line number (input)
	 * @param time time stamp of the scan it could me ut or lst
	 * @param duration duration of the returned schedule line (output)
	 * @param scan identifier of the scan (inside the scan list) of the current line (output)
	 * @param pre identifier of the procedure inside the procedure file to be executed before that scan (output)
	 * @param preBlocking this flag is true if the end of the pre scan procedure must be waited for (output) 
	 * @param post identifier of the procedure inside the procedure file to be executed after that scan (output)
	 * @param postBlocking this flag is true if the end of the post scan procedure must be waited for (output) 
	 * @param bckProc identifier of the backend conf procedre inside the backend file (output)
	 * @param wrtInstance name of the instance of the storage to be used (output)
	 * @param suffix string that will be appended at the end of the file name 
	 * @return true if the line can be returned 
	 */
	bool getLine(const DWORD& line,unsigned long long& time,double& duration,DWORD& scan,IRA::CString& pre,bool& preBlocking,IRA::CString& post,bool& postBlocking,IRA::CString& bckProc,
			IRA::CString& wrtInstance,IRA::CString& suffix);
	
	/**
	 * Used to retrive the next scan number of the schedule. 
	 * @param id line identifier or line number (input/output)
	 * @param duration duration of the returned schedule line (output)
	 * @param scan identifier of the scan (inside the scan list) of the current line (output)
	 * @param pre identifier of the procedure inside the procedure file to be executed before that scan (output)
	 * @param preBlocking this flag is true if the end of the pre scan procedure must be waited for (output)
	 * @param post identifier of the procedure inside the procedure file to be executed after that scan (output)
	 * @param postBlocking this flag is true if the end of the post scan procedure must be waited for (output) 
	 * @param bckProc identifier of the backend conf procedre inside the backend file (output)
	 * @param wrtInstance name of the instance of the storage to be used (output) 
	 * @param suffix string that will be appended at the end of the file name(output)
	 * @return true if the line can be returned 
	 */
	bool getScan_SEQ(DWORD& id,double& duration,DWORD& scan,IRA::CString& pre,bool& preBlocking,IRA::CString& post,bool& postBlocking,IRA::CString& bckPorc,IRA::CString& wrtInstance,IRA::CString& suffix);
	
	/**
	 * Used to retrive the desired scan number. This method is a wrapper of the previuos method used to retrieve the informations using a structure.
	 * @param rec stucture that stores all the scan parameters
	 * @return true if the line can be returned 
	 */	
	bool getScan_SEQ(TRecord& rec);
	
	/**
	 * Used to retrive the desired  line based on start LST. This method will returns the information of the scan
	 * that has the start lst immediately after the lst provided. The parameter <i>id</i> is also used. if it is non zero
	 * the search is started from the line number which is greater than <i>id</i>.
	 * @param lst local sidereal time used for the search (input). It returns the effective lst of the located scan (output) 
	 * @param id line identifier or line number (input). If non zero the search is started from that line. 
	 *                It returns the effective line number of the located scan (output).
	 * @param duration duration of the returned schedule line (output)
	 * @param scan identifier of the scan (inside the scan list) of the current line (output)
	 * @param pre identifier of the procedure inside the procedure file to be executed before that scan (output)
	 * @param preBlocking this flag is true if the end of the pre scan procedure must be waited for (output) 
	 * @param post identifier of the procedure inside the procedure file to be executed after that scan (output)
	 * @param postBlocking this flag is true if the end of the post scan procedure must be waited for (output) 
	 * @param bck identifier of the backend conf procedure inside the backend file (output)
	 * @param wrtInstance name of the instance of the storage to be used (output)
	 * @param suffix string that will be appended at the end of the file name (output)
	 * @return true if the line can be returned 
	 */
	bool getScan_LST(ACS::TimeInterval& lst,DWORD& id,double& duration,DWORD& scan,IRA::CString& pre,bool& preBlocking,IRA::CString& post,bool& postBlocking,IRA::CString& bckProc,
			IRA::CString& wrtInstance,IRA::CString& suffix);

	/**
	 * Used to retrive the desired  line based on start LST. This method is a wrapper of the previuos method used to retrieve the informations using a structure.
	 * @param rec stucture that stores all the scan parameters
	 * @return true if the line can be returned 
	 */
	bool getScan_LST(TRecord& rec);

	/**
	 * Used to retrive the desired  line based on start UT. This method will returns the information of the scan
	 * that has the start ut immediately after the lst provided. The parameter <i>id</i> is also used. if it is non zero
	 * the search is started from the line number which is greater than <i>id</i>.
	 * @param ut local sidereal time used for the search (input). It returns the effective ut of the located scan (output) 
	 * @param id line identifier or line number (input). If non zero the search is started from that line. 
	 *                It returns the effective line number of the located scan (output).
	 * @param duration duration of the returned schedule line (output)
	 * @param scan identifier of the scan (inside the scan list) of the current line (output)
	 * @param pre identifier of the procedure inside the procedure file to be executed before that scan (output)
	 * @param post identifier of the procedure inside the procedure file to be executed after that scan (output)
	 * @param bckPorc identifier of the backend conf procedure inside the backend file (output)
	 * @param wrtInstance name of the instance of the storage to be used (output)
	 * @param suffix string that will be appended at the end of the file name(output)
	 * @return true if the line can be returned 
	 */
	/*bool getLine_UT(ACS::Time& ut,DWORD& id,double& duration,DWORD& scan,IRA::CString& pre,IRA::CString& post,IRA::CString& bckProc,
			IRA::CString &wrtInstance,IRA::CString& suffix);*/
	
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
	 * @return the name of the project
	 */
	const IRA::CString& getProjectName() const { return m_projectName; }
	
	/**
	 * @return the name of the observer
	 */ 
	const IRA::CString& getObserverName() const { return m_observer; }
private:
	typedef std::vector<TRecord *> TSchedule;
	typedef TSchedule::iterator TIterator;
	IRA::CString m_projectName;
	IRA::CString m_observer;
	IRA::CString m_scanList;
	IRA::CString m_configList;
	IRA::CString m_backendList;
	Schedule::CScanList *m_scanListUnit;
	Schedule::CProcedureList *m_preScanUnit;
	Schedule::CProcedureList *m_postScanUnit;
	Schedule::CBackendList *m_backendListUnit;
	TScheduleMode m_mode;
	long m_repetitions;
	ACS::TimeInterval m_startLST;
	long m_scanId;
	bool m_modeDone;
	IRA::CString m_initProc;
	TSchedule m_schedule;

	bool parseLST(const IRA::CString& val,ACS::TimeInterval& lst);
	bool parseUT(const IRA::CString& val,ACS::Time& ut);
	
	/**
	 * it checks if the procedure must be executed asynchronously or not. The async character is also cut from the name.
	 */
	bool isSync(char *procName);
	
	/**
	 * This pure virtual method is called to check is the schedule is consistent. In particular
	 * it checks that the scan identifiers is present in the scanlist, and the configuration prcedures are in the configuration file
	 * @param line it returns the line number at which the inconsistency has been found	 * 
	 * @return true if the check is ok
	 */
	virtual bool checkConsistency(DWORD& line);
	
	/**
	 * This method is called by the parser in order to read a main schedule line
	 * @param line input line to be parsed
	 * @param lnNumber number that indicates the number of line currently processed
	 * @return false if the line contains parsing errors
	 */
	virtual bool parseLine(const IRA::CString& line,const DWORD& lnNumber);
	
};

#endif /*SCHEDULE_H_*/
