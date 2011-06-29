// $Id: TestWriter.cpp,v 1.7 2011-02-28 17:46:12 a.orlati Exp $

#include <BackendsDefinitionsC.h>
#include "FitsWriter.h"
#include <IRA>
#include <DateTime.h>

#define CHANNELS 14
#define FEEDS 7
#define BINS 256
#define POLS 4
#define DATA_TYPE float

using namespace IRA;

int main(int argc, char *argv[]) 
{
	unsigned inputsNumber=0;
	ACS::longSeq pols;
	ACS::doubleSeq calib;
	ACS::doubleSeq los;
	ACS::doubleSeq recvFreq;
	ACS::doubleSeq recvBw;
	CFitsWriter *file;
	IRA::CString fileName;
	TIMEVALUE now;
	Backends::TMainHeader mH;
	Backends::TSectionHeader cH[CHANNELS];
	double tsys[CHANNELS];
	CFitsWriter::TFeedHeader fH[FEEDS];
	CFitsWriter::TDataHeader tdh;
	IRA::CIRATools::getTime(now);
	fileName.Format("%02d_%02d_%02d.fits",now.hour(),now.minute(),now.second());
	file = new CFitsWriter();
	file->setBasePath("./");
	file->setFileName((const char *)fileName);
	if (!file->create()) {
		printf("FITS Error: %s\n",(const char *)file->getLastError());
		exit(-1);
	}
	//create the header dataset
	mH.sections=CHANNELS;				
	mH.beams=FEEDS;
	mH.integration=0;
	mH.sampleSize=sizeof(DATA_TYPE);
	
	for (int i=0;i<CHANNELS;i++) {
		cH[i].id=i;  
		cH[i].bins=BINS;
		cH[i].polarization=Backends::BKND_FULL_STOKES;  
		cH[i].inputs=2;
		inputsNumber+=2;
		cH[i].bandWidth=2000.0;				
		cH[i].frequency=110.0;                
		cH[i].attenuation[0]=9.0;
		cH[i].attenuation[1]=14.0;
		cH[i].IF[0]=0;
		cH[i].IF[1]=1;
		cH[i].sampleRate=0.000025;              
		cH[i].feed=i/2;
		tsys[i]=100.0+i;
	}
	for (int i=0;i<FEEDS;i++) {
		fH[i].id=i;
		fH[i].xOffset=0.12*i;
		fH[i].yOffset=0.14*i;
		fH[i].relativePower=0.02*i*i;
	}
	los.length(2);
	los[0]=8080.0;
	los[1]=2020.0;
	calib.length(inputsNumber);
	for (unsigned k=0;k<inputsNumber;k++) {
		calib[k]=32.56+k;
	}
	pols.length(2);
	pols[0]=Receivers::RCV_LEFT;
	pols[1]=Receivers::RCV_RIGHT;	
	recvFreq.length(2);
	recvFreq[0]=100.0;
	recvFreq[1]=100.0;
	recvBw.length(2);
	recvBw[0]=800.0;
	recvBw[1]=800.0;
	if (!file->saveMainHeader(mH)) {
		printf("FITS Error: %s\n",(const char *)file->getLastError());
		exit(-1);
	}
	else if (!file->setPrimaryHeaderKey("Project_Name","Dummy_Project","Name of the project")) {
		printf("FITS Error: %s\n",(const char *)file->getLastError());
		exit(-1);
	}
	else if (!file->setPrimaryHeaderKey("Observer","Dummy Observer","Name of the observer")) {
		printf("FITS Error: %s\n",(const char *)file->getLastError());
		exit(-1);
	}
	else if(!file->setPrimaryHeaderKey("Receiver_code","Dummy Recv","Keyword that identifies the receiver")) {
		printf("FITS Error: %s\n",(const char *)file->getLastError());
		exit(-1);
	}
	if (!file->saveSectionHeader(cH)) {
		printf("FITS Error: %s\n",(const char *)file->getLastError());
		exit(-1);
	}
	if (!file->addSectionTable(pols,los,recvFreq,recvBw,calib)) {
		printf("FITS Error: %s\n",(const char *)file->getLastError());
		exit(-1);
	}
	if (!file->addFeedTable("FEED TABLE")) {
		printf("FITS Error: %s\n",(const char *)file->getLastError());
		exit(-1);		
	}
	for (int j=0;j<FEEDS;j++) {
		if (!file->saveFeedHeader(fH[j])) {
			printf("FITS Error: %s\n",(const char *)file->getLastError());
			exit(-1);					
		}
	}
	if (!file->addDataTable("DATA TABLE")) {
		printf("FITS Error: %s\n",(const char *)file->getLastError());
		exit(-1);					
	}
	file->flush();
	for (int j=0;j<1000;j++) {
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		IRA::CDateTime mjd(now); 
		tdh.time=mjd.getMJD();
		//srandom((unsigned)tdh.time);
		tdh.raj2000=j*0.0023;
	    tdh.decj2000=j*0.0033;
	    tdh.az=sin(j*0.00023);
	    tdh.el=cos(j*0.0033);
	    tdh.par_angle=0.1236*j*j;
	    tdh.derot_angle=0.0;
	   // tdh.tsys=1.0;
	    tdh.flag_cal=0;
	    tdh.flag_track=1;
	    tdh.weather[0]=0.2;
	    tdh.weather[1]=100.0;
	    tdh.weather[2]=22.45;
	    if (!file->storeAuxData(tdh,tsys)) {
	    	printf("FITS Error: %s\n",(const char *)file->getLastError());
	    	exit(-1);
	    }
	    for (int i=0;i<CHANNELS;i++) {
			DATA_TYPE channel[BINS*POLS];
			int counter=0;
			for (int k=0;k<POLS;k++) {
				for (int y=0;y<BINS;y++) {
					channel[counter]=/*10*sin(j*6.28/1000);*/((DATA_TYPE)random()/(DATA_TYPE)RAND_MAX)+k;
					counter++;
				}
			}
			if (!file->storeData(channel,BINS*POLS,i)) {
		    	printf("FITS Error: %s\n",(const char *)file->getLastError());
		    	exit(-1);
			}
	    }
	    file->add_row();
	}
	delete file;
	printf("Done!\n");
}
