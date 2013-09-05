#include "IRA_plotter.h"
#include "IRA_box.h"
#include "IRA_boxscope.h"

namespace IRA_GRAPH {

  IRA_plotter::IRA_plotter(int width, int height, QWidget* parent, const char* name, WFlags f) : QCanvasView(parent,name,f) {
    setCanvas(new QCanvas(width,height));
    init();
  }
  
  IRA_plotter::IRA_plotter(QWidget* parent, const char* name, WFlags f) : QCanvasView(parent,name,f) {
    setCanvas(new QCanvas(visibleWidth(),visibleHeight()));
    init();
  }

  IRA_plotter::~IRA_plotter() {
  }
  
  void  IRA_plotter::init() {
    expfac=1.0;
    hold_key=-1;
       showscope=true;
      viewport()->setMouseTracking(TRUE);
    scope=new IRA_boxscope(this);
    scope->setZ(4);
    scope->hide();

    //boxzoom
    boxzoom1=new QCanvasRectangle(canvas());
    boxzoom1->setPen(QColor(255,255,255));
    boxzoom1->setZ(10);
    boxzoom1->hide();
    boxzoom2=new QCanvasRectangle(canvas());
    boxzoom2->setPen(QColor(0,0,0));
    boxzoom2->setPen(Qt::DotLine);
    boxzoom2->setZ(10);
    boxzoom2->hide();
    zoomfactor=0.5;

    //popup menu
    showPopupMenu=true;
    menu=new QPopupMenu(this);
    menu->insertItem("Zoom",this, SLOT(zoom()));
    menu->insertItem("Unzoom",this, SLOT(unzoom()));
    menu->insertItem("Offzoom",this, SLOT(offzoom()));
    menu->insertItem("Centre",this, SLOT(moveto()));
    menu->insertItem("Expand Graph",this, SLOT(expand()));
    menu->insertItem("Reduce Graph",this, SLOT(reduce()));
    menu->insertSeparator();
    int setid=menu->insertItem("Settings...");
    int prtid=menu->insertItem("&Print",this, SLOT(print()));
    menu->insertSeparator();
    int cloid=menu->insertItem("&Close",qApp, SLOT( closeAllWindows()));
    menu->setAccel( CTRL + Key_P, prtid);
    menu->setAccel( CTRL + Key_C, cloid);
  } 

  void IRA_plotter::show_popupmenu(bool s) {
    showPopupMenu=s;
  }

  void IRA_plotter::drawContents(QPainter* p, int clipx, int clipy, int clipw, int cliph) {
    canvas()->drawArea(canvas()->rect(),p,FALSE);   
    canvas()->update();
   }
 
  int IRA_plotter::width() {return canvas()->width();}
  int IRA_plotter::height() {return canvas()->height();}

  void IRA_plotter::leaveEvent(QEvent* e) {
    releaseKeyboard();
    canvas()->update();
  }

  void IRA_plotter::enterEvent(QEvent* e) {
    grabKeyboard();
    setCursor(QCursor(Qt::CrossCursor));
  }
  
  void IRA_plotter::resizeEvent(QResizeEvent* e) {
    QCanvasView::resizeEvent(e);
    canvas()->resize(visibleWidth(),visibleHeight());
    updateContents();
  }

  void IRA_plotter::contentsWheelEvent(QWheelEvent* e) {
    //canvas()->setAllChanged();
    if(hold_key==Qt::Key_Control) {
      e->accept();
      if(e->delta()<0) {
	expand();
      } else if(e->delta()>0) {
	reduce();
      }
    } else if (hold_key==Qt::Key_Z) {
       if(e->delta()<0) {
	 zoomfactor=zoomfactor*1.2;
       } else if(e->delta()>0) {
	 zoomfactor=zoomfactor*0.8;
       }
       if(zoomfactor>0.95) zoomfactor=0.95;
       if(zoomfactor<0.05) zoomfactor=0.05;
       show_zoombox();
    }
    
    else {
      e->ignore();
    }
  }

  void IRA_plotter::expand() {
    int newx=int(1.1*canvas()->width());
    int newy=int(1.1*canvas()->height());
    canvas()->resize(newx,newy);
    updateContents();
    //canvas()->update();
  }
  void IRA_plotter::reduce() {
    int newx=int(0.9*canvas()->width());
    int newy=int(0.9*canvas()->height());
    if(newx<10)newx=10;
    if(newy<10)newy=10;
    canvas()->resize(newx,newy);
    updateContents();
    //canvas()->update();
  }

   void IRA_plotter::zoom() {
     IRA_box* box=box_under();
   
    if(box) {
      int minside=box->width();
      if (box->height()<minside) minside=box->height();

      minside=zoomfactor*minside;
      double x1, x2, y1, y2;
   
      box->BoxToWorld(cursorPos.x()-0.5*minside,cursorPos.y()+0.5*minside,x1,y1);
      box->BoxToWorld(cursorPos.x()+0.5*minside,cursorPos.y()-0.5*minside,x2,y2);
      if(x2>x1 && y2 >y1) box->zoom(x1,x2,y1,y2);
      repaintContents(box->boundingRect());
    }
  }


  void IRA_plotter::show_zoombox() {
    IRA_box* box=box_under();
    if(box) {
      int minside=box->width();
      if (box->height()<minside) minside=box->height();

      minside=zoomfactor*minside;
      boxzoom1->setX(cursorPos.x()-0.5*minside);
      boxzoom1->setY(cursorPos.y()-0.5*minside);
      boxzoom1->setSize(int(minside),int(minside));
      boxzoom1->show();
      boxzoom2->setX(cursorPos.x()-0.5*minside);
      boxzoom2->setY(cursorPos.y()-0.5*minside);
      boxzoom2->setSize(int(minside),int(minside));
      boxzoom2->show();
    }
  }

  void IRA_plotter::unzoom() {
    IRA_box* box=box_under();
    if(box) {
      box->unzoom();
      repaintContents(box->boundingRect());
    }
  }
  
  void IRA_plotter::offzoom() {
    IRA_box* box=box_under();
    if(box) {
      box->offzoom();
      repaintContents(box->boundingRect());
    }
  }

  void IRA_plotter::moveto() {
    IRA_box* box=box_under();
    if(box) {
      double x, y;
      box->BoxToWorld(cursorPos.x(),cursorPos.y(),x,y);
      box->move_to(x,y);
      repaintContents(box->boundingRect());
    }
  }

  void IRA_plotter::show_boxscope(bool s) {
    showscope=s;
    if(!showscope) scope->hide(); 
  }

  void IRA_plotter::contentsMousePressEvent(QMouseEvent* e) {
    cursorPos=viewportToContents(mapFromGlobal(QCursor::pos()));

    if ( e->button() == Qt::RightButton) {
      if(showPopupMenu)menu->popup(QCursor::pos()+QPoint(10,10));
    }
  }

  void IRA_plotter::contentsMouseReleaseEvent(QMouseEvent* e) {
  }
  
  void IRA_plotter::keyPressEvent(QKeyEvent* e) {
    hold_key=e->key();
    cursorPos=viewportToContents(mapFromGlobal(QCursor::pos()));

    //zoom
    if(hold_key==Qt::Key_Z &&   e->isAutoRepeat()==false) show_zoombox();      
    
    //unzoom
    if(hold_key==Qt::Key_X && e->isAutoRepeat()==false) unzoom();

    //unzoom
    if(hold_key==Qt::Key_X && e->isAutoRepeat()==true) offzoom();
    
    //shift
    if(hold_key==Qt::Key_S && e->isAutoRepeat()==false) moveto();
    canvas()->update();
  }

  void IRA_plotter::keyReleaseEvent(QKeyEvent* e) {
    hold_key=-1;
    if(e->key()==Qt::Key_Z  && e->isAutoRepeat()==false) {
      boxzoom1->hide();
      boxzoom2->hide();
      zoom();
    }
    canvas()->update();
  }

  IRA_box* IRA_plotter::box_under() {
    QCanvasItemList l=canvas()->collisions(cursorPos);
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
      if((*it)->rtti()==IRA_plotter::Box) return (IRA_box*)*it;
    return NULL;
  };
  
  IRA_image* IRA_plotter::image_under() {
    QCanvasItemList l=canvas()->collisions(cursorPos);
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
      if((*it)->rtti()==IRA_plotter::Image) return (IRA_image*)*it;
    return NULL;
  };
  

  void IRA_plotter::update_contrastBias(QPoint p) {
    IRA_image* img=image_under();
    if(img) img->set_contrast_bias_from_mouse(p.x(),p.y());
  }

  void IRA_plotter::contentsMouseMoveEvent(QMouseEvent* e) {
    QPoint p = inverseWorldMatrix().map(e->pos());
   
    scope->hide();
    //setCursor(QCursor(Qt::ArrowCursor));

    viewportToContents(p);

    QCanvasItemList l=canvas()->collisions(p);
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
      
      if((*it)->rtti()==IRA_plotter::Box) {
	
	//setCursor(QCursor (Qt::CrossCursor));
	
	//the boxscope
	if(showscope) {
	  scope->set_box((IRA_box*)*it);
	  scope->set_pos(p.x(),p.y());
	  scope->show();
	}
	
      } else if((*it)->rtti()==IRA_plotter::Curve) {
      } else {
      }
    }
    
    if(boxzoom1->isVisible()) show_zoombox();
    
    
    if ( e->state() == Qt::LeftButton) update_contrastBias(p);
    
    canvas()->update();
  }

  int IRA_plotter::get_minside() {
    int scale=width();
    if(height() < width() )scale =height();
    return scale;
  }
  
  void IRA_plotter::print() { 

    cout<<"PRINTING CANVAS TO IRA_plotter.ps"<<endl; 
    int xx=canvas()->width();
    int yy=canvas()->height();

    QPrinter* printer=new QPrinter;
    printer->setColorMode(QPrinter::Color);
    printer->setOutputToFile(TRUE);
    printer->setOutputFileName("IRA_plotter.ps");
    QPainter pp(printer);

    //A4 uses 210, A5 148
    int pagepix=int(210/25.4)*printer->resolution();
    
    canvas()->resize(pagepix,pagepix);    
    canvas()->update();
       
    //Determine rightmost item
     int w=0;
     QCanvasItemList list = canvas()->allItems();
     QCanvasItemList::Iterator it = list.begin();
     int xmax=0;
     int xmin=1e30;
     int ymin=1e30;
     int ymax=0;
     for (; it != list.end(); ++it) {
	int x1=((*it)->boundingRect ()).left ();
	int x2=((*it)->boundingRect ()).right ();
	int y2=((*it)->boundingRect ()).bottom ();
	int y1=((*it)->boundingRect ()).top ();
	if(x1<xmin) xmin=x1;
	if(x2>xmax) xmax=x2;
	if(y1<ymin) ymin=y1;
	if(y2>ymax) ymax=y2;
	
	cerr<<xmin<<" "<<ymin<<" "<<xmax<<" "<<ymax<<endl;

	//if(((*it)->boundingRect ()).right () > w) 
	 // w=((*it)->boundingRect ()).right ();
     }

     pp.save();
     double scale=1.0*pagepix/xmax;
     pp.scale(scale,scale);
     pp.translate( xmin, ymin );
     pp.setClipRegion(QRect(xmin,ymin,xmax-xmin,ymax-ymin),QPainter::CoordPainter);
     
     canvas()->drawArea(canvas()->rect(),&pp,FALSE);
     //canvas()->drawArea(QRect ( xmin, ymin, xmax-xmin, ymax-ymin ),&pp,FALSE);
     canvas()->resize(xx,yy);
     updateContents();
     pp.restore();
			  	     
  }
}
