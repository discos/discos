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

#ifndef __IRACURVE__
#define __IRACURVE__
#include "IRA_plotter.h"
#include "IRA_box.h"
#include<deque>
using namespace std;

namespace IRA_GRAPH {

  //The IRA_curve class provides a curve in a box. 
  class IRA_curve: public QCanvasPolygon {
  
  public:

    IRA_curve(IRA_box*);
    ~IRA_curve();

    int rtti () const { return IRA_plotter::Curve; }

    void set_buffer_size(int);
    void set_points(deque<double> x, deque<double> y);
    void add_point(double x, double y);
    void set_color(QColor);
       
    enum curveStyle {Solid=0, Dotted=1, Dashed=2, 
		     LongDashed=3, DotDashed=4};
    void setStyle(curveStyle cs);
    
    void autorescale_box(bool);

  protected:
    void drawShape( QPainter & );
    virtual QPointArray areaPoints () const;
  private:
    IRA_box* box;
    void init(); 
    int buffer_size;  // -1 => illimited
    QPointArray paf;
    QPointArray pa;
    QColor color;
    curveStyle style;
    deque<double> xp;
    deque<double> yp;
    void convert(double f);
    bool autorescale;
  };
}

#endif
