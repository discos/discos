
#include <slalib.h>
#include <slamac.h>
#include <stdio.h>
#include <IRA>
#include <cstdlib>
#include <getopt.h>

using namespace IRA;

#define FREQUENCY 5700.0
#define HEIGHT 120.0
#define SITELAT 44.52048889
#define TEMPERATURE 16.0
#define PRESSURE 1019.0
#define HUMIDITY 0.60


void printHelp()
{
	printf("\n");
	printf("Refraction [-h] [-f frequency][-a height] [-l latitude] [-t temperature] [-p pressure] [-y humidity]\n");
	printf("\n");
	printf("Computes the refraction as it would be computed by the control software from 0 to 90°\n");
	printf("\n");
	printf("[-h]\t\t\tShows this help\n");
	printf("[-f frequency]\t\tProvides the frequency in MHz, default is (%lfMHz)\n",FREQUENCY);
	printf("[-a height]\t\tTelescope height above the sea level, default is (%lf)m.\n",HEIGHT);
	printf("[-l latitude]\t\tTelescope latitude in degrees, default is (%lf)\n",SITELAT);
	printf("[-t temperature]\tTemperature in °C at the ground level, default is (%lf°C)\n",TEMPERATURE);
	printf("[-d pressure]\t\tPressure in millibars at the ground level, default is (%lfmBar)\n",PRESSURE);
	printf("[-y humidity]\t\tHumidity percentage measured at the ground, default is (%lf)\n",HUMIDITY);
	printf("\n");
}


int main (int argc, char *argv[]) {
	double latitude,height,temp,pressure,humidity,frequency;
	int out;
	frequency=FREQUENCY;
	height=HEIGHT;
	latitude=SITELAT;
	temp=TEMPERATURE;
	pressure=PRESSURE;
	humidity=HUMIDITY;
	while ((out=getopt(argc,argv,"hf:a:l:t:p:y:"))!=-1) {
		switch (out) {
			case 'h': {
				printHelp();
				exit(0);
				break;
			}
			case 'f': {
				frequency=atof(optarg);
				break;
			}
			case 'a': {
				height=atof(optarg);
				break;
			}
			case 'l': {
				latitude=atof(optarg);
				break;
			}
			case 't': {
				temp=atof(optarg);
				break;
			}
			case 'p': {
				pressure=atof(optarg);
				break;
			}
			case 'y': {
				humidity=atof(optarg);
				break;
			}
		}
	}
	double tdk = temp+273.0;
	double wl = -(frequency/1000.0);
	double phi = latitude*DD2R; // radians
	double tlr = 0.0065;
	double eps = 1e-8;
	double corZenithDistance;
	double obsZenithDistance;
	printf("Temperature: %lf(°C), Pressure: %lf(mbar), Humidity: %lf(%%), Height: %lf(m), Latitude %lf(°), Frequency %lfMHz\n",
	 temp,pressure,humidity,height,latitude,frequency);
	for (unsigned i=0;i<=90;i+=5) {
		obsZenithDistance=(double)(90-i);
		obsZenithDistance*=DD2R;	
		slaRefro(obsZenithDistance,height,tdk,pressure,humidity,wl,phi,tlr,eps,&corZenithDistance);
		printf("%lf\t%lf\n",(double)i,corZenithDistance*DR2D);
	}
}