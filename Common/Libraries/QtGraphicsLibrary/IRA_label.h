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

#ifndef __IRALABEL__
#define __IRALABEL__
#include "IRA_plotter.h"
#include "IRA_box.h"

using namespace std;

namespace IRA_GRAPH {

  //The IRA_box class provides a label in a IRA_box 
  class IRA_label : public QCanvasText {
  public:
    IRA_label(IRA_box*);
    ~IRA_label();

    void frac_pos(double x, double y);
    void wcs_pos(double x, double y);
    void set_angle(double);
    void setPrintColor(QColor);
    void printLabel(bool);
    void expand(double);
    void centre(bool);

    int rtti() {return IRA_plotter::Label;}    

  protected:
    void draw(QPainter&);

  private:
    IRA_box* box;
    double fx, fy;
    double angle;
    double e;
    bool centre_justified;
    QColor print_color;
    bool print;
    void init();
    void update_pos();
  };
 
}
  
#endif
