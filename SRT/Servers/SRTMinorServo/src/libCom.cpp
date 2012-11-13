/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include "libCom.h"
#include <limits>
#include <string>
#include <bitset>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cstring>
#include <baciDB.h>
#include <ManagmentDefinitionsS.h>
#include <iostream>
#include "macros.def"
#include <cctype>


string make_request(
        const unsigned int cmd_idx, 
        const CDBParameters *const cdb_ptr, 
        CSecureArea<unsigned int> *cmd_num,
        const int slave,
        const int index,
        const int sub_index,
        const ACS::Time exe_time,
        const ACS::doubleSeq *positions
        ) throw (ComponentErrors::SocketErrorExImpl)
{
    // The cmd_number increment heve to be an atomic operation
    CSecAreaResourceWrapper<unsigned int> secure_cmd_num = cmd_num->Get();
    unsigned long icmd_num = ++(*secure_cmd_num);
    secure_cmd_num.Release();

    string cmd_number = number2str(icmd_num);
    string app_number = number2str(cdb_ptr->SERVO_ADDRESS);
    string slave_str = number2str(slave);
    string index_str = number2str(index);
    string sub_index_str = number2str(sub_index);
    string exe_time_str = number2str(exe_time);
    string request;

    switch(cmd_idx) {
        // getpos (data channel, high speed)
        case 0: {
            // The syntax: "#getpos:<cmd_number>=<app_number>\r"
            request = req_header + commands[cmd_idx] + ":" + cmd_number + "=" + app_number + message_closer;
            break;
        }

        // getspar (parameters channel, low speed)
        case 1: {
            // The syntax: "#getspar:<cmd_number>=<app_number>,<slave>,<index>,<sub_index>\r"
            request = req_header + commands[cmd_idx] + ":" + cmd_number + "=" + app_number + 
                      "," + slave_str + "," + index_str + "," + sub_index_str + message_closer;
            break;
        }

        // setpos (data channel, high speed)
        case 2: {
            // If the number of positions is unexpected raise an exception
            if(positions->length() != cdb_ptr->NUMBER_OF_AXIS)
                THROW_EX(ComponentErrors, SocketErrorEx, "Received a wrong number of positions.", false);

            // Convert from virtual to real positions
            ACS::doubleSeq positions_seq = *positions;
            if(cdb_ptr->VIRTUAL_RS == 1)
                virtual2real(positions_seq, cdb_ptr->SERVO_ADDRESS);

            // The syntax: "#setpos:<cmd_number>=<app_number>,<time>,<no slave>,<exe_mode>,<pos1>,...,<posN>\r"
            request = req_header + commands[cmd_idx] + ":" + cmd_number + "=" + app_number + "," + exe_time_str;
            request += ",0,0";
            
            for(size_t idx=0; idx != positions_seq.length(); ++idx)
                request += "," + number2str(positions_seq[idx]);

            request += message_closer;
            break;
        }

        // setup (data channel, high speed)
        case 3: {
            // The syntax: "#setup:<cmd_number>=<app_number>,<time>\r"
            request = req_header + commands[cmd_idx] + ":" + cmd_number + "=" + app_number + "," + exe_time_str;
            request += message_closer;
            break;
        }

        // stow (data channel, high speed)
        case 4: {
            // The syntax: "#stow:<cmd_number>=<app_number>,<time>\r"
            request = req_header + commands[cmd_idx] + ":" + cmd_number + "=" + app_number + "," + exe_time_str;
            request += message_closer;
            break;
        }

        // calibrate (data channel, high speed)
        case 5: {
            // The syntax: "#calibrate:<cmd_number>=<app_number>,<time>\r"
            request = req_header + commands[cmd_idx] + ":" + cmd_number + "=" + app_number + "," + exe_time_str;
            request += message_closer;
            break;
        }
               
        // getappstatus (data channel, high speed)
        case 6: {
            // The syntax: "#getappstatus:<cmd_number>=<app_number>\r"
            request = req_header + commands[cmd_idx] + ":" + cmd_number + "=" + app_number + message_closer;
            break;
        }
               
        // getstatus (data channel, high speed)
        case 7: {
            // The syntax: "#getstatus:<cmd_number>=<app_number>\r"
            request = req_header + commands[cmd_idx] + ":" + cmd_number + "=" + app_number + message_closer;
            break;
        }
                
        // clean (data channel, high speed)
        case 8: {
            // The syntax: "#clean:<cmd_number>=<app_number>\r"
            request = req_header + commands[cmd_idx] + ":" + cmd_number + "=" + app_number + message_closer;
            break;
        }

        default:
            request = "";
    }
    return request;
}


void verify(string answer) throw (ComponentErrors::SocketErrorExImpl) {

    if(startswith(answer, nak_header))
        THROW_EX(ComponentErrors, SocketErrorEx, "Nak answer: " + answer, false);

    unsigned short cmd_len = sizeof(commands) / sizeof(string);
    bool found = false;
    for(size_t idx = 0; idx != cmd_len; idx++)
        if(answer.find(commands[idx]) != string::npos) {
            found = true;
            break;
        }
    if(!found)
        THROW_EX(ComponentErrors, SocketErrorEx, "Command not found. Answer was: " + answer, false);
}


void process(
        string answer, 
        ACS::doubleSeq *data, 
        unsigned long *udata,
        StatusParameters *status_par, 
        const unsigned int cmd_idx, 
        const CDBParameters *const cdb_ptr,
        ACS::Time &timestamp,
        const int slave
   ) throw (ComponentErrors::SocketErrorExImpl) 
{
   istringstream istm;
   istm.precision(__UTILS_STREAM_PREC__);
   try {

       // ``sections`` are the two sections of the answer: 
       //     
       //     ["request", "time, data_1, ..., data_N"]
       //
       // "request" is the message sent to MinorServo and the the second 
       // item is a string of all parameters.
       vector<string> sections = split(answer, answer_separators);

       // ``parameters`` is a vector of strings containing all parameters.
       vector<string> parameters = split(sections[1], parameter_separators);

       // Remove blank spaces at the end and at the beginning of every parameter
       for (vector<string>::size_type idx = 0; idx != parameters.size(); ++idx)
           strip(parameters[idx]);

       istm.clear ();
       istm.str("");
       istm.str(parameters[0]);
       istm >> timestamp;

       switch(cmd_idx) {
           case 0: {
               // Erase the element of index 0
               parameters.erase(parameters.begin());
               // If the number of positions retrieved is that we expect, assign the positions
               if (parameters.size() == cdb_ptr->NUMBER_OF_AXIS) {

                   // Set the length of DoubleSeq
                   data->length(cdb_ptr->NUMBER_OF_AXIS);
                   for (vector<string>::size_type idx = 0; idx != parameters.size(); ++idx) {
                       // Clear the stream
                       istm.clear();
                       istm.str("");
                       // Insert the parameter in the stream
                       istm.str(parameters[idx]);
                       // Redirect the stream into ``data[idx]`` (#idx item of DoubleSeq)
                       istm >> (*data)[idx];
                   }

                   if(cdb_ptr->VIRTUAL_RS == 1)
                       real2virtual(*data, cdb_ptr->SERVO_ADDRESS);
               }
               else
                   // If the number of positions is unexpected raise an exception
                   THROW_EX(ComponentErrors, SocketErrorEx, "Received a wrong number of positions.", false);
               break;
           }

           case 1: {
               // If the slave has index different from 0 (not PLC)
               if(slave) {
                   // Set the length of DoubleSeq (PLC not included)
                   data->length(cdb_ptr->NUMBER_OF_SLAVES - 1);
                   // Clear the stream
                   istm.clear();
                   istm.str("");
                   // Insert the parameter in the stream
                   istm.str(parameters[0]);
                   // Redirect the stream into ``data[slave - 1]`` (#idx item of DoubleSeq)
                   istm >> (*data)[slave - 1];
               }
               // If the slave is PLC
               else {
                   // Set the length of DoubleSeq
                   data->length(1);
                   // Clear the stream
                   istm.clear();
                   istm.str("");
                   // Insert the parameter in the stream
                   istm.str(parameters[0]);
                   // Redirect the stream into ``data[slave - 1]`` (#idx item of DoubleSeq)
                   istm >> (*data)[0];
               }

               break;
           }

           case 2: case 3: case 4: case 5: {
               break;
           }

           // status
           case 6: {
               // Clear the stream
               istm.clear();
               istm.str("");
               // Insert the parameter in the stream
               istm.str(parameters[0]);
               // Redirect the stream into ``data[slave - 1]`` (#idx item of DoubleSeq)
               *udata = strtol((istm.str()).c_str(), NULL, 16);
               break;
           }

           // getstatus
           case 7: {
               (status_par->actual_pos).exe_time = timestamp;
               (status_par->actual_elongation).exe_time = timestamp;
               // Erase the element of index 0
               parameters.erase(parameters.begin());
               status_par->appState = str2int(parameters[0]);
               parameters.erase(parameters.begin());
               istm.clear();
               istm.str("");
               // Insert the parameter in the stream
               istm.str(parameters[0]);
               status_par->appStatus = strtol((istm.str()).c_str(), NULL, 16);
               parameters.erase(parameters.begin());
               status_par->cabState = str2int(parameters[0]);
               parameters.erase(parameters.begin());

               // If the number of positions retrieved is that we expect, assign the positions
               if (parameters.size() == cdb_ptr->NUMBER_OF_AXIS) {

                   // Set the length of DoubleSeq
                   (status_par->actual_pos).position.length(cdb_ptr->NUMBER_OF_AXIS);
                   (status_par->actual_elongation).position.length(cdb_ptr->NUMBER_OF_AXIS);
                   for (vector<string>::size_type idx = 0; idx != parameters.size(); ++idx) {
                       // Clear the stream
                       istm.clear();
                       istm.str("");
                       // Insert the parameter in the stream
                       istm.str(parameters[idx]);
                       // Redirect the stream into ``position[idx]`` (#idx item of DoubleSeq)
                       istm >> ((status_par->actual_pos).position)[idx];
                   }
                   for (vector<string>::size_type idx = 0; idx != parameters.size(); ++idx) {
                       ((status_par->actual_elongation).position)[idx] = ((status_par->actual_pos).position)[idx];
                   }

                   if(cdb_ptr->VIRTUAL_RS == 1)
                       real2virtual((status_par->actual_pos).position, cdb_ptr->SERVO_ADDRESS);
               }
               else
                   // If the number of positions is unexpected raise an exception
                   THROW_EX(ComponentErrors, SocketErrorEx, "Received a wrong number of positions.", false);
               break;
           }

           default: 
               // If the command does not exist raise an exception
               THROW_EX(ComponentErrors, SocketErrorEx, "Command not found!", false);
       }
   }
   catch (ComponentErrors::SocketErrorExImpl &ex)
       THROW_EX(ComponentErrors, SocketErrorEx, ("Wrong response. Answer was:\n" + answer).c_str(), false);
}


unsigned long get_request_id(const string message) 
{

    vector<string> tmp;
    string cmd_num, inv_cmd_num;
    tmp = split(message, reqid_separators);
    for(string::reverse_iterator iter = tmp[0].rbegin(); iter != tmp[0].rend(); ++iter)
        if(isdigit(*iter))
            inv_cmd_num += *iter;
        else
           break;
    for(string::reverse_iterator iter = inv_cmd_num.rbegin(); iter != inv_cmd_num.rend(); ++iter)
        cmd_num += *iter;
   
    return str2int(cmd_num);
}

unsigned short get_msaddrs(const string message) 
{

    vector<string> tmp;
    string saddr;
    tmp = split(message, reqid_separators);
    for(string::iterator iter = tmp[1].begin(); iter != tmp[1].end(); ++iter)
        if(isdigit(*iter))
            saddr += *iter;
        else
           break;
   
    return str2int(saddr);
}


void real2virtual(ACS::doubleSeq &positions, DWORD servo_address) {

    switch(servo_address) {

        // Subreflector Positioner
        case 1: {
            struct rparams p;
            load_p(&p);
            set_rot(RZRYRX);

            for(size_t idx = 0; idx < positions.length(); ++idx)
                p.d[idx] = positions[idx]; 

            inv(&p);

            for (size_t idx = 0; idx < positions.length(); ++idx)
                positions[idx] = p.x[idx]; 
        }

        // GFR, M3R, PFP
        default:
            break;
    }

}


void virtual2real(ACS::doubleSeq &positions, DWORD servo_address) {

    switch(servo_address) {

        // Subreflector Positioner
        case 1: {
            struct rparams p;
            load_p(&p);
            set_rot(RZRYRX);

            for (size_t idx = 0; idx != positions.length(); ++idx)
                p.x[idx] = positions[idx]; 

            dir(&p);

            for (size_t idx = 0; idx != positions.length(); ++idx)
                positions[idx] = p.d[idx]; 
        }

        // GFR, M3R, PFP
        default:
            break;
    }

}

