#include <string.h>
#include <iostream>
#include <IRA>

using namespace IRA;

#define SXKL_ADDRESS "192.167.189.2"
#define SXKL_PORT 9876

#include "sxklLine.i"

int main(int argc, char *argv[]) {
	if (strcmp(argv[1],"CALON")==0) {
		if (!sendToSXKL(1)) {
			std::cout << "Noise diode turned on!!!" << std::endl;			
		}
		else {
			std::cout << "Error!!!" << std::endl;			
		}
	}
	else if (strcmp(argv[1],"CALOFF")==0) {
		if (!sendToSXKL(2)) {
			std::cout << "Noise diode turned off!!!" << std::endl;			
		}
		else {
			std::cout << "Error!!!" << std::endl;			
		}		
	}
	else if (strcmp(argv[1],"SETUP")==0) {
		if (!sendToSXKL(3)) {
			std::cout << "XXP Receiver configured!!!" << std::endl;			
		}
		else {
			std::cout << "Error!!!" << std::endl;			
		}		
	}
	else {
		std::cout << "nothing to do!!!" << std::endl;
	}
}