#include "xplot.h"
#include "DataIntegrazione.h"
#include "plot.h"
#include <qwt_plot_zoomer.h>
#include <QtGui>
#include <vector>
#include <qfiledialog.h>
#include <fstream>
#include <iostream>

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas, bool doReplot):
        QwtPlotZoomer(xAxis, yAxis, canvas, doReplot){

        setSelectionFlags(QwtPicker::RectSelection | QwtPicker::CornerToCorner);
        setTrackerMode(QwtPicker::AlwaysOff);
        setRubberBand(QwtPicker::NoRubberBand);
        setMaxStackDepth(-1);//dimensione illimitata
        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size
        setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier);
        setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);
    }
};

Xplot::Xplot(QWidget *parent,int sez, vector<DataIntegrazione> *dati)
    : QWidget(parent), MaxSezioni(sez), dato(dati){// Constructors

	ui.setupUi(this);
	connect( ui.pushButtonPrec, SIGNAL( clicked() ), this, SLOT( Precedente_activated() ) );
	connect( ui.pushButtonSucc, SIGNAL( clicked() ), this, SLOT( Successivo_activated() ) );
	ui.pushButtonZoom->setIcon(QIcon("../icon/zoomin.png"));
	connect( ui.pushButtonZoom, SIGNAL( clicked() ), this, SLOT( enableZoom() ) );
	ZOOM=false;
	initPlot();
	N=0;
}




void Xplot::enableZoom(){

	ZOOM=!ZOOM;
	enableZoomMode(ZOOM);
}

void Xplot::enableZoomMode(bool on){

	Base=d_zoomer->zoomRect();//Recupero le dimensioni plottate
 	d_zoomer->setZoomBase(Base);//pulisco lo stack imponendo le dimensioni già visibili
	d_panner->setEnabled(on);
    d_zoomer->setEnabled(on);
    d_zoomer->zoom(0);
    d_picker->setEnabled(true);//!on
    if(on) ui.pushButtonZoom->setIcon(QIcon("../icon/zoomin.png"));
    else ui.pushButtonZoom->setIcon(QIcon("../icon/zoomout.png"));
    showInfo();
}

void Xplot::showInfo(QString text){

    if ( text == QString::null ){
        if ( d_picker->rubberBand() )
            text = "Cursor Pos: Press left mouse button in plot region";
        else
            text = "Zoom: Press mouse button and drag";
    }
}

void Xplot::selected(const QwtPolygon &){

    showInfo();
}

void Xplot::moved(const QPoint &pos){

    QString info;
    info.sprintf("Freq=%g, Ampl=%g",
    d_plot->invTransform(QwtPlot::xBottom, pos.x()),
    d_plot->invTransform(QwtPlot::yLeft, pos.y()));
    showInfo(info);
}

void Xplot::Visual(int tmp){
	
	Sezioni=tmp; 
	if (Sezioni>MaxSezioni) Sezioni=MaxSezioni;
	visualizza(N);
}


void Xplot::Successivo_activated(){

	if (N<(Sezioni-1)){
		ZOOM=false;
		enableZoomMode(ZOOM);
		N++;
	}
	visualizza(N);
}

void Xplot::Precedente_activated(){

	if (N>0){
		ZOOM=false;
		enableZoomMode(ZOOM);
		N--;
	}
	visualizza(N);
}

void Xplot::visualizza(int n){
	DataIntegrazione di;

	ui.SezioneCorrente->setText(QString::number( n,'f',0 ));
	if(dato!=NULL){
		di=(*dato)[n];
		if(di.GetValoriValidi()==true){
			plotData(&di);
		}else plotDataZero();
	}
}

void Xplot::plotDataZero(){
	int nx=4096;
	double BufferX[nx];
	double CurvaA[nx];
	double CurvaB[nx];
	double CurvaC[nx];
	double CurvaD[nx];
	double band = 125;	// Band in MHz

	for (int i=0; i<nx; ++i) BufferX[i]=CurvaA[i]=CurvaB[i]=CurvaC[i]=CurvaD[i]=0;
	d_plot->setDamp(BufferX, CurvaA,CurvaB,CurvaC,CurvaD,nx);
//	d_plot->setDamp(BufferX, CurvaA,CurvaB,nx);
	ui.Xx->setText(QString::number( 0,'f',0 ));
 	ui.Yy->setText(QString::number( 0,'f',0 ));
 	ui.checkOverFlow->setChecked(false);
	d_plot->setAxisScale(QwtPlot::xBottom,-band/2,band/2);
 	d_plot->setAxisScale(QwtPlot::yLeft,0,1000);
 	Base.setRect(-band/2,0,band,1000);
 	d_zoomer->setZoomBase(Base);
}

void Xplot::plotData(DataIntegrazione *di){
    int i, na,nx, nb2,na2;
    double BufferX[di->GetLungArrayDati()];
    double CurvaA[di->GetLungArrayDati()];
    double CurvaB[di->GetLungArrayDati()];
    double CurvaC[di->GetLungArrayDati()];
    double CurvaD[di->GetLungArrayDati()];

    double band =di->GetHead().GetBanda()*1e-6;	// Band in MHz
    double lo1 = di->GetHead().GetFlo()*1e-6;
    double miny=1e99, maxy=-1e99;
    double norm=1;

    if (band==125) lo1=0;//centro il plot sulla freq zero in quanto visualizzo tutta la banda
    double minx=-band/2 + lo1;
    double maxx=band/2 + lo1;
    double yval=0;

    norm=1.0/di->GetTempoIntegrazione();
    ui.Xx->setText(QString::number(di->GetTp()[0] ,'e',2 ));
    ui.Yy->setText(QString::number(di->GetTp()[1],'e',2 ));
    ui.checkOverFlow->setChecked(di->GetOverflow());
    na=nx=di->GetLungArrayDati();
    na2=nb2=di->GetLungArrayDati()/2;
    if(di->GetHead().GetModoPol()==true){
    	for (i=0; i<di->GetLungArrayDati(); ++i) {// Plot X limits
    		yval=di->GetXx()[i]*norm;
			if (yval > maxy) maxy=yval;		// Amplitude limits
			if (yval < miny) miny=yval;
			yval = di->GetYy()[i]*norm;
			if (yval > maxy) maxy=yval;		// Amplitude limits
			if (yval < miny) miny=yval;
			yval = di->GetXy()[i].real()*norm;
			if (yval > maxy) maxy=yval;		// Amplitude limits
			if (yval < miny) miny=yval;			
			yval = di->GetXy()[i].imag()*norm;
			if (yval > maxy) maxy=yval;		// Amplitude limits
			if (yval < miny) miny=yval;
		}
    }else{
    	 for (i=0; i<di->GetLungArrayDati(); ++i) {// Plot X limits
			yval =di->GetXx()[i]*norm;
			if (yval > maxy) maxy=yval;		// Amplitude limits
			if (yval < miny) miny=yval;
		}
    }
    double xmin,xmax,ymin,ymax;// Expand limits by 10%
    xmin=minx*1.05 - maxx*0.05;
    xmax=maxx*1.05 - minx*0.05;
    if (miny == maxy) {
		ymin = miny*0.99;
		ymax = maxy*1.01;
    } else {
		ymin=miny*1.05 - maxy*0.05;
		ymax=maxy*1.05 - miny*0.05;
    }
    for (i=0; i<nx; ++i)	BufferX[i]=CurvaA[i]=CurvaB[i]=CurvaD[i]=CurvaC[i]=0;
    BufferX[0]=minx;
    for (i=1; i<nx; ++i) BufferX[i]=BufferX[i-1]+((maxx-minx)/nx);
    if(di->GetHead().GetModoPol()==true){
    	for (i=0; i<nx; ++i) CurvaA[i]=di->GetXx()[i]*norm;
    	for (i=0; i<nx; ++i) CurvaB[i]=di->GetYy()[i]*norm;
    	for (i=0; i<nx; ++i) CurvaC[i]=di->GetXy()[i].real()*norm;
    	for (i=0; i<nx; ++i) CurvaD[i]=di->GetXy()[i].imag()*norm;
    }else for (i=0; i<nx; ++i) CurvaA[i]=di->GetXx()[i]*norm;

    d_plot->setAxisScale(QwtPlot::xBottom,xmin,xmax,0);
    d_plot->setAxisScale(QwtPlot::yLeft,ymin,ymax,0);
    Base.setRect(xmin-band,ymin,xmax-xmin,ymax-ymin);
 	d_zoomer->setZoomBase(Base);
 	
 	d_plot->setDamp(BufferX, CurvaA, CurvaB,CurvaC,CurvaD,nx);//Disegna le curve
 //	d_plot->setDamp(BufferX, CurvaA, CurvaB,nx);
}

void Xplot::initPlot(){

	d_plot = new Plot(ui.mywidget);
	ui.gridLayout1->addWidget(d_plot, 0, 0, 1, 1);
    d_zoomer = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft,d_plot->canvas(),false);
    d_zoomer->setRubberBand(QwtPicker::RectRubberBand);
    d_zoomer->setRubberBandPen(QColor(Qt::green));
    d_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    d_zoomer->setTrackerPen(QColor(Qt::green));
    d_panner = new QwtPlotPanner(d_plot->canvas());
    d_panner->setMouseButton(Qt::MidButton);
    d_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPicker::PointSelection | QwtPicker::DragSelection,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        d_plot->canvas());
    d_picker->setRubberBandPen(QColor(Qt::green));
    d_picker->setRubberBand(QwtPicker::CrossRubberBand);
    d_picker->setTrackerPen(QColor(Qt::green));
	enableZoomMode(ZOOM);
    double band = 125;	// Band in MHz
    d_plot->setAxisScale(QwtPlot::xBottom,0,band);
 	d_plot->setAxisScale(QwtPlot::yLeft,0,1000);
 	Base.setRect(-band/2,0,band,1000);
 	d_zoomer->setZoomBase(Base);
    showInfo();
    connect(d_picker, SIGNAL(moved(const QPoint &)),SLOT(moved(const QPoint &)));
    connect(d_picker, SIGNAL(selected(const QwtPolygon &)),SLOT(selected(const QwtPolygon &)));
}

Xplot::~Xplot(){ }//Destructors
