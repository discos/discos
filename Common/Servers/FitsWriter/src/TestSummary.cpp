#include "SummaryWriter.h"
#include "File.h"

using namespace FitsWriter_private;

int main(int argc, char *argv[])
{
	CSummaryWriter sumWriter;
	sumWriter.setBasePath("./");
	sumWriter.setFileName("summary1.fits");


	if (!sumWriter.create()) {
		printf("Error: %s\n",(const char *)sumWriter.getLastError());
		return 1;
	}

	std::list<double> restFreq;
	restFreq.push_back(100.234);
	restFreq.push_back(70.12345);
	//restFreq.push_back(90.1);
	//restFreq.push_back(123.77);

	sumWriter.getFile().setKeyword("TELESCOP","MED");  //string single
	sumWriter.getFile().setKeyword("EXPTIME",10.34); //double single
	sumWriter.getFile().setKeyword("NUSEBANDS",10L); //long single
	sumWriter.getFile().setKeyword("RESTFREQ",restFreq); //double multi
	sumWriter.getFile().setKeyword("TOPOFREQ",restFreq); //double multi

	//
	//sumWriter.getFile().setKeyword("test",2341.2213); //double single, table header keyword


	if (!sumWriter.write()) {
		printf("Error: %s\n",(const char *)sumWriter.getLastError());
		return 1;
	}
	if (!sumWriter.close()) {
		printf("Error: %s\n",(const char *)sumWriter.getLastError());
		return 1;
	}

};


