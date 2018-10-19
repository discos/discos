#include "SummaryWriter.h"
#include "File.h"
#include <DiscosVersion.h>

using namespace FitsWriter_private;

int main(int argc, char *argv[])
{
	CSummaryWriter sumWriter;
	sumWriter.setBasePath("./");
	sumWriter.setFileName("summary1.fits");
	TIMEVALUE currentUT;
	IRA::CDateTime now;
	TIMEDIFFERENCE currentLST;
	IRA::CString timeStr;
	IRA::CSite site;


	if (!sumWriter.create()) {
		printf("Error: %s\n",(const char *)sumWriter.getLastError());
		return 1;
	}

	std::list<double> restFreq;
	restFreq.push_back(100.234);
	restFreq.push_back(70.12345);
	//restFreq.push_back(90.1);
	//restFreq.push_back(123.77);

	// time
	IRA::CIRATools::getTime(currentUT); // get the current time
	now.setDateTime(currentUT,0.0);  // transform the current time in a CDateTime object
 	now.LST(site).getDateTime(currentLST);  // get the current LST time
 	currentLST.day(0);
 	IRA::CIRATools::intervalToStr(currentLST.value().value,timeStr);
	sumWriter.getFile().setKeyword("LST",timeStr);
 	IRA::CIRATools::timeToStrExtended(currentUT.value().value,timeStr);
 	sumWriter.getFile().setKeyword("DATE-OBS",timeStr);

	sumWriter.getFile().setKeyword("TELESCOP",DiscosVersion::CurrentVersion::station);  //string single
	sumWriter.getFile().setKeyword("CREATOR",DiscosVersion::CurrentVersion::getVersion());  //string single
	sumWriter.getFile().setKeyword("EXPTIME",10.34); //double single
	sumWriter.getFile().setKeyword("NUSEBANDS",10L); //long single
	sumWriter.getFile().setKeyword("RESTFREQ",restFreq); //double multi
	sumWriter.getFile().setKeyword("TOPOFREQ",restFreq); //double multi
	sumWriter.getFile().setKeyword("BackendName","TP");  //string single

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


