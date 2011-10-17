#include "CommandLine.h"


int main(int argv, char** argc)
{
	

	CommandLine* cl=new CommandLine();
	cout << "initializing board" << endl;
	string reply;
	double freq;

	try{
		cl->configure(0,10,0,T10s,1,0);
		cl->query("*IDN?",reply);

		cout <<reply.c_str() << endl;
		cl->sendCMD("*CLS");

		cl->setFreq(1.2345);// set 1 MHz.

		cl->query("FREQ?",reply);
		cout <<reply.c_str() << endl;

//		cl->query("POW AMPL?",reply);
		cl->setPower(1.1);

		cl->query("SYST:ERR?",reply);

		cout <<reply.c_str() << endl;
		cl->getFreq(freq);
		cout <<"freq:" <<freq <<endl;


//		int configure(int boardID, int pad, int sad, int tmo, int eot, int eos) throw (GPIBException);

	} catch (GPIBException& ex)
	{
	cout << ex.what() << endl;


	}


	return 0;
	
}
