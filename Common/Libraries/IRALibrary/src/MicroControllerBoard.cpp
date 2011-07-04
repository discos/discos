#include "MicroControllerBoard.h"



MicroControllerBoard::MicroControllerBoard(
        const std::string IP, const unsigned short port,
        const BYTE master_address, const BYTE slave_address, 
        const DWORD timeout) throw (MicroControllerBoardEx) : 
                m_IP(IP), m_port(port), m_timeout(timeout), m_master_address(master_address), 
                m_slave_address(slave_address),
                m_socket(NULL), m_id(0), m_command_type(0x00)
{
    try {
        m_socket = new CSocket::CSocket();
    }
    catch (std::bad_alloc& ex) {
        throw MicroControllerBoardEx("Memory allocation error");
    }

    if(m_socket->Create(m_Error, CSocket::STREAM) == CSocket::FAIL)
        throw MicroControllerBoardEx("Error creating socket");
}

MicroControllerBoard::~MicroControllerBoard() {
    closeConnection();
    if(m_socket != NULL && m_socket->getStatus())
         delete m_socket;
}


void MicroControllerBoard::openConnection(void) throw (MicroControllerBoardEx) {

    if (m_socket->Connect(m_Error, m_IP.c_str(), m_port) == CSocket::FAIL) {
        closeConnection();
        throw MicroControllerBoardEx(std::string("Error connecting to ") + m_IP + ":" + MicroControllerBoard::any2string(m_port));
    }
    else {
        if(m_socket->setSockMode(m_Error, CSocket::BLOCKINGTIMEO, m_timeout) != CSocket::SUCCESS) {
            closeConnection();
            throw MicroControllerBoardEx("Error configuring socket");
        }
    }
}

CSocket::SocketStatus MicroControllerBoard::getConnectionStatus(void) { return(m_socket->getStatus()); }

void MicroControllerBoard::closeConnection(void) { 

    if(m_socket != NULL && m_socket->getStatus()) {
        m_socket->Close(m_Error);

        if(!m_Error.isNoError()) 
            std::cerr << "An error occured closing the socket" << endl;
        else
            std::cout << "Socket connection closed" << endl;
    }
}

std::vector<BYTE> MicroControllerBoard::receive(void) throw (MicroControllerBoardEx) { 
    BYTE msg[MCB_BUFF_LIMIT] = {0x00};
    bool is_short_cmd = false, has_data_cmd = false;
    BYTE sh_command = 0x00; // Shifted command (command_type - MCB_CMD_TYPE_EXTENDED)
    std::vector<BYTE> data;
    
    std::ostringstream msg_stream;
    
    // Receive the response form the minorServo one byte at once 
    for(size_t j = 0; j < MCB_HT_COUNTER; j++) {
        if(m_socket->Receive(m_Error, (void *)(&msg[0]), 1) == 1) {
            if(msg[0] == MCB_CMD_STX) // MCB_CMD_STX is the answer header
                break;
            else
                continue;
        }
    }
    if(msg[0] != MCB_CMD_STX)
        throw MicroControllerBoardEx("No answer header received");
    else {
        m_answer.clear();
        m_answer.push_back(msg[0]);
        // Receive the response form the minorServo one byte at once 
        // starting from the second byte (the first is the header already stored)
        // The first 6 bytes are the same for wide and short commands
        for(size_t i=1; i<MCB_BASE_ANSWER_LENGTH; i++) {
            if(m_socket->Receive(m_Error, (void *)(&msg[i]), 1) == 1) {
                msg_stream.clear();
                msg_stream.str("");
                msg_stream << msg[i];
                m_answer.push_back(msg[i]);
            }
        }

        // It the command received does not exist, then raise an exception
        if(msg[MCB_CMD_COMMAND] < MCB_CMD_TYPE_MIN_EXT || msg[MCB_CMD_COMMAND] > MCB_CMD_TYPE_MAX_ABB) {
            std::string err_msg("Command " + any2string(int(msg[MCB_CMD_COMMAND])) + " does not exist.");
            err_msg += "\nExecution result: " + any2string(int(msg[MCB_BASE_ANSWER_LENGTH]));
            throw MicroControllerBoardEx(err_msg);
        }

        // If the command is short, then I shift it to obtain the extended command,
        // so I can use only one switch.
        if(msg[MCB_CMD_COMMAND] >= MCB_CMD_TYPE_MIN_ABB) {
            is_short_cmd = true;
            sh_command = msg[MCB_CMD_COMMAND] - MCB_CMD_TYPE_NOCHECKSUM;
        }
        else
            sh_command = msg[MCB_CMD_COMMAND];

        // Commands that return parameters
        switch(sh_command) {
            case MCB_CMD_INQUIRY:
            case MCB_CMD_VERSION:  
            case MCB_CMD_GET_ADDR: 
            case MCB_CMD_GET_TIME:
            case MCB_CMD_GET_FRAME:
            case MCB_CMD_GET_PORT: 
            case MCB_CMD_GET_DATA: 
                has_data_cmd = true;
                break;
            default:
                has_data_cmd = false;
        }

        if(msg[MCB_BASE_ANSWER_LENGTH])
            throw MicroControllerBoardEx("An error occurs. Error code: " + any2string(int(msg[MCB_BASE_ANSWER_LENGTH])));

        // If the answer shoud have data and there isn't any error, then continue the reception
        if(has_data_cmd) {
            data.clear();
            if(m_socket->Receive(m_Error, (void *)(&msg[MCB_BASE_ANSWER_LENGTH]), 1) == 1) {
                msg_stream.clear();
                msg_stream.str("");
                msg_stream << msg[MCB_BASE_ANSWER_LENGTH];
                m_answer.push_back(msg[MCB_BASE_ANSWER_LENGTH]);
            }
            else
                throw MicroControllerBoardEx("Error: data lenght not received.");

            // Get the parameters
            for(int i=MCB_BASE_ANSWER_LENGTH + 1; i < MCB_BASE_ANSWER_LENGTH + 1 + int(msg[MCB_BASE_ANSWER_LENGTH]); i++) {
                if(m_socket->Receive(m_Error, (void *)(&msg[i]), 1) == 1) {
                    msg_stream.clear();
                    msg_stream.str("");
                    msg_stream << msg[i];
                    m_answer.push_back(msg[i]);
                    data.push_back(msg[i]);
                }
            }
        }

        if(!is_short_cmd) {
            size_t idx = MCB_BASE_ANSWER_LENGTH + 1 + msg[MCB_BASE_ANSWER_LENGTH];
            size_t counter = 0;
            // The first iteration should receive the checksum
            do {
                if(m_socket->Receive(m_Error, (void *)(&msg[idx]), 1) == 1) {
                    msg_stream.clear();
                    msg_stream.str("");
                    msg_stream << msg[idx];
                    m_answer.push_back(msg[idx]);
                    // If we catch the answer terminator and it is not the checksum, then stop
                    if(msg[idx] == MCB_CMD_ETX && counter != 0)
                        break;
                    idx++;
                    counter++;
                }
                else
                    break;
            } while(counter < MCB_HT_COUNTER);

            if(msg[idx] != MCB_CMD_ETX)
                throw MicroControllerBoardEx("Answer terminator not found.");
        }

        // Check if master and slave are the same for answer and request
        if(m_request[MCB_CMD_SLAVE] != m_answer[MCB_CMD_MASTER])
            throw MicroControllerBoardEx("Mismatch between master and slave addresses of request and answer.");
        
        // Check if answer and request have the same command type
        if(m_request[MCB_CMD_COMMAND] != m_answer[MCB_CMD_COMMAND])
            throw MicroControllerBoardEx("Mismatch between command code of request and answer.");

        // Check if answer and request have the same id
        if(m_request[MCB_CMD_COMMAND_ID] != m_answer[MCB_CMD_COMMAND_ID])
            throw MicroControllerBoardEx("Mismatch between command id of request and answer.");

        // Compute and verify the checksum
        std::vector<BYTE> base_checksum;
        for(std::vector<BYTE>::size_type idx = 0; idx <= m_answer.size() - 3; idx++)
            base_checksum.push_back(m_answer[idx]);

        if(!is_short_cmd)
            if(computeChecksum(base_checksum) != m_answer[m_answer.size()-2])
                throw MicroControllerBoardEx("Checksum error.");

    }

    return(data); // Return the parameters 
}

void MicroControllerBoard::send(const BYTE command, std::vector<BYTE> parameters) throw (MicroControllerBoardEx) {
    if(command >= MCB_CMD_TYPE_MIN_EXT && command <= MCB_CMD_TYPE_MAX_ABB) {
        m_request.clear();
        m_command_type = command;
        m_request.push_back(MCB_CMD_SOH);
        m_request.push_back(m_slave_address);
        m_request.push_back(m_master_address);
        m_request.push_back(m_command_type);
        m_request.push_back(++m_id);
        if(!parameters.empty()) {
            m_request.push_back(parameters.size());
            for(std::vector<BYTE>::iterator iter = parameters.begin(); iter != parameters.end(); iter++)
                m_request.push_back(*iter);
        }
        // If the command is extended, put the checksum in the request
        if(m_command_type <= MCB_CMD_TYPE_MAX_EXT) {
            m_request.push_back(computeChecksum(m_request));
            m_request.push_back(MCB_CMD_ETX);
        }
    }
    else 
        throw MicroControllerBoardEx("Command '" + any2string(int(command)) + "' not found");

    size_t num_bytes=0, sent_bytes=0, i=0;  
    size_t len = m_request.size();
    BYTE msg[len];
    for(std::vector<BYTE>::iterator iter = m_request.begin(); iter != m_request.end(); iter++) {
        msg[i] = *iter;
        i++;
    }

    while(sent_bytes < len) {
        // Send returns the total number of bytes sent
        if ((num_bytes = m_socket->Send(m_Error, (const void *)(msg + sent_bytes), len - sent_bytes)) < 0) 
            break ;
        else sent_bytes += num_bytes;
    }

    if (sent_bytes != len)
        throw MicroControllerBoardEx("Not all bytes sent");
}


BYTE MicroControllerBoard::computeChecksum(std::vector<BYTE> message) throw (MicroControllerBoardEx) {
    if(message.empty())
        throw MicroControllerBoardEx("Checksum error: no bytes to check");

    BYTE checksum = message[0];
    for(std::vector<BYTE>::iterator iter = message.begin(); iter != message.end() - 1; iter++)
        checksum = checksum ^ *(iter + 1);

    return(checksum);
}

