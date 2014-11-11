#ifndef _THREAD_H_
#define _THREAD_H_
/**
 *
 * Osservatorio Astrofisico di Arcetri
 *
 * This code is under GNU General Public Licence (GPL).
 *
 *  Who                                        when            What
 * Liliana Branciforti(bliliana@arcetri.astro.it)  01/10/2009      Creation
 */
#include <pthread.h>

/**
 * The Thread class provides management of threads.
 */
class Thread
{
public:
  /**
     * \enum DetachFlag
     *
     * A thread may either be created Detached or NotDetached (attached)
     * to its parent thread. The DetachFlag is used to determine the
     * threads behavior when it exits.
     */
    enum DetachFlag
    {
    Detached,    /*!< Detached threads are capable of
          reclaiming their resources on exit.*/
    NotDetached    /*!< Attached threads must be re-joined with
          the parent thread, allowing on-exit synchronization.*/
    };

    /**
     * \enum ErrorType
     *
     * Error types associated with the Thread class.
     */
    enum ErrorType
    {
    NoError,    /*!<No error on the thread */
    ThreadFailure,    /*!< The thread has failed */
    InvalidPriority,  /*!< The thread was given an invalid priority */
    MutexAlreadyLocked,  /*!< Mutex is already locked */
    NotStarted,    /*!<The thread has not started */
    UnknownError    /*!< Unknown error. */
    };

    /**
    * Constructor.
    * @param detached is the tupe to creation the thread. The default is Thread::NotDetached.
    */
    Thread(DetachFlag detached = NotDetached);
    /**
   * Destructor.
   */
    virtual ~Thread ();

    /**
     * The startup method calls the run method.
     * @param arg  Thread parameters
     */
    void startup(void *arg);

    /**
     *  The create_thread method is used to actually create the thread context and
     * start the thread initialization.
     * @return a thread error condition
     */
    ErrorType  create_thread();

    /**
     * The run method is method which every thread executes. It's the
     * right place to do the work.
     * Overload run method and call start on class instance.
     *
     * Example:
     * @code
     * class MyThread : public virtual Thread
     * {
     * public:
     *   void run(void* arg = NULL)
     *   {
     *     // do the work here
     *   }
     * };
     * ....
     * MyThread* thr = new MyThread;
     * thr->start();
     * thr->wait();
     * @endcode
     */
    virtual void run() = 0;

    /**
     * The wait method blocks the current thread until the thread being called upon exits.
     * @param exitval  A place to store the exit value of the finished thread
     */
    void wait(void** exitval = NULL){ pthread_join(this->ID(), exitval); };

    /**
    * The stop method is method which every thread executes. It's the
    * right place to do the work.
    * Overload stop method and call stop on class instance.
    */
    virtual void stop() = 0;

   /**
     * The start method is method which every thread executes. It's the
     * right place to do the work.
     * Overload start method and call start on class instance.
     */
    virtual void start() = 0;

    /**
     * The body is a static member function that
     * pthread_create(...) (or the like) can call as its "thread start routine".
     * Any object whose methods are to be invoked as a thread, must derive from
     * the class "Thread".  Thread has a method, run(),
     * which starts the Thread (or a derivative) "in motion".
     * @param arg is arguments to the thread context
     * @return a value or pointer.
     *
     * The body method is passed to pthread_create() when
     * a new thread is created. The argument is always a pointer to
     * the thread object. This method calls startup on the thread.
     */
    static void* body(void* arg);


private:
    pthread_t id;/*!< The system thread id */
    DetachFlag _detached;  /*!< Thread attached or detached? */
    void* _arg;      /*!< Parameters passed in on start */
    bool active;
    ErrorType start_error;/*!< Used to carry starting error from Costructor to start method */

public:

    int Join();
     /**
      * The ID method returns the threads system (pthread) id.
      * @return The thread's system id
      */
    pthread_t ID() const { return id;};

    /**
      * The self method returns the threads system (pthread) id. This
      * method may be called without a thread object since it is
      * static.
      * @return The thread's system id.
      */
    static pthread_t self() { return pthread_self(); };
};

#endif // _THREAD_H_
