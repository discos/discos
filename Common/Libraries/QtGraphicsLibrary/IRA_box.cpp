#include "IRA_box.h"

namespace IRA_GRAPH {
 

  IRA_box::IRA_box(IRA_plotter* plot, QString n) : QCanvasRectangle(plot->canvas()) {
    plotter=plot;
    boxname=n;
    init();
  }

  IRA_box::IRA_box(double x, double y, double w, double h, IRA_plotter* plot, QString n) : QCanvasRectangle(plot->canvas()) {
    plotter=plot;
    boxname=n;
    init();
    relX=x;
    relY=y;
    relW=w;
    relH=h;
  }

  IRA_box::~IRA_box() {
    hide();
  }
  
  void IRA_box::init() {
    relX=0.15;
    relY=0.15;
    relW=0.7;
    relH=0.7;
    oxl=0.0;
    oyl=0.0;
    oxh=1.0;
    oyh=1.0;
    zoomFac=1.0;
    
    axis_limits(oxl,oxh,oyl,oyh);
    
    xlabel="x";
    ylabel="y";
    dir=1;
    tickslen=0.025;
    labelsize=0.015;
    bottom_axislabs=true;
    left_axislabs=true;
    top_axislabs=false;
    right_axislabs=false;
    plot_grid=true;
    bottom_labs=true;
    left_labs=true;
    top_labs=false;
    right_labs=false;
    bottom_ticks=true;
    left_ticks=true;
    top_ticks=true;
    right_ticks=true;
    preserve_aspect=true;
    showZoom=true;
    scope=true;
    x_scaling=Lin;
    y_scaling=Lin;
    box_font=QFont("Helvetica", 12 ,75);
    QPen p;
    p.setWidth(2);
    setPen(p);
    gc=QColor(200,200,200);
    tc=pen().color();

  }
  QString IRA_box::name() {return boxname;}

  void IRA_box::allow_boxscope(bool b) {
    scope=b;
  }

  bool IRA_box::show_boxscope() {
    return scope;
  } 

  void IRA_box::labels_size(double s) {
    if(s>0) labelsize=s;
  }
  void IRA_box::show_zoom(bool z) {
    showZoom=z;
  }
  void IRA_box::move_to(double x1, double y1) {
    double dx=0.5*(xh-xl);
    double dy=0.5*(yh-yl);
    
    xl=x1-dx;
    xh=x1+dx;
    yl=y1-dy;
    yh=y1+dy;    
    if(xl <oxl) {
      xl=oxl;
      xh=xl+2*dx;
    }
     if(xh > oxh) {
      xh=oxh;
      xl=xh-2*dx;
    }
     if(yl <oyl) {
      yl=oyl;
      yh=yl+2*dy;
    }
     if(yh > oyh) {
      yh=oyh;
      yl=yh-2*dy;
    }

    update_cellsize();
    update_tickmarks();
  }

  void IRA_box::zoom(double x1, double x2, double y1, double y2) {
    if(x2<=x1|| y2<=y1) return;
    xl=x1;
    xh=x2;
    yl=y1;
    yh=y2;
    if(xl<oxl)xl=oxl;
    if(yl<oyl)yl=oyl;
    if(xh>oxh)xh=oxh;
    if(yh>oyh)yh=oyh;
    update_cellsize();
    update_tickmarks();
    zoom_histo.push_back(zoomRect(xl,xh,yl,yh));
  }
  void IRA_box::offzoom() {
    zoom(oxl,oxh,oyl,oyh);
    zoom_histo.clear();
  }

  void IRA_box::unzoom() {
    if(zoom_histo.size()<2) {
      offzoom();
      return;
    }
    zoomRect r=zoom_histo[zoom_histo.size()-2];
    zoom(r.x1,r.x2,r.y1,r.y2);
    zoom_histo.pop_back();
    zoom_histo.pop_back();
  }

  void IRA_box::preserve_aspectratio(bool p) {
    preserve_aspect=p;
  }

  double IRA_box::xlow() {return xl;}
  double IRA_box::xhig() {return xh;}
  double IRA_box::ylow() {return yl;}
  double IRA_box::yhig() {return yh;}

  
  double IRA_box::oxlow() {return oxl;}
  double IRA_box::oxhig() {return oxh;}
  double IRA_box::oylow() {return oyl;}
  double IRA_box::oyhig() {return oyh;}

  void IRA_box::update_geom() {

    if(preserve_aspect) {
      int minsize=canvas()->width();
      if(canvas()->width()> canvas()->height()) minsize=canvas()->height();
      setX(int(relX*minsize));
      setY(int(relY*minsize));
      setSize(int(relW*minsize),int(relH*minsize));
    } else {
      setX(int(relX*canvas()->width()));
      setY(int(relY*canvas()->height()));
      setSize(int(relW*canvas()->width()),int(relH*canvas()->height()));
    }
    QCanvasRectangle::areaPoints().point(0,&axmin,&aymin);
    QCanvasRectangle::areaPoints().point(2,&axmax,&aymax);
  }

  int IRA_box::get_fontsize(double fs) {
    int scale=plotter->get_minside();
    int s=int(fs*scale);
    if(s<=1) s=1;
    return s;
  }

  void IRA_box::set_tickscolor(QColor c) {
    tc=c;
  }

  void IRA_box::drawShape(QPainter& p) {

    //follow canvas resize
    update_geom();

    update_cellsize();
    QFont ff=box_font;
    ff.setPixelSize(get_fontsize(labelsize));  //use canvas min size
    p.setFont(ff);
    
    double f=10.0;   //expand factor for high precision plots

    p.save();
    p.scale(1.0/f,1.0/f); 

    QPen pen=QCanvasPolygonalItem::pen();
    if(p.device()->isExtDev()) {
      pen.setWidth(int(pen.width()*f));
    }
    p.setPen(pen);
   
    if(plot_grid) draw_grid(p,f);
   
    draw_ticks(p,f);
       
    
    p.restore();

    draw_tickslabels(p);
    draw_axislabels(p);   
   
    if(!p.device()->isExtDev() && showZoom) {
      double xz=1.0/((xh-xl)/(oxh-oxl));
      double yz=1.0/((yh-yl)/(oyh-oyl));
      
      if(xz > 1 || yz  > 1) {
	zoomFac=xz;
	if(yz>xz) zoomFac=yz; 

	QString sz;
	if(zoomFac<2) {
	  sz.setNum(zoomFac,'f',1);
	} else {
	  sz.setNum(zoomFac,'f',0);
	}
	
	QString s="Zoom: x"+sz+" ";
	QFontMetrics fm=p.fontMetrics();
	int ll=fm.width(s);
	int lh=fm.height();
	p.setPen(QColor(0,0,255));
	p.drawText(int(x()+width()-ll),int(y()+lh),s);
	p.setPen(pen);
      }
    }
    
    //control polygon for areapoints
    //p.save();
    //p.setPen(QColor(255,0,0));
    //p.drawPolygon(areaPoints());
    //p.restore();    

    update_geom();
  }

  double IRA_box::get_zoomFac() {
    return zoomFac;
  }

  void IRA_box::tickslenght(double frac) {
      tickslen=frac;
  }

  void IRA_box::show_grid(bool s) {
    plot_grid=s;
  }

  void IRA_box::invert_ticks(bool inv) {
    dir=1;
    if(inv) dir=-1;
  }
  void IRA_box::set_boxFont(QFont font) {
    box_font=font;
  }
  void IRA_box::set_gridColor(QColor c) {
    gc=c;
  }
  void IRA_box::tickslabels_mask(bool b, bool l, bool t, bool r) {
    bottom_labs=b;
    left_labs=l;
    top_labs=t;
    right_labs=r;
  }
  void IRA_box::axislabels_mask(bool b, bool l, bool t, bool r) {
    bottom_axislabs=b;
    left_axislabs=l;
    top_axislabs=t;
    right_axislabs=r;
  }

  void IRA_box::tickmarks_mask(bool b, bool l, bool t, bool r) {
    bottom_ticks=b;
    left_ticks=l;
    top_ticks=t;
    right_ticks=r;
  }
  void IRA_box::axis_scaling(axisScaling x, axisScaling y) {
    x_scaling=x;
    y_scaling=y;
  }
 void IRA_box::axis_labels(string x, string y) {
    xlabel=x;
    ylabel=y;
  }

  void IRA_box::update_cellsize() {
    xcell=1.0*width()/(xh-xl);
    ycell=1.0*height()/(yh-yl);
  }

  void IRA_box::update_tickmarks() {
    if(x_scaling==Log) {
      axistick(xl,xh,int(xh-xl),1000,"log",xticks);
      small_xticks.clear();
      for (int i=1; i<=10; i++) small_xticks.push_back(log10(1.0*i));

    } else {
      axistick(xl,xh,6,10,"lin",xticks);
      axistick(0,xticks[1]-xticks[0],6,10,"lin",small_xticks);
    }
    if(y_scaling==Log) {
      axistick(yl,yh,int(yh-yl),1000,"log",yticks);
      small_yticks.clear();
      for (int i=1; i<=10; i++) small_yticks.push_back(log10(1.0*i));
    } else {
      axistick(yl,yh,6,10,"lin",yticks);
      axistick(0,yticks[1]-yticks[0],6,10,"lin",small_yticks);
    }

    /*
    cout<<"XTICKS"<<endl;
    for (int i=0; i< xticks.size(); i++) cout<<xticks[i]<<endl;
    cout<<"YTICKS"<<endl;
    for (int i=0; i< yticks.size(); i++) cout<<yticks[i]<<endl;

    cout<<"SXTICKS"<<endl;
     for (int i=0; i< small_xticks.size(); i++) cout<<small_xticks[i]<<endl;
    cout<<"SYTICKS"<<endl;
    for (int i=0; i< small_yticks.size(); i++) cout<<small_yticks[i]<<endl;    
    */
  }

  void IRA_box::axis_limits(double xlow, double xhig, double ylow, double yhig, bool preserve_zoom) {
    oxl=xlow;
    oxh=xhig;
    oyl=ylow;
    oyh=yhig;
    if(!preserve_zoom || zoom_histo.size()==0) {
      xl=oxl;
      xh=oxh;
      yl=oyl;
      yh=oyh;
    }

    update_cellsize();
    update_tickmarks();    
 }
  
  void IRA_box::BoxToWorld(double xg,double yg,double& xw,double& yw) {
    xw=xl+(xg-x())/xcell;
    yw=yl+(y()+height()-yg)/ycell;
  }
    
  void IRA_box::WorldToBox(double xw,double yw,double& xg,double& yg) {
    xg=x()+xcell*(xw-xl);
    yg=y()+height()-ycell*(yw-yl);
  }
  
  void IRA_box::draw_axislabels(QPainter& p) {

    QString s;
    double offset=0;
    if(dir==-1)offset=len;

    QFontMetrics fm=p.fontMetrics();
    double x1=x();
    double x2=x()+width();
    double y1=y();
    double y2=y()+height();

    //bottom
    if(bottom_axislabs) {
      int ll=fm.width(xlabel);
      int lh=fm.height();
      p.drawText(int(x1+0.5*(x2-x1)-0.5*ll),int(y2+2*lh-offset),xlabel); 
      //ymax=y2+2*lh-offset-lh;
    }
    //top
    if(top_axislabs) {
      int ll=fm.width(xlabel);
      int lh=fm.height();
      p.drawText(int(x1+0.5*(x2-x1)-0.5*ll),int(y1-1.35*lh+offset),xlabel); 
    }
    //left
    if(left_axislabs) {
      int ll=fm.width(ylabel);
      int lh=fm.width("c");
      int xx=int(xmin);
      int yy=int(y1+0.5*(y2-y1));
      p.saveWorldMatrix();
      p.translate(xx,yy);
      p.rotate(-90);
      p.drawText(-int(0.5*ll),-2*lh,ylabel);
      p.restoreWorldMatrix();
      //xmin=xmin-int(3.5*lh);
    }    
    //right
    if(right_axislabs) {
      int ll=fm.width(ylabel);
      int lh=fm.width("c");
      int xx=int(xmax);
      int yy=int(y1+0.5*(y2-y1));
      p.saveWorldMatrix();
      p.translate(xx,yy);
      p.rotate(-90);
      p.drawText(-int(0.5*ll),int(2.35*lh),ylabel);
      p.restoreWorldMatrix();
    }    
    
  }

  QPointArray IRA_box::areaPoints () const {
 
    QPointArray p(4);
    //p.setPoint(0,axmin-50,aymin-50);
    //p.setPoint(1,axmax+50,aymin-50);
    //p.setPoint(2,axmax+50,aymax+50);
    //p.setPoint(3,axmin-50,aymax+50);
    
    int xoff=int(0.5*width());
    int yoff=int(0.5*height());

    p.setPoint(0,int(x()-xoff),int(y()-yoff));
    // p.setPoint(0,xmin,int(y()-yoff));
    p.setPoint(1,int(x()+width()+xoff),int(y()-yoff));
    p.setPoint(2,int(x()+width()+xoff),int(y()+height()+yoff));
    p.setPoint(3,int(x()-xoff),int(y()+height())+yoff);
   
    return p;
  }

  void IRA_box::draw_tickslabels(QPainter& p) {
    QString s;
    double offset=0;
    if(dir==-1)offset=len;

    QFontMetrics fm=p.fontMetrics();
    double xw,yw,x1,y1;
    
    //QCanvasRectangle::areaPoints().point(0,&axmin,&aymin);
    //QCanvasRectangle::areaPoints().point(2,&axmax,&aymax);

    //bottom
    for(uint i=0;i<xticks.size();i++) {
      xw=xticks[i];
      yw=yl;
      WorldToBox(xw,yw,x1,y1);
      if(x_scaling==Log) {
	s=QString("%1").arg(pow(10.0,xw),0,'E',0);
      } else {
	s=QString("%1").arg(xw,0,'g');
      }
      int ll=fm.width(s);
      int lh=fm.height();
      if(bottom_labs) {
	int xpos=int(x1-0.5*ll);
	int ypos=int(y1+1.0*lh-offset);
	//if(xpos<axmin) axmin=xpos;
	//if(xpos>axmax) axmax=xpos;
	//if(ypos<aymin) aymin=ypos;
	//if(ypos>aymax) aymax=ypos;
	p.drawText(xpos,ypos,s); 
      }
    }
    //top
    for(uint i=0;i<xticks.size();i++) {
      xw=xticks[i];
      yw=yh;
      WorldToBox(xw,yw,x1,y1);
      if(x_scaling==Log) {
	s=QString("%1").arg(pow(10.0,xw),0,'E',0);
      } else {
	s=QString("%1").arg(xw,0,'g');
      }
      int ll=fm.width(s);
      int lh=fm.height();
      
      if(top_labs) {
	int xpos=int(x1-0.5*ll);
	int ypos=int(y1-0.35*lh+offset);
	//if(xpos<axmin) axmin=xpos;
	//if(xpos>axmax) axmax=xpos;
	//if(ypos<aymin) aymin=ypos;
	//if(ypos>aymax) aymax=ypos;
	p.drawText(xpos,ypos,s); 
      }
    }
    //left
    xmin=int(x()+width());
    for(uint i=0;i<yticks.size();i++){
      xw=xl;
      yw=yticks[i];
      WorldToBox(xw,yw,x1,y1);	
      if(y_scaling==Log) {
	s=QString("%1").arg(pow(10.0,yw),0,'E',0);
      } else {
	s=QString("%1").arg(yw,0,'g');
      }
      int lc=fm.width("c");
      int ll=fm.width(s);
      int lh=fm.height();
      if(left_labs) {
	int xpos=int(x1-ll-lc+offset);
	int ypos=int(y1+0.35*lh);
	//if(xpos<axmin) axmin=xpos;
	//if(xpos>axmax) axmax=xpos;
	//if(ypos<aymin) aymin=ypos;
	//if(ypos>aymax) aymax=ypos;
	p.drawText(xpos,ypos,s); 
      }
      if(int(x1-ll-lc+offset) <= xmin)xmin=int(x1-ll-lc+offset);
    }
    //right
    xmax=-1;
    for(uint i=0;i<yticks.size();i++){
      xw=xh;
      yw=yticks[i];
      WorldToBox(xw,yw,x1,y1);	
      if(y_scaling==Log) {
	s=QString("%1").arg(pow(10.0,yw),0,'E',0);
      } else {
	s=QString("%1").arg(yw,0,'g');
      }
      int lh=fm.height();
      int ll=fm.width(s);
      if(right_labs) {
	int xpos=int(x1+0.35*lh-offset);
	int ypos=int(y1+0.35*lh);
	//if(xpos<axmin) axmin=xpos;
	//if(xpos>axmax) axmax=xpos;
	//if(ypos<aymin) aymin=ypos;
	//if(ypos>aymax) aymax=ypos;
	p.drawText(xpos,ypos,s); 
      }
      if(int(x1+0.35*lh-offset+ll) >= xmax)xmax=int(x1+0.35*lh-offset+ll); 
    }
  }

  void IRA_box::draw_ticks(QPainter& p, double f) {
    double xw,yw,x1,y1,x2,y2;
    double sx1,sy1,sx2,sy2;
    
    
    QPen old_pen=p.pen();
    QPen pen;
    pen.setColor(tc); 
    if(p.device()->isExtDev()) {
      if(pen.width()==0) {
	pen.setWidth(int(1*f));
      } else {
	pen.setWidth(int(pen.width()*f));
      }
    }
    p.setPen(pen);


    if(height() > width()) {
      len=dir*tickslen*width();
    } else {
      len=dir*tickslen*height();
    }
    slen=0.4*len;
    
    //bottom
    if(bottom_ticks) {
      for(uint i=0;i<xticks.size();i++) {
	xw=xticks[i];
	yw=yl;
	WorldToBox(xw,yw,x1,y1);
	x2=x1;
	y2=y1-len;
	p.drawLine(int(f*x1),int(f*y1),int(f*x2),int(f*y2));
	
	for(uint j=1;j<small_xticks.size()-1;j++) {
	  xw=xticks[i]+small_xticks[j];
	  yw=yl;
	  WorldToBox(xw,yw,sx1,sy1);
	  sx2=sx1;
	  sy2=y1-slen;
	  if(sx1<x()+width()) p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
	}      
      }
      for(uint j=1;j<small_xticks.size()-1;j++) {
	if(x_scaling==Log) {
	  xw=xticks[0]-(1-small_xticks[small_xticks.size()-j-1]);
	} else {
	  xw=xticks[0]-small_xticks[j];
	}
	yw=yl;
	WorldToBox(xw,yw,sx1,sy1);
	sx2=sx1;
	sy2=y1-slen;
	if(sx1>x()) p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
      }
    }
    //top
    if(top_ticks) {
      for(uint i=0;i<xticks.size();i++) {
	xw=xticks[i];
	yw=yh;
	WorldToBox(xw,yw,x1,y1);
	x2=x1;
	y2=y1+len;
	p.drawLine(int(f*x1),int(f*y1),int(f*x2),int(f*y2));
	
	for(uint j=1;j<small_xticks.size()-1;j++) {
	  xw=xticks[i]+small_xticks[j];
	  yw=yh;
	  WorldToBox(xw,yw,sx1,sy1);
	  sx2=sx1;
	  sy2=y1+slen;
	  if(sx1<x()+width())p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
	}
      }
      for(uint j=1;j<small_xticks.size()-1;j++) {
	if(x_scaling==Log) {
	  xw=xticks[0]-(1-small_xticks[small_xticks.size()-j-1]);
	} else {
	  xw=xticks[0]-small_xticks[j];
	}
	yw=yh;
	WorldToBox(xw,yw,sx1,sy1);
	sx2=sx1;
	sy2=y1+slen;
	if(sx1>x()) p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
      }
    }
    //left
    if(left_ticks) {
      for(uint i=0;i<yticks.size();i++){
	xw=xl;
	yw=yticks[i];
	WorldToBox(xw,yw,x1,y1);
	y2=y1;
	x2=x1+len;
	p.drawLine(int(f*x1),int(f*y1),int(f*x2),int(f*y2));
	
	for(uint j=1;j<small_yticks.size()-1;j++) {
	  yw=yticks[i]+small_yticks[j];
	  xw=xl;
	  WorldToBox(xw,yw,sx1,sy1);
	  sy2=sy1;
	  sx2=x1+slen;
	  if(sy1<y()+height()&& sy1>y())p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
	  
	}
      }
      for(uint j=1;j<small_yticks.size()-1;j++) {
       	if(y_scaling==Log) {
	  yw=yticks[0]-(1-small_yticks[small_yticks.size()-j-1]);
	} else {
	  yw=yticks[0]-small_yticks[j];
	}
	xw=xl;
	WorldToBox(xw,yw,sx1,sy1);
	sy2=sy1;
	sx2=x1+slen;
	if(sy1<y()+height()&& sy1>y())p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
      }
    }
    //right
    if(right_ticks) {
      for(uint i=0;i<yticks.size();i++){
	xw=xh;
	yw=yticks[i];
	WorldToBox(xw,yw,x1,y1);
	y2=y1;
	x2=x1-len;
	p.drawLine(int(f*x1),int(f*y1),int(f*x2),int(f*y2));
	
	for(uint j=1;j<small_yticks.size()-1;j++) {
	  yw=yticks[i]+small_yticks[j];
	  xw=xh;
	  WorldToBox(xw,yw,sx1,sy1);
	  sy2=sy1;
	  sx2=x1-slen;
	  if(sy1<y()+height()&& sy1>y())p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
	  
	}
      }
      for(uint j=1;j<small_yticks.size()-1;j++) {
	if(y_scaling==Log) {
	  yw=yticks[0]-(1-small_yticks[small_yticks.size()-j-1]);
	} else {
	  yw=yticks[0]-small_yticks[j];
	}
	xw=xh;
	WorldToBox(xw,yw,sx1,sy1);
	sy2=sy1;
	sx2=x1-slen;
	if(sy1<y()+height() && sy1>y())p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
      }
    }
    // the box
    p.setPen(old_pen);
    p.drawRect(int(f*x()),int(f*y()),int(f*width()),int(f*height()));
   
  }
  
  void IRA_box::draw_grid(QPainter& p, double f) {
   
    QPen old_pen=p.pen();
    QPen pen;
    pen.setColor(gc);
    p.setPen(pen);
    
    double xw,yw,x1,y1,x2,y2;
    double sx1,sy1,sx2,sy2;   

    //bottom to top
    for(uint i=0;i<xticks.size();i++) {
      xw=xticks[i];
      yw=yl;
      WorldToBox(xw,yw,x1,y1);
      yw=yh;
      WorldToBox(xw,yw,x2,y2);
      pen.setWidth(old_pen.width());
      p.setPen(pen);
      p.drawLine(int(f*x1),int(f*y1),int(f*x2),int(f*y2));
  
      pen.setWidth(0);
      if(p.device()->isExtDev()) pen.setWidth(int(1*f));
      p.setPen(pen);

      for(uint j=1;j<small_xticks.size()-1;j++) {
	xw=xticks[i]+small_xticks[j];
	yw=yl;
	WorldToBox(xw,yw,sx1,sy1);
	yw=yh;
	WorldToBox(xw,yw,sx2,sy2);

	if(sx1<x()+width()) p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
      }      
    }
    for(uint j=1;j<small_xticks.size()-1;j++) {
      if(x_scaling==Log) {
	xw=xticks[0]-(1-small_xticks[small_xticks.size()-j-1]);
      } else {
	xw=xticks[0]-small_xticks[j];
      }
      yw=yl;
      WorldToBox(xw,yw,sx1,sy1);
      yw=yh;
      WorldToBox(xw,yw,sx2,sy2);
      if(sx1>x()) p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
    }
   
    //left to right
    for(uint i=0;i<yticks.size();i++){
      xw=xh;
      yw=yticks[i];
      WorldToBox(xw,yw,x1,y1);
      xw=xl;
      WorldToBox(xw,yw,x2,y2);
      pen.setWidth(old_pen.width());
      p.setPen(pen);
      p.drawLine(int(f*x1),int(f*y1),int(f*x2),int(f*y2));

      pen.setWidth(0);
      if(p.device()->isExtDev()) pen.setWidth(int(1*f));
      p.setPen(pen);
      for(uint j=1;j<small_yticks.size()-1;j++) {
	yw=yticks[i]+small_yticks[j];
	xw=xh;
	WorldToBox(xw,yw,sx1,sy1);
	xw=xl;
	WorldToBox(xw,yw,sx2,sy2);
	 if(sy1<y()+height() && sy1 > y())p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
	
      }
    }
    for(uint j=1;j<small_yticks.size()-1;j++) {
      if(y_scaling==Log) {
	yw=yticks[0]-(1-small_yticks[small_yticks.size()-j-1]);
      } else {
	yw=yticks[0]-small_yticks[j];
      }
      xw=xh;
      WorldToBox(xw,yw,sx1,sy1);
      xw=xl;
      WorldToBox(xw,yw,sx2,sy2);
      if(sy1<y()+height() && sy1 > y())p.drawLine(int(f*sx1),int(f*sy1),int(f*sx2),int(f*sy2));
    }

    p.setPen(old_pen);
  }

  int IRA_box::axistick( double a1, double a2, int nlabel, int maxlabels,
			 string tickScal, vector<double>& list ) {
  list.clear();

  /* adapted from SM */
  int num, iexp, off, n;
  double adiff, diff, amant, step, value;
  // double xlen;

  if( nlabel==0 ) return 0;

  n=0;
  
  if( a1 == a2 ) {
    list.push_back(a1);
    return 1;
  }
  
  /* order is irrelevant, so make a1 the min value */
  if( a1>a2 ) {
    value = a1;
    a1 = a2;
    a2 = value;
  }
  adiff=a2-a1;
  a1 -= 1e-6 * adiff;
  a2 += 1e-6 * adiff;
  
  /*  Identify the scaling method  */
  if("ra"==tickScal) {
    diff  = log10((adiff/15.0)/nlabel ) / log10(60.0);
    iexp  = (int)floor(diff);
    amant = diff - iexp;
    if( iexp<-2 ) {
      /*  This is the sub-second level... resort to base 10/3600 steps  */
      
      diff  = log10( (adiff*3600.0/15.0)/nlabel );
      iexp  = (int)floor(diff);
      amant = diff - iexp;
      if     ( amant < .15 ) num =  1;
      else if( amant < .50 ) num =  2;
      else if( amant < .85 ) num =  5;
      else                   num = 10;
      step = num * pow(10.0, (double)iexp) * 15.0 / 3600.0;
      
    } else if( iexp<0 ) {
      if     ( amant < .10 ) num =  1;
      else if( amant < .21 ) num =  2;
      else if( amant < .30 ) num =  3;
      else if( amant < .36 ) num =  4;
      else if( amant < .43 ) num =  5;
      else if( amant < .46 ) num =  6;
      else if( amant < .60 ) num = 10;
      else if( amant < .70 ) num = 15;
      else if( amant < .79 ) num = 20;
      else if( amant < .92 ) num = 30;
      else                   num = 60;
      step = num * pow(60.0, (double)iexp) * 15.0;
    } else {
      if     ( amant < .10 ) num =  1;
      else if( amant < .20 ) num =  2;
      else if( amant < .30 ) num =  3;
      else if( amant < .40 ) num =  4;
      else if( amant < .46 ) num =  6;
      else if( amant < .55 ) num =  8;
      else                   num = 12;
      step = num * pow(60.0, (double)iexp) * 15.0;
    }
    
  } else if( "dec"==tickScal ) {
    diff  = log10( adiff/nlabel ) / log10(60.0);
    iexp  = (int)floor(diff);
    amant = diff - iexp;
    if( iexp<-2 ) {
      /*  This is the sub-second level... resort to base 10/3600 steps  */
      
      diff  = log10( (adiff*3600.0)/nlabel );
      iexp  = (int)floor(diff);
      amant = diff - iexp;
      if     ( amant < .15 ) num =  1;
      else if( amant < .50 ) num =  2;
      else if( amant < .85 ) num =  5;
      else                   num = 10;
      step = num * pow(10.0, (double)iexp) / 3600.0;
    } else {
      if     ( amant < .10 ) num =  1;
      else if( amant < .21 ) num =  2;
      else if( amant < .30 ) num =  3;
      else if( amant < .36 ) num =  4;
      else if( amant < .43 ) num =  5;
      else if( amant < .46 ) num =  6;
      else if( amant < .60 ) num = 10;
      else if( amant < .70 ) num = 15;
      else if( amant < .79 ) num = 20;
      else if( amant < .92 ) num = 30;
      else                   num = 60;
      step = num * pow(60.0, (double)iexp);
    }

  } else if( "log"==tickScal ) {
    static int logTicks[][10] = {
      { 1, 10 },
      { 1, 3, 10 },
      { 1, 2, 5, 10 },
      { 1, 2, 4, 6, 10 },
      { 1, 2, 4, 6, 8, 10 },
      { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
    };
    double logDiff, base;
    int i, idx;

    if( fabs(a1)>300 || fabs(a2)>300 ) {
      return 0;
    }
    base    = pow(10.0, floor(a1) );
    a1      = pow(10.0,a1);
    a2      = pow(10.0,a2);
    logDiff = adiff / nlabel;

    if( logDiff < 0.15 ) {

      value = a1;
      do {
        diff  = log10( value*pow(10.0, logDiff) - value );
        iexp  = (int)floor(diff);
        amant = diff - iexp;
        if     ( amant < .10 ) num =  1;
        else if( amant < .45 ) num =  2;
        else if( amant < .80 ) num =  5;
        else   { iexp++;       num =  1; }

        base = pow(10.0, (double)iexp);
        step = num * base;

        off  = (int)floor( value / step ) + 1;
        do {
          value = step * off++;
          if( value>=a1 && value<=a2 ) {
            list.push_back(log10(value));
          }
          if( ((int)(value/base))%10 == 0 ) break;
        } while( value<=a2 && n<maxlabels );

        if( value>a2 ) return n;

      } while(1);
    }

    if     ( logDiff < 0.19 ) idx = 5;  /*  1, 2, 3, 4, 5, ...  */
    else if( logDiff < 0.24 ) idx = 4;  /*  1, 2, 4, 6, 8       */
    else if( logDiff < 0.30 ) idx = 3;  /*  1, 2, 4, 6          */
    else if( logDiff < 0.45 ) idx = 2;  /*  1, 2, 5             */
    else if( logDiff < 0.75 ) idx = 1;  /*  1, 3                */
    else                      idx = 0;  /*  1                   */
    if( logDiff > 1.8 )
      step = pow(10.0, floor(logDiff+0.2) );
    else
      step = 10.0;

    i = 0;
    do {
      do {
        value = logTicks[idx][i] * base;
        if( value>=a1 && value <=a2 ) {
          list.push_back(log10(value));
        }
      } while( logTicks[idx][i++]<10 );
      base *= step;
      i = 1;
    } while( value<a2 );

    return n;

  } else {  /*  Default to linear  */

    diff  = log10(adiff / nlabel);
    iexp  = (int)floor(diff);
    amant = diff - iexp;
    if     ( amant < .15 ) num =  1;
    else if( amant < .50 ) num =  2;
    else if( amant < .85 ) num =  5;
    else                   num = 10;
    step = num * pow(10.0, (double)iexp);

  }

  off = (int)floor(a1 / step);
  value = step * (off+1);

  do {
    /*  Handle near-zero case  */
    if( value != 0.0 && fabs(step/value)>1000.0 ) value = 0.0;

    list.push_back(value);
    value += step;
  } while( value<=a2 && n<maxlabels );

  return n;
  }

}
