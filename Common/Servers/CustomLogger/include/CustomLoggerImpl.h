#ifndef CUSTOMLOGGERIMPL_H
#define CUSTOMLOGGERIMPL_H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <acsutil.h>
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

typedef CosNotification::StructuredEvent CustomLoggingEvent;

using namespace Management; //LogLevel definition

class CustomStructuredPushConsumer; //fwd decl
class CustomLogWriterThread; //fwd decl

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
        XML_Parser log_parser;
        void handle(LogRecord_sp log_record);
        virtual void setLogfile(const char *base_path_log, const char *base_path_full_log,  
                                    const char *filename_log, const char *filename_full_log) 
                                throw (CORBA::SystemException, ManagementErrors::CustomLoggerIOErrorEx);
        virtual void closeLogfile() throw (CORBA::SystemException, ManagementErrors::CustomLoggerIOErrorEx);
        virtual void emitLog(const char *msg, LogLevel level) throw (CORBA::SystemException);
        virtual void flush() throw (CORBA::SystemException);
        void writeLoggingQueue(bool age_check=true); //invoked by writer thread
    private:
        baci::SmartPropertyPointer<baci::ROstring> m_filename_sp;
        baci::SmartPropertyPointer<baci::ROlong> m_nevents_sp;
        baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(LogLevel), POA_Management::ROLogLevel> > m_min_level_sp;
        baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(LogLevel), POA_Management::ROLogLevel> > m_max_level_sp;
        baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(TBoolean), POA_Management::ROTBoolean> > m_isLogging_sp;
        CosNaming::NamingContext_var namingContext_m;
        CosNotifyChannelAdmin::EventChannel_var ec_;
        CosNotifyChannelAdmin::InterFilterGroupOperator ifgop_;
        CosNotifyChannelAdmin::ConsumerAdmin_var consumer_admin_;
        CustomStructuredPushConsumer* consumer_;
        virtual bool filter(LogRecord& log_record);
        std::ofstream _custom_log, _full_log;
        LogRecordQueue _log_queue;
        BACIMutex _log_queue_mutex;
        CustomLogWriterThread *_writer;
        ACS::TimeInterval _log_max_age;
        /* Synchronized methods */
        void setLogging(bool val);
        bool checkLogging();
        void addToLoggingQueue(LogRecord_sp log_record);
};

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
        virtual void push_structured_event (const CosNotification::StructuredEvent & notification);
        virtual void disconnect_structured_push_consumer ();
};

class CustomLogWriterThread : public ACS::Thread
{
    public:
        CustomLogWriterThread(const ACE_CString& name, CustomLoggerImpl *logger); 
        virtual ~CustomLogWriterThread();
        virtual void runLoop();
    private:
        CustomLoggerImpl *_logger;
};

#endif

