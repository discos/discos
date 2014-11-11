#include "TpThread.h"

#include <sys/timeb.h>

TpThread::TpThread( Dbbc *aDbbc)//Costruttore della classe
    : dbbc(aDbbc), ciclo(false), semaphore(0)
{
}

// 
// This is the main loop routine. It waits for something to do
void TpThread::run()
{	
//  struct tm *TM;
//  timeb Btime;
  struct timespec time_cycle, time_left;
  time_cycle.tv_sec = 0;
  time_cycle.tv_nsec = 200000000;  // 200 ms
  while(true){//**********Loop del Qthread***************
    semaphore.acquire(); // wait for something to do
    while (ciclo) {
//      int time_ms = 100;   //(int)(dbbc->IntTime()*1000.);
//      while (time_ms > 0) {
        nanosleep(&time_cycle, &time_left);
//        if (!ciclo) break;
//        time_ms -= 200;
//      }
//      if (ciclo) {
        dbbc->SetEndIntegration();
//        ftime(&Btime);// recuperiamo il tempo dall'orologio di sistema in secondi
//        TM=localtime(&Btime.time);	
//        printf( "%i::endInt::%s",Btime.millitm,asctime (TM));
//      }
    } // end loop on ciclo=true i.e. integration is active
  }   // end event loop. It never exits. 
}

//Disattivo il Qthread tramite l'uso della variabile ciclo
void TpThread::start()
{
    if (!ciclo) { // if already started do nothing
      ciclo=true;
      semaphore.release();
    }
}

//Disattivo il Qthread tramite l'uso della variabile ciclo
void TpThread::stop()
{
    if (ciclo) ciclo=false;
}

//Distruttore della classe
TpThread::~TpThread(){}
