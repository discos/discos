/*
 * TestTTCArray.cpp
 *
 *  Created on: Jan 21, 2013
 *      Author: andrea
 */

#include "Definitions.h"
#include "DateTime.h"
#include "IRATools.h"
#include "TimeTaggedCircularArray.h"

#define GAP 100000
#define SIZE 50
#define ELEMENTS 10

using namespace IRA;

int main(int argc, char *argv[])
{
	TIMEVALUE now;
	TIMEVALUE current;
	CString out;
	double az,el;
	unsigned elem;
	//fill up array..........................
	IRA::CIRATools::getTime(now);
	CTimeTaggedCircularArray array(SIZE);
	array.empty();
	for (unsigned i=0;i<ELEMENTS;i++) {
		TIMEVALUE  time(now.value().value+GAP*i);
		array.addPoint(0.01+0.01*i,0.02+0.02*i,time);
	}
	//Testing......
	elem=array.elements();
	printf("Elements: %u\n",elem);
	for (int j=elem-1;j>=0;j--) {
		array.getPoint(j,az,el,current);
		CIRATools::timeToStr(current.value().value,out);
		printf("%u time, az, el: %s, %lf, %lf\n",j,(const char *)out,az,el);
	}
	//middle time
	TIMEVALUE mid(now.value().value+GAP*(elem/2)+GAP/2);
	array.selectPoint(mid.value().value,az,el);
	CIRATools::timeToStr(mid.value().value,out);
	printf("mid Time : time, az, el: %s, %lf, %lf\n",(const char *)out,az,el);
	// later time.......
	TIMEVALUE later((ACS::Time)10);
	array.selectPoint(later.value().value,az,el);
	CIRATools::timeToStr(later.value().value,out);
	printf("later Time : time, az, el: %s, %lf, %lf\n",(const char *)out,az,el);
	// earlier time.......
	TIMEVALUE early(now.value().value+GAP*(ELEMENTS));
	array.selectPoint(early.value().value,az,el);
	CIRATools::timeToStr(early.value().value,out);
	printf("earlier Time : time, az, el: %s, %lf, %lf\n",(const char *)out,az,el);
}
