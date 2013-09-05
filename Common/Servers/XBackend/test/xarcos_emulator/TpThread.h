#ifndef _TP_THREAD_H_
#define _TP_THREAD_H_

#include "Thread.h"
#include "Semaphore.h"
class Dbbc;   // forward declaration 
/*
 *
 * This class subclassing a class <i>Thread</i> with the task of managing the 
 * integration time flag. 
 */
class TpThread: public virtual Thread
{

public:
    /**
     * Constructor
     * @param groupSpec is pointer the only one GroupSpectrometer object of prog
ram
     */
     TpThread( Dbbc *aDbbc=0);
    
    /**
     * Destructor.
     */
    ~TpThread();
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
    Dbbc *dbbc;
    bool ciclo;
    Semaphore semaphore;
};
#include "Dbbc.h"
#endif  // _TP_THREAD_
