#include "IRA_label.h"

namespace IRA_GRAPH {

  IRA_label::IRA_label(IRA_box* b) : QCanvasText( b->canvas()) {
    box=b;
    init();
  }

  IRA_label::~IRA_label(){
    hide();
  };

  void IRA_label::init() {
    angle=0.0;
    print_color=QColor(0,0,0);
    setColor(QColor(0,0,0));
    print=true;
    e=1.0;
    centre_justified=false;
    setFont(QFont("Helvetica", 12 ,75));
  }
  
  void IRA_label::expand(double ee) {
    e=ee; 
  }

  void IRA_label:: centre(bool c) {
    centre_justified=c;
  }
  
  void IRA_label::frac_pos(double ffx, double ffy) {
    fx=ffx;
    fy=ffy;
   
  };

  void IRA_label::wcs_pos(double x, double y){
    fx=(box->xhig()-x)/(box->xhig()-box->xlow());
    fx=(box->yhig()-y)/(box->yhig()-box->ylow());
  };

  void IRA_label::set_angle(double a) {
    angle=a;
  }

  void IRA_label::setPrintColor(QColor c) {
    print_color=c;
  }

  void IRA_label::printLabel(bool prt) {
    print=prt;
  }
  void IRA_label::update_pos(){
   
    setX(box->x()+fx*box->width());
    setY(box->y()+(1-fy)*box->height());
  }

  void IRA_label::draw(QPainter& p) {

    update_pos();

    p.save();
    p.setClipping(false);
    QPen pen;
    pen.setColor(color());
    p.setPen(pen);
    
    QFont ff=font();
    ff.setPixelSize(box->get_fontsize(e*0.015));
    p.setFont(ff);

    QFontMetrics fm=p.fontMetrics();
    int ll=fm.width(text());
    int lh=fm.height();
    int xx=int(x());
    int yy=int(y());
   
    if(centre_justified==false) {
      ll=0;
      lh=0;
    }
    p.saveWorldMatrix();
    p.translate(xx,yy);
    p.rotate(-angle);
    p.drawText(int(-0.5*ll),int(0.5*lh),text());
    p.restoreWorldMatrix();
    p.restore();

  };
}
