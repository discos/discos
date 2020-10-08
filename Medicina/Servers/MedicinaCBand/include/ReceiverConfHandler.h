#ifndef MED_C_BAND_RECEIVER_CONF_H
#define MED_C_BAND_RECEIVER_CONF_H

#include <IRA>
#include <map>
#include <vector>
#include "Defs.h"
#include "Commons.h"

/**
 * @brief Receiver configuration holder/helper
 * It keeps receiver configuration usefull datain one place
 */
struct ReceiverConfHandler{

    /**
     * @rief Allowed receiver configurations
     */
    typedef enum {
        CCC_Normal =0,
        CCC_Narrow,
        CHC_Normal,
        CHC_Narrow
    } ConfigurationName; 

    typedef enum{
        CCC=0,
        CHC=1
    } ReceiverName;

    typedef enum{
        Normal=0,
        Narrow
    }ModeName;

    /**
     * @brief Infos on current selected configuration
     */
    struct CurrentConfiguration{
        ConfigurationName m_name;
        ReceiverName m_receiver;
        ModeName m_mode;
    };

    /**
     * @brief Configuration file access infos
     */
    struct ConfigurationAccess{
        ConfigurationName m_name;     /**< Conf enum name */
        IRA::CString m_name_str;      /**< Conf string name */
        IRA::CString m_conf_file_path;/**< Conf data block file path */
        IRA::CString m_noisemark_file_path;/**< Noise mark data block file path */
    };

     /**
	 * @brief Configuration setup values
     * This is a receiver representation from configuraion point of view
	 */
	struct ConfigurationSetup{
		DWORD m_IFs;
		DWORD m_feeds;
        IRA::CString m_name;
		std::vector<Receivers::TPolarization> m_polarizations;		
		std::vector<double> m_RFMin;
		std::vector<double> m_RFMax;
		std::vector<double> m_IFMin;
		std::vector<double> m_IFBandwidth;			
		std::vector<double> m_defaultLO;
		std::vector<double> m_fixedLO2;
		std::vector<double> m_LOMin;
		std::vector<double> m_LOMax;
		std::vector<double> m_noise_mark_lcp_coeffs; /**< Noise mark polynomial coeff. left pol. */		
		std::vector<double> m_noise_mark_rcp_coeffs; /**< Noise mark polynomial coeff. right pol. */		
	};

    /**
     * @brief Construct a new Receiver Conf Handler object
     */
    ReceiverConfHandler();

    /**
     * @brief Destroy the Receiver Conf object     
     */
    ~ReceiverConfHandler();        

    /**
     * @brief Get the Instance to realize Singleton impl. 
     * @return Instance ( Singl. impl.)
     */
    static ReceiverConfHandler* getInstance();

    /**
     * @brief Set the Configuration from enum tag
     * @param p_conf_name Desired conf
     * @return true 
     * @return True if requested configuration is valid.
     */
    bool setConfiguration(ConfigurationName p_conf_name);

    /**
     * @brief Set the Configuration from string tag
     * @param p_conf_name Desired conf
     * @return true 
     * @return True if requested configuration is valid.
     */
    bool setConfiguration(IRA::Cstring p_conf_name);

    /**
     * @brief Set configuration starting from given receiver keeping default Normal as mode
     * 
     * @param p_receiver_name Requested receiver
     * @return true receiver correctly set    
     */
    bool setConfigurationReceiver(ReceiverName p_receiver_name);

    /**
     * @brief Set the Receiver keeping default Normal as mode (string tag)
     * 
     * @param p_receiver_name Requested receiver
     * @return true receiver correctly set    
     */
    bool setConfigurationReceiver(IRA::CString p_receiver_name);

    /**
     * @brief Set the Mode keeping actual receiver selected
     * 
     * @param p_mode_name Requeste mode
     * @return true Mode correctly set
     */
    bool setMode(ModeName p_mode_name);

    /**
     * @brief Set the Mode keeping actual receiver selected (string tag)
     * 
     * @param p_mode_name Requeste mode
     * @return true Mode correctly set
     */
    bool setMode(IRA::CString p_mode_name);

    /**
     * @brief Set Setup struct to requested configuration tag
     * 
     * @param p_conf_name Conf name
     * @param p_setup New Setup
     * @return True for valid operation
     */
    bool setConfigurationSetup(ConfigurationName p_conf_name,
                                const ConfigurationSetup & p_setup);

    /* *** GETTERs *** */

    /**
     * @brief Get the Available configuration by enum name
     * @return Available conf names
     */
    const std::vector<ConfigurationName> getAvailableConfs() const;

    /**
     * @brief Get the actual configuration by its string tag
     * @details Side-effect, set default in case of conf mismatch 
     * @return Conf string tag
     */
    const IRA::CString getActualConfStr();

    /**
     * @brief Get the Current configuration setup
     * 
     * @return ConfigurationSetup for selected working mode
     */
    ConfigurationSetup getCurrentSetup() const;    

    /**
     * @brief Get the Available configuration by enum name
     * @return Available conf names
     */
    const std::map<ConfigurationName, IRA::CString> getAvailableConfsMap() const;

    /**
     * @brief Get the Configuration infos needed to find conf files
     * @details Beware of reference output
     * @param[in] p_conf_name Conf name 
     * @param[out] p_out_conf Output holder
     * @return True if p_conf_name is present on access container
     */
    bool getConfigurationAccess(ConfigurationName p_conf_name,
                                ConfigurationAccess & p_out_conf) const;

    /**
     * @brief Get the requested configuration setup       
     * @details Beware of reference output
     * @param[in] p_conf_name Conf name
     * @param[out] p_out_setup Setup richiesto
     * @return True when p_output_setup is valid
     */
    bool getConfigurationSetup(ConfigurationName p_conf_name,
                                ConfigurationSetup & p_out_setup) const;


    private:

    /**
     * @brief Find if given conf enum tag is present
     * 
     * @param p_conf_name Tag to find
     * @return true If Conf is valid
     */
    bool findConfiguration(ConfigurationName p_conf_name) const ;

    /**
     * @brief Find if given conf string tag is present
     * 
     * @param p_conf_name Tag  to find
     * @return true If Conf is valid
     */
    bool findConfiguration(IRA::CString p_conf_name) const;

    /**
     * @brief 
     * 
     * @param p_conf_name String tag to find
     * @param[out] p_conf_enum Conf enum tag relative to p_conf_name
     * @return true Requested configuration is valid
     */
    bool findEnumFromString(IRA::CString p_conf_name,
                             ConfigurationName & p_con_enum) const;

    /**
     * @brief Find Receiver enum from string tag
     * 
     * @param p_receiver_name String tag receiver name
     * @param[out] p_receiver_enum Found receiver enum
     * @return true Receiver enum found
     */
    bool findReceiverFromString(IRA::CString p_receiver_name,
                             ReceiverName & p_receiver_enum) const;

    /**
     * @brief Find mode enum from string tag
     * 
     * @param p_mode_name String tag mode name
     * @param[out] p_mode_enum Found mode enum
     * @return true  Mode enum found
     */
    bool findModeFromString(IRA::CString p_mode_name,
                             ReceiverName & p_mode_enum) const;
                             

    private:

    std::vector<ConfigurationName> m_allowed_conf; /**< Allowed configuration registered */
    std::map<ConfigurationName, IRA::CString> m_conf_name; /**< Map configuration name - mnemonic string */
    std::map<ConfigurationName, ConfigurationAccess> m_conf_access; /**< Map configuration name - conf file access infos */
    std::map<ConfigurationName, ConfigurationSetup> m_conf_data; /**< Map configuration name - conf setup */
    
    std::map<ReceiverName, IRA::CString> m_receiver_name; /**< Map receiver name - mnemonic string */
    std::map<ModeName, IRA::CString> m_mode_name; /**< Map mode name - mnemonic string */
    
    std::map<ConfigurationName, CurrentConfiguration> m_available_current;  /**< Map configuration name - current conf setup */
    CurrentConfiguration m_current_conf;    /**< Currente selected conf */
    
};

#endif