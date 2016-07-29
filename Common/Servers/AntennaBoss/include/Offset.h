#ifndef OFFSET_H_
#define OFFSET_H_


/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                              when             What                                        */
/* Andrea Orlati(aorlati@ira.inaf.it)  09/06/2016   Creation                                                     */

#include <AntennaDefinitionsS.h>

class TOffset {
public:
	double lon;
	double lat;
	Antenna::TCoordinateFrame frame;
	bool isSet;
} ;

/**
 * This class handles all the supported coordinate offsets. This class is not thread safe.
 * Feed offset are not implemented yet.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Orlati Andrea</a>
 * Istituto di Radioastronomia, Italia
 * <br>
 */
class COffset
{
public:
	COffset();
	~COffset();
	void reset();

	/*void setUserOffset(const double& lon,const double& lat,const Antenna::TCoordinateFrame& frame);
	void setUserOffset(const TOffset& off);
	void resetUser();
	bool isUserSet() const { return userOffset.isSet; }
	const TOffset& getUserOffset() const;
	TOffset& getUserOffset();*/

	void setScanOffset(const double& lon,const double& lat,const Antenna::TCoordinateFrame& frame);
	void setScanOffset(const TOffset& off);
	void resetScan();
	bool isScanSet() const { return scanOffset.isSet; }
	const TOffset& getScanOffset() const;
	TOffset& getScanOffset();

	void setSystemOffset(const double& lon,const double& lat);
	void resetSystem();
	bool isSystemSet() const { return systemOffset.isSet; }
	double getSystemAzimuth() const;
	double getSystemElevation() const;
	
	void setFeedOffset(const double& lon,const double& lat);
	void resetFeed();
	bool isFeedSet() const { return feedOffset.isSet; }
	double getFeedAzimuth() const;
	double getFeedElevation() const;

	/**
	 * This function returns the offsets setup for the ephemeris generator components. The offset returned are the
	 * combination of user and scan offsets. If the frames of both offsets match, the result is the sum of both the
	 * offset, otherwise the scan offset prevails.
	 * @return the offset structure containing the resulting offset
	 * @param true if the scan user offset has been masked by the scan offset
	 */
	//TOffset ephemGeneratorOffsets(bool &override) const;
	
	/**
	 * This list of methods is required to manage the correction to be applied and to be compensated when commanding and reading the coordinate to/from the telescope. 
	 * Essentially the latter set of methods differs from the former for the feed offset. This offset has to be applied when forming the commanded coordinate, but 
	 * not compensated when the reading out the observed coordinate (the reference coordinated are always referred to the central beam, per convenction).  
	*/
	double getAzimuthCorrection() const;
	double getElevationCorrection() const;
	double getAzimuthCompensation() const;
	double getElevationCompensation() const;
	
	COffset& operator= (const COffset & obj);

private:
	TOffset scanOffset;
	TOffset systemOffset;
	TOffset feedOffset;
};




#endif /* OFFSET_H_ */
