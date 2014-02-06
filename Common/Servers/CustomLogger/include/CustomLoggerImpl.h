#ifndef CUSTOMLOGGERIMPL_H
#define CUSTOMLOGGERIMPL_H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <acsutil.h>
#include <acsncSimpleSupplier.h>
#include <orbsvcs/CosNamingC.h>
#include <orbsvcs/CosNotifyChannelAdminS.h>
#include <orbsvcs/CosNotifyCommC.h>
#include "logging_idlC.h"
#include <CustomLoggerS.h>
#include <ManagmentDefinitionsS.h>
#include <ManagementErrors.h>
#include <CustomLoggerUtils.h>
#include <ComponentErrors.h>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROstring.h>
#include <baciROlong.h>
#include <enumpropROImpl.h>
#include <maciContainerImpl.h>
#include <ACSErrTypeCommon.h>
#include <acsThread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <IRA>
#include <expat_log_parsing.h>

#define WRITER_THREAD_NAME "logwriter"

/**
 * Event Type used in the ACS Notification Channel.
 */
typedef CosNotification::StructuredEvent CustomLoggingEvent;

using namespace Management; //LogLevel definition

class CustomStructuredPushConsumer; //fwd decl
class CustomLogWriterThread; //fwd decl

/**
 * Component used to manage ACS log events and log files.
 * The public interface is documented in the idl module Interfaces/ManagementInterface/idl/CustomLogger.midl
 */
class CustomLoggerImpl: public virtual baci::CharacteristicComponentImpl,
                          public virtual POA_Management::CustomLogger
{   
    public:
        CustomLoggerImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);
        virtual ~CustomLoggerImpl();
        virtual ACS::ROstring_ptr filename() throw (CORBA::SystemException);
        virtual ACS::ROlong_ptr nevents() throw (CORBA::SystemException);
        virtual ROLogLevel_ptr minLevel() throw (CORBA::SystemException);
        virtual ROLogLevel_ptr maxLevel() throw (CORBA::SystemException);
        virtual ROTBoolean_ptr isLogging() throw (CORBA::SystemException);
        virtual void initialize() throw (ACSErr::ACSbaseExImpl);
        virtual void execute();
        virtual void cleanUp();
        virtual void setMinLevel(LogLevel level) throw (CORBA::SystemException);
        virtual void setMaxLevel(LogLevel level) throw (CORBA::SystemException);
        /**
         * The expat log parser instance.
         */
        XML_Parser log_parser;
        /**
         * Handle a Log record coming from  ACS LOGGING CHANNEL.
         * Write the log to the system log file, filter the events based on our criteria and 
         * upon success sends the event to our custom notification channel and stores the record
         * in the queue of our custom events.
         * @param log_record a smart pointer to a LogRecord
         */
        void handle(LogRecord_sp log_record);
        virtual void setLogfile(const char *base_path_log, const char *base_path_full_log,  
                                    const char *filename_log, const char *filename_full_log) 
                                throw (CORBA::SystemException, ManagementErrors::CustomLoggerIOErrorEx);
        virtual void closeLogfile() throw (CORBA::SystemException, ManagementErrors::CustomLoggerIOErrorEx);
        virtual void emitACSLog(const char *msg) throw (CORBA::SystemException);
        virtual void emitLog(const char *msg, LogLevel level) throw (CORBA::SystemException);
        virtual void emitStaticLog(const char *msg, LogLevel level) throw (CORBA::SystemException);
        virtual void emitACSExceptionLog();
        virtual void emitExceptionLog();
        virtual void flush() throw (CORBA::SystemException);
        void writeLoggingQueue(bool age_check=true); //invoked by writer thread
    private:
        baci::SmartPropertyPointer<baci::ROstring> m_filename_sp;
        baci::SmartPropertyPointer<baci::ROlong> m_nevents_sp;
        baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(LogLevel), POA_Management::ROLogLevel> > m_min_level_sp;
        baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(LogLevel), POA_Management::ROLogLevel> > m_max_level_sp;
        baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(TBoolean), POA_Management::ROTBoolean> > m_isLogging_sp;
        nc::SimpleSupplier *m_loggingSupplier;
        CosNaming::NamingContext_var namingContext_m;
        CosNotifyChannelAdmin::EventChannel_var ec_;
        CosNotifyChannelAdmin::InterFilterGroupOperator ifgop_;
        CosNotifyChannelAdmin::ConsumerAdmin_var consumer_admin_;
        CustomStructuredPushConsumer* consumer_;
	/**
	* Filter the log records.
	* This filter shuold return true for all the messages produced by custom logging functions and false for every ACS 
	* system log record.
	* @return True if the log record is a custom one.
	*/
        virtual bool filter(LogRecord& log_record);
        static void _emitStaticLog(const char *msg, LogLevel level); 
        /**
         * Log files.
         */ 
        std::ofstream _custom_log, _full_log;
        /**
         * Priority queue of custom log records.
         */
        LogRecordQueue _log_queue;
        BACIMutex _log_queue_mutex;
        /**
         * Thread responsible for asynchronously acceess the _log_queue and pop the aged records for
         * file output.
         */
        CustomLogWriterThread *_writer;
        /**
         * The time interval after which a log record is popped from the queue and written to file.
         */
        ACS::TimeInterval _log_max_age;
        /** @name synchonized queue access
         * These methods must be synchronized for async thread operations.
         */
        /**@{*/
        void setLogging(bool val);
        /**
         * @return True if logging is enabled.
         */
        bool checkLogging();
        /**
         * Add a LogRecord to the queue of the custom records for later file output.
         */
        void addToLoggingQueue(LogRecord_sp log_record);
        /**@}*/
};

/**
 * Used to connect to ACS Notification Channel. 
 * All our custom logics reside in the CustomStructuredPushConsumer::push_structured_event method.
 */
class CustomStructuredPushConsumer : public POA_CosNotifyComm::StructuredPushConsumer,
                                     public PortableServer::RefCountServantBase
{
    public:
        CustomStructuredPushConsumer(CustomLoggerImpl* logger);
        virtual ~CustomStructuredPushConsumer (void);
        void connect(CosNotifyChannelAdmin::ConsumerAdmin_ptr consumer_admin);
        virtual void disconnect();
        CosNotifyChannelAdmin::StructuredProxyPushSupplier_ptr get_proxy_supplier (void);
    protected:
        CosNotifyChannelAdmin::StructuredProxyPushSupplier_var proxy_supplier_;
        CosNotifyChannelAdmin::ProxyID proxy_supplier_id_;
        CustomLoggerImpl* logger_;
        virtual void offer_change(const CosNotification::EventTypeSeq & added,
			      const CosNotification::EventTypeSeq & removed);
        /**
         * Invoked every time a notification is received via the ACS LOGGING_CHANNEL.
         * Tries to unpack the notification into one or more xml strings (one for each log record) 
         * and instantiates the realtive LogRecord objects which constitute the minimal logic unit of 
         * the custom logging logics.
         */
        virtual void push_structured_event (const CosNotification::StructuredEvent & notification);
        virtual void disconnect_structured_push_consumer ();
};

/**
 * Thread class used for periodically pop old LogRecord elements from the logger _log_queue to
 * the output file.
 */
class CustomLogWriterThread : public ACS::Thread
{
    public:
        /**
         * Constructor
         */
        CustomLogWriterThread(const ACE_CString& name, CustomLoggerImpl *logger); 
        virtual ~CustomLogWriterThread();
        /**
         * Called once every period, simply invokes the CustomLoggerImpl::writeLoggingQueue method from the thread.
         */
        virtual void runLoop();
    private:
        CustomLoggerImpl *_logger;
};

#endif

