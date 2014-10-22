#include "IRA_curve.h"

namespace IRA_GRAPH {
 
  IRA_curve::IRA_curve(IRA_box* b) : QCanvasPolygon(b->canvas()) {
    box=b;
    init();
  }

  IRA_curve::~IRA_curve() {
    hide();
  }
  
  void IRA_curve::init() {
    buffer_size=512;
    color=QColor(0,0,200);
    style=Solid;
    autorescale=false;
  }

  void IRA_curve::set_color(QColor c) {
    color=c;
  }
  
  
  void IRA_curve::setStyle(curveStyle s) {
    style=s;
  } 
  void IRA_curve::set_buffer_size(int size) {
    buffer_size=size;
  }

  void IRA_curve::autorescale_box(bool a) {
    autorescale=a;
  }

  void IRA_curve::add_point(double xx, double yy) {
    xp.push_back(xx);
    yp.push_back(yy);
    if (buffer_size > 0 ) {
      while(xp.size() > buffer_size) {
	xp.pop_front();
	yp.pop_front();
      }
    }
    convert(10.0);
   
  }
 

  void IRA_curve::set_points(deque<double> xx, deque<double> yy) {
    xp=xx;
    yp=yy;
    if (buffer_size > 0 ) {
      while(xp.size() > buffer_size) {
	xp.pop_front();
	yp.pop_front();
      }
    }
    convert(10.0);   
    
  }
  
  void IRA_curve::convert(double f) {
    pa.resize(xp.size());
    paf.resize(xp.size());

    double xmin=1e300;
    double xmax=-1e300;
    double ymin=1e300;
    double ymax=-1e300;
    for(uint i=0; i< xp.size(); i++) {
      double xw,yw,x,y;
      double xx=xp[i];
      double yy=yp[i];

      if(box->xscaling()==IRA_box::Log) {
	if(xx<=0) {
	  xw=-999;
	} else { 
	  xw=log10(xx);
	}
      } else {
	xw=xx;
      }
      
      if(box->yscaling()==IRA_box::Log) {
	if(yy <= 0 ) {
	  yw=-999;
	} else {
	  yw=log10(yy);
	}
      } else {
	yw=yy;
      }
      
      box->WorldToBox(xw,yw,x,y);
    
      if(xw>xmax)xmax=xw;
      if(xw<xmin)xmin=xw;
      if(yw>ymax)ymax=yw;
      if(yw<ymin)ymin=yw;

      pa.setPoint(i,int(x),int(y));
      paf.setPoint(i,int(f*x),int(f*y));
    }

    if(autorescale) {
      double x1=box->oxlow();
      double x2=box->oxhig();
      double y1=box->oylow();
      double y2=box->oyhig();
      
      if(xmin>x1)x1=xmin-0.0*(x2-x1);
      if(xmax>x2)x2=xmax+0.0*(x2-x1);
      if(ymin<y1)y1=ymin-0.1*(y2-y1);
      if(ymax>y2)y2=ymax+0.1*(y2-y1);    

      box->axis_limits(x1,x2,y1,y2,true); //preserve zoom
    }

  }

  QPointArray IRA_curve::areaPoints () const {
    return pa;
  }

  void IRA_curve::drawShape(QPainter& p) {
    double f=10.0;
    convert(f);
    p.save();
    p.setClipRect(int(box->x()),int(box->y()),int(box->width()),int(box->height()),QPainter::CoordPainter);
    
   
    p.scale(1.0/f,1.0/f);
    QPen pen;
    pen.setColor(color);
    pen.setWidth(2);
    if(p.device()->isExtDev()) {
      pen.setWidth(int(pen.width()*f));
    }
    p.setPen(pen);

    switch(style) {
      
    case(Dashed):
      break;
    default:
      break;
    }  
    p.drawPolyline(paf);
   
    p.restore();
    //box->draw_ticks(p,f);
  } 

}
