#include "../include/XaMutex.h"

#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include <ctype.h>

static void report_error(int code, const char *where, const char *what)
{ 
	char str[512]="";
    if (code != 0){
    	strcat(str, where);strcat(str," ");
    	strcat(str,what);strcat(str," ");
    	strcat(str,"failure");strcat(str," ");
        perror(str);
    }
}
//! \name Constructor
Mutex::Mutex(bool locked, Attribute attr )
{	
	int result=0;

	pthread_mutexattr_t m_attr;
	_id = 0;
	_cnt = 0;
	_rec = 0;
	result = pthread_mutexattr_init(&m_attr);
	report_error(result, "Mutex Constructor", "mutex attr_init");
	if (attr != Normal) {
		switch (attr) {
			case Recursive:
			    _rec = 1;
			        result = pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
			        report_error (result, "Mutex Constructor", "mutex attr_settype");
			    break;
			default:
			    break;
		}
	}
	result = pthread_mutex_init(&_mutex, &m_attr);
	report_error(result, "Mutex Constructor", "mutex init");
	result = pthread_mutexattr_destroy(&m_attr); 
	report_error(result, "Mutex Constructor", "mutex attr_destroy");
	if (locked)	this->lock();

}

//! \name Destructor
Mutex::~Mutex()
{
	int result = pthread_mutex_destroy(&_mutex);
	report_error(result, "Mutex Destructor","mutex destroy");
}

void Mutex::lock()
{
	if (_rec) {
	    if (_id == pthread_self()) {      // we already have the look
			_cnt++;
			return;
	    }
	    pthread_mutex_lock(&_mutex);
	    _id = pthread_self();
	    _cnt = 1;
	}
	else {
	    pthread_mutex_lock(&_mutex);
	}
}

    /*!
     * The unlock method is used to release the ownership of a mutex.
     * Threads waiting on the locked mutex are re-woken and may attempt
     * to lock the mutex themselves.
     */
void Mutex::unlock()
{
	if (_rec) {
	    _cnt--;
	    if (!_cnt) {
			_id = 0;                    // hopefully 0 is not a valid thread id  
			pthread_mutex_unlock(&_mutex);
	    }
	}
	else {
	    pthread_mutex_unlock(&_mutex);
	}
}

