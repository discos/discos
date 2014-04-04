#ifndef _SPECTROMETER
#define _SPECTROMETER

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Gianni Comoretto (comore@arcetri.astro.it)                       Creation                                                  
 */

#include "HardwareBlock.h"
#include "BbcFilter.h"

/** 
 *
 * Class based on Altera correlator
 */
class Spectrometer : public HardwareBlock {
   protected:
	double cycleTime;/*!< in seconds */
	int nCycles;/*!< integration time in cycles */
	double integrationTime;/*!< integration time in seconds */
	bool aborted;/*!< */
	bool active;/*!< */
	int nChans;	/*!<  Actual number of channels */
	int resolution;	/*!<  Resolution factor */
	BbcFilter * bbc;/*!< */

   private:
   /**
    * This method set integration time
    * @return error code
    */
   int SetIntTime();
   public:
   /**
    * This method set integration time.
    * Integration time, specified in seconds.
    * Rounded to nearest possible value.
    * Integration granularity depends on bandwidth, so it must be set
    * AFTER bandwidth.
    * @return actual integration time
    */
   double IntTime(double a_time);	
   
   /**
    * Integration time limited to 2^24 cycles by hardware
    * Each cycle lasts (nchans*tsample) and tsample = 1/band
    * Care must be taken not to overflow the integrator,
    * this may occur much sooner
    */
   double IntTime() const;
	
   /**
	 * Constructor
	 * @param intf 
	 * @param address,
	 * @param samplefreq 
	 * @param nchans
	 */
	Spectrometer (Cpld2Intf * intf, int address,double samplefreq=125.e6, int nchans=4096);
   
	/**
    * Destructor.
    */
	~Spectrometer() {}
	
	int Init();
	int Set();
	
	/**
	 * This method resets status bits if nec
	 */
	int ResetStatus();
	
	/**
	 * This method resets status bits if nec
	 */
	int Status(); 			
	
	/**
	 * This method start integration
	 */
	int Start();			
	
	/**
	 * This method stop integration
	 */
	int Stop();			
	
	/**
	 * This method abort integration
	 */
	int Abort();			
	
	/**
	 * This method check if new data are ready
	 */
	bool DataReady();		
	
	/**
	 * This method check if new data are i overflow
	 */
	bool Overflow();		
	
	/**
	 * This method reads buffer
	 */
	int GetResults(long data[],int size, int start=0);	
	
	/**
	 * This method connect to associate bbc
	 */
	void Bbc(BbcFilter * a_bbc)	{bbc = a_bbc; }
	
	/**
	 * This method set resolution factor
	 */
	int Resolution(int a_res);	// Set resolution factor
	
	/**
	 * This method get resolution factor
	 */
	int Resolution() const { return resolution; }
};

#endif
