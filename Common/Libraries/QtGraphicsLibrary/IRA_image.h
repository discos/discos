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

#ifndef __IRAIMAGE__
#define __IRAIMAGE__
#include <qimage.h>
#include <qpixmap.h>
#include <map> 
#include "IRA_plotter.h"
#include "IRA_box.h"

using namespace std;

namespace IRA_GRAPH {

  //The IRA_image class provides a image in a box. 
  class IRA_image: public QCanvasRectangle {
  
  public:

    IRA_image(IRA_box*);
    ~IRA_image();

    int rtti () const { return IRA_plotter::Image; }

    
    enum Colormap {Cold=0, Smooth=1, Geography=2, GreenLight=3, ReversedSpectrum=4,
		   Spectrum=5 ,DarkZero=6, HeavyMetal=7, Hot=8, Red=9, Green=10, Blue=11, 
		   RedBow=12, GreenBow=13, BlueBow=14, BlueNoise=15, Flame=16, Gray=17};
    enum Lut {Linear=0, Log=1, Sqrt=2};
    
    void set_colormap(Colormap);
    void set_lut(Lut);
    void set_bias(double);
    void set_contrast(double);
    void set_pixrange(double, double);

    void set_function(bool (*function)(float x, float y, float& value));
    void sample_image();
    QString get_value(double xw, double yw, int prec=6);

    void set_contrast_bias_from_mouse(int x, int y);

  protected:
    void drawShape( QPainter & );
    
  private:
    IRA_box* box;
    void init();
    bool (*Func)(float x, float y, float& val);
    float blank;
    QColor blank_color;

    float* img;

    Colormap cmap;
    bool invert_cmap;
    Lut scaling;
    double contrast, bias;
    double pixmax, pixmin, dynamic_range;
    double linearLut(double);
    double logLut(double);
    double sqrtLut(double);
    double xlow,ylow,xhig,yhig;

    bool box_changed(); //needed to call sample image

    void map_allimage();
    QRgb map_value(float);

    QImage image;
    QPixmap pixmap;

    //colormaps
    std::map<Colormap, vector<QRgb> > colormaps_table;
    void built_colortables();
    
    vector<QRgb> cmap_bluenoise();
    vector<QRgb> cmap_cold();
    vector<QRgb> cmap_smooth();
    vector<QRgb> cmap_geography();
    vector<QRgb> cmap_greenlight();
    vector<QRgb> cmap_reversedspectrum();
    vector<QRgb> cmap_spectrum();
    vector<QRgb> cmap_darkzero();
    vector<QRgb> cmap_heavymetal();
    vector<QRgb> cmap_hot();
    vector<QRgb> cmap_flame();
    vector<QRgb> cmap_gray();
    vector<QRgb> cmap_red();
    vector<QRgb> cmap_green();
    vector<QRgb> cmap_blue();
    vector<QRgb> cmap_redbow();
    vector<QRgb> cmap_greenbow();
    vector<QRgb> cmap_bluebow();    
  };
}

#endif
