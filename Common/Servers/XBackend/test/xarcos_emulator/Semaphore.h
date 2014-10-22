#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Liliana Branciforti(bliliana@arcetri.astro.it)  01/10/2009      Creation                                                  
 */

#include <pthread.h>

/** 
 * The Semaphore class provides a general counting semaphore.
 */

class Semaphore
{
	int n;/*!< Counter of semaphore object*/
	pthread_mutex_t mut; /*!< Pthread mutex object */
	pthread_cond_t cond; /*!< Pthread condition object */

public:
   /** 
	* Constructor.
	* @param num is the state to creation the mutex. The default is 0.
	*/	
	Semaphore(int num=0);
    
    /**
	 * Destructor.
	 */
	~Semaphore();
	
	 /** 
	  * The available method returns the number of resources currently available to the semaphore.
	  * @return a integer number of resources available.
	  */
	int available() const
		{return n;} 
	
	/**
	 * The acquire method acquires n resources guarded by the semaphore.
	 * @param n is number of resources acquire
	 * If n > available(), this call will block until enough resources are available.
	*/		
	int acquire(int n=1);
	
	/**
	 * The tryAcquire method tries to acquire n resources guarded by the semaphore and returns true on success. 
	 * @param n is number of resources acquire
	 * If available() < n, this call immediately returns false without acquiring any resources.
	*/
	int tryAcquire(int n=1);
	
	/**
	 * The release method releases n resources guarded by the semaphore.
	 * @param n is number of resources relese
	 */
	int release(int n=1); 
	int CondWait(int n=1);
};
#endif // _SEMAPHORE_H_
