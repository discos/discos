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

#ifndef __IRABOXSCOPE__
#define __IRABOXSCOPE__
#include "IRA_plotter.h"
#include "IRA_box.h"
#include "IRA_image.h"

using namespace std;

namespace IRA_GRAPH {

  //The IRA_boxscope class provides a scope in a box. 
  class IRA_boxscope: public QCanvasRectangle {
  
  public:

    IRA_boxscope(IRA_plotter*);
    ~IRA_boxscope();

    int rtti () const { return IRA_plotter::BoxScope; }
    
    void set_box(IRA_box*);
    void set_pos(double x, double y);

  protected:
    void drawShape( QPainter & );
    
  private:
    IRA_box* box;
    IRA_plotter* plot;
    void init();
    double x,y;
  };
}

#endif
