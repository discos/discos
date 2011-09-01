/** 
 * $Id: RScanSched.cpp,v 1.13 2009-02-20 15:17:08 s.righini Exp $
 **/

#include "RScanSched.h"
#include <getopt.h>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace IRA;
/**
 * This client is aimed at producing a schedule to perform on-the-fly
 * recursive horizontal scans, moving the antenna back and forth in azimuth only.  
 * It accepts in input the following parameters:
 * startAz = azimuth where acquisition starts for the first subscan (degrees)
 * stopAz = azimuth where acquisition stops for the first subscan (degrees)
 * elevation = constant elevation for all the subscans (degrees)
 * interval = data acquisition sampling interval (ms)
 * vel = azimuth constant speed for the acquisition in °/min (@ El=0°) 
 * tOver =  total overhead time of a single subscan (ramps + overhead) (seconds) 
 *          Values must fulfill the tested ranges, given in the following table:
 *           scanning speed between  0 and 5  °/min --> 
 *           scanning speed between  5 and 10 °/min -->
 *           scanning speed between 10 and 15 °/min --> 13.3 sec
 *           scanning speed between 15 and 20 °/min -->
 * LSTstep = local sidereal time elapsing between two adjacent sequences (seconds)
 * Nstep = order number of the specific sequence (0,1,2...)
 * nCycles = total number of forth+back scans in the sequence
 * repeat = number of repetitions for the schedule (1,2...) -1 means "ad libitum"
 * 
 * Four output files are produced: 
 * .lis - contains a list of the subscans (one "forth" and one "back") 
 * .scd - gives the LST start instants for the subscans of the specified sequence 
 * .bck - contains the backend configuration parameters (-1 means external values are loaded)
 * .cfg - contains the observation/system configuration parameters (-1 means external values are loaded)
 **/

void printHelp() {
	printf("Recursive Azimuth Scans Scheduler\n");
	printf("\n");
	printf("Please at least specify the savefile name\n");
	printf("[-f savefile]\n");
	printf("[-s startAz(deg)]\tdefault: 235.5\n");
    printf("[-p stopAz(deg)]\tdefault: 261.5\n");
    printf("[-e elevation(deg)]\tdefault: 30.0\n");
	printf("[-i interval(ms)]\tdefault: 40.0\n");
	printf("[-v velocity(°/min)]\tdefault: 15.0\n");
	printf("[-o tOver(seconds)]\tdefault: 13.389769\n");
	printf("[-l LSTstep(seconds)]\tdefault: 3.36530\n");
	printf("[-n Nstep(integer)]\tdefault: 0\n");
	printf("[-c nCycles(integer)]\tdefault: 367\n");
	printf("[-r repeat(integer)]\tdefault: 1\n\n\n");
}


int main(int argc, char *argv[]){
	baci::BACIValue bValue;
	CString name;
	double startAz, stopAz, elevation;
	double interval, vel, tOver;
	double LST0;
	double tScan;
	double* tf;
	double* tb;
	int* hhf;
	int* hhb;
	int* mmf;
	int* mmb;
	double* ssf;
	double* ssb;
	double LSTstep;
	double gap;
	int Nstep;
	int nCycle;
	int repeat;
	double tCruise;
	int sampleNumber;
	CString outString;
	CString saveFile, saveSched, saveList;
	CString config, backend;
	char str[512];
	char source[16];
	ofstream fileScd, fileLis; 
	ofstream fileCfg, fileBck;
	
	if (argc==1) {
		printf("Missing  arguments.......\n\n");
		printHelp();
		exit(-1);
	}
	
	int out;
	//default values
	startAz=235.5;
	elevation=30.0;
	stopAz=261.5;
	interval=40.0;
	vel=15.0;
	tOver=13.389769;
	LSTstep=3.36530;
    Nstep=0;
    nCycle=367;
    repeat=1;
	saveFile="";
	while ((out=getopt(argc,argv,"f:s:p:e:i:v:o:l:n:c:"))!=-1) {
		switch (out) {
			case 'f': {
				saveFile=CString(optarg);
				break;
			}
			case 's': {
				if (sscanf(optarg,"%lf",&startAz)!=1){
					printf("couldn't get start azimuth\n\n");
					printHelp();
					exit(-1);
				}
				break;
			}
			case 'p': {
				if (sscanf(optarg,"%lf",&stopAz) !=1) {
					printf("couldn't get stop azimuth\n\n");
					printHelp();
					exit(-1);
				}	
				break;
			}
			case 'e': {
				if (sscanf(optarg,"%lf",&elevation) !=1) {
					printf("couldn't get elevation\n\n");
					printHelp();
					exit(-1);
				}
				break;
			}
			case 'i': {
				if (sscanf(optarg,"%lf",&interval) !=1) {
					printf("couldn't get sampling interval\n\n");
					printHelp();
					exit(-1);
				}	
				break;
			}
			case 'v': {
				if (sscanf(optarg,"%lf",&vel) !=1) {
					printf("couldn't get velocity\n\n");
					printHelp();
					exit(-1);
				}	
				break;
			}
			case 'o': {
				if (sscanf(optarg,"%lf",&tOver) !=1) {
					printf("couldn't get overhead time\n\n");
					printHelp();
					exit(-1);
				}	
				break;
			}
			case 'l': {
				if (sscanf(optarg,"%lf",&LSTstep) !=1) {
					printf("couldn't get LSTstep\n\n");
					printHelp();
					exit(-1);
				}	
				break;
			}
			case 'n': {
				if (sscanf(optarg,"%d",&Nstep) !=1) {
					printf("couldn't get Nstep\n\n");
					printHelp();
					exit(-1);
				}	
				break;
			}
			case 'c': {
				if (sscanf(optarg,"%d",&nCycle) !=1) {
					printf("couldn't get nCycle\n\n");
					printHelp();
					exit(-1);
				}	
				break;
			}
			case 'r': {
				if (sscanf(optarg,"%d",&repeat) !=1) {
					printf("couldn't get repeat\n\n");
					printHelp();
					exit(-1);
				}	
				break;
			}			
		}
	}

	// computing the integer number of samples contained in the given duration
	sampleNumber=(int)(((stopAz-startAz)/(vel/60.0))/(interval/1000.0));

	// resizing the scan duration in order to make it be an integer number of samples
	// This duration is a solar time interval
	tCruise=(sampleNumber*interval)/1000.0;

	// computing the initial LST corresponding to the given Nstep
	LST0=LSTstep*Nstep;

	// computing the total duration of a forth+back couple of subscans
	// This duration is a solar time interval
	tScan=(tCruise+tOver)*2.0;
	gap=tOver/2.0;
	
	tf=new double[nCycle];
	tb=new double[nCycle];
	hhf=new int[nCycle];
	hhb=new int[nCycle];
	mmf=new int[nCycle];
	mmb=new int[nCycle];
	ssf=new double[nCycle];
	ssb=new double[nCycle];

	//computing the initial LST instant for every single forth and back subscan
	for (int i=0; i<nCycle; i++) {
		tf[i]=LST0+(i*tScan)*DT_SIDERSEC;//Scan duration is converted to sideral
		tb[i]=tf[i]+tScan*DT_SIDERSEC/2.0;
		hhf[i]=(int)(tf[i]/3600.0);
		mmf[i]=(int)(tf[i]/60.0-hhf[i]*60.0);
		ssf[i]= tf[i]-hhf[i]*3600.0-mmf[i]*60.0;
		hhf[i]=hhf[i]%24;
		hhb[i]=(int)(tb[i]/3600.0);
		mmb[i]=(int)(tb[i]/60.0-hhb[i]*60.0);
		ssb[i]= tb[i]-hhb[i]*3600.0-mmb[i]*60.0;
		hhb[i]=hhb[i]%24;
	}
    
	//opening the output files
	if (saveFile!="") {
		//creating the schedule file (.scd) and the subscan list file (.lis)
		sprintf(source,"%s",(const char *)saveFile);
		saveSched=saveFile+".scd";
		fileScd.open((const char *)saveSched,ios_base::out|ios_base::trunc);
		saveList=saveFile+".lis";
		fileLis.open((const char *)saveList,ios_base::out|ios_base::trunc);
		config=saveFile+".cfg";
		fileCfg.open((const char *)config,ios_base::out|ios_base::trunc);
		backend=saveFile+".bck";
		fileBck.open((const char *)backend,ios_base::out|ios_base::trunc);	
		if (((!fileScd.is_open())&&(!fileLis.is_open()))&&
			((!fileCfg.is_open())&&(!fileBck.is_open()))){
			printf("File opening failed\n");
		}
		else {
			//schedule header
			fileScd << "PROJECT:\tKNoWS\nOBSERVER:\tCarretti-Righini\n" 
					<< "SCANLIST:\t" << saveList << "\n"
					<< "MODE:\tLST\t" << repeat << "\n"
					<< "CONFIGLIST:\t" << config << "\n"
					<< "BACKENDLIST:\t" << backend << "\n\n";
			fileScd << "#ID\tsource\tstartLST\tdurat\tgap\tscan#\tconfig\tbckend\n";
			//subscan list header
			fileLis << "#ID\ttype\tLon1\t Lat1\tLon2\t Lat2\tframe\tsFrame\tgeom\tdescr\tdir\tduration\n";
			//filling the files
			sprintf(str,"%d\tOTF\t%7.3lf\t%7.3lf\t%7.3lf\t%7.3lf\tHOR\tHOR\tLAT\tSS\tINC\t%6.2lf\n"
					,1,startAz,elevation,stopAz,elevation,tCruise);
			fileLis <<  str;
			sprintf(str,"%d\tOTF\t%7.3lf\t%7.3lf\t%7.3lf\t%7.3lf\tHOR\tHOR\tLAT\tSS\tDEC\t%6.2lf\n"
					,2,stopAz,elevation,startAz,elevation,tCruise);
			fileLis <<  str;
			for (int k=0; k<nCycle; k++){
			sprintf(str,"%d\t%s_%03d\t%02d:%02d:%06.3lf\t%06.3lf\t%06.3lf\t%d\t%d\t%d\n",2*k+1,source,2*k+1,hhf[k],mmf[k],ssf[k],tCruise,gap,1,-1,-1);
			fileScd << str;
			sprintf(str,"%d\t%s_%03d\t%02d:%02d:%06.3lf\t%06.3lf\t%06.3lf\t%d\t%d\t%d\n",2*k+2,source,2*k+2,hhb[k],mmb[k],ssb[k],tCruise,gap,2,-1,-1);
			fileScd << str;
			}
		fileScd.close();
		fileLis.close();
		fileCfg.close();
		fileBck.close();
		}
	}
	//freeing memory
   	delete []tf;    
	delete []tb;
}
