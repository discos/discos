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

#ifndef __IRADATAPOINT__
#define __IRADATAPOINT__
#include "IRA_plotter.h"
#include "IRA_box.h"

using namespace std;

namespace IRA_GRAPH {

  //The IRA_datapoint class provides a data point in a box with errorbars. 
  class IRA_datapoint: public QCanvasRectangle {
  
  public:

    IRA_datapoint(IRA_box*);
    ~IRA_datapoint();

    int rtti () const { return IRA_plotter::DataPoint; }

    void position(double xw, double yw);
    void x_errorbars(double xleft, double xright);
    void y_errorbars(double ybottom, double ytop);
    void expand(double);
    void pointColor(QColor);
    void barColor(QColor);
    
    enum pointShape {Circle=0, Triangle=1, InvTriangle=2, 
		     Square=3, Cross=4, Dot=5, Diamond=6};
    void setShape(pointShape ps);
    void fill(bool);
    
  protected:
    void drawShape( QPainter & );
    
  private:
    IRA_box* box;
    void init();       
    void convert(double f);
    double xw,yw,xlw,xhw,ylw,yhw;
    double x,y,xl,xh,yl,yh;
    int size;
    double e;
    double left,right,top,bottom;
    QColor color;
    QColor color_error;
    bool filled;
    pointShape shape;
  };
}

#endif
