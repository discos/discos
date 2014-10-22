/**************************************************************************
*   IRA Istituto di Radioastronomia                                       *
*   Created Feb 2005 by matteo murgia                                     *
*   murgia@ira.cnr.it                                                     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef __IRAPLOTTER__
#define __IRAPLOTTER__
#include <iostream>
#include <qwmatrix.h> 
#include <string>
#include <vector>
#include <qcanvas.h> 
#include <qpainter.h>
#include <qprinter.h> 
#include <qcursor.h> 
#include <qpopupmenu.h> 
#include <qapplication.h> 

using namespace std;

namespace IRA_GRAPH {

  class IRA_box;
  class IRA_curve;
  class IRA_datapoint;
  class IRA_label;
  class IRA_histogram;
  class IRA_boxscope;
  class IRA_image;

  //The IRA_plotter class provides an on-screen view of a QCanvas. 
  class IRA_plotter: public QCanvasView {

    Q_OBJECT;
  public:
    IRA_plotter(QWidget* parent=0, const char* name=0, WFlags f=0);
    IRA_plotter(int width, int height, QWidget* parent=0, const char* name=0, WFlags f=0);
    ~IRA_plotter();

    enum graphItem {Box=2000, Label=2001, DataPoint=2002, Curve=2003,
		    Histogram=2004, BoxScope=2005, Image=2006};

    int width();
    int height();

    int get_minside();
    
    void show_boxscope(bool);
    void show_popupmenu(bool);

    IRA_boxscope* scope;
    IRA_box* box_under(); //returns a pointer to a box behind cursor (null if none)
    IRA_image* image_under();
  protected:
    // Reimplemented From QWidget
    virtual void contentsMouseMoveEvent(QMouseEvent* e);
    virtual void contentsMousePressEvent(QMouseEvent* e);
    virtual void contentsMouseReleaseEvent(QMouseEvent* e);
    virtual void contentsWheelEvent(QWheelEvent* e);
    virtual void keyReleaseEvent(QKeyEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void leaveEvent(QEvent* e);
    virtual void enterEvent(QEvent* e);
    
    void drawContents(QPainter* p, int clipx, int clipy, int clipw, int cliph); 
    void resizeEvent( QResizeEvent* );

    void update_contrastBias(QPoint p);

    public slots:
      void print();
    void expand();
    void reduce();
    void zoom();
    void show_zoombox();
    void unzoom();
    void offzoom();
    void moveto();
    
  private:
    int hold_key;
    QPoint cursorPos;
    double expfac;
    bool showscope;
    QCanvasRectangle* boxzoom1;
    QCanvasRectangle* boxzoom2;
    void init();
    bool showPopupMenu;
    QPopupMenu* menu;
    
    double zoomfactor;
    
  };
}

#endif
