/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
 *
 *  External functions
 *  ==================
 *    * split, strip, startswith, endswith: functions used to manipulate 
 *      strings (utils.h)
 *
\*******************************************************************************/

#include "WPServoTalker.h"
#include "libCom.h" 
#include <limits>
#include <string>
#include <bitset>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <baciDB.h>
#include <ManagmentDefinitionsS.h>
#include <iostream>
#include <new>
#include <MinorServoErrors.h>
#include <MSParameters.h>
#include "macros.def"
#include <time.h>


WPServoTalker::WPServoTalker(
        const CDBParameters *const cdb_ptr, 
        ExpireTime *const expire_ptr,
        CSecureArea< map<int, vector<PositionItem> > > *cmdPos_list,
        const Offsets *const offsets
        ) throw (ComponentErrors::MemoryAllocationExImpl): m_cdb_ptr(cdb_ptr), m_offsets(offsets), m_cmdPos_list(cmdPos_list)
{
    AUTO_TRACE("WPServoTalker::WPServoTalker();");

    try {
        vector<string> *requests = new vector<string>;
        m_requests = new CSecureArea<vector<string> >(requests);

        CSecAreaResourceWrapper<vector<string> > secure_requests = m_requests->Get();
        secure_requests->clear();

        map<int, string> *responses = new map<int, string>;
        m_responses = new CSecureArea<map<int, string> >(responses);
        unsigned int *cmd_number = new unsigned int;
        m_cmd_number = new CSecureArea<unsigned int>(cmd_number);
    }
    catch (std::bad_alloc& ex)
        THROW_EX(ComponentErrors, MemoryAllocationEx, "WPServoTalker::WPServoTalker(): 'new' failure", false);

    CSecAreaResourceWrapper<unsigned int> secure_cmd_num = m_cmd_number->Get();
    *secure_cmd_num = 0;
    secure_cmd_num.Release();
}


WPServoTalker::~WPServoTalker() { 
    AUTO_TRACE("WPServoTalker::~WPServoTalker();"); 
    delete m_responses;
    delete m_requests;
}


void WPServoTalker::getActPos(
        ACS::doubleSeq &positions, 
        ACS::Time &timestamp
        ) throw (
            ComponentErrors::SocketErrorExImpl, 
            MinorServoErrors::CommunicationErrorEx
            )
{
    
    AUTO_TRACE("WPServoTalker::getActPos()");
    timeval now;
    gettimeofday(&now, NULL);
    double starting_time = now.tv_sec + now.tv_usec / TIME_SF;
    string request = make_request(0, m_cdb_ptr, m_cmd_number);
    
    // Schedule a position request
    CSecAreaResourceWrapper<vector<string> > secure_requests = m_requests->Get();
    secure_requests->push_back(request);
    secure_requests.Release();
    // If you want to use this method you must subtract the system offset to the positions
    timestamp = look_for_a_response(get_request_id(request), starting_time, 0, true, &positions);

    if(positions.length() != m_cdb_ptr->NUMBER_OF_AXIS)
        THROW_EX(MinorServoErrors, CommunicationErrorEx, "Cannot get minor servo position: wrong number of axis", true);
}   


void WPServoTalker::setCmdPos(
        const ACS::doubleSeq &cmd_positions, 
        ACS::Time &timestamp,
        const ACS::Time exe_time,
        const bool is_dummy
        ) throw (
            ComponentErrors::SocketErrorExImpl, 
            MinorServoErrors::PositioningErrorEx, 
            MinorServoErrors::CommunicationErrorEx
            ) {
    
    AUTO_TRACE("WPServoTalker::setCmdPos()");
    timeval now;
    gettimeofday(&now, NULL);
    double starting_time = now.tv_sec + now.tv_usec / TIME_SF;

    if(cmd_positions.length() != m_cdb_ptr->NUMBER_OF_AXIS)
        THROW_EX(MinorServoErrors, PositioningErrorEx, "Cannot set minor servo position: wrong number of axis", true);

    CSecAreaResourceWrapper<map<int, vector<PositionItem> > > lst_secure_requests = m_cmdPos_list->Get(); 

    if(!is_dummy) { // If we really want to command the position to the MSCU
        ACS::doubleSeq positions = cmd_positions;
        // Add the offsets to the positions before make_request (so positions and offsets are both in virtual coordinate)
        for(size_t i=0; i<positions.length(); i++)
            positions[i] += (m_offsets->user)[i] + (m_offsets->system)[i];

        // The first argument is the index of a vector of commands; make_request converts the position to virtual
        string request = make_request(2, m_cdb_ptr, m_cmd_number, -1, -1, -1, exe_time, &positions);
        // Schedule a position setting
        CSecAreaResourceWrapper<vector<string> > secure_requests = m_requests->Get();
        secure_requests->push_back(request);
        secure_requests.Release();

        timestamp = look_for_a_response(get_request_id(request), starting_time, 2);
    }

    // Set the position in the vector list when look_for_a_response found the response
    // We must use the cmd_positions insead of positions because make_request(...) modifies the positions making 
    // a virtual2real transformation
    PositionItem item;
    item.exe_time = exe_time;
    (item.position).length(cmd_positions.length());
    ((item.offsets).user).length(cmd_positions.length());
    ((item.offsets).system).length(cmd_positions.length());
    for(size_t idx = 0; idx != cmd_positions.length(); idx++) {
        (item.position)[idx] = cmd_positions[idx];
        ((item.offsets).user)[idx] = (m_offsets->user)[idx];
        ((item.offsets).system)[idx] = (m_offsets->system)[idx];
    }
    
    try {
        vector<PositionItem>::size_type idx = findPositionIndex(lst_secure_requests, exe_time, m_cdb_ptr->SERVO_ADDRESS);
        ((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).insert(
            ((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).begin() + idx + 1, item);
    }
    catch(PosNotFoundEx) {
        ((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).insert(
            ((*lst_secure_requests)[m_cdb_ptr->SERVO_ADDRESS]).begin(), item);
    }
    lst_secure_requests.Release();
}   


void WPServoTalker::getAppStatus(
        unsigned long &status, 
        ACS::Time &timestamp
        ) throw (
            ComponentErrors::SocketErrorExImpl, 
            MinorServoErrors::CommunicationErrorEx
            )
{
    
    AUTO_TRACE("WPServoTalker::getAppStatus()");
    timeval now;
    gettimeofday(&now, NULL);
    double starting_time = now.tv_sec + now.tv_usec / TIME_SF;
    string request = make_request(6, m_cdb_ptr, m_cmd_number);
    
    // Insert the request in the queue
    CSecAreaResourceWrapper<vector<string> > secure_requests = m_requests->Get();
    secure_requests->push_back(request);
    secure_requests.Release();
    look_for_a_response(get_request_id(request), starting_time, 6, true, NULL, &status, -1);
    timestamp = getTimeStamp();
    bitset<16> status_bset(status);
    // Flip bits to obtain a coherent pattern
    for(size_t idx=0; idx != sizeof(status_flipped_bits)/sizeof(*status_flipped_bits); idx++)
        status_bset.flip(status_flipped_bits[idx]);
    // Analize the status skipping the moving bit
    for(size_t idx=0; idx != status_bset.size()-1; idx++)
        if(status_bset.test(idx))
            ACS_SHORT_LOG((LM_WARNING, ("@ %u: " + status_messages[idx] + " status bit high").c_str(), timestamp));

    status = status_bset.to_ulong();
}   


void WPServoTalker::getStatus(
        StatusParameters &status_par, 
        ACS::Time &timestamp
        ) throw (
            ComponentErrors::SocketErrorExImpl, 
            MinorServoErrors::CommunicationErrorEx
            )
{
    
    AUTO_TRACE("WPServoTalker::getStatus()");
    timeval now;
    gettimeofday(&now, NULL);
    double starting_time = now.tv_sec + now.tv_usec / TIME_SF;
    string request = make_request(7, m_cdb_ptr, m_cmd_number);
    
    // Insert the request in the queue
    CSecAreaResourceWrapper<vector<string> > secure_requests = m_requests->Get();
    secure_requests->push_back(request);
    secure_requests.Release();
    timestamp = look_for_a_response(get_request_id(request), starting_time, 7, true, NULL, NULL, -1, &status_par);
}   


void WPServoTalker::action(
        const unsigned int code,
        const ACS::Time exe_time
        ) throw (
            ComponentErrors::SocketErrorExImpl, 
            MinorServoErrors::CommunicationErrorEx
            ) {
    
    AUTO_TRACE("WPServoTalker::action()");
    timeval now;
    gettimeofday(&now, NULL);
    double starting_time = now.tv_sec + now.tv_usec / TIME_SF;
    
    // The first argument is the index of a vector of commands
    string request = make_request(code, m_cdb_ptr, m_cmd_number, -1, -1, -1, exe_time);

    // Schedule a position setting
    CSecAreaResourceWrapper<vector<string> > secure_requests = m_requests->Get();
    secure_requests->push_back(request);
    secure_requests.Release();

    look_for_a_response(get_request_id(request), starting_time, code);
}   


void WPServoTalker::getParameter(
        ACS::doubleSeq &parameters, 
        ACS::Time &timestamp,
        unsigned int index,
        unsigned int sub_index,
        bool from_plc
        ) throw (
            ComponentErrors::SocketErrorExImpl, 
            MinorServoErrors::CommunicationErrorEx
          ) 
{
    
    AUTO_TRACE("WPServoTalker::getParameter(" + number2str(index) + ", " + number2str(sub_index) + ")");
    timeval now;
    gettimeofday(&now, NULL);
    double starting_time = now.tv_sec + now.tv_usec / TIME_SF;
    
    string answer;
    string request;
    unsigned int number_of_req, starting_address;

    // If the slave is the PLC
    number_of_req = from_plc ? 1 : m_cdb_ptr->NUMBER_OF_SLAVES;
    starting_address = from_plc ? 0 : 1;

    // unsigned long max_time = 0;
    // For every slave that accepts this kind of request
    for(size_t j = starting_address; j < number_of_req; j++) {
        // The first argument is the index of a vector of commands
        request = make_request(1, m_cdb_ptr, m_cmd_number, j, index, sub_index);
        // Schedule a parameters request
        CSecAreaResourceWrapper<vector<string> > secure_requests = m_requests->Get();
        secure_requests->push_back(request);
        secure_requests.Release();
        timestamp = look_for_a_response(get_request_id(request), starting_time, 1, true, &parameters, NULL, j);
    }

    if(from_plc) {
        if(parameters.length() != 1) 
            THROW_EX(MinorServoErrors, CommunicationErrorEx, "Cannot get minor servo parameters: wrong number of slaves", true);
    }
    else {
        if(parameters.length() != m_cdb_ptr->NUMBER_OF_SLAVES - 1)
            THROW_EX(MinorServoErrors, CommunicationErrorEx, "Cannot get minor servo parameters: wrong number of slaves", true);
    }

}   


ACS::Time WPServoTalker::look_for_a_response(
        const unsigned long request_id, 
        const double starting_time, 
        const unsigned int cmd_idx, 
        bool process_enabled, 
        ACS::doubleSeq *parameters,
        unsigned long *udata,
        const int slave,
        StatusParameters *status_par
        )  throw (MinorServoErrors::CommunicationErrorEx) 
{
    string answer;
    timeval now;
    gettimeofday(&now, NULL);
    ACS::Time timestamp = getTimeStamp();
    bool found = false;

    while(((now.tv_sec + now.tv_usec / TIME_SF + GUARD_TIME) - starting_time) < m_cdb_ptr->SERVER_TIMEOUT / TIME_SF) {
        // If the right answer is in the `map` of responses
        try {
            if((m_responses->Get())->count(request_id)) {
                answer = (*(m_responses->Get()))[request_id];
                // Raise an exception if the answer is a NAK
                verify(answer);
                if(process_enabled) process(answer, parameters, udata, status_par, cmd_idx, m_cdb_ptr, timestamp, slave);
                // Delete the answer from the map of responses
                CSecAreaResourceWrapper<map<int, string> > secure_responses = m_responses->Get();
                secure_responses->erase(request_id);
                secure_responses.Release();
                found = true;
                break;
            }
            usleep(USLEEP_TIME);
            gettimeofday(&now, NULL);
        }
        catch(...) {
            CSecAreaResourceWrapper<map<int, string> > secure_responses = m_responses->Get();
            secure_responses->erase(request_id);
            secure_responses.Release();
            break;
        }
    }
    if(!found)
        THROW_EX(MinorServoErrors, CommunicationErrorEx, "Answer not found", true);

    return timestamp;
}
