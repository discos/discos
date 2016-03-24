#ifndef MUTEX_H_
#define MUTEX_H_

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
 * The Mutex class provides access serialization between threads.
 */

class Mutex
{
public:
    /**
     * \enum Attribute
     *
     * The Attribute enum defines a specific attribute for the Mutex
     * class. An attribute may be either Normal or Recursive. A normal
     * mutex may only be locked once by a thread. Attempts to lock it
     * more than once will result in a deadlock. A recursive mutex may
     * be locked multiple times by the same thread.
     */
    enum Attribute
    {
	Normal,
	Recursive
    };

    /**
     * \enum ErrorType
     *
     * Error types associated with a mutex.
     */
    enum ErrorType
    {
	NoError,		/*!< No error on the mutex */
	AlreadyLocked,	/*!< The mutex is already locked. */
	TryAgain,		/*!< Try locking again. */
	Invalid,		/*!<Invalid mutex. */
	Fault,			/*!< A fault occurred. */
	DeadLock,		/*!< The operation would result in deadlock */
	UnknownError	/*!< An unknown error occurred */
    };
    
    /** 
	 * Constructor.
	 * @param locked is the state to creation the mutex. The default is false.
	 * @param attr is Mutex::Recursive, a thread can lock the same mutex 
	 *	multiple times and the mutex won't be unlocked until a corresponding 
	 *	number of unlock() calls have been made. The default is Mutex::Normal.
      */
     Mutex(bool locked = false, Attribute attr = Normal);
     
     /**
 	  * Destructor.
 	  */
     ~Mutex();

    /*!
     * The lock method is used to lock the mutex. A thread locking
     * the mutex will have ownership of the lock until it is unlocked.
     * Any thread trying to subsequently lock the mutex will block
     * until the owning thread has unlocked it.
     */
    void lock();
    
    /**
     * The unlock method is used to release the ownership of a mutex.
     * Threads waiting on the locked mutex are re-woken and may attempt
     * to lock the mutex themselves.
     */
    void unlock();
    
    /**
     * The trylock method attempts to lock the mutex. 
     * If the mutex is already locked, an error is returned. 
     * @returns with the mutex in the locked state with the 
     * calling thread as its owner. 
     */
    bool trylock(){ return pthread_mutex_trylock(&_mutex); }

protected:
    pthread_mutex_t _mutex;	/*!< Pthread mutex object */
    pthread_t _id;	/*!< Id of thread which hold ownership */
    unsigned int _cnt;	/*!< Counter of recursive mutex */
    unsigned int _rec;	/*!< Is mutex recursive? */
};

#endif /*MUTEX_H_*/
