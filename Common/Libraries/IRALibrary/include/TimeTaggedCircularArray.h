#ifndef TIMETAGGEDCIRCULARARRAY_H_
#define TIMETAGGEDCIRCULARARRAY_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TimeTaggedCircularArray.h,v 1.7 2010-09-08 15:16:43 a.orlati Exp $	    												  */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 27/04/2007      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 31/01/2008      changed the algorithm of selectPoint()                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 31/01/2008      fixed a bug in selectPoint() that causes  problem when longitude crosses the 360  */
/* Andrea Orlati(aorlati@ira.inaf.it) 08/09/2010      added the averagePoint method */
/* G. Carboni(giuseppe.carboni@inaf.it) 26/07/2019    the class is now thread safe                      */

#include <acstimeEpochHelper.h>
#include "Definitions.h"

namespace IRA {

/**
 * This class is used to store and manipulate a bunch of coordinates (Point) arranged and sorted according to marks of time.
 * In order to keep the array sorted is not possible to add point that are earlier than the last inserted point. 
 * A sample (the pair point plus time mark) cannont be removed from the array esplicitally, but:
 * @arg \c If the array is full and a new sample is inserted the older point is removed.
 * @arg \c The user can esplicitally clear the whole vector (<i>empty()</i>).
 * @arg \c the user can purge sample based on time, that means that all the point with a time mark older that certain time will be removed (<i>purge</i>)
 * The previous listed items garantee that the array is never full and it is always sorted towards increasing values of time.
*/
class CTimeTaggedCircularArray {
public:
	/**
	 * Constructor.
	 * It construct the buffer with the specified number of elements. The array cannot be resized.
	 * @param positions the size of the buffer.
	 * @param isRadians specifiesif the coordinates are to be interpreted as radians(truet) or degrees(false)
	*/
	CTimeTaggedCircularArray(const unsigned& positions,bool isRadians=true);
	
	/**
	 * Destructor.
	*/
	~CTimeTaggedCircularArray();
	
	/**
	 * It appends one sample(azimuth,elevation,time) to the array. If a sample is earlier that the last inserted sample the triplet is
	 * neglected. In this way the array is garanteed to always sorted for increasing time.
	 * @param azimuth the azimuth coordinate
	 * @param elevation the elevation coordinate
	 * @param time the time mark of the point
	 * @return true if the sample has been added, false otherwise
	*/
	bool addPoint(const double& azimuth,const double& elevation,const TIMEVALUE& time);
	
	/**
	 * This method could be use to scroll all the data points of the array.
	 * @param pos the position of the point that must be returned. A zero indicates the first element of the array, 1 the second and so on....
	 * @param azimuth the azimuth coordinate of the returned data point
	 * @param elevation the elevation coordinate of the returned data point
	 * @param time the time stamp associated to the data point
	 * @return false if the array contained no more others data points. 
	*/ 
	bool getPoint(unsigned pos,double& azimuth,double& elevation,TIMEVALUE& time) const;
	
	/**
	 * Used to get the azimuth of the last data point. If the array is empty the result is unpredictable.
	 * @return the reference to the azimuth coordinate of the last inserted data point
	*/ 
	const double& getLastAzimuth() const;

	/**
	 * Used to get the elevation of the last data point. If the array is empty the result is unpredictable.
	 * @return the reference to the elevation coordinate of the last inserted data point
	*/ 	
	const double& getLastElevation() const;

	/**
	 * Used to get the timestamp of the last data point. If the array is empty the result is unpredictable.
	 * @return the reference to the TIMEVALUE of the last inserted data point
	*/ 	
	const TIMEVALUE& getLastTime() const;
	
	/**
	 * It clears the contents of the array and resets the counters.
	*/ 
	void empty();
	
	/**
	 * @return true if the Array has no element
	*/
	bool isEmpty() const { return (m_head==m_free); }
	
	/**
	 * @return the number of samples in the array.
	*/
	unsigned elements() const;
	
	/**
	 * This method takes charge of purge samples that have a time mark older than the one specified as argument.
	 * @param time time threshold, if a point in the array is earlier than that value is removed 
	*/
	void purge(const TIMEVALUE& time);
	
	/**
	 * This method adds an offset to every point in the array that are later than a given time mark.
	 * @param azOff offset to be added to all azimuth coordinates
	 * @param elOff offset to be added to all elevation coordinates
	 * @param time time threshold
	*/
	void addOffsets(const double& azOff,const double& elOff,const TIMEVALUE& time);
	
	/**
	 * This method can be used to get a point given a mark of time. If such a point does not exist
	 * (i.e no sample with the requested time) the routine tries to estimate it using a linear fit between 
	 * the two nearest known points that contain it. In that case at least two entries in the array are required.
	 * If  one or less point is present it is returned  the last inserted point. 
	 * @param time the mark of time used to select the point
	 * @param azimuth azimuth coordinate of the point
	 * @param elevation elevation coordinate of the point 
	 */
	void selectPoint(const TIMEVALUE& time,double& azimuth,double& elevation) const;
	
	/**
	 * This method can be used to get a point given two marks of time. The point is the result of the average of all points included between the two marks.
	 * If  one or less point is present it is returned  the last inserted point.
	 * If a dataset of points could not be selected (because the no times were included inside the given time range) the nearer known point is returned.
	 * @param startTime the mark of time used to select the beggining of the point set to be averaged
	 * @param stopTime the mark of time used to select the end of the point set to be averaged  
	 * @param azimuth resulting longitude point
	 * @param elevation resulting latitude point 
	 */	
	void averagePoint(const TIMEVALUE& startTime,const TIMEVALUE& stopTime,double& azimuth,double& elevation) const;
	
protected:
	
	typedef struct {
		double azimuth;
		double elevation;
		acstime::Epoch time;
	} TArrayRecord; 
	/** the array containing the samples */
	TArrayRecord *m_array;
	/** array size */
	unsigned m_size;
	/** beggining of the array */
	unsigned m_head;
	/** first free position in the array */
	unsigned m_free;
	double m_lastAzimuth;
	double m_lastElevation;
	TIMEVALUE m_lastTime;
	bool m_isRad;
	/**
	 * Adds one element to the array and updates the counters. if the array is full the first element will be replaced by the new one.
	*/
	void push(const double& azimuth,const double& elevation,TIMEVALUE* time);
	/**
	 * Gets the reference to the last inserted element. The element is not removed or the counter updated. 
	*/
	TArrayRecord& pop();
private:
	CTimeTaggedCircularArray(const CTimeTaggedCircularArray&);
	const CTimeTaggedCircularArray& operator =(const CTimeTaggedCircularArray& src);
	mutable BACIMutex m_mutex;
};
	
}

#endif /*TIMETAGGEDCIRCULARARRAY_H_*/

