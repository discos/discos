#include "../include/XaSemaphore.h"

#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static void report_error(int code, const char *where, const char *what)
{ 
	char str[512]="";
    if (code != 0){
    	strcat(str,"\n ");
    	strcat(str, where);strcat(str," ");
    	strcat(str,what);strcat(str," ");
    	strcat(str,"failure");strcat(str," ");
        perror(str);
        }
}
//! \name Constructor
Semaphore::Semaphore(int num)
{
	int result=0;
	if (num <=0) num=0;
	n=num;
	result=pthread_mutex_init(&mut, NULL);
	report_error(result, "Semaphore Constructor","mutex init");
	result=pthread_cond_init(&cond, NULL);
	report_error(result, "Semaphore Constructor","cond init");
}
//! \name Destructor
Semaphore::~Semaphore()
{
	int result=0;
	
	result = pthread_mutex_destroy(&mut);
	report_error(result, "Semaphore Destructor","mutex destroy");
	result = pthread_cond_destroy(&cond);
	report_error(result, "Semaphore Destructor","cond destroy");
}

//
// Try to get <num> tokens
// 
int Semaphore::acquire(int num)
{
	int err =0;
	
	err=pthread_mutex_lock(&mut);
//
// If no all token are available must wait
// 
	while (n < num) {
	    num -= n;
	    n = 0;
	    pthread_cond_wait(&cond, &mut);
	} 
//
// Take remaining tokens
// and return
// 
	n -= num;	
	pthread_mutex_unlock(&mut);
	return err;
}

//
// Check if <num> tokens are available
// If they are, take it
// 
int Semaphore::CondWait(int num)
{
	int ok=true;
	pthread_mutex_lock(&mut);

	if (num <= n) 	n -= num;
	else	 	ok = false;

	pthread_mutex_unlock(&mut);
	return ok;
}
//
// Give <num> tokens
// 
int Semaphore::release(int num)
{
	int err = pthread_mutex_lock(&mut);
	n += num;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mut);
	return err;
}

