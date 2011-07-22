
#include "IRA_monitor.h"

namespace IRA_GRAPH {

  IRA_monitor::IRA_monitor(IRA_plotter* p) {
    plot=p;
    curve=0;
    count=0;
    update_period=1000;
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(update_curve()));
  }
  
  IRA_monitor::~IRA_monitor() {
    stop();
  }

  void IRA_monitor::set_curve(IRA_curve* c) {
    curve=c;
  }
  
  void IRA_monitor::set_update_period(int p) {
    update_period=p;
  }

  void IRA_monitor::start() {
    timer->start(update_period);
  }
  
   void IRA_monitor::stop() {
     timer->stop();
  }
  
  void IRA_monitor::update_curve() {
    
    if(!curve) return;
    double r1=1.0*rand()/RAND_MAX;
    curve->add_point(1.0*count, 100*r1+300*fabs((sin(count/(5*r1+30.0))*cos(count/(10*r1+150.0)))));
    count++;
    plot->canvas()->setAllChanged();
    plot->canvas()->update();
    
  }

}
