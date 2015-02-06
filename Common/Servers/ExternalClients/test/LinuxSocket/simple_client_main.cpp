#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>

int main ( int argc, int argv[] )
{
    try {
        ClientSocket client_socket ( "192.167.187.17", 30000 );
        std::string reply;
        std::string command;

        int i;
        for (i = 0; i <= 5; i++) {
        //for (;;) {
            try {
                client_socket << "antennaParameters";
                //client_socket << "antennaParameters";
                //std::cin >> command;
                //client_socket << command;
                client_socket >> reply;
		        std::cout << reply << "\n";
            }
            catch ( SocketException& ) {}

            //std::cout << "Closing communication" << "\n";
        }
    }
    catch ( SocketException& e ) {
        std::cout << "Exception was caught:" << e.description() << "\n";
    }

    return 0;
}
