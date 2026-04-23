template <class T>
CZMQConfiguration<T>::CZMQConfiguration()
{
}

template <class T>
CZMQConfiguration<T>::~CZMQConfiguration()
{
    if (m_table) {
        m_table->closeTable();
        delete m_table;
        m_table = NULL;
    }
}

/*
   throw (
   ComponentErrors::CDBAccessExImpl,
   ComponentErrors::MemoryAllocationExImpl,
*/
template <class T>
void CZMQConfiguration<T>::init(const T *Services)
{
    m_services = Services;
    IRA::CError error;
    IRA::CString field;
    IRA::CString buffer;
    try {
        m_table = new IRA::CDBTable(const_cast<const T*>(Services),"FlowRecord",CONFIG_PATH);
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CZMQConfiguration::init()");
        throw dummy;
    }
    error.Reset();
    if (!m_table->addField(error,"BackendName",IRA::CDataField::STRING)) {
        field="BackendName";
    }
    else if (!m_table->addField(error,"EndPoint",IRA::CDataField::STRING)) {
        field="Endpoint";
    }
    else if (!m_table->addField(error,"FlowNumber",IRA::CDataField::LONGLONG)) {
        field="Feed";
    }
    else if (!m_table->addField(error,"TCPNoDelay",IRA::CDataField::LONGLONG)) {
        field="TCPNoDelay";
    }
    else if (!m_table->addField(error,"CPUAffinity",IRA::CDataField::LONGLONG)) {
        field="CPUAffinity";
    }
    if (!error.isNoError()) {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
        dummy.setFieldName((const char *)field);
        throw dummy;
    }
    if (!m_table->openTable(error)) {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
        throw dummy;
    }
}

/*
   throw (
   ComponentErrors::CDBAccessExImpl,
*/
template <class T>
bulkdataZMQImpl::TZMQConfig  CZMQConfiguration<T>::getConfiguration(const CORBA::ULong& flowNumber)
{
    WORD len;
    TZMQConfig config;
    m_table->First();
    len = m_table->recordCount();
    bool found = false;
    for (WORD i = 0; i < len; i++) {
        if ((*m_table)["FlowNumber"]->asLongLong() == flowNumber) {
            config.backendName = (*m_table)["BackendName"]->asString();
            config.endPoint = (*m_table)["EndPoint"]->asString();
            config.flowNumber = (CORBA::ULong)(*m_table)["FlowNumber"]->asLongLong();
            config.tcpNoDelay = (*m_table)["TCPNoDelay"]->asLongLong();
            config.cpuAffinity = (*m_table)["CPUAffinity"]->asLongLong();
            found = true;
            break;
        }
        m_table->Next();
    }
    if (!found) {
        _EXCPT(ComponentErrors::CDBAccessExImpl, dummy, "CZMQConfiguration::getConfiguration()");
        dummy.setFieldName("FlowNumber");
        throw dummy;
    }
    return config;
}

/*
   throw (
   ComponentErrors::CDBAccessExImpl,
*/
template <class T>
bulkdataZMQImpl::TZMQConfig CZMQConfiguration<T>::getConfiguration(const IRA::CString& comp_name)
{
    WORD len;
    TZMQConfig config;
    m_table->First();
    len = m_table->recordCount();
    bool found = false;
    for (WORD i = 0; i < len; i++) {
        if ((*m_table)["BackendName"]->asString() == comp_name) {
            config.backendName = (*m_table)["BackendName"]->asString();
            config.endPoint = (*m_table)["EndPoint"]->asString();
            config.flowNumber = (CORBA::ULong)(*m_table)["FlowNumber"]->asLongLong();
            config.tcpNoDelay = (*m_table)["TCPNoDelay"]->asLongLong();
            config.cpuAffinity = (*m_table)["CPUAffinity"]->asLongLong();
            found = true;
            break;
        }
        m_table->Next();
    }
    if (!found) {
        _EXCPT(ComponentErrors::CDBAccessExImpl, dummy, "CZMQConfiguration::getConfiguration()");
        dummy.setFieldName("FlowNumber");
        throw dummy;
    }
    return config;
}
