#ifndef CORE_COMMON_H_
#define CORE_COMMON_H_

// $Id: Core_Common.h,v 1.1 2011-04-15 09:06:59 a.orlati Exp $

#define COMMON_INIT 


/**
 * Check the scan against the instrumentation. If it could be executed or not. The result could depend also on schedule mode, scan type, instrument configuration and so on.
 * @param mode schedule mode
 * @param rec structure that stores the selected schedule line
 * @param scanRec structure that contains the scan parameters
 * @param antBoss reference to the antenna boss
 * @param antBossErr flags the status of the antenna boss reference
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @thorw ComponentErrors::CORBAProblemExImpl
 * @return true if the scan has been checked succesfully
 */ 	
static bool checkScan(const CSchedule::TScheduleMode& mode,const CSchedule::TRecord& rec,const Schedule::CScanList::TRecord& scanRec,Antenna::AntennaBoss_ptr antBoss,bool& antBossError) 
	throw (ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl);


/**
 * Send to the antenna boss the commands required to start scan.
 * @thorw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @param scan structure that contains the scan to be executed. The ut field can be modified. 
 * @param scanRec structure that contains the scan parameters
 * @param par structure that describes the scan
 * @param antBoss reference to the antenna boss
 * @param antBossErr flags the status of the antenna boss reference
*/
static void doScan(CSchedule::TRecord& scan,const Schedule::CScanList::TRecord& scanRec,Antenna::AntennaBoss_ptr antBoss,bool& antBossError) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::ComponentNotActiveExImpl);

/**
 * Given an UT time, this function returns the next ut at which the provided lst takes place. It takes into
 * account that two different UT instants can correspond to the same lst. 
 * @param currentUT current UT time used for recursion
 * @param checkUT current UT time used for comparison
 * @param lst local sidereal time to be converted in UT time (radians)
 * @param site information about the site
 * @param dut1 difference between UTC and UT1 
 * @return UT corresponding to the input lst
 */ 
static ACS::Time getUTFromLST(const IRA::CDateTime& currentUT,const IRA::CDateTime& checkUT,const ACS::TimeInterval& lst,const IRA::CSite& site,const double& dut1);

/**
 * Computes the name of the output file, including the lst time
 * @param ut universal time
 * @param lst localsidereal time
 * @param prj name of the project
 * @param suffix string that will be appended to the end of file name
 * @return the name of the file to be created
*/
static IRA::CString computeOutputFileName(const ACS::Time& ut,const ACS::TimeInterval& lst,const IRA::CString& prj,const IRA::CString& suffix);

/**
 * Computes the name of the output file, not including the lst time
 * @param ut universal time
 * @param lst localsidereal time
 * @param prj name of the project
 * @param suffix string that will be appended to the end of file name
 * @return the name of the file to be created
*/	
static IRA::CString computeOutputFileName(const ACS::Time& ut,const IRA::CString& prj,const IRA::CString& suffix);

/**
 * This is the handler of the events coming from the notification channel published by the Antenna subsystem
 */
static void antennaNCHandler(Antenna::AntennaDataBlock antennaData,void *handlerParam);

/**
 * used as wrapper to function that are in charge of forwarding commands to other packages  
*/
IRA::CString remoteCall(const IRA::CString& command,const IRA::CString& package,const long& par) throw (ParserErrors::ExecutionErrorExImpl,ParserErrors::PackageErrorExImpl); 


#endif /*CORE_COMMON_H_*/

