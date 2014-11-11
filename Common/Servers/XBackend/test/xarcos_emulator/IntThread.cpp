#include "IntThread.h"
#include "iostream"
#include <sys/timeb.h>

using namespace std;

#define DEBUG 0

IntThread::IntThread( FftSpectrometer *aSpec)//Costruttore della classe
    : spectrometer(aSpec), ciclo(false), semaphore(0)
{
}

// 
// This is the main loop routine. It waits for something to do
void IntThread::run()
{	
//  struct tm *TM;
//  timeb Btime;
  struct timespec time_cycle, time_left;
  time_cycle.tv_sec = 0;
  time_cycle.tv_nsec = 200000000;  // 200 ms
  while(true){//**********Loop del Qthread***************
    semaphore.acquire(); // wait for something to do
    while (ciclo) {
      int time_ms = (int)(spectrometer->IntTime()*1000.);
      do {
        nanosleep(&time_cycle, &time_left);
        if (!ciclo) break;
        time_ms -= 200;
      } while (time_ms > 0);
      if (ciclo) {  // not a break
        spectrometer->SetEndIntegration();
//        ftime(&Btime);// recuperiamo il tempo dall'orologio di sistema in secondi
//        TM=localtime(&Btime.time);	
//  printf( "%i::endInt::%s",Btime.millitm,asctime (TM));
      }
    } // end loop on ciclo=true i.e. integration is active
  }   // end event loop. It never exits. 
}

//Disattivo il Qthread tramite l'uso della variabile ciclo
void IntThread::start()
{
    if (!ciclo) { // if already started do nothing
      ciclo=true;
      semaphore.release();
      if (DEBUG) cout << this << " " << ciclo << " - ";
    }
}

//Disattivo il Qthread tramite l'uso della variabile ciclo
void IntThread::stop()
{
    if (ciclo) {
      ciclo=false;
      if (DEBUG) cout << this << " " << ciclo << " - ";
    }
}

//Distruttore della classe
IntThread::~IntThread(){}
