#ifndef _INT_THREAD_H_
#define _INT_THREAD_H_

#include "Thread.h"
#include "Semaphore.h"
class FftSpectrometer;   // forward declaration 
/*
 *
 * This class subclassing a class <i>Thread</i> with the task of managing the 
 * integration time flag. 
 */
class IntThread: public virtual Thread
{

public:
    /**
     * Constructor
     * @param groupSpec is pointer the only one GroupSpectrometer object of prog
ram
     */
     IntThread( FftSpectrometer *aSpec=0);
    
    /**
     * Destructor.
     */
    ~IntThread();
    /**
	 * This method shall carry out the actual recovery of the data of an 
	 * single integration  
     */
    void run();
    
    /**
	 * This method switches off the loop of thread
	 */  
    void stop();
    
    /**
	 * This method switches on the loop of thread
	 */  
    void start();

  private:
    FftSpectrometer *spectrometer;
    bool ciclo;
    Semaphore semaphore;
};
#include "FftSpectrometer.h" // now that TpThread is fully defined, can define 
#endif  // _INT_THREAD_H_
