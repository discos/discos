/*************************************************************/
/* Use the files in test directory as input */
/* Also provide the elevation */
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include <slalib.h>
#include <slamac.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#define HEIGHT 67.0
#define LATITUDE 45.0
#define WAVELEN 0.036
using namespace std;

int main (int argc, char *argv[]) {

	std::ifstream in;
	double temp,press,hum;
	double hm;
	double tdk;
	double wl;
	double phi;
	double tlr = 0.0065;
	double eps = 1e-8;
	double elev;
	double obsZenithDistance,corZenithDistance;

	in.open(argv[1]);

	cout << argv[1] << endl;
	
	elev=atof(argv[2]);

	cout << elev <<  endl;

	while (!in.eof()) {
		in >> temp >> press >> hum;

		obsZenithDistance=(90-elev)*DD2R;

		hm = HEIGHT; // meters
		tdk = temp+273.0;
		wl = WAVELEN*1000000; //micro meters
		phi = LATITUDE*DD2R; // radians		
		slaRefro(obsZenithDistance, hm, tdk, press, hum/100, wl, phi, tlr, eps, &corZenithDistance);
		corZenithDistance*=DR2D;
		cout << temp << " " << press << " " << hum << " " << corZenithDistance << endl;
		//cout << corZenithDistance << endl;

	}

	/*printf("Refraction, Temp %lf(°),Pressure %lf(mbar),Humidity %lf(RH),HEIGHT %lf(m), LATITUDE %lf(°)\n",TEMP,PRESSURE,HUMIDITY,HEIGHT,LATITUDE);
	for (unsigned i=0;i<=90;i+=5) {
		double obsZenithDistance=(double)(90-i);
		obsZenithDistance*=DD2R;
		double corZenithDistance;
		double tdk = TEMP + 273.0;
		double wl = 50000.0;
		double phi = LATITUDE*DD2R; // radians
		double tlr = 0.0065;
		double eps = 1e-8;
		slaRefro(obsZenithDistance, HEIGHT, tdk, PRESSURE, HUMIDITY, wl, phi, tlr, eps, &corZenithDistance);
		printf("%lf\t%lf\n",(double)i,corZenithDistance*DR2D);
	}*/


}
