/*
 * ManagementModule.cpp
 *
 *  Created on: Jan 9, 2015
 *      Author: andrea
 */

#include "ManagementModule.h"

#define SCANTYPES_TEST1 "SKYDIP"
#define SCANTYPES_TEST2 "ZOOT"

int main(int argc, char *argv[])
{
	Management::TScanTypes scan;
	printf("%s\n",(const char *)Management::Definitions::map(Management::MNG_OTF));
	printf("%s\n",(const char *)Management::Definitions::map(Management::MNG_OTFC));
	printf("%s\n",(const char *)Management::Definitions::map(Management::MNG_SKYDIP));
	printf("%s\n",(const char *)Management::Definitions::map(Management::MNG_SIDEREAL));
	if (Management::Definitions::map(SCANTYPES_TEST1,scan)) {
		printf("%s\n",(const char *)Management::Definitions::map(scan));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)SCANTYPES_TEST1);
	}
	if (Management::Definitions::map(SCANTYPES_TEST2,scan)) {
		printf("%s\n",(const char *)Management::Definitions::map(scan));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)SCANTYPES_TEST2);
	}
	return 0;
}
