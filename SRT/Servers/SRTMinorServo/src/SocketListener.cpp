/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include "SocketListener.h"
#include "libCom.h"
#include "utils.h"
#include "macros.def"


struct ThreadParameters;

SocketListener::SocketListener(
        const ACE_CString& name, 
        ThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime
        ) : ACS::Thread(name, responseTime, sleepTime), m_params(&params)
{
    AUTO_TRACE("SocketListener::SocketListener()");
}

SocketListener::~SocketListener() { 
    AUTO_TRACE("SocketListener::~SocketListener()"); 
}

void SocketListener::onStart() { AUTO_TRACE("SocketListener::onStart()"); }

void SocketListener::onStop() { AUTO_TRACE("SocketListener::onStop()"); }

void SocketListener::runLoop() throw (ComponentErrors::SocketErrorExImpl)
{
    string rec_answer, key;
        
    try {
        (m_params->socket_ptr)->receiveBuffer(rec_answer, message_closer, headers);
        if(!rec_answer.empty()) {
            if(!startswith(rec_answer, ack_header) && 
                    !startswith(rec_answer, timed_ack_header) && 
                    !startswith(rec_answer, nak_header)
            )
                THROW_EX(ComponentErrors, SocketErrorEx, "Header error. Message was: " + rec_answer, false);
            try {
                vector<string> sec, tmp;
                sec = split(rec_answer, answer_separators);
                // The first section of the answer, generally the sent command
                if(rec_answer.find(command_separators) != string::npos)
                    tmp = split(sec[0], command_separators); // <comando, cmd_num=app_num>
                else
                    THROW_EX(ComponentErrors, SocketErrorEx, 
                            "Missing command separator in the answer: " + rec_answer + "\n", false);
                if(rec_answer.find(reqid_separators) != string::npos) {
                    key = tmp[1];
                    tmp = split(tmp[1], reqid_separators);
                    strip(key); 
                }
                else
                    THROW_EX(ComponentErrors, SocketErrorEx, 
                            "Missing reqid separator in the answer: " + rec_answer + "\n", false);

                unsigned long cmd_number = str2int(tmp[0]);
                unsigned short saddress = str2int(tmp[1]);
                WPServoTalker *talker = NULL; 

                try {
                    pthread_mutex_lock(m_params->listener_mutex); 
                    if((m_params->map_of_talkers_ptr)->count(saddress))
                        talker = (*(m_params->map_of_talkers_ptr))[saddress];
                    else
                        THROW_EX(ComponentErrors, SocketErrorEx, 
                                "Found a wrong servo address parsing the answer: " + rec_answer + "\n", false);
                    pthread_mutex_unlock(m_params->listener_mutex); 
                }
                catch(...) {
                    pthread_mutex_unlock(m_params->listener_mutex); 
                    throw;
                }

                CSecAreaResourceWrapper<map<int, string> > secure_responses = (talker->m_responses)->Get();
                (*secure_responses)[cmd_number] = rec_answer;

                // Erase the first character of rec_answer (the header)
                rec_answer.erase(0, 1);

                // If the answer is a NAK_setup turn the setup-bit off and delete the item from the map
                if(startswith(rec_answer, "NAK_setup")) {
                    *(m_params->is_setup_exe_ptr) = false;
                    if(m_setup_counter.count(key))
                        m_setup_counter.erase(key);
                    return;
                }
                
                // If the answer is a NAK_stow turn the stow-bit off and delete the item from the map
                if(startswith(rec_answer, "NAK_stow")) {
                    *(m_params->is_stow_exe_ptr) = false;
                    if(m_stow_counter.count(key))
                        m_stow_counter.erase(key);
                    return;
                }

                // If the answer is the second setup turn the setup-bit off and delete the item from the map
                if(startswith(rec_answer, "setup")) {
                    if(m_setup_counter.count(key)) {
                        m_setup_counter.erase(key);
                        *(m_params->is_setup_exe_ptr) = false;
                    } 
                    else
                        m_setup_counter[key] = 1;
                    return;
                }
               
                // If the answer is the second stow turn the stow-bit off and delete the item from the map
                if(startswith(rec_answer, "stow")) {
                    if(m_stow_counter.count(key)) {
                        m_stow_counter.erase(key);
                        *(m_params->is_stow_exe_ptr) = false;
                    } 
                    else
                        m_stow_counter[key] = 1;
                    return;
                }

            }
            catch(...) 
                THROW_EX(ComponentErrors, SocketErrorEx, 
                        "Unexpected error parsing the answer: " + rec_answer + "\n", false);
        }
    }
    catch (ACSErr::ACSbaseExImpl& E) {
        ACS_SHORT_LOG((LM_ERROR, "Unexpected error in SocketListener"));
    }
}
 
