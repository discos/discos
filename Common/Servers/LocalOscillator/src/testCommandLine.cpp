#include "CommandLine.h"




int main(int argv, char** argc)
{
	

	CommandLine* cl=new CommandLine();
	cout << "initializing board" << endl;
	string reply;
	double freq;

	try{
		cl->configure(0,19,0,T10s,1,0);
//		cout <<"configured" <<endl;
		cl->query("*IDN?",reply);
		cout << "sent *IDN?" << endl;

		cl->query("SYST:ERR?",reply);
                cout <<reply.c_str() << endl;
                cout << "sent UNIT:FREQ Mhz" << endl;
//                cl->sendCMD("UNIT:FREQ khz ");
//                cout << "sent UNIT:FREQ?" << endl;
                cl->query("UNIT:FREQ?",reply);
                cout <<reply.c_str() << endl;



//		cl->sendCMD("*RST");
//		cl->query("SYST:ERR?",reply);

//		cl->setFreq(10000);// set 10000 MHz.
//		cl->query("SYST:ERR?",reply);
//		cout <<reply.c_str() << endl;

		cout << cl->getFreq(freq) <<endl;
		cout << "FREQ:" << freq <<endl;
//		cl->query("FREQ?",reply);
//		cl->query("SYST:ERR?",reply);
//		cout <<reply.c_str() << endl;

//		cl->setPower(-1.1);

//		FREQ:CW 1234.567MHZ
//		cl->query("POW?",reply);
//		cl->sendCMD("POW -1.1dBM");
		cl->queryErrors(reply);
		int code;
		code= cl->parseSCPIErrors(reply,reply);
		cout <<reply.c_str() << code << endl;
		cl->getFreq(freq);

		cout <<"freq:" <<freq <<endl;
		double pow;
		cl->getPower(pow);

	    cout <<"pow:" <<pow <<endl;
	    cl->disconnect();

//		int configure(int boardID, int pad, int sad, int tmo, int eot, int eos) throw (GPIBException);

	} catch (GPIBException& ex)
	{
	cout << ex.what() << endl;


	}


	return 0;
	
}
