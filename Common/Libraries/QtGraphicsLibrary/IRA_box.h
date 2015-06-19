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

#ifndef __IRABOX__
#define __IRABOX__
#include "IRA_plotter.h"
#include "IRA_label.h"
#include <cmath>
#include <qobject.h>

using namespace std;

namespace IRA_GRAPH {

  class zoomRect {
  public:
    zoomRect(double xx1,double xx2, double yy1, double yy2) {
      x1=xx1; x2=xx2; y1=yy1; y2=yy2;
    }
    double x1,x2,y1,y2;
  };

  //The IRA_box class provides a box with coordinate axes on a QCanvas. 
  class IRA_box: public QCanvasRectangle {
  
  
  public:

    IRA_box(IRA_plotter* plot, QString name="");
    IRA_box(double relx, double rely, double relw, double relh, IRA_plotter* plot, QString name="");
    ~IRA_box();
    
    
    int rtti () const { return IRA_plotter::Box; }

    void axis_limits(double xl, 
		     double xh, 
		     double yl, 
		     double yh, bool preserve_zoom=false); // box limits in the WCS 
    
    void BoxToWorld(double xb,
		    double yb,
		    double& xw,
		    double& yw);
    
    void WorldToBox(double xw,
		    double yw,
		    double& xb,
		    double& yb);

    enum axisScaling {Lin=0, Log=1, Deg=2};

    void labels_size(double); // [0,1] i.e. fraction of smallest canvas side
    void axis_labels(string xlab, string ylab);
    void axislabels_mask(bool bottom, bool left, bool top, bool right);
    void axis_scaling(axisScaling xscale, axisScaling yscale);
    void tickslenght(double); // [0,1] i.e. fraction of smallest axis
    void invert_ticks(bool invert); // if true ticks are drawn outside box  
    void tickslabels_mask(bool bottom, bool left, bool top, bool right);
    void tickmarks_mask(bool bottom, bool left, bool top, bool right);
    void set_tickscolor(QColor);
    void show_grid(bool);
    void set_gridColor(QColor);
    void set_boxFont(QFont);    
    void preserve_aspectratio(bool);
    QString name();
    void move_to(double x1, double y1);
    void zoom(double x1, double x2, double y1, double y2);
    void offzoom();
    void unzoom();
    void show_zoom(bool);
    double get_zoomFac();

    axisScaling xscaling() {return x_scaling;}
    axisScaling yscaling() {return y_scaling;}

    double get_xcell() {return xcell;}
    double get_ycell() {return ycell;}
  
    int get_fontsize(double fs);
    
    void draw_ticks(QPainter&, double f=1.0); // draw ticks 
    double ylow();  //zoomed values
    double yhig();
    double xlow();
    double xhig();
    double oylow(); //unzoomed values
    double oyhig();
    double oxlow();
    double oxhig();
    
    void allow_boxscope(bool);
    bool show_boxscope();

   
    
  protected:
    void drawShape( QPainter & );
    void draw_scope(QPainter &);
    /*
    void degToRA(double deg,string& h, string& m, string& s);
    void degToDEC(double deg,string& d, string& m, string& s);
    */
    virtual QPointArray areaPoints () const;

  private:
    IRA_plotter* plotter;

    vector<IRA_label> labels;

    int axmin, axmax, aymin, aymax; //extend area point to cover labels...
    
    double relX,relY,relW,relH;
    QString boxname;
    
    void init();                // variable initialization
    QFont box_font;             // font for labels

    bool preserve_aspect;        // preserve box aspect ratio on canvas resize
    double labelsize;            // fractional labelsize

    double oxl,oxh,oyl,oyh;     // box limits in WCS coords
    double xl,xh,yl,yh;         // zoomed wcs limits
    double xcell,ycell;         // graph to world cellsizes
    void update_geom();         // update box size and position on canvas resize
    void update_tickmarks();
    void update_cellsize();
    int axistick(double a1, 
		 double a2, 
		 int nlabel, 
		 int maxlabels, 
		 string tickScal, 
		 vector<double>& list); // tickScal= ra, dec, log, lin
    
    vector<double> xticks, yticks;      // ticks location calculated by axistick()
    vector<double> small_xticks, small_yticks; // small ticks location calculated by axistick()

    //void draw_ticks(QPainter&, double f=1.0); // draw ticks 
    void draw_grid(QPainter&, double f=1.0); // draw grid
    void draw_tickslabels(QPainter&); //draw tickslabels
    void draw_axislabels(QPainter&);
    bool top_labs,bottom_labs,right_labs,left_labs;
    bool top_axislabs,bottom_axislabs,right_axislabs,left_axislabs;
    bool top_ticks,bottom_ticks,right_ticks,left_ticks;
    axisScaling x_scaling, y_scaling;
    string xlabel,ylabel;
    int xmin,xmax;  // used to place ylabel
    int ymin,ymax;

    double tickslen,slen,len; // % of smallest axis
    int dir;       
    bool plot_grid;
    QColor gc; //grid color
    QColor tc; //tick color

    vector<zoomRect> zoom_histo;   //zooming history
    bool showZoom;                 //show_zoom label
    double zoomFac;
    
    bool scope;

  };



}

#endif
