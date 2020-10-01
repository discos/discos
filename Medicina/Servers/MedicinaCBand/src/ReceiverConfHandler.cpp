#include "ReceiverConfHandler.h"

/* ** SINGLETON ** */

ReceiverConfHandler* ReceiverConfHandler::getInstance(){
    if (!m_instance)
        m_instance= new ReceiverConfHandler();
    return m_instance;
}

/* ** CLASS PUBLIC CTOR ** */

ReceiverConfHandler::ReceiverConfHandler()
{    
    /* Before C++11 member initialization does not allow to do it in header 
        member declaration */
    /* Allowed confs */
    m_allowed_conf.push_back(CCC_Normal);
    m_allowed_conf.push_back(CCC_Narrow);
    m_allowed_conf.push_back(CHC_Normal);
    m_allowed_conf.push_back(CHC_Narrow);
    /* Filling conf setup holders */
    std::vector<ConfigurationName>::const_iter l_it;
    for(l_it= m_allowed_conf.begin(); it != m_allowed_conf.end(); l_it++){
        m_conf_data.push_back(ConfigurationSetup());
    }
    /* Filling conf files information */
    /* CCC normal */
    IRA::CString l_ccc_normal_str= "CCCNormalSetup";
    ConfigurationAccess l_ccc_normal_acc;
    l_ccc_normal_acc.m_name= l_ccc_normal_str;
    l_ccc_normal_acc.m_name = CCC_Normal;
    l_ccc_normal_acc.m_conf_file_path= "DataBlock/MedicinaCBand/NormalModeSetup/CCC";
    l_ccc_normal_acc.m_noisemark_file_path= "DataBlock/MedicinaCBand/NoiseMark/CCC";
    m_conf_access[CCC_Normal].push_back(l_ccc_normal_acc);
    /* CCC narrow */
    IRA::CString l_ccc_narrow_str= "CCCNarrowSetup";
    ConfigurationAccess l_ccc_narrow_acc;
    l_ccc_narrow_acc.m_name= l_ccc_narrow_str;
    l_ccc_narrow_acc.m_name = CCC_Narrow;
    l_ccc_narrow_acc.m_conf_file_path= "DataBlock/MedicinaCBand/narrowModeSetup/CCC";
    l_ccc_narrow_acc.m_noisemark_file_path= "DataBlock/MedicinaCBand/NoiseMark/CCC";
    m_conf_access[CCC_Narrow].push_back(l_ccc_narrow_acc);
    /* CHC Normal */
    IRA::CString l_chc_normal_str= "chcNormalSetup";
    ConfigurationAccess l_chc_normal_acc;
    l_chc_normal_acc.m_name= l_chc_normal_str;
    l_chc_normal_acc.m_name = CHC_Normal;
    l_chc_normal_acc.m_conf_file_path= "DataBlock/MedicinaCBand/NormalModeSetup/CHC";
    l_chc_normal_acc.m_noisemark_file_path= "DataBlock/MedicinaCBand/NoiseMark/CHC";
    m_conf_access[CHC_Normal].push_back(l_chc_normal_acc);
    /* CHC narrow */
    IRA::CString l_chc_narrow_str= "chcNarrowSetup";
    ConfigurationAccess l_chc_narrow_acc;
    l_chc_narrow_acc.m_name= l_chc_narrow_str;
    l_chc_narrow_acc.m_name = CHC_Narrow;
    l_chc_narrow_acc.m_conf_file_path= "DataBlock/MedicinaCBand/narrowModeSetup/CHC";
    l_chc_narrow_acc.m_noisemark_file_path= "DataBlock/MedicinaCBand/NoiseMark/CHC";
    m_conf_access[CHC_Narrow].push_back(l_chc_narrow_acc);
}

ReceiverConfHandler::~ReceiverConfHandler()
{
    /* doing nothing at the moment */
}

/* ** CONF GETTERS ** */

const std::vector<ConfigurationName> getAvailableConfs() const
{
    return m_allowed_conf;
}

bool ReceiverConfHandler::getConfigurationAccess(ConfigurationName p_conf_name,
                                                 ConfigurationAccess & p_out_conf) const
{
    std::map<ConfigurationName, ConfigurationAccess>::const_iter l_it;
    l_it= m_conf_access.find(p_conf_name);
    if (l_it != m_conf_access.end()){
        p_out_conf= l_it->second;
        return true
    }
    return false;
}

 bool   ReceiverConfHandler::getConfigurationSetup(ConfigurationName p_conf_name,
                                            ConfigurationSetup & p_out_setup) const
 {
    std::map<ConfigurationName, ConfigurationSetup>::const_iter l_it;
    l_it= m_conf_access.find(p_conf_name);
    if (l_it != m_conf_access.end()){
        p_out_conf= l_it->second;
        return true
    }
    return false;
 }

bool ReceiverConfHandler::setConfSetup(ConfigurationName p_conf_name,
                                ConfigurationSetup p_setup)
{
    std::map<ConfigurationName, ConfigurationSetup>::iter l_it;
    l_it= m_conf_access.find(p_conf_name);
    if (l_it != m_conf_access.end()){
        m_conf_data[p_conf_name]= p_setup;
        return true
    }
    return false;
}