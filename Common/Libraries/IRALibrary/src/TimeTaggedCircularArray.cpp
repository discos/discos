// $Id: TimeTaggedCircularArray.cpp,v 1.9 2010-09-08 15:16:44 a.orlati Exp $
#include "TimeTaggedCircularArray.h"
#include "String.h"
#include "IRATools.h"
#include <slamac.h>
#include <slalib.h>

using namespace IRA;

#define _TOLLERENCE 1.0

CTimeTaggedCircularArray::CTimeTaggedCircularArray(const unsigned& positions,
		bool isRadians) : m_array(NULL), m_size(positions)
{
	m_array=new TArrayRecord[m_size];
	m_lastAzimuth=m_lastElevation=0.0;
	m_isRad=isRadians;
	m_lastTime.value().value=0;
}

CTimeTaggedCircularArray::~CTimeTaggedCircularArray() 
{
	if (m_array!=NULL) delete[] m_array;
}

void CTimeTaggedCircularArray::empty()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_head=m_free=0;	
}

bool CTimeTaggedCircularArray::addPoint(const double& azimuth,const double& elevation,const TIMEVALUE& time)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE *tmp;
	if (isEmpty()) {
	}
	else {
		// a sample with the time mark smaller than the last time mark in the array is neglected
		if (time<=pop().time) return false;
	}
	tmp=const_cast<TIMEVALUE*>(&time);	
	push(azimuth,elevation,tmp);		
	m_lastAzimuth=azimuth;
	m_lastElevation=elevation;
	CIRATools::timeCopy(m_lastTime,time);
	return true;
}

bool CTimeTaggedCircularArray::getPoint(unsigned pos,double& azimuth,double& elevation,TIMEVALUE& time) const
{
	baci::ThreadSyncGuard guard(&m_mutex);
	unsigned ss,pp;
	if (isEmpty()) return false;
	ss=elements();
	if (pos>=ss) return false;
	pp=(m_head+pos)%m_size;
	azimuth=m_array[pp].azimuth;
	elevation=m_array[pp].elevation;
	time.value(m_array[pp].time);
	return true;
}

const double& CTimeTaggedCircularArray::getLastAzimuth() const
{
	baci::ThreadSyncGuard guard(&m_mutex);
	return m_lastAzimuth;
}

const double& CTimeTaggedCircularArray::getLastElevation() const
{
	baci::ThreadSyncGuard guard(&m_mutex);
	return m_lastElevation;
}

const TIMEVALUE& CTimeTaggedCircularArray::getLastTime() const
{
	baci::ThreadSyncGuard guard(&m_mutex);
	return m_lastTime;
}

unsigned CTimeTaggedCircularArray::elements() const
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (isEmpty()) return 0;
	if (m_head>m_free) return (m_size-m_head)+m_free;
	else return m_free-m_head;
}

void CTimeTaggedCircularArray::purge(const TIMEVALUE& time)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	unsigned ss=elements();
	unsigned pp=0;
	for (unsigned i=0;i<ss;i++) {
		pp=(m_head+i)%m_size;
		if (time<=m_array[pp].time) { m_head=pp; return; }
	}
	m_head=pp+1;
}

void CTimeTaggedCircularArray::addOffsets(const double& azOff,const double& elOff,const TIMEVALUE& time)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	unsigned ss=elements();
	unsigned pp=0;
	for (unsigned i=0;i<ss;i++) {
		pp=(m_head+i)%m_size;
		if (time<m_array[pp].time) { m_array[pp].azimuth+=azOff; m_array[pp].elevation+=elOff; }
	}
}

void CTimeTaggedCircularArray::selectPoint(const TIMEVALUE& time,double& azimuth,double& elevation) const
{
	baci::ThreadSyncGuard guard(&m_mutex);
	unsigned ss=elements();
	unsigned pp=0;
	TArrayRecord m1,m2;
	long long dx1,dx2;
	double slope;
	int  i;
	double maxEdge=360.0-_TOLLERENCE;
	double minEdge=_TOLLERENCE;

	if (ss<=1) {
		azimuth=m_lastAzimuth;
		elevation=m_lastElevation;
		return;
	}
	for (i=ss-1;i>=0;i--) { //at least two elements are present
		pp=(m_head+i)%m_size;
		if (time==m_array[pp].time) { //the requested time is there
			azimuth=m_array[pp].azimuth; 
			elevation=m_array[pp].elevation;
			return;
		}
		else if (time>m_array[pp].time) {
			if ((unsigned)i==ss-1) {  // the requested time is greater than all other entries in the array
				m2=m_array[(m_head+ss-1)%m_size];
				m1=m_array[(m_head+ss-2)%m_size];
				break;
			}
			else { // the requested time is in between two entries
				m1=m_array[pp];
				m2=m_array[(m_head+i+1)%m_size];
				break;
			}
		}
	}
	// in that case the requested time is smaller than all other points in the vector
	if (pp==m_head && time<m_array[m_head].time) {
		/*m1=m_array[m_head];
		m2=m_array[(m_head+1)%m_size];*/
		azimuth=m_array[m_head].azimuth;
		elevation=m_array[m_head].elevation;
		return;
	}
	// now compute the linear fit
	TIMEVALUE second(m2.time);
	TIMEVALUE first(m1.time);
	dx1=CIRATools::timeSubtract(second,first);
	dx2=CIRATools::timeSubtract(time,first);
	// we need to take into account that 359.999 and 0.001 are close and not separated by a round angle
	if (m_isRad) {
		maxEdge*=DD2R;
		minEdge*=DD2R;
	}
	if (m1.azimuth<m2.azimuth) {
		if ((m1.azimuth<minEdge)&&(m2.azimuth>maxEdge)) {
			if (m_isRad) m1.azimuth+=D2PI;
			else m1.azimuth+=360;
		}		
	}
	else {
		if ((m2.azimuth<minEdge)&&(m1.azimuth>maxEdge)) {
			if (m_isRad) m2.azimuth+=D2PI;
			else m2.azimuth+=360;
		}		
	}
	// now compute the result......
	slope=(m2.azimuth-m1.azimuth)/(double)dx1;
	azimuth=slope*dx2+m1.azimuth;
	//printf("%lf\n",azimuth);
	//printf("%lf\n",slope*(double)dx2+m1.azimuth);
	if (m_isRad) {
		azimuth=slaDranrm(azimuth);
	}
	else {
		 double w=dmod(azimuth,360.0);
		 azimuth=(w>=0.0)?w:w+360.0;		
	}
	slope=(m2.elevation-m1.elevation)/(double)dx1;
	elevation=slope*dx2+m1.elevation;
}

void CTimeTaggedCircularArray::averagePoint(const TIMEVALUE& startTime,const TIMEVALUE& stopTime,double& azimuth,double& elevation) const
{
	baci::ThreadSyncGuard guard(&m_mutex);
	unsigned scroller;
	unsigned elem=elements();
	double avgAz=0.0,avgEl=0.0;
	long samples=0;
	if (elem<=1) {
		azimuth=m_lastAzimuth;
		elevation=m_lastElevation;
		return;
	}
	for (int  i=elem-1;i>=0;i--) { //at least two elements are present
		scroller=(m_head+i)%m_size;
		if ((startTime<=m_array[scroller].time) && (stopTime>=m_array[scroller].time)) {
			samples++;
			avgAz+=m_array[scroller].azimuth;
			avgEl+=m_array[scroller].elevation;
		}
	}
	if (samples>0) {
		azimuth=avgAz/(double)samples;
		elevation=avgEl/(double)samples;
	}
	else { //the data set could not be selected.....the requested interval is
		if (startTime>m_array[(m_head+elem-1)%m_size].time) { //....later then use the last know point
			azimuth=m_array[(m_head+elem-1)%m_size].azimuth;
			elevation=m_array[(m_head+elem-1)%m_size].elevation;
		}
		else if (stopTime<m_array[m_head%m_size].time) { //....sooner the use the first point
			azimuth=m_array[m_head%m_size].azimuth;
			elevation=m_array[m_head%m_size].elevation;			
		}
	}
	if (m_isRad) {
		azimuth=slaDranrm(azimuth);
	}
	else {
		 double w=dmod(azimuth,360.0);
		 azimuth=(w>=0.0)?w:w+360.0;		
	}
}

// PRIVATE METHODS

void CTimeTaggedCircularArray::push(const double& azimuth,const double& elevation,TIMEVALUE* time)
{
	m_array[m_free].azimuth=azimuth;
	m_array[m_free].elevation=elevation;
	m_array[m_free].time=time->value();
	m_free=(m_free+1)%m_size;	
	if (m_free==m_head) m_head=(m_head+1)%m_size;		
}

CTimeTaggedCircularArray::TArrayRecord& CTimeTaggedCircularArray::pop()
{
	if (m_free==0) {
		return m_array[m_size-1];
	}
	else {
		return m_array[m_free-1];
	}
}


