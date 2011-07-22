// $Id: TestSiderealToUTC.cpp,v 1.1 2008-12-12 13:51:22 a.orlati Exp $


#include "Definitions.h"
#include "DateTime.h"
#include "IRATools.h"
#include "Site.h"
#include <slamac.h>

using namespace IRA;

int main(int argc, char *argv[]) 
{
	TIMEVALUE now;
	CSite site;
	CDateTime ut1,ut2;
	CDateTime start,lst;
	// Medicina coordinates
	site.geodeticCoordinates(11.64693*DD2R,44.52049*DD2R,28.0);
	for(;;) {
		CIRATools::getTime(now);
		start=CDateTime(now);
		printf("UTC: %d-%u-%02d:%02d:%02d.%03d\n",start.getYear(),start.getDayOfYear(),start.getHours(),start.getMinutes(),start.getSeconds(),
				start.getMilliSeconds());
		lst=start.LST(site);
		printf("LST: %02d:%02d:%02d.%03d\n",lst.getHours(),lst.getMinutes(),lst.getSeconds(),lst.getMilliSeconds());
		printf("LST: %lf\n",lst.getDayRadians());
		
		if (CDateTime::sidereal2UT(ut1,ut2,lst.getDayRadians(),site,start.getYear(),start.getMonth(),start.getDay())) {
			printf("UTC: %d-%u-%02d:%02d:%02d.%03d\n",ut1.getYear(),ut1.getDayOfYear(),ut1.getHours(),ut1.getMinutes(),ut1.getSeconds(),
					ut1.getMilliSeconds());
		}
		else {
			printf("UTC1: %d-%u-%02d:%02d:%02d.%03d\n",ut1.getYear(),ut1.getDayOfYear(),ut1.getHours(),ut1.getMinutes(),ut1.getSeconds(),
				ut1.getMilliSeconds());
			printf("UTC2: %d-%u-%02d:%02d:%02d.%03d\n",ut2.getYear(),ut2.getDayOfYear(),ut2.getHours(),ut2.getMinutes(),ut2.getSeconds(),
								ut2.getMilliSeconds());
		}
		printf("\n\n");
		CIRATools::Wait(5,0);
	}
}
