#include "Offset.h"

COffset::COffset()
{
	reset();
}

COffset::~COffset()
{

}

void COffset::reset()
{
	userOffset.lon=userOffset.lat=0.0;
	scanOffset.lon=scanOffset.lat=0.0;
	systemOffset.lon=systemOffset.lat=0.0;
	systemOffset.frame=Antenna::ANT_HORIZONTAL;
	feedOffset.lon=feedOffset.lat=0.0;
	feedOffset.frame=Antenna::ANT_HORIZONTAL;
	userOffset.isSet=scanOffset.isSet=systemOffset.isSet=feedOffset.isSet=false;
}

//USER
void COffset::setUserOffset(const double& lon,const double& lat,const Antenna::TCoordinateFrame& frame)
{
	userOffset.lon=lon;
	userOffset.lat=lat;
	userOffset.frame=frame;
	userOffset.isSet=true;
}
void COffset::setUserOffset(const TOffset& off)
{
	userOffset.lon=off.lon;
	userOffset.lat=off.lat;
	userOffset.frame=off.frame;
	userOffset.isSet=true;
}
void COffset::resetUser()
{
	userOffset.isSet=false;
	userOffset.lon=userOffset.lat=0.0;
	userOffset.frame=Antenna::ANT_HORIZONTAL;
}

const TOffset& COffset::getUserOffset() const
{
	return userOffset;
}

TOffset& COffset::getUserOffset()
{
	return userOffset;
}

//SCAN
void COffset::setScanOffset(const double& lon,const double& lat,const Antenna::TCoordinateFrame& frame)
{
	scanOffset.lon=lon;
	scanOffset.lat=lat;
	scanOffset.frame=frame;
	scanOffset.isSet=true;
}
void COffset::setScanOffset(const TOffset& off)
{
	scanOffset.lon=off.lon;
	scanOffset.lat=off.lat;
	scanOffset.frame=off.frame;
	scanOffset.isSet=true;
}
void COffset::resetScan()
{
	scanOffset.isSet=false;
	scanOffset.lon=scanOffset.lat=0.0;
	scanOffset.frame=Antenna::ANT_HORIZONTAL;
}
const TOffset& COffset::getScanOffset() const
{
	return scanOffset;
}
TOffset& COffset::getScanOffset()
{
	return scanOffset;
}
//SYSTEM
void COffset::setSystemOffset(const double& lon,const double& lat)
{
	systemOffset.lon=lon;
	systemOffset.lat=lat;
	systemOffset.isSet=true;
}
void COffset::resetSystem()
{
	systemOffset.isSet=false;
	systemOffset.lon=systemOffset.lat=0.0;
	scanOffset.frame=Antenna::ANT_HORIZONTAL;
}

double COffset::getSystemAzimuth() const
{
	return systemOffset.lon;
}
double COffset::getSystemElevation() const
{
	return systemOffset.lat;
}

TOffset COffset::ephemGeneratorOffsets(bool &override) const
{
	TOffset out;
	if (userOffset.isSet && scanOffset.isSet) { // is both user and scan offset are set
		if (userOffset.frame==scanOffset.frame) {
			out.lon=userOffset.lon+scanOffset.lon;
			out.lat=userOffset.lat+scanOffset.lat;
			out.frame=scanOffset.frame;
			out.isSet=true;
			override=false;
		}
		else { // if the frames differ the scanOffset prevails
			out.lon=scanOffset.lon;
			out.lat=scanOffset.lat;
			out.frame=scanOffset.frame;
			out.isSet=true;
			override=true;
		}
	}
	else if (userOffset.isSet) {
		out.lon=userOffset.lon;
		out.lat=userOffset.lat;
		out.frame=userOffset.frame;
		out.isSet=true;
		override=false;
	}
	else if (scanOffset.isSet) {
		out.lon=scanOffset.lon;
		out.lat=scanOffset.lat;
		out.frame=scanOffset.frame;
		out.isSet=true;
		override=false;
	}
	else {
		out.lon=0.0;
		out.lat=0.0;
		out.frame=Antenna::ANT_HORIZONTAL;
		out.isSet=false;
		override=false;
	}
	return out;
}
