#include "ReceiversModule.h"


#define REWIND_TEST1 "MANUAL"
#define REWIND_TEST2 "FOO"

#define UPDATE_TEST1 "DISABLED"
#define UPDATE_TEST2 "OPTIMIZED"

#define POL_TEST1 "LEFT"
#define POL_TEST2 "DUMMY"

int main(int argc, char *argv[])
{
	//printf("%s\n",(const char *)Receivers::Definitions::mapRewindModes(Receivers::Definitions::mapRewindModes(Receivers::RCV_AUTO_REWIND)));
	Receivers::TRewindModes rewindMode;
	Receivers::TUpdateModes updateMode;
	Receivers::TPolarization polMode;


	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_AUTO_REWIND));
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_MANUAL_REWIND));
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_UNDEF_REWIND));
	if (Receivers::Definitions::map(REWIND_TEST1,rewindMode)) {
		printf("%s\n",(const char *)Receivers::Definitions::map(rewindMode));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)REWIND_TEST1);
	}
	if (Receivers::Definitions::map(REWIND_TEST2,rewindMode)) {
		printf("%s\n",(const char *)Receivers::Definitions::map(rewindMode));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)REWIND_TEST2);
	}

	printf("\n");
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_FIXED_UPDATE));
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_OPTIMIZED_UPDATE));
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_DISABLED_UPDATE));
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_UNDEF_UPDATE));

	if (Receivers::Definitions::map(UPDATE_TEST1,updateMode)) {
		printf("%s\n",(const char *)Receivers::Definitions::map(updateMode));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)UPDATE_TEST1);
	}
	if (Receivers::Definitions::map(UPDATE_TEST2,updateMode)) {
		printf("%s\n",(const char *)Receivers::Definitions::map(updateMode));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)UPDATE_TEST2);
	}

	printf("\n");
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_LCP));
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_RCP));
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_VLP));
	printf("%s\n",(const char *)Receivers::Definitions::map(Receivers::RCV_HLP));

	if (Receivers::Definitions::map(POL_TEST1,polMode)) {
		printf("%s\n",(const char *)Receivers::Definitions::map(polMode));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)POL_TEST1);
	}
	if (Receivers::Definitions::map(POL_TEST2,polMode)) {
		printf("%s\n",(const char *)Receivers::Definitions::map(polMode));
	}
	else {
		printf("Conversion Error: %s\n",(const char *)POL_TEST2);
	}

}
