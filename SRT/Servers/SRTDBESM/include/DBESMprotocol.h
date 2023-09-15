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
        inline static string comm_clr_mode(string cfg_name){return Commands::m_clr_mode + " " + cfg_name + TAIL;}
        inline static string comm_get_status(string b_addr){return Commands::m_get_status + " " + "BOARD" + " " + b_addr;}
        inline static string comm_set_att(string b_addr, string out_ch, string att_val){return Commands::m_set_att + " " + out_ch + " BOARD " + b_addr + " VALUE " + att_val + TAIL;}
        inline static string comm_get_diag_all(){return Commands::m_get_diag_all + TAIL;}
        inline static string comm_get_diag(string b_addr){return Commands::m_get_diag + " " + "BOARD" + " " + b_addr;}
        inline static string comm_get_comp(string b_addr){return Commands::m_get_comp + " " + "BOARD" + " " + b_addr;}
        
    private:
        /*
        * Commands
        */
        struct Commands{
            inline static const string m_set_all {"DBE SETALLMODE"};
            inline static const string m_set_mode { "DBE MODE" };
            inline static const string m_store_allmode { "DBE STOREALLMODE" };
            inline static const string m_clr_mode { "DBE CLRMODE" };
            inline static const string m_get_status { "DBE GETSTATUS" };
            inline static const string m_set_att { "DBE SETATT" };
            inline static const string m_get_diag_all { "DBE ALLDIAG" };
            inline static const string m_get_diag { "DBE DIAG" };
            inline static const string m_get_comp { "DBE GETCOMP" };
    };    
}; //class CDBESMCommand

#endif