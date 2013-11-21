//
// Thread management routines.
// A thread is created by a constructor specifying a function returning
// an int, and, optionally, a stack size. All other parameters are
// defaulted.
// The thread is joined when it is deleted, and can be joined explicitly
// to recover the return value. After joining, it can only be deleted.
//
#include "../include/XaThread.h"

//
// Function called by the thread
//
void *Thread::body(void * arg)
{
    static_cast<Thread*>(arg)->startup(arg);
    return NULL;          // the "start_routine" must return a void *.
}

Thread::ErrorType  Thread::create_thread()
{
    if (pthread_create(&id, NULL, body, this) != 0) {
        return ThreadFailure;
    }
	if (_detached == Detached) pthread_detach (id);
	return NoError;
};



void Thread::startup(void *arg)
{
	_arg=arg;
	this->run();
}

Thread::Thread(Thread::DetachFlag detached)

{
    _detached = detached;
    start_error = this->create_thread();
}

Thread::~Thread()
{
    // make sure the thread is really finished, before we remove it data structures 
    this->wait();
}

