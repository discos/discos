// $Id: TestTypeConversion.cpp,v 1.2 2010-08-16 13:25:28 a.orlati Exp $

#include "IRA"

int main(int argc, char *argv[])
{
	char input[255];
	ACS::TimeInterval interval;
	ACS::Time time;
	double rad;
	
	
	printf("Insert a longitude> ");
	scanf("%s",input);
	printf("you typed: %s\n",input);
	if (!IRA::CIRATools::longitudeToRad(input,rad)) printf("Conversion error \n");
	else {
		IRA::CString out;
		IRA::CIRATools::radToSexagesimalAngle(rad,out);
		printf("converted value: %s\n",(const char *)out);
		IRA::CIRATools::radToAngle(rad,out);
		printf("converted value: %s\n",(const char *)out);
		printf("converted value(rad): %lf\n",rad);
	}
	printf("Insert a latitude> ");
	scanf("%s",input);
	printf("you typed: %s\n",input);
	if (!IRA::CIRATools::latitudeToRad(input,rad)) printf("Conversion error \n");
	else {
		IRA::CString out;
		IRA::CIRATools::radToSexagesimalAngle(rad,out);
		printf("converted value: %s\n",(const char *)out);
		IRA::CIRATools::radToAngle(rad,out);
		printf("converted value: %s\n",(const char *)out);
		printf("converted value(rad): %lf\n",rad);
	}
	printf("Insert a Right Ascension> ");
	scanf("%s",input);
	printf("you typed: %s\n",input);
	if (!IRA::CIRATools::rightAscensionToRad(input,rad)) printf("Conversion error \n");
	else {
		IRA::CString out;
		IRA::CIRATools::radToHourAngle(rad,out);
		printf("converted value: %s\n",(const char *)out);
		IRA::CIRATools::radToAngle(rad,out);
		printf("converted value: %s\n",(const char *)out);
		printf("converted value(rad): %lf\n",rad);
	}
	printf("Insert a Declination> ");
	scanf("%s",input);
	printf("you typed: %s\n",input);
	if (!IRA::CIRATools::declinationToRad(input,rad)) printf("Conversion error \n");
	else {
		IRA::CString out;
		IRA::CIRATools::radToSexagesimalAngle(rad,out);
		printf("converted value: %s\n",(const char *)out);
		IRA::CIRATools::radToAngle(rad,out);
		printf("converted value: %s\n",(const char *)out);
		printf("converted value(rad): %lf\n",rad);
	}
	printf("Insert a Azimuth> ");
	scanf("%s",input);
	printf("you typed: %s\n",input);
	if (!IRA::CIRATools::azimuthToRad(input,rad)) printf("Conversion error \n");
	else {
		IRA::CString out;
		IRA::CIRATools::radToAngle(rad,out);
		printf("converted value: %s\n",(const char *)out);
		printf("converted value(rad): %lf\n",rad);
	}	
	printf("Insert a time interval> ");
	scanf("%s",input);
	printf("you typed: %s\n",input);
	if (!IRA::CIRATools::strToInterval(input,interval)) printf("Conversion error \n");
	else {
		IRA::CString out;
		IRA::CIRATools::intervalToStr(interval,out);
		printf("converted value: %s\n",(const char *)out);
	}
	printf("Insert a time epoch> ");
	scanf("%s",input);
	printf("you typed: %s\n",input);
	if (!IRA::CIRATools::strToTime(input,time)) printf("Conversion error \n");
	else {
		IRA::CString out;
		IRA::CIRATools::timeToStr(time,out);
		printf("converted value: %s\n",(const char *)out);
	}
	
}
