#include "IRA_boxscope.h"

namespace IRA_GRAPH {
 

  IRA_boxscope::IRA_boxscope(IRA_plotter* p) : QCanvasRectangle(p->canvas()) {
    plot=p;
    setPen(QColor(0,0,250));
  }

  IRA_boxscope::~IRA_boxscope() {
    hide();
  }
  
  void IRA_boxscope::init(){
    setX(box->x());
    setY(box->y());
    setSize(box->width(),box->height());
  }


  void IRA_boxscope::set_box(IRA_box* b) {
    box=b;   
  }
  
 void IRA_boxscope::set_pos(double xx, double yy) {
   x=xx;
   y=yy;
  }
  
  void IRA_boxscope::drawShape(QPainter& p) {
    
    if(!box) return;
    init();
    if(p.device()->isExtDev()) return;
    
    if(int(x)<box->x() || int(x) >box->x()+box->width() || int(y) < box->y() || int(y) > box->y()+box->height()) return;
    
    p.save();
    p.setClipRect(int(box->x()),int(box->y()),int(box->width()),int(box->height()),QPainter::CoordPainter);


    p.drawLine(int(box->x()),int(y),int(x)-9,int(y));
    p.drawLine(int(x)+9,int(y),int(box->x()+box->width()),int(y));
    p.drawLine(int(x),int(box->y()),int(x),int(y)-9);
    p.drawLine(int(x),int(y)+9,int(x),int(box->y()+height()));

    QFont ff=p.font();
    int fs=box->get_fontsize(0.016);
    ff.setPixelSize(fs);
    p.setFont(ff);

    double xw,yw;
    box->BoxToWorld(x,y,xw,yw);
    p.setClipping(false);
    QString sx,sy;
    sx.setNum(xw,'f',3);
    sy.setNum(yw,'f',3);
    p.drawText(int(x+fs),int(y-fs),"x:"+sx+" y:"+sy);
    
    IRA_image* img=plot->image_under();
    if(img) {
      QString s=img->get_value(xw,yw);
      p.drawText(int(x+fs),int(y+2*fs),"img:"+s);
    }

    p.restore();

    //plot->canvas()->setAllChanged();
  
  } 

}
