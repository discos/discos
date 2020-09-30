
#include "AntennaModule.h"


#define FRAME_TEST1 "HOR"
#define FRAME_TEST2 "DUMMY"

int main(int argc, char *argv[])
{
	Antenna::TCoordinateFrame frame;
	printf("galactic frame: %s\n",(const char *)Antenna::Definitions::map(Antenna::ANT_GALACTIC));
	printf("equatorial frame: %s\n",(const char *)Antenna::Definitions::map(Antenna::ANT_EQUATORIAL));
	printf("horizontal frame: %s\n",(const char *)Antenna::Definitions::map(Antenna::ANT_HORIZONTAL));
	if (Antenna::Definitions::map(FRAME_TEST1,frame)) {
		printf("%s maps to %s\n",FRAME_TEST1,(const char *)Antenna::Definitions::map(frame));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)FRAME_TEST1);
	}
	if (Antenna::Definitions::map(FRAME_TEST2,frame)) {
		printf("%s maps to %s\n",FRAME_TEST2,(const char *)Antenna::Definitions::map(frame));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)FRAME_TEST2);
	}
	return 0;
}
