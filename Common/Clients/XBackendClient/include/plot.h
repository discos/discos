#ifndef PLOT_H
#define PLOT_H

#include <qwt_plot.h>

class QwtPlotCurve;

class Plot: public QwtPlot
{
//    Q_OBJECT 
public:
    /** 
   	* Constructor.
   	* @param parent is the reference at the QWidget on the Dialog.
   	*/
 	Plot(QWidget *parent);
 	
 public slots:
 	/**
      * The set method attempts to display two curve.
      * @param *x array of values of abscissa of the curve 
      * @param *y1 array of values of Order of the first curve
      * @param *y2 array of values of Order of the second curve 
      * @param count is the number of points to draw
      */
     void setDamp(double *x, double *y1, int count);
     void setDamp(double *x, double *y1,double *y2, int count);
     void setDamp(double *x, double *y1,double *y2, double *y3, double *y4, int count);
 private:
 	/**
      * The showData method attempts to display two curve.
      * @param *x array of values of abscissa of the curve 
      * @param *y1 array of values of Order of the first curve
      * @param *y2 array of values of Order of the second curve 
      * @param count is the number of points to draw
      */
 	void showData(double *x,double *y1, int count);
 	void showData(double *x,double *y1, double *y2, int count);
 	void showData(double *x,double *y1, double *y2, double *y3, double *y4, int count);
     
 	QwtPlotCurve *d_crv1;/*!< The Pointer QwtPlotCurve in the Plot object */
    QwtPlotCurve *d_crv2;/*!< The Pointer QwtPlotCurve in the Plot object */    
    QwtPlotCurve *d_crv3;/*!< The Pointer QwtPlotCurve in the Plot object */
    QwtPlotCurve *d_crv4;/*!< The Pointer QwtPlotCurve in the Plot object */
};

#endif // PLOT_H
