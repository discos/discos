#include "IRA_histogram.h"

namespace IRA_GRAPH {
 
  IRA_histogram::IRA_histogram(IRA_box* b) : QCanvasRectangle(b->canvas()) {
    box=b;
    init();
  }

  IRA_histogram::~IRA_histogram() {
    hide();
  }
  
  void IRA_histogram::init(){
    QBrush  b(QColor(255,50,50));
    b.setStyle(Qt::SolidPattern );
    setBrush(b);

    xmin=0;
    xmax=0;
    setX(box->x());
    setY(box->y());
    setSize(box->width(),box->height());
    binsize=-1;
    histo.clear();
    bincenter.clear();
  }

  void IRA_histogram::set_sample( vector<double> data, double bins) {

    if(data.size() < 1) return;
    
    binsize=bins;

    //determine min, max;
    xmin=data[0];
    xmax=data[0];
    for(uint i=0; i<data.size(); i++) {
      if(data[i]>xmax)xmax=data[i];
      if(data[i]<xmin)xmin=data[i];
    }
    
    //guess binsize
    if(binsize<0) {
      binsize=5*(xmax-xmin)/data.size();
      if(binsize < (xmax-xmin)/70.0) binsize=(xmax-xmin)/70.0;
      if(binsize > (xmax-xmin)/5.0) binsize=(xmax-xmin)/5.0;
    }
    
    //guess bin number
    uint nbin=int((xmax-xmin)/binsize);
    
    if(xmax==xmin) {
      nbin=1;
      if(binsize<=0) binsize=0.05*(box->xhig()-box->xlow());
    }
    
    nbin++;

    //determine histogram
    histo.clear();
    bincenter.clear();
    histo.resize(nbin);
    bincenter.resize(nbin);
   
    for(uint i=0; i<histo.size(); i++) {
      histo[i]=0;
      bincenter[i]=0;
    }

    
    double val;
    double hbin=0.5*binsize;
   
    for(uint j=0; j<data.size(); j++) {
      
      val=data[j]; 
      double xl=xmin-hbin;
      int i=0;
      while(1) {
	double xh=xl+binsize;
	bincenter[i]=xl+hbin;

	if (val >= xl && val < xh) histo[i]=histo[i]+1;
	i++;
	xl=xl+binsize;
	if(xl >= xmax) break;
	if(i>=nbin) break;
      }
     
    }
  
    return;    
  }

  void IRA_histogram::set_histogram(vector<double> bc, vector<int> hi) {
    bincenter=bc;
    histo=hi;
  }

  void IRA_histogram::drawShape(QPainter& p) {
   
    if(histo.size()<1)return;

    double f=10.0;

  

    //update bounding rect
    setX(box->x());
    setY(box->y());
    setSize(box->width(),box->height());

    p.save();
 p.setClipRect(int(box->x()),int(box->y()),int(box->width()),int(box->height()),QPainter::CoordPainter);


    p.scale(1.0/f,1.0/f);
    
    QPen pen;
    pen.setWidth(1);
    if(p.device()->isExtDev()) {
      pen.setWidth(int(pen.width()*f));
    }
    p.setPen(pen);

    for(uint i=0; i<histo.size();i++) {
      double x,y;
      box->WorldToBox(bincenter[i]-0.5*binsize,histo[i],x,y);
     
      p.drawRect(int(f*x),int(f*y),int(ceil(f*binsize*box->get_xcell())),
		 int(ceil(f*histo[i]*box->get_ycell())));
    }
   
    //repaint box ticks
    p.setBrush(box->brush());
    pen=box->pen();
    if(p.device()->isExtDev()) {
      pen.setWidth(int(pen.width()*f));
    }
    p.setPen(pen);

    box->draw_ticks(p,f);

    p.restore();
    
  } 

}
