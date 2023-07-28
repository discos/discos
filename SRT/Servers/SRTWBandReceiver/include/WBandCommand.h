#ifndef W_BAND_COMMAND
#define W_BAND_COMMAND

#include <iostream>
#include <string>

using namespace std;



#define TAIL = "\r\n";

class CWBandCommand
{
    public:

        /**
         * Constructor
         */
        CWBandCommand( );
        /**
         * Destructor
         */
        virtual ~CWBandCommand( );
        
        inline static string comm_enable_USB_devs(){return Commands::m_enable_USB_devs;}
        inline static string comm_disable_USB_devs(){return Commands::m_disable_USB_devs;}
        inline static string comm_set_w_LO_freq_PolH(){return Commands::m_set_w_LO_freq_PolH;}
        inline static string comm_set_w_LO_freq_PolV(){return Commands::m_set_w_LO_freq_PolV;}
        inline static string comm_get_w_LO_PolH(){return Commands::m_get_w_LO_PolH;}
        inline static string comm_get_w_LO_PolV(){return Commands::m_get_w_LO_PolV;}
        inline static string comm_get_w_LO_Pols(){return Commands::m_get_w_LO_Pols;}
        inline static string comm_get_w_LO_Synths_Temp(){return Commands::m_get_w_LO_Synths_Temp;}
        inline static string comm_get_w_LO_HKP_Temp(){return Commands::m_get_w_LO_HKP_Temp;}
        inline static string comm_set_W_LO_RefH(){return Commands::m_set_W_LO_RefH;}
        inline static string comm_set_W_LO_RefV(){return Commands::m_set_W_LO_RefV;}
        inline static string comm_get_W_LO_RefH(){return Commands::m_get_W_LO_RefH;}
        inline static string comm_get_W_LO_RefV(){return Commands::m_get_W_LO_RefV;}
        inline static string comm_get_w_LO_status(){return Commands::m_get_w_LO_status;}
        inline static string comm_set_LO_att_PolH(){return Commands::m_set_LO_att_PolH;}
        inline static string comm_set_LO_att_PolV(){return Commands::m_set_LO_att_PolV;}
        inline static string comm_get_LO_att_PolH(){return Commands::m_get_LO_att_PolH;}
        inline static string comm_get_LO_att_PolV(){return Commands::m_get_LO_att_PolV;}
        inline static string comm_get_LO_atts(){return Commands::m_get_LO_atts;}
        inline static string comm_set_w_home(){return Commands::m_set_w_home;}
        inline static string comm_set_w_solar_attn(){return Commands::m_set_w_solar_attn;}
        inline static string comm_set_w_cal(){return Commands::m_set_w_cal;}
        inline static string comm_set_w_passthrough(){return Commands::m_set_w_passthrough;}
        inline static string comm_get_w_mod(){return Commands::m_get_w_mod;}
        inline static string comm_set_IF_switch_config(){return Commands::m_set_IF_switch_config;}
        inline static string comm_get_IF_switch_config(){return Commands::m_get_IF_switch_config;}
        
    private:
        /*
        * Commands
        */
        struct Commands{
            // w local oscillator
            inline static const string m_enable_USB_devs  {"enable USB_devs" };
            inline static const string m_disable_USB_devs { "disable USB_devs" };
            inline static const string m_set_w_LO_freq_PolH { "set W_LO_freq_PolH" };
            inline static const string m_set_w_LO_freq_PolV { "set W_LO_freq_PolV" };
            inline static const string m_get_w_LO_PolH { "get W_LO_PolH" };
            inline static const string m_get_w_LO_PolV { "get W_LO_PolV" };
            inline static const string m_get_w_LO_Pols { "get W_LO_Pols" };
            inline static const string m_get_w_LO_Synths_Temp { "get W_LO_Synths_Temp" };
            inline static const string m_get_w_LO_HKP_Temp { "get W_LO_HKP_Temp" };
            inline static const string m_set_W_LO_RefH { "set W_LO_RefH" };
            inline static const string m_set_W_LO_RefV { "set W_LO_RefV" };
            inline static const string m_get_W_LO_RefH { "get W_LO_RefH" };
            inline static const string m_get_W_LO_RefV { "get W_LO_RefV" };
            inline static const string m_get_w_LO_status { "get W_LO_status" };
            inline static const string m_set_LO_att_PolH { "set LO_att_PolH" };
            inline static const string m_set_LO_att_PolV { "set LO_att_PolV" };
            inline static const string m_get_LO_att_PolH { "get LO_att_PolH" };
            inline static const string m_get_LO_att_PolV { "get LO_att_PolV" };
            inline static const string m_get_LO_atts { "get LO_atts" };            
            // solar attenuator
            inline static const string m_set_w_home { "set W_home" };
            inline static const string m_set_w_solar_attn { "set W_solar_attn" };
            inline static const string m_set_w_cal { "set W_cal" };
            inline static const string m_set_w_passthrough { "set W_passthrough" };
            inline static const string m_get_w_mod { "get W_mode" };
            // switch matrix
            inline static const string m_set_IF_switch_config { "set IF_switch_config" };
            inline static const string m_get_IF_switch_config { "get IF_switch_config" };
    };    
}; //class CWBandCommand

#endif

