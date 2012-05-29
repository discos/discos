#include <baciDB.h>
#include <CustomLoggerImpl.h>

CustomLoggerImpl::CustomLoggerImpl(const ACE_CString& CompName,
                                   maci::ContainerServices* containerServices):
    CharacteristicComponentImpl(CompName, containerServices),
    m_filename_sp(this),
    m_nevents_sp(this),
    m_min_level_sp(this),
    m_max_level_sp(this),
    m_isLogging_sp(this)
{   
    m_filename_sp = new baci::ROstring(
                                            CompName + ":filename",
                                            getComponent()
                                            );
    m_nevents_sp = new baci::ROlong(
                                            CompName + ":nevents",
                                            getComponent()
                                            );
    m_min_level_sp = new ROEnumImpl<ACS_ENUM_T(LogLevel), POA_Management::ROLogLevel>(
                                            getContainerServices()->getName() + ":minLevel",
                                            getComponent()
                                            );
    m_max_level_sp = new ROEnumImpl<ACS_ENUM_T(LogLevel), POA_Management::ROLogLevel>(
                                            CompName + ":maxLevel",
                                            getComponent()
                                            );
    m_isLogging_sp = new ROEnumImpl<ACS_ENUM_T(TBoolean), POA_Management::ROTBoolean>(
                                            CompName + ":isLogging",
                                            getComponent()
                                            );
}

CustomLoggerImpl::~CustomLoggerImpl(){}

ACS::ROstring_ptr CustomLoggerImpl::filename() throw (CORBA::SystemException)
{   
    if (m_filename_sp==0)
        return ACS::ROstring::_nil();
    ACS::ROstring_var prop = ACS::ROstring::_narrow(
                                     m_filename_sp->getCORBAReference()
                                     );
    return prop._retn();
}

ACS::ROlong_ptr CustomLoggerImpl::nevents() throw (CORBA::SystemException)
{   
    if (m_nevents_sp==0)
        return ACS::ROlong::_nil();
    ACS::ROlong_var prop = ACS::ROlong::_narrow(
                                     m_nevents_sp->getCORBAReference()
                                     );
    return prop._retn();
}

ROLogLevel_ptr 
CustomLoggerImpl::minLevel() throw (CORBA::SystemException)
{   
    if (m_min_level_sp==0)
        return ROLogLevel::_nil();
    ROLogLevel_var prop = ROLogLevel::_narrow(
                                     m_min_level_sp->getCORBAReference()
                                     );
    return prop._retn();
}

ROLogLevel_ptr 
CustomLoggerImpl::maxLevel() throw (CORBA::SystemException)
{   
    if (m_max_level_sp==0)
        return ROLogLevel::_nil();
    ROLogLevel_var prop = ROLogLevel::_narrow(
                                     m_max_level_sp->getCORBAReference()
                                     );
    return prop._retn();
}

ROTBoolean_ptr 
CustomLoggerImpl::isLogging() throw (CORBA::SystemException)
{   
    if (m_isLogging_sp==0)
        return ROTBoolean::_nil();
    ROTBoolean_var prop = ROTBoolean::_narrow(
                                     m_isLogging_sp->getCORBAReference()
                                     );
    return prop._retn();
}

void
CustomLoggerImpl::setMinLevel(LogLevel level) throw (CORBA::SystemException)
{
    ACS::Time ts;
    m_min_level_sp->getDevIO()->write(level, ts);
    if((m_max_level_sp->getDevIO()->read(ts)) < level)
        m_max_level_sp->getDevIO()->write(level, ts);
};

void
CustomLoggerImpl::setMaxLevel(LogLevel level) throw (CORBA::SystemException)
{
    ACS::Time ts;
    m_max_level_sp->getDevIO()->write(level, ts);
    if((m_min_level_sp->getDevIO()->read(ts)) > level)
        m_min_level_sp->getDevIO()->write(level, ts);
};

/*
* Resolve the naming service and subscribe to the logging channel
*/
void 
CustomLoggerImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
    ACS::Time _timestamp;
    setLogging(false);
    CosNaming::Name name(1);
    name.length(1);
    name[0].id = CORBA::string_dup(CUSTOM_LOGGING_CHANNEL);
    name[0].kind = acscommon::LOGGING_CHANNEL_KIND;
    ifgop_ = CosNotifyChannelAdmin::OR_OP;
    m_nevents_sp->getDevIO()->write((long)0, _timestamp);
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : resolving name service"));
    namingContext_m = maci::ContainerImpl::getContainer()->getService<CosNaming::NamingContext>(acscommon::NAMING_SERVICE_NAME, 0, true); 
    if(CORBA::is_nil(namingContext_m))
    {
            ACS_SHORT_LOG((LM_ERROR, "CutomLoggerImpl unable to resolve name service"));
            ACSErrTypeCommon::CORBAProblemExImpl err = ACSErrTypeCommon::CORBAProblemExImpl(__FILE__,__LINE__,"CustomLoggerImpl::initialize");
            throw err.getCORBAProblemEx();
    }
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : resolving logging channel name"));
    CORBA::Object_var obj = namingContext_m -> resolve(name);  
//TODO: obj != NULL
    ec_ = CosNotifyChannelAdmin::EventChannel::_narrow(obj.in());
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : subscribing to the logging channel"));
    CosNotifyChannelAdmin::AdminID adminid;
    consumer_admin_ = ec_->new_for_consumers(ifgop_, adminid);
    ACE_ASSERT(!CORBA::is_nil(consumer_admin_.in()));
    consumer_ = new CustomStructuredPushConsumer (this);
    consumer_->connect (consumer_admin_.in());
    CosNotification::EventTypeSeq added(1);
    CosNotification::EventTypeSeq removed (0);
    added.length (1);
    removed.length (0);
    added[0].domain_name =  CORBA::string_dup ("*");
    added[0].type_name = CORBA::string_dup ("*");
    consumer_admin_->subscription_change (added, removed);
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : Initializing EXPAT for xml parsing"));
    log_parser = init_log_parsing();

    /* LOGGING INITIALIZATION
     ========================*/
     IRA::CString _c_path, _c_file, _a_path, _a_file;
     if(
	IRA::CIRATools::getDBValue(getContainerServices(), "DefaultACSLogDir", _a_path) &&
	IRA::CIRATools::getDBValue(getContainerServices(), "DefaultACSLogFile", _a_file) &&
	IRA::CIRATools::getDBValue(getContainerServices(), "DefaultCustomLogDir", _c_path) &&
	IRA::CIRATools::getDBValue(getContainerServices(), "DefaultCustomLogFile", _c_file)
     ){
	setLogfile(_c_path, _a_path, _c_file, _a_file);
     }
     //TODO: else ERROR
     setMinLevel(C_TRACE);
     setMaxLevel(C_EMERGENCY);
};

void 
CustomLoggerImpl::cleanUp()
{
    closeLogfile();
    free_log_parsing(log_parser);
    CosNotification::EventTypeSeq added(0);
    CosNotification::EventTypeSeq removed (1);
    added.length (0);
    removed.length (1);
    removed[0].domain_name =  CORBA::string_dup ("*");
    removed[0].type_name = CORBA::string_dup ("*");
    consumer_admin_->subscription_change (added, removed);
    added.length(0);
    removed.length(0);
    consumer_->disconnect ();
    consumer_admin_->destroy();
};

/*
Used for debugging purpose, emits a custom log event with the given message and level
@param msg: the log message.
@param level: the CustomLogger::LogLevel level
*/
void
CustomLoggerImpl::emitLog(const char *msg, LogLevel level) throw (CORBA::SystemException)
{
    CUSTOM_LOG(LM_FULL_INFO, "CustomLoggerImpl::emit_log", (IRA::CustomLoggerUtils::custom2aceLogLevel(level), msg));
};

/*
* Flushes the loggingProxy of this container in order to immediatly
* get all the queued messages
*/
void
CustomLoggerImpl::flush() throw (CORBA::SystemException)
{
    maci::ContainerImpl::getLoggerProxy()->flush();
    if(checkLogging())
    {
        _custom_log.flush();
        _full_log.flush();
    }
};

void 
CustomLoggerImpl::closeLogfile() throw (CORBA::SystemException, ManagementErrors::CustomLoggerIOErrorEx)
{
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : closing logfile"));
    if(checkLogging())
    {
        setLogging(false);
        flush();
        _custom_log.clear();
        _custom_log.close();
        if(_custom_log.rdstate() == std::ofstream::failbit)
        {
            _EXCPT(ManagementErrors::CustomLoggerIOErrorExImpl, dummy, "CustomLoggerImpl::closeLogfile"); 
            dummy.setReason("error closing custom logfile");
            dummy.log(LM_DEBUG);
            throw dummy.getManagementErrorsEx();
	}
        _full_log.clear();
        _full_log.close();
        if(_full_log.rdstate() == std::ofstream::failbit)
        {
            _EXCPT(ManagementErrors::CustomLoggerIOErrorExImpl, _dummy, "CustomLoggerImpl::closeLogfile"); 
            _dummy.setReason("error closing acs logfile");
            _dummy.log(LM_DEBUG);
            throw _dummy.getManagementErrorsEx();
	}
    }
};

void 
CustomLoggerImpl::setLogfile(const char *base_path_log, const char *base_path_full_log,
                             const char *filename_log, const char *filename_full_log) 
                             throw (CORBA::SystemException, ManagementErrors::CustomLoggerIOErrorEx)
{
    closeLogfile();
    ACS::Time ts;
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : custom log directory: %s", base_path_log));
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : acs log directory: %s", base_path_full_log));
    if((mkdir(base_path_log, S_IRWXO|S_IRWXG|S_IRWXU) != 0) && (errno != EEXIST)){
        _EXCPT(ManagementErrors::CustomLoggerIOErrorExImpl, dummy, "CustomLoggerImpl::setLogfile"); 
        dummy.setReason("cannot create custom log directory");
        dummy.setPath(base_path_log);
        dummy.log(LM_DEBUG);
        throw dummy.getManagementErrorsEx();
    }
    if((mkdir(base_path_full_log, S_IRWXO|S_IRWXG|S_IRWXU) != 0) && (errno != EEXIST)){
        _EXCPT(ManagementErrors::CustomLoggerIOErrorExImpl, _dummy, "CustomLoggerImpl::setLogfile"); 
        _dummy.setReason("cannot create acs log directory");
        _dummy.setPath(base_path_full_log);
        _dummy.log(LM_DEBUG);
        throw _dummy.getManagementErrorsEx();
    }
    /*
     * Here logging dirs have been created
     */
    std::string base_path(base_path_log);
    if(base_path.at(base_path.size() - 1) != '/')
       base_path.append("/");
    base_path.append(filename_log);
    std::string custom_path(base_path);
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : custom log file: %s", custom_path.c_str()));
    std::string full_path(base_path_full_log);
    if(full_path.at(full_path.size() - 1) != '/')
       full_path.append("/");
    full_path.append(filename_full_log);
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : full log file: %s", full_path.c_str()));
    _custom_log.clear();
    _full_log.clear();
    _custom_log.open(custom_path.c_str(), std::ofstream::app);
    if(_custom_log.rdstate() != std::ofstream::goodbit){
        _EXCPT(ManagementErrors::CustomLoggerIOErrorExImpl, __dummy, "CustomLoggerImpl::setLogfile"); 
        __dummy.setReason("cannot open custom log file");
        __dummy.setPath(custom_path.c_str());
        __dummy.log(LM_DEBUG);
        throw __dummy.getManagementErrorsEx();
    }
    _full_log.open(full_path.c_str(), std::ofstream::app);
    if(_full_log.rdstate() != std::ofstream::goodbit){
        _EXCPT(ManagementErrors::CustomLoggerIOErrorExImpl, ___dummy, "CustomLoggerImpl::setLogfile"); 
        ___dummy.setReason("cannot open acs log file");
        ___dummy.setPath(full_path.c_str());
        ___dummy.log(LM_DEBUG);
        throw ___dummy.getManagementErrorsEx();
    }
    /*
     * Here logging files have been opened
     */
    setLogging(true);
    m_filename_sp->getDevIO()->write(custom_path.c_str(), ts);
    ACS_SHORT_LOG((LM_DEBUG, "CutomLoggerImpl : logging"));
};

/*
* Filter the log messages,
* this filter shuold return true for all the messages produced by ESCS and false for every ACS 
* message.
* @return True if the log record must be processed further; False if the log has to be discarded 
*/
bool 
CustomLoggerImpl::filter(LogRecord& log_record)
{
    std::string result;
    ACS::Time ts;
    bool filtered=false;
    if((log_record._finished) && //check if we are logging
       // check if the log level is accepted by our logger
       (m_min_level_sp->getDevIO()->read(ts) <= log_record.log_level) && 
       (m_max_level_sp->getDevIO()->read(ts) >= log_record.log_level))
    {
        if(!log_record.kwargs.empty())
        {
            result = log_record.get_data(std::string(CUSTOM_LOGGING_DATA_NAME));
            if(result == CUSTOM_LOGGING_DATA_VALUE)
                filtered = true;
        }
    }
    return filtered;
};

void
CustomLoggerImpl::setLogging(bool val)
{
    ACS::Time ts;
    if(val)
	m_isLogging_sp->getDevIO()->write(MNG_TRUE, ts);
    else
	m_isLogging_sp->getDevIO()->write(MNG_FALSE, ts);
};

bool
CustomLoggerImpl::checkLogging()
{
    ACS::Time ts;
    if(m_isLogging_sp->getDevIO()->read(ts) == MNG_TRUE)
        return true;
    else
	return false;
};

/*
* Handle a log record. 
*/
void 
CustomLoggerImpl::handle(boost::shared_ptr<LogRecord> log_record)
{
    ACS::Time ts;
    long _nevents;
    if(checkLogging())
    {
        _full_log << log_record->xml_text << '\n';
        if(filter(*log_record))
        {
            _nevents = m_nevents_sp->getDevIO()->read(ts);
            _custom_log << log_to_string(*log_record) << '\n';
            m_nevents_sp->getDevIO()->write(_nevents + 1, ts);
        };
    }
};

CustomStructuredPushConsumer::CustomStructuredPushConsumer(CustomLoggerImpl* logger) : 
    logger_(logger)
{
};

CustomStructuredPushConsumer::~CustomStructuredPushConsumer()
{
};

void 
CustomStructuredPushConsumer::connect(CosNotifyChannelAdmin::ConsumerAdmin_ptr consumer_admin)
{
    CosNotifyComm::StructuredPushConsumer_var objref = _this();
    CosNotifyChannelAdmin::ProxySupplier_var proxysupplier =
        consumer_admin->obtain_notification_push_supplier (CosNotifyChannelAdmin::STRUCTURED_EVENT, proxy_supplier_id_);
    ACE_ASSERT (!CORBA::is_nil (proxysupplier.in ()));
    proxy_supplier_ = CosNotifyChannelAdmin::StructuredProxyPushSupplier::_narrow (proxysupplier.in ());
    ACE_ASSERT (!CORBA::is_nil (proxy_supplier_.in ()));
    proxy_supplier_->connect_structured_push_consumer (objref.in ());
};

void 
CustomStructuredPushConsumer::disconnect()
{
    proxy_supplier_->disconnect_structured_push_supplier();
};

void 
CustomStructuredPushConsumer::disconnect_structured_push_consumer ()
{
  // No-Op.
};

void
CustomStructuredPushConsumer::offer_change (const CosNotification::EventTypeSeq & /*added*/,
					 const CosNotification::EventTypeSeq & /*removed*/)
{
  // No-Op.
};

CosNotifyChannelAdmin::StructuredProxyPushSupplier_ptr
CustomStructuredPushConsumer::get_proxy_supplier (void)
{
  return proxy_supplier_.in ();
};

void CustomStructuredPushConsumer::push_structured_event (const CosNotification::StructuredEvent & notification)
{
    const char *domain_name = notification.header.fixed_header.event_type.domain_name;
    const char *type_name = notification.header.fixed_header.event_type.type_name;
    const char *xmlLog;
    notification.remainder_of_body >>= xmlLog;
    Logging::XmlLogRecordSeq *reclist;
    notification.remainder_of_body >>= reclist;
    boost::shared_ptr<LogRecord> lr;
    if(xmlLog)
    {
        //parse the xml to a LogRecord
        lr = get_log_record(logger_->log_parser, xmlLog);
        //handle the log record and the xml
        logger_->handle(lr);
    }
    else if (reclist)
        for(unsigned int i = 0; i < reclist->length(); i++)
        {
            xmlLog = (*reclist)[i].xml;
            lr = get_log_record(logger_->log_parser, xmlLog);
            logger_->handle(lr);
        }
};

#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS (CustomLoggerImpl)

