#ifndef DBESM_protocol
#define DBESM_protocol

#include <iostream>
#include <string>

using namespace std;

#define TAIL std::string("\r\n");

class CDBESMCommand
{
    public:

        /**
         * Constructor
         */
        CDBESMCommand();
        /**
         * Destructor
         */
        virtual ~CDBESMCommand();
              
        
        inline static string comm_set_allmode(string cfg_name){return Commands::m_set_all + " " + cfg_name + TAIL;}
        inline static string comm_set_mode(string b_addr, string cfg_name){return Commands::m_set_mode + " " + "BOARD" + " " + b_addr + " " + cfg_name + TAIL;}
        inline static string comm_store_allmode(string cfg_name){return Commands::m_store_allmode + " " + cfg_name + TAIL;}
        inline static string comm_delete_file(string cfg_name){return Commands::m_delete_file + " " + cfg_name + TAIL;}
        inline static string comm_get_status(string b_addr){return Commands::m_get_status + " BOARD " + b_addr + TAIL;}
        inline static string comm_set_att(string b_addr, string out_ch, string att_val){return Commands::m_set_att + " " + out_ch + " BOARD " + b_addr + " VALUE " + att_val + TAIL;}
       // inline static string comm_get_diag_all(){return Commands::m_get_diag_all + TAIL;}
        inline static string comm_get_diag(string b_addr){return Commands::m_get_diag + " BOARD " + b_addr + TAIL;}
        inline static string comm_get_comp(string b_addr){return Commands::m_get_comp + " BOARD " + b_addr + TAIL;}
        inline static string comm_get_cfg(){return Commands::m_get_cfg + TAIL;}
        inline static string comm_set_dbeatt(string out_dbe, string att_val){return Commands::m_set_dbeatt + " " + out_dbe + " " + att_val + TAIL;}
        inline static string comm_get_dbeatt(string out_dbe){return Commands::m_get_dbeatt + " " + out_dbe + TAIL;}
        inline static string comm_get_firm(string b_addr){return Commands::m_get_firm + " BOARD " + b_addr + TAIL;}
        inline static string comm_set_dbeamp(string out_dbe, string amp_val){return Commands::m_set_dbeamp + " " + out_dbe + " " + amp_val + TAIL;}
        inline static string comm_get_dbeamp(string out_dbe){return Commands::m_get_dbeamp + " " + out_dbe + TAIL;}
        inline static string comm_set_dbeeq(string out_dbe, string eq_val){return Commands::m_set_dbeeq + " " + out_dbe + " " + eq_val + TAIL;}
        inline static string comm_get_dbeeq(string out_dbe){return Commands::m_get_dbeeq + " " + out_dbe + TAIL;}
        inline static string comm_set_dbebpf(string out_dbe, string bpf_val){return Commands::m_set_dbebpf + " " + out_dbe + " " + bpf_val + TAIL;}
        inline static string comm_get_dbebpf(string out_dbe){return Commands::m_get_dbebpf + " " + out_dbe + TAIL;}
        
    private:
        /*
        * Commandsf
        */
        struct Commands{
            inline static const string m_set_all {"DBE SETALLMODE"};
            inline static const string m_set_mode { "DBE MODE" };
            inline static const string m_store_allmode { "DBE STOREALLMODE" };
            inline static const string m_delete_file { "DBE DELETEFILE" };
            inline static const string m_get_status { "DBE GETSTATUS" };
            inline static const string m_set_att { "DBE SETATT" };
            //inline static const string m_get_diag_all { "DBE ReadALLDIAG" };
            inline static const string m_get_diag { "DBE ReadDIAG" };
            inline static const string m_get_comp { "DBE GETCOMP" };
            inline static const string m_get_cfg { "DBE GETCFG" };
            inline static const string m_set_dbeatt { "DBE SETDBEATT" };
            inline static const string m_get_dbeatt { "DBE GETDBEATT" };
            inline static const string m_get_firm { "DBE GETFIRM" };
            inline static const string m_set_dbeamp { "DBE SETDBEAMP" };
            inline static const string m_get_dbeamp { "DBE GETDBEAMP" };
            inline static const string m_set_dbeeq { "DBE SETDBEEQ" };
            inline static const string m_get_dbeeq { "DBE GETDBEEQ" };
            inline static const string m_set_dbebpf { "DBE SETDBEBPF" };
            inline static const string m_get_dbebpf { "DBE GETDBEBPF" };
    };    
}; //class CDBESMCommand

#endif