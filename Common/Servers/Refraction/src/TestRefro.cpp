/*
 * TestRefro.cpp
 *
 *  Created on: Oct 8, 2013
 *      Author: andrea
 */
#include <slalib.h>
#include <slamac.h>
#include <stdio.h>

#define HEIGHT 67.0
#define TEMP 13.8
#define LATITUDE 45.0
#define HUMIDITY 0.95
#define PRESSURE 1019.8

int main (int argc, char *argv[]) {

	printf("Refraction, Temp %lf(°),Pressure %lf(mbar),Humidity %lf(RH),HEIGHT %lf(m), LATITUDE %lf(°)\n",TEMP,PRESSURE,HUMIDITY,HEIGHT,LATITUDE);
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
	}


}
