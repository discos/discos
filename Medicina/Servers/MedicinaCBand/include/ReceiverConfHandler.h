#ifndef MED_C_BAND_RECEIVER_CONF_H
#define MED_C_BAND_RECEIVER_CONF_H

#include <IRA>
#include <map>
#include <vector>
#include "Defs.h"
#include "Commons.h"

/**
 * @brief Receiver configuration holder/helper
 * @details Singleton implementation
 * It keeps receiver configuration usefull datain one place
 */
struct ReceiverConfHandler{

    /**
     * @rief Allowed receiver configurations
     */
    typedef enum {
        CCC_Normal =0,
        CCC_Narrow,
        CHC_Normal ,
        CHC_Narrow
    } ConfigurationName; 

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
     * @brief Get the Available configuration by enum name
     * @return Available conf names
     */
    const std::vector<ConfigurationName> getAvailableConfs() const;

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

    /**
     * @brief Set Setup to requested conf
     * 
     * @param p_conf_name Conf name
     * @param p_setup New Setup
     * @return True for valid operation
     */
    bool setConfSetup(ConfigurationName p_conf_name,
                                ConfigurationSetup  p_setup);

    private:

    static ReceiverConfHandler* m_instance;  /**< Class instance for singleton impl */
    std::vector<ConfigurationName> m_allowed_conf; /**< Allowed configuration registered */
    std::map<ConfigurationName, ConfigurationAccess> m_conf_access; /**< Map configuration name - conf file access infos */
    std::map<ConfigurationName, ConfigurationSetup> m_conf_data; /**< Map configuration name - conf setup */
};

#endif