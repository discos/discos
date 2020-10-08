#include "ReceiverConfHandler.h"

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
    /* Conf - names */
    m_conf_name[CCC_Normal]= "CCCNormal";
    m_conf_name[CCC_Narrow]= "CCCNarrow";
    m_conf_name[CHC_Normal]= "chcNormal";
    m_conf_name[CHC_Narrow]= "chcNarrow";
    /* Filling partial conf definitions */
    m_receiver_name[CCC]= "CCC";
    m_receiver_name[CHC]= "CHC";
    m_mode_name[Normal]= "Normal";
    m_mode_name[Narrow]= "Narrow";
    /* Filling conf files information */
    /* CCC normal */    
    ConfigurationAccess l_ccc_normal_acc;
    l_ccc_normal_acc.m_name=  m_conf_name[CCC_Normal];
    l_ccc_normal_acc.m_name = CCC_Normal;
    l_ccc_normal_acc.m_conf_file_path= "DataBlock/MedicinaCBand/NormalModeSetup/CCC";
    l_ccc_normal_acc.m_noisemark_file_path= "DataBlock/MedicinaCBand/NoiseMark/CCC";
    m_conf_access[CCC_Normal].push_back(l_ccc_normal_acc);
    /* CCC narrow */    
    ConfigurationAccess l_ccc_narrow_acc;
    l_ccc_narrow_acc.m_name= m_conf_name[CCC_Narrow];
    l_ccc_narrow_acc.m_name = CCC_Narrow;
    l_ccc_narrow_acc.m_conf_file_path= "DataBlock/MedicinaCBand/NarrowModeSetup/CCC";
    l_ccc_narrow_acc.m_noisemark_file_path= "DataBlock/MedicinaCBand/NoiseMark/CCC";
    m_conf_access[CCC_Narrow].push_back(l_ccc_narrow_acc);
    /* CHC Normal */
    ConfigurationAccess l_chc_normal_acc;
    l_chc_normal_acc.m_name= m_conf_name[CHC_Normal];
    l_chc_normal_acc.m_name = CHC_Normal;
    l_chc_normal_acc.m_conf_file_path= "DataBlock/MedicinaCBand/NormalModeSetup/CHC";
    l_chc_normal_acc.m_noisemark_file_path= "DataBlock/MedicinaCBand/NoiseMark/CHC";
    m_conf_access[CHC_Normal].push_back(l_chc_normal_acc);
    /* CHC narrow */
    ConfigurationAccess l_chc_narrow_acc;
    l_chc_narrow_acc.m_name= m_conf_name[CHC_Narrow];
    l_chc_narrow_acc.m_name = CHC_Narrow;
    l_chc_narrow_acc.m_conf_file_path= "DataBlock/MedicinaCBand/NarrowModeSetup/CHC";
    l_chc_narrow_acc.m_noisemark_file_path= "DataBlock/MedicinaCBand/NoiseMark/CHC";
    m_conf_access[CHC_Narrow].push_back(l_chc_narrow_acc);
    /* Filling conf setup holders */
    std::vector<ConfigurationName>::const_iter l_it;
    for(l_it= m_allowed_conf.begin(); it != m_allowed_conf.end(); l_it++){
        m_conf_data.push_back(ConfigurationSetup());
    }
    /* Current confs composition */
    CurrentConfiguration l_ccc_normal;
    l_ccc_normal= CCC_Normal;
    l_ccc_normal.m_receiver= CCC;
    l_ccc_normal.m_mode= Normal;
    m_available_current.push_back(l_ccc_normal);
    CurrentConfiguration l_ccc_narrow;
    l_ccc_narrow.m_name= CCC_Narrow;
    l_ccc_narrow.m_receiver= CCC;
    l_ccc_narrow.m_mode= Narrow;
    m_available_current.push_back(l_ccc_narrow);
    CurrentConfiguration l_chc_normal;
    l_chc_normal.m_name= CHC_Normal;
    l_chc_normal.m_receiver= CHC;
    l_chc_normal.m_mode= Normal;
    m_available_current.push_back(l_chc_normal);
    CurrentConfiguration l_chc_narrow;
    l_chc_narrow.m_name= CHC_Narrow;
    l_chc_narrow.m_receiver= CHC;
    l_chc_narrow.m_mode= Narrow;
    m_available_current.push_back(l_chc_narrow);
}

ReceiverConfHandler::~ReceiverConfHandler()
{
    /* doing nothing at the moment */
}

/* *** SETTERS *** */

bool ReceiverConfHandler::setConfiguration(ConfigurationName p_conf_name){
    if (findConfiguration(p_conf_name)){
        m_current_conf= m_available_current[p_conf_name];
        return true;
    }
    return false;
}

bool ReceiverConfHandler::setConfiguration(IRA::CString p_conf_name){
    ConfigurationName l_conf_enum;
    if (findEnumFromString(p_conf_name, l_conf_enum)){
        m_current_conf= m_available_current[l_conf_enum];
        return true;
    }
    return false;
}

bool ReceiverConfHandler::setConfigurationSetup(ConfigurationName p_conf_name,
                                const ConfigurationSetup & p_setup)
{
    if (findConfiguration(p_conf_name)){
        m_conf_data[p_conf_name]= p_setup;
        return true;
    }    
    return false;
}

bool ReceiverConfHandler::setConfigurationReceiver(ReceiverName p_receiver_name)
{
    switch (p_receiver_name){
    case CCC:
        setConfiguration(CCC_Normal);
        return true;
        break;
    case CHC:
        setConfiguration(CHC_Normal);
        return true;
        break;    
    }
}

bool ReceiverConfHandler::setConfigurationReceiver(IRA::CString p_receiver_name)
{
    ReceiverName l_receiver_enum;
    bool l_ret= findReceiverFromString(p_receiver_name, l_receiver_enum);
    if (!l_ret)
        return false;
    switch (l_receiver_enum){
    case CCC:
        setConfiguration(CCC_Normal);
        return true;
        break;
    case CHC:
        setConfiguration(CHC_Normal);
        return true;
        break;    
    }
    return false;
}

bool ReceiverConfHandler::setMode(ModeName p_mode_name)
{        
    switch (p_mode_name){
        case Normal:
            if (m_current_conf.m_receiver == CCC){
                setConfiguration(CCC_Normal);
                return true;
            }
            if (m_current_conf.m_receiver == CHC){
                setConfiguration(CHC_Normal);
                return true;
            }
        break;           
        case Narrow:
            if (m_current_conf.m_receiver == CCC){
                setConfiguration(CCC_Narrow);
                return true;
            }
            if (m_current_conf.m_receiver == CHC){
                setConfiguration(CHC_Narrow);
                return true;
            }
        break;           
    }
    return false;
}

bool ReceiverConfHandler::setMode(ModeName p_mode_name)
{        
    ModeName l_mode_enum;
    bool l_ret= findModeFromString(p_mode_name, l_mode_enum);
    if (!l_ret)
        return false;    
    switch (l_mode_enum){
        case Normal:
            if (m_current_conf.m_receiver == CCC){
                setConfiguration(CCC_Normal);
                return true;
            }
            if (m_current_conf.m_receiver == CHC){
                setConfiguration(CHC_Normal);
                return true;
            }
        break;           
        case Narrow:
            if (m_current_conf.m_receiver == CCC){
                setConfiguration(CCC_Narrow);
                return true;
            }
            if (m_current_conf.m_receiver == CHC){
                setConfiguration(CHC_Narrow);
                return true;
            }
        break;           
    }    
    return false;
}

/* *** GETTERS *** */

const std::vector<ConfigurationName> ReceiverConfHandler::getAvailableConfs() const
{
    return m_allowed_conf;
}

const IRA::CString ReceiverConfHandler::getActualConfStr()
{
    ConfigurationName l_name_enum = m_current_conf.m_name;
    /* Setting default if no conf is found */
    if (!findConfiguration(l_name_enum)){
        setConfiguartion(CCC_Normal);
        return m_conf_name[CCC_Normal];
    }
    return m_conf_name[l_name_enum];    
}

ConfigurationSetup ReceiverConfHandler::getCurrentSetup() {
    ConfigurationName l_name_enum = m_current_conf.m_name;
    if (!findConfiguration(l_name_enum)){    
        setConfiguartion(CCC_Normal);
        return m_conf_data[CCC_Normal];
    }
    return m_conf_data[l_name_enum];
}

const std::map<ConfigurationName, IRA::CString> ReceiverConfHandler::getAvailableConfsMap() const
{
    return m_conf_name;
}

bool ReceiverConfHandler::getConfigurationAccess(ConfigurationName p_conf_name,
                                                 ConfigurationAccess & p_out_conf) const
{
    if (findConfiguration(p_conf_name)){
        p_out_conf= m_conf_access[p_conf_name];
        return true
    }
    return false;
}

bool ReceiverConfHandler::getConfigurationSetup(ConfigurationName p_conf_name,
                                ConfigurationSetup & p_out_setup) const
{
    if (findConfiguration(p_conf_name)){
        p_out_setup= m_conf_data[p_conf_name];
        return true
    }
    return false;
}


/* *** PRIVATE *** */

bool ReceiverConfHandler::findConfiguration(ConfigurationName p_conf_name) const
{
     std::vector<ConfigurationName>::iterator l_it;
    l_it= std::find(m_allowed_conf.begin(), m_allowed_conf.end(), p_conf_name);
    if (l_it != m_conf_access.end()){     
        return true
    }
    return false;
}

bool ReceiverConfHandler::findConfiguration(IRA::CString p_conf_name) const {
    std::map<ConfigurationName, IRA::CString>::const_iterator l_it;
    for ( l_it= m_conf_name.begin(); it != m_conf_name.end(); ++it ){
        if (l_it->second == p_conf_name ){
            return true;
        }
    }
    return false;
}

bool ReceiverConfHandler::findEnumFromString(IRA::CString p_conf_name,
                                 ConfigurationName & p_con_enum) const
{
    std::map<ConfigurationName, IRA::CString>::const_iterator l_it;
    for ( l_it= m_conf_name.begin(); l_it != m_conf_name.end(); ++l_it ){
        if (l_it->second == p_conf_name ){
            p_conf_enum= *l_it->first;
            return true;
        }
    }
    return false;
}

bool ReceiverConfHandler::findReceiverFromString(IRA::CString p_receiver_name,
                             ReceiverName & p_receiver_enum) const
{
    std::map<ReceiverName, IRA::CString>::const_itererator l_it;
    for (l_it= m_receiver_name.begin(); l_it != m_receiver_name.end(); ++l_it ){
        if (l_it->second == p_receiver_name){
            p_receiver_enum= *l_it->first;
            return true;
        }
    }
    return false;
}                            

bool ReceiverConfHandler::findModeFromString(IRA::CString p_mode_name,
                             ReceiverName & p_mode_enum) const
{
    std::map<ModeName, IRA::CString>::const_itererator l_it;
    for (l_it= m_mode_name.begin(); l_it != m_mode_name.end(); ++l_it ){
        if (l_it->second == p_mode_name){
            p_mode_enum= *l_it->first;
            return true;
        }
    }
    return false;
}                             