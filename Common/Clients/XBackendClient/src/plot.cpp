#include <qwt_math.h>
#include <qwt_scale_engine.h>
#include <qwt_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include "plot.h"


Plot::Plot(QWidget *parent): //Costruttore della classe
    QwtPlot(parent)
{
    setAutoReplot(false);

    setCanvasBackground(QColor(Qt::white));

    // legend
    QwtLegend *legend = new QwtLegend;
    legend->setFrameStyle(QFrame::Sunken);
    insertLegend(legend, QwtPlot::RightLegend);

    // grid 
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::blue, 0, Qt::DotLine));
    grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(this);

    // axes
    setAxisTitle(QwtPlot::xBottom, "Frequency (MHz)");
    setAxisTitle(QwtPlot::yLeft, "Amplitude");
    
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
    setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  
    // curves
    d_crv1 = new QwtPlotCurve("Xx");
    d_crv1->setRenderHint(QwtPlotItem::RenderAntialiased);
    d_crv1->setPen(QPen(Qt::blue));
    d_crv1->setYAxis(QwtPlot::yLeft);
    d_crv1->attach(this);

    d_crv2 = new QwtPlotCurve("Yy");
    d_crv2->setRenderHint(QwtPlotItem::RenderAntialiased);
    d_crv2->setPen(QPen(Qt::red));
    d_crv2->setYAxis(QwtPlot::yLeft);
    d_crv2->attach(this);
 
    d_crv3 = new QwtPlotCurve("Xy.Real");
    d_crv3->setRenderHint(QwtPlotItem::RenderAntialiased);
    d_crv3->setPen(QPen(Qt::cyan));
    d_crv3->setYAxis(QwtPlot::yLeft);
    d_crv3->attach(this);   
    
    d_crv4 = new QwtPlotCurve("Xy.Imag");
    d_crv4->setRenderHint(QwtPlotItem::RenderAntialiased);
    d_crv4->setPen(QPen(Qt::magenta));
    d_crv4->setYAxis(QwtPlot::yLeft);
    d_crv4->attach(this);   
    
    setAutoReplot(true);
}


void Plot::showData(//Visualizza i le curve
		double *x,//varori delle ascisse
		double *y1,//varori delle ordinate della curva Ch A
		int count)//Numero di punti da disegnare
{
    d_crv1->setData(x, y1, count);//Ch A
 
}

void Plot::showData(//Visualizza i le curve
		double *x,//varori delle ascisse
		double *y1,//varori delle ordinate della curva Ch A
		double *y2,//varori delle ordinate della curva Ch B
		int count)//Numero di punti da disegnare
{
    d_crv1->setData(x, y1, count);//Ch A
    d_crv2->setData(x, y2, count);//Ch B
}

void Plot::showData(//Visualizza i le curve
		double *x,//varori delle ascisse
		double *y1,//varori delle ordinate della curva Ch A
		double *y2,//varori delle ordinate della curva Ch B
		double *y3,//varori delle ordinate della curva Ch Real
		double *y4,//varori delle ordinate della curva Ch Imag		
		int count)//Numero di punti da disegnare
{
    d_crv1->setData(x, y1, count);//Ch A
    d_crv2->setData(x, y2, count);//Ch B
    d_crv3->setData(x, y3, count);//Ch Real
    d_crv4->setData(x, y4, count);//Ch Imag
}

void Plot::setDamp(//Visualizza i le curve
		double *x,//varori delle ascisse
		double *y1,//varori delle ordinate della curva Ch A
		int count)//Numero di punti da disegnare
{
    const bool doReplot = autoReplot();
    setAutoReplot(false);
    showData(x, y1, count);
    setAutoReplot(doReplot);
    replot();
}

void Plot::setDamp(//Visualizza i le curve
		double *x,//varori delle ascisse
		double *y1,//varori delle ordinate della curva Ch A
		double *y2,//varori delle ordinate della curva Ch B
		int count)//Numero di punti da disegnare
{
    const bool doReplot = autoReplot();
    setAutoReplot(false);
    showData(x, y1, y2, count);
    setAutoReplot(doReplot);
    replot();
}

void Plot::setDamp(//Visualizza i le curve
		double *x,//varori delle ascisse
		double *y1,//varori delle ordinate della curva Ch A
		double *y2,//varori delle ordinate della curva Ch B
		double *y3,//varori delle ordinate della curva Ch Real
		double *y4,//varori delle ordinate della curva Ch Imag		
		int count)//Numero di punti da disegnare
{
    const bool doReplot = autoReplot();
    setAutoReplot(false);
    showData(x, y1, y2,  y3, y4, count);
    setAutoReplot(doReplot);
    replot();
}
