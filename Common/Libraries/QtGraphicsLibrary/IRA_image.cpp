#include "IRA_image.h"

namespace IRA_GRAPH {
 
  IRA_image::IRA_image(IRA_box* b) : QCanvasRectangle(b->canvas()) {
    box=b;
    //connect(box->bgeom,SIGNAL(box_changed()),this,SLOT(sample_image()));
    init();
  }

  IRA_image::~IRA_image() {
    image.reset();
    if(img) delete img;
    hide();
  }
  
  void IRA_image::init(){
    built_colortables();


    img=0;
    cmap=Flame;
    invert_cmap=false;
    scaling=Sqrt;
    contrast=0;
    bias=0;
    Func=0;
    pixmin=-1;
    pixmax=1;
    blank=-999.999;
    blank_color=QColor(255,255,255);
    //update bounding rect
    setX(box->x());
    setY(box->y());
    setSize(box->width(),box->height());
    xlow=box->xlow();
    ylow=box->ylow();
    xhig=box->xhig();
    yhig=box->yhig();
  
  }

  void IRA_image::set_pixrange(double min, double max) {
    pixmin=min;
    pixmax=max;
    dynamic_range=pixmax-pixmin;
  }

  void IRA_image::set_function(bool (*function)(float x, float y, float& value)) {
    Func=function;
  }

  QString IRA_image::get_value(double xw, double yw, int prec) {
    QString s;
    float val=blank;
    if(Func) Func(xw,yw,val);
    return s.setNum(val,'f',prec);
  }

  void IRA_image::set_contrast_bias_from_mouse(int xx, int yy) {
    
    bias=-1+2.0*(xx-x())/width();
    contrast=-1+2.0*(yy-y())/height();
    
    if(bias<-1 )bias=-1;
    if(bias>1 )bias=1;
    
    if(contrast<-1 )contrast=-1;
    if(contrast>1 )contrast=1;    
    
    map_allimage();
  }
  
  QRgb IRA_image::map_value(float val) {
    QColor c=blank_color;
    if(val != blank) {
      switch(scaling) {
      case(Linear):
	c= (colormaps_table[cmap])[int((colormaps_table[cmap].size()-1)*linearLut(val))];
	break;
      case(Sqrt):
	c= (colormaps_table[cmap])[int((colormaps_table[cmap].size()-1)*sqrtLut(val))];
	break;
      case(Log):
	c= (colormaps_table[cmap])[int((colormaps_table[cmap].size()-1)*logLut(val))];    
	break;
      }
    }
    return c.rgb();
  }

  void IRA_image::map_allimage() {
   
    QColor c=blank_color;
    
    image.reset();
    image.create(box->width(),box->height(),32);
    
    for(int j=0; j<box->height(); j++) {
      for(int i=0; i<box->width(); i++) {
	float val=img[i+j*box->width()];
	/*
	if(val != blank) {
	  
	  switch(scaling) {
	  case(Linear):
	    c= (colormaps_table[cmap])[int((colormaps_table[cmap].size()-1)*linearLut(val))];
	    break;
	  case(Sqrt):
	    c= (colormaps_table[cmap])[int((colormaps_table[cmap].size()-1)*sqrtLut(val))];
	    break;
	  case(Log):
	    c= (colormaps_table[cmap])[int((colormaps_table[cmap].size()-1)*logLut(val))];    
	    break;
	  }
	}
	image.setPixel(i,j,c.rgb());
	*/
	image.setPixel(i,j,map_value(val));
	
      }
    }
    //image.detach();
   
  }


  void IRA_image::sample_image() {
   
    double xw, yw;
    float val=blank;

    if(img) delete img;
    img=new float[box->width()*box->height()];

    image.reset();
    image.create(box->width(),box->height(),32,256);

    for (int j=0; j< box->height(); j++) {
      for (int i=0; i< box->width(); i++) {
	box->BoxToWorld(box->x()+i,box->y()+j,xw,yw); 
	if(!Func(xw,yw,val)) val=blank;
	image.setPixel(i,j,map_value(val));
	img[i+j* box->width()]=val;
	
      }
    }
    //map();

    //update bounding rect
    setX(box->x());
    setY(box->y());
    setSize(box->width(),box->height());
    xlow=box->xlow();
    ylow=box->ylow();
    xhig=box->xhig();
    yhig=box->yhig();
 
  }  

  double IRA_image::linearLut(double z) {
    double min=pixmin;
    double max=pixmax;
    double l=dynamic_range;
    if(z>=max) {
      z=max;
    }else if (z<=min) {
      z=min;
    }
    
    //bias
    z=z-bias*l;
    //contrast
    double xc=0.5*(min+max);
    l=l*(1-contrast)/(1.0000001+contrast);
    min=xc-l/2.0;
    max=xc+l/2.0;
    
    double lut;
    if(z>=max) {
      lut=1;
    }else if (z<=min) {
      lut=0;
    } else {
      lut=(z-min)/l;
    }
    
    if(invert_cmap)lut=1-lut;
    return lut;
  }

  double IRA_image::logLut(double z){
    double min=pixmin;
    double max=pixmax;
    double l=dynamic_range;
    
    if(z>=max) {
      z=max;
    }else if (z<=min) {
      z=min;
    }
    
    //bias
    z=z-bias*l;
    //contrast
    double xc=0.5*(min+max);
    l=l*(1-contrast)/(1.0000001+contrast);
    min=xc-l/2.0;
    max=xc+l/2.0;
    
    double lut;
    if(z>=max) {
      lut=1;
    }else if (z<=min) {
      lut=0;
    } else {
      lut=1/(1-log10((z-min)/l));
    }
    
    if(invert_cmap)lut=1-lut;
    return lut;
  }
  
  double IRA_image::sqrtLut(double z){
    double min=pixmin;
    double max=pixmax;
    double l=dynamic_range;
    
    if(z>=max) {
      z=max;
    }else if (z<=min) {
      z=min;
    }
    
    //bias
    z=z-bias*l;
    //contrast
    double xc=0.5*(min+max);
    l=l*(1-contrast)/(1.0000001+contrast);
    min=xc-l/2.0;
    max=xc+l/2.0;
    
    double lut;
    if(z>=max) {
      lut=1;
    }else if (z<=min) {
      lut=0;
    } else {
      lut=sqrt((z-min)/l);
    }
    
    if(invert_cmap)lut=1-lut;
    return lut;
  }
  
  void IRA_image::set_colormap(Colormap c) {
    cmap=c;
  }

  void IRA_image::set_lut(Lut l) {
    scaling=l;
  }

  bool IRA_image::box_changed() {
    if(x()!=box->x()) return true;
    if(y()!=box->y()) return true;
    if(width() != box->width()) return true;
    if(height() != box->height()) return true;
    if(xlow!=box->xlow()) return true;
    if(xhig!=box->xhig()) return true;
    if(ylow!=box->ylow()) return true;
    if(yhig!=box->yhig()) return true;
    return false;
  }

  void IRA_image::drawShape(QPainter& p) {
  
    if(box_changed()) {
      sample_image();     
    }
    
    p.drawImage( int(x()), int(y()), image);
  } 

  
  void IRA_image::built_colortables() {
    colormaps_table[BlueNoise]=cmap_bluenoise();
    colormaps_table[Cold]=cmap_cold();
    colormaps_table[Smooth]=cmap_smooth();
    colormaps_table[Geography]=cmap_geography();
    colormaps_table[GreenLight]=cmap_greenlight();
    colormaps_table[ReversedSpectrum]=cmap_reversedspectrum();
    colormaps_table[Spectrum]=cmap_spectrum();
    colormaps_table[DarkZero]=cmap_darkzero();
    colormaps_table[HeavyMetal]=cmap_heavymetal();
    colormaps_table[Hot]=cmap_hot();
    colormaps_table[Flame]=cmap_flame();
    colormaps_table[Gray]=cmap_gray();
    colormaps_table[Red]=cmap_red();
    colormaps_table[Green]=cmap_green();
    colormaps_table[Blue]=cmap_blue();
    colormaps_table[RedBow]=cmap_redbow();
    colormaps_table[GreenBow]=cmap_greenbow();
    colormaps_table[BlueBow]=cmap_bluebow();
  }

  vector<QRgb> IRA_image::cmap_bluenoise() {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 255; c++) {
      qc.setHsv(270,255,c);
      v.push_back(qc.rgb());
    }
    for (int c=128; c< 255; c++) {
      qc.setRgb(c,0,255);
      v.push_back(qc.rgb());
    }
    for (int c=0; c<=255; c++) {
      qc.setRgb(255,c,255-c);
      v.push_back(qc.rgb());
    }
    for (int c=255; c>=0; c-=2) {
      qc.setHsv(60,c,255);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_smooth() {
    vector<QRgb> v; QColor qc;
    
    for (int c=0; c<= 255; c++) {
      qc.setRgb(c,0,255-c);
      v.push_back(qc.rgb());
    }
    
    for (int c=0; c<=200; c++) {
      qc.setRgb(255,c,int(c/2.0));
      v.push_back(qc.rgb());
    }
    for (int c=0; c<= 255; c++) {
      qc.setHsv(39,155,255-c);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_spectrum()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<=255; c++) {
      qc.setHsv(0,c,255);
      v.push_back(qc.rgb());
    }
    for (int c=0; c<=6*80; c++) {
      qc.setHsv(int(c/6),255,255); //color range
      v.push_back(qc.rgb());
    }
    for (int c=2*80; c<=2*160; c++) {
      qc.setHsv(int(c/2),255,255); //color range
      v.push_back(qc.rgb());
    }
    for (int c=4*160; c<=4*220; c++) {
      qc.setHsv(int(c/4),255,255); //color range
      v.push_back(qc.rgb());
    }
    for (int c=220; c<=250; c++) {
      qc.setHsv(int(c),255,255); //color range
      v.push_back(qc.rgb());
    }
    for (int c=0; c<=255/4; c++) {
      qc.setHsv(250,255,255-4*c);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_geography()  {
    vector<QRgb> v; QColor qc;
    
    
    for (int c=150; c<= 220; c++) {
      qc.setHsv(240,255,c);
      v.push_back(qc.rgb());
    }
    
    for (int c=0; c<= 255+100+65+100; c++) {
      if(c<=255) qc.setHsv(240,255-c,220);  //deep blue to QColor(255,255,255)
      if(c>255 &&c <=255+100) qc.setHsv(100,255,100+c-255); //rise dark green
      if(c>255+100 &&c <=255+100+60)qc.setHsv(100-c+255+100,255-c+255+100,200);
      if(c>255+100+60)qc.setHsv(35,195,200-c+255+100+65);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_flame()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 255+255+255; c++) {
      if(c<=255) qc.setRgb(c,0,0);
      if(c>255 && c <= 510) qc.setRgb(255,c-255,0);
      if(c>510) qc.setRgb(255,255,c-510);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_cold()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 255+255+255; c++) {
      if(c<=255) qc.setRgb(0,0,c);
      if(c>255 && c <= 510) qc.setRgb(0,c-255,255);
      if(c>510) qc.setRgb(c-510,255,255);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_greenlight()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 255+255; c++) {
      if(c<=255) qc.setRgb(0,c,0);
      if(c>255) qc.setRgb(c-255,255,c-255);
      v.push_back(qc.rgb());
    }
    return v;
  }

  vector<QRgb> IRA_image::cmap_hot()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 510; c++) {
      if(c<=255) qc.setHsv(20,255,c);
      if(c>255)qc.setHsv(20,510-c,255);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_gray()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 255; c++) {
      qc.setRgb(c,c,c);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_heavymetal()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 155+80+50; c++) {
      if(c<155) qc.setRgb(c,c,c);
      if( c >= 155 && c<=155+80)qc.setRgb(155,155,c);
      if( c>155+80)qc.setRgb(155,155+c-155-80,155+80-c+155+80);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_darkzero()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 255; c++) {
      qc.setRgb(255,204-int(0.8*c),255-c);
      v.push_back(qc.rgb());
    }
    
    for (int c=0; c<= 255+80+175+255; c++) {
      if(c<=255) qc.setHsv(20,255,255-c);  //dim red
      if(c>255 &&c <=255+80) qc.setHsv(20+c-255,255,c-255); //rise to green
      if(c>255+80 && c<=255+80+175)qc.setHsv(100,255,80+c-255-80); //enhance green
      if(c>255+80+175)qc.setHsv(100,255-c+255+80+175,255);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_reversedspectrum()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<=255; c++) {
      qc.setHsv(250,c,255);
      v.push_back(qc.rgb());
    }
    for (int c=6*250; c>=6*200; c--) {
      qc.setHsv(int(c/6.0),255,255); //color range
      v.push_back(qc.rgb());
    }
    
    for (int c=6*199; c>=6*140; c--) {
      qc.setHsv(int(c/6.0),255,255); //color range
      v.push_back(qc.rgb());
    }
    for (int c=139; c>=80; c--) {
      qc.setHsv(c,255,255); //color range
      v.push_back(qc.rgb());
    }
    
    for (int c=10*79; c>=10*40; c--) {
      qc.setHsv(int(c/10),255,255); //color range
      v.push_back(qc.rgb());
    }
    for (int c=10*39; c>=0; c--) {
      qc.setHsv(int(c/10),255,255); //color range
      v.push_back(qc.rgb());
    }
    
    for (int c=255; c>=0; c--) {
      qc.setHsv(0,255,c);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_red()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 255; c++) {
      qc.setRgb(c,0,0);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_green()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 255; c++) {
      qc.setRgb(0,c,0);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_blue()  {
    vector<QRgb> v; QColor qc;
    for (int c=0; c<= 255; c++) {
      qc.setRgb(0,0,c);
      v.push_back(qc.rgb());
    }
    return v;
  }
  vector<QRgb> IRA_image::cmap_redbow()  {
    vector<QRgb> v; QColor qc;
    for (int c=255; c>= 0; c--) {
      qc.setRgb(c,0,0);
      v.push_back(qc.rgb());
    }
    for (int c=0; c<= 255; c++) {
      qc.setRgb(c,0,0);
      v.push_back(qc.rgb());
    }
  return v;
  }
  
  vector<QRgb> IRA_image::cmap_greenbow()  {
    vector<QRgb> v; QColor qc;
    for (int c=255; c>= 0; c--) {
      qc.setRgb(0,c,0);
      v.push_back(qc.rgb());
    }
    for (int c=0; c<= 255; c++) {
      qc.setRgb(0,c,0);
      v.push_back(qc.rgb());
    }
    return v;
  }
  
  vector<QRgb> IRA_image::cmap_bluebow()  {
    vector<QRgb> v; QColor qc;
    for (int c=255; c>= 0; c--) {
      qc.setRgb(0,0,c);
      v.push_back(qc.rgb());
    }
    for (int c=0; c<= 255; c++) {
      qc.setRgb(0,0,c);
      v.push_back(qc.rgb());
    }
    return v;
  }
}
