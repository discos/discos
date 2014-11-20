#ifndef CORE_EXTRA_H_
#define CORE_EXTRA_H_

// Call back and event handlers

/**
 * This is the handler of the events coming from the notification channel published by the Antenna subsystem
 */
static void antennaNCHandler(Antenna::AntennaDataBlock antennaData,void *handlerParam);

/**
 * This is the handler of the events coming from the notification channel published by the Minor Servo subsystem
 */
static void minorServoNCHandler(MinorServo::MinorServoDataBlock servoData,void *handlerParam);

/**
 * This is the handler of the events coming from the notification channel published by the Receivers subsystem
 */
static void receiversNCHandler(Receivers::ReceiversDataBlock receiversData,void *handlerParam);

/**
 * timer event handler user to implement <i>waitUntil()</i> function
 */
static void waitUntilHandler(const ACS::Time& time,const void *par);

/**
 * used as wrapper to function that are in charge of forwarding commands to other packages
*/
bool remoteCall(const IRA::CString& command,const IRA::CString& package,const long& par,IRA::CString& out) throw (ParserErrors::PackageErrorExImpl,ManagementErrors::UnsupportedOperationExImpl);


#endif

