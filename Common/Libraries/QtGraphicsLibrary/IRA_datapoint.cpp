#include "IRA_datapoint.h"

namespace IRA_GRAPH {
 
  IRA_datapoint::IRA_datapoint(IRA_box* b) : QCanvasRectangle(b->canvas()) {
    box=b;
    init();
  }

  IRA_datapoint::~IRA_datapoint() {
    hide();
  }
  
  void IRA_datapoint::init() {
    x=0.0;
    y=0.0;
    xw=0;
    yw=0;
    xl=0;
    yl=0;
    xh=0;
    yh=0;
    size=1;
    e=1.0;
    filled=true;
    color=QColor(255,0,0);
    color_error=QColor(100,100,100);
    shape=Circle;
  }

  void IRA_datapoint::pointColor(QColor c) {
    color=c;
  }
  
  void IRA_datapoint::barColor(QColor c) {
    color_error=c;
  }

  void IRA_datapoint::expand(double ee) {
    e=ee;
  }
  void IRA_datapoint::fill(bool f) {
    filled=f;
  } 
  void IRA_datapoint::setShape(pointShape s) {
    shape=s;
  }
  void IRA_datapoint::x_errorbars(double xxl, double xxh) {
    xlw=xxl;
    xhw=xxh;
    convert(10);
  }
  
  void IRA_datapoint::y_errorbars(double yyl, double yyh) {
    ylw=yyl;
    yhw=yyh;
    convert(10);
  }

  void IRA_datapoint::position(double xx, double yy) {

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
    convert(10);
  }
  
  void IRA_datapoint::convert(double f){
    box->WorldToBox(xw,yw,x,y);
    
    //update rectangle
    setX(x-0.5*f*size);
    setY(y-0.5*f*size);
    setSize(size,size);

    //xerrorbar
    double yy;
    if(box->xscaling()==IRA_box::Log) {
      box->WorldToBox(log10(pow(10,xw)-xlw),0,xl,yy);
      box->WorldToBox(log10(pow(10,xw)+xhw),0,xh,yy);
    } else {
      box->WorldToBox(xw-xlw,0,xl,yy);
      box->WorldToBox(xw+xhw,0,xh,yy);
    }
    
    //update rectangle
    setX(int(xl));
    setSize(int(xh-xl),int(yh-yl));

    //yerrorbar
    double xx;
    if(box->yscaling()==IRA_box::Log) {
      box->WorldToBox(0,log10(pow(10,yw)-ylw),xx,yl);
      box->WorldToBox(0,log10(pow(10,yw)+yhw),xx,yh);
    } else {
      box->WorldToBox(yw,yw-ylw,xx,yl);
      box->WorldToBox(yw,yw+yhw,xx,yh);
    }
    //update rectangle
    setY(int(yl));
    setSize(int(xh-xl),int(yh-yl));
    
    setX(box->x());
    setY(box->y());
    setSize(box->width(),box->height());
    
  }

  void IRA_datapoint::drawShape(QPainter& p) {
    double f=10.0;

    size=box->get_fontsize(e*0.01);
    if(size<1)size=1;

    convert(1);
    p.save();
    p.setClipRect(int(box->x()),int(box->y()),int(box->width()),int(box->height()),QPainter::CoordPainter);
    
  
    p.scale(1.0/f,1.0/f);
    
    if(filled) {
      p.setBrush(QColor(color)); 
    } else {
      p.setBrush(Qt::NoBrush); 
    }
    QPen pen;
    pen.setColor(QColor(0,0,0));
    pen.setWidth(1);
    if(p.device()->isExtDev()) {
      pen.setWidth(int(pen.width()*f));
    }
    p.setPen(pen);

    //errorbars
   
    if(xl > 0)  {
      p.drawLine(int(f*(xl)),int(f*(y)),int(f*(x)),int(f*(y)));
      p.drawLine(int(f*(xl)),int(f*(y-0.5*size)),int(f*(xl)),int(f*(y+0.5*size)));    
    }
    if(xh > 0)  {
      p.drawLine(int(f*(xh)),int(f*(y)),int(f*(x)),int(f*(y)));
      p.drawLine(int(f*(xh)),int(f*(y-0.5*size)),int(f*(xh)),int(f*(y+0.5*size)));
    }
    if(yl > 0)  {
      p.drawLine(int(f*(x)),int(f*(yl)),int(f*(x)),int(f*(y)));
      p.drawLine(int(f*(x-0.5*size)),int(f*(yl)),int(f*(x+0.5*size)),int(f*(yl)));
    }
    if(yh > 0)  {
      p.drawLine(int(f*(x)),int(f*(yh)),int(f*(x)),int(f*(y)));
      p.drawLine(int(f*(x-0.5*size)),int(f*(yh)),int(f*(x+0.5*size)),int(f*(yh)));
    }
   
    p.saveWorldMatrix();
    p.translate(f*x,f*y);
    p.scale(e,e);
    switch(shape) {
      
    case(Cross):
      {
	p.rotate(45);	
	QPointArray pa(12);
	pa.putPoints(0,12,int(f*(-0.125*size)),int(-f*0.5*size),
		     int(f*(0.125*size)),int(-f*0.5*size),
		     int(f*(0.125*size)),int(-f*0.125*size),
		     int(f*(0.5*size)),int(-f*0.125*size),
		     int(f*(0.5*size)),int(f*0.125*size),
		     int(f*(0.125*size)),int(f*0.125*size),
		     int(f*(0.125*size)),int(f*0.5*size),
		     int(f*(-0.125*size)),int(f*0.5*size),
		     int(f*(-0.125*size)),int(f*0.125*size),
		     int(f*(-0.5*size)),int(f*0.125*size),
		     int(f*(-0.5*size)),int(f*-0.125*size),
		     int(f*(-0.125*size)),int(f*-0.125*size)
		     );
	p.drawPolygon(pa);
      }
      break;
    case(Triangle):
      {
      QPointArray pa(3);
      pa.putPoints(0,3,int(f*(-0.5*size)),int(f*(0.5*size)),int(f*(0.5*size)),\
		   int(f*(0.5*size)),int(0),int(f*(-0.5*size)));
      p.drawPolygon(pa);
      }
      break;
      
    case(Diamond):
      {
	p.rotate(45);
	p.drawRect(int(f*(-0.5*size)),int(f*(-0.5*size)),int(f*size),int(f*size));
      }
      break;
    case(InvTriangle):
      {
      QPointArray pa(3);
      pa.putPoints(0,3,int(f*(-0.5*size)),int(f*(-0.5*size)),int(f*(0.5*size)),\
		   int(f*(-0.5*size)),int(0),int(f*(0.5*size)));
      p.drawPolygon(pa);
      }
      break;
    case(Square):
      p.drawRect(int(f*(-0.5*size)),int(f*(-0.5*size)),int(f*size),int(f*size));
      break;
    case(Dot):
      p.drawRect(int(f*(-1)),int(f*(-1)),int(f*2),int(f*2));
      break;
    default:
      p.drawEllipse(int(f*(-0.5*size)),int(f*(-0.5*size)),int(f*size),int(f*size));
      break;
    }  
    
    p.restoreWorldMatrix();

    p.restore();
  } 

}
