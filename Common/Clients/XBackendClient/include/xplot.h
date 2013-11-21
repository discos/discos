#ifndef XPLOT_H
#define XPLOT_H

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Liliana Branciforti(bliliana@arcetri.astro.it)  02/04/2009      Creation                                                  
 */

#include <QtGui/QWidget>
#include "xplotGUI.h"
#include <qwt_plot.h>
#include <qwt_counter.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_text.h>
#include <qwt_math.h>
#include "DataIntegrazione.h"
#include "plot.h"
#include <qmainwindow.h>
#include <qwt_polygon.h>
#include <qregexp.h>
#include <qapplication.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qprinter.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qfiledialog.h>
#include <qprintdialog.h>
#include <qfileinfo.h>
#include <vector>

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;

/**
 * The Xplot class administrate a windows. 
 * This windows show the results of integration each section.
 * It provides display of curves of results in region plotting.
 * It also provides a cursor position and implement zooming in plot region.
 * 
 * Using the Xplot class
 * 1.Create the xplot.ui that contain
 * 		ui.pushButtonPrec
 * 		ui.pushButtonSucc
 * 		ui.pushButtonZoom
 * 		ui.mywidget 
 * 		ecc...
 * 2.Make sure that you include Xplot.h 
 */ 
class Xplot: public QWidget
{
    Q_OBJECT
public:
	int N;/*!< is the current section show */
	int Sezioni,MaxSezioni;/*!< is the MaxSection can to be display */
	vector<DataIntegrazione> *dato;/*!<is a pointer vector <dataIntegrazione> results to show */
    QwtDoubleRect Base,Rect;/*!< QwtDoubleRect object */
    bool ZOOM;/*!< is the state of zooming */
    
    /** 
     * Constructor.
 	 * @param parent is the reference at the QMainWindow  
 	 * @param sez is the MaxSection that can to be display
 	 * @param dati is a pointer vector <dataIntegrazione> results to show
 	 */
     Xplot(QWidget *parent=0,int sez=8, vector<DataIntegrazione> *dati=0);
    
    /**
  	 * Destructor.
  	 */
     ~Xplot();    
    
public slots:

	/**
     * The Successivo_activated slots method attempts to display following section. 
     */
	void Successivo_activated();
	
	/**
     * The Precedente_activated slots method attempts to display previous section. 
     */
	void Precedente_activated(); 
	
	/**
     * The Visual slots method attempts to display the section tmp. 
     * @param tmp is the number of section to display.
     */
	void Visual(int tmp);
	
	/**
     * The initPlot slots method attempts to  initialize the plot region. 
     */
	void initPlot();
	
	/**
     * The plotDataZero slots method attempts to display the plot region empty. 
     */
	void plotDataZero();
	
	/**
     * The plotData slots method attempts to display the result in plot region. 
     */
	void plotData(DataIntegrazione *tp);
	
	/**
     * The enableZoom slots method attempts to activate or deactivate zoom. 
     */
	void enableZoom();
	
private slots:
	/**
	 * The moved slots method attempts to move the cursor.
	 * @param QPoint is the position of cursor
	 */
    void moved(const QPoint &pos);
    
    /**
	 * The selected slots method attempts to selecte the zoom region. 
	 * @param QwtPolygon
	 */
    void selected(const QwtPolygon &pol);
    
	/**
     * The enableZoom method attempts set the zoom. 
     * @param on is state the zoom to set.
     */
    void enableZoomMode(bool on);
   	
private:
	/**
     * The visualizza method attempts to display the section n. 
     * @param n is the number of section to display.
     */
    void visualizza(int n);  
    /**
     * The showInfo method attempts visual the information position on the plot region. 
     * @param on is state the zoom to set.
     */
    void showInfo(QString text = QString::null);
   
    Ui::XplotGUIClass ui;/*!< Ui::XplotGUIClass create by Qt moc  */
    Plot *d_plot;/*!< The pointer to Plot in the Xplot object */
    QwtPlotZoomer *d_zoomer;/*!< The pointer to QwtPlotZoomer in the Xplot object */
    QwtPlotPicker *d_picker;/*!< The pointer to QwtPlotPicker in the Xplot object */
    QwtPlotPanner *d_panner;/*!< The pointer to QwtPlotPanner in the Xplot object */
};

#endif // XPLOT_H
