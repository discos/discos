#ifndef LED_H_
#define LED_H_

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Liliana Branciforti(bliliana@arcetri.astro.it)  01/10/2009      Creation                                                  
 */

#include <QLabel>
#include <QIcon>
#include <QString>

/**
* Sometimes you find yourself needing an indicator to reflect various statuses.
* The Led class is a QWidget control for a LED control and is union of the QIcon and QLabel class. 
* The code assumes LEDs of 18 X 18 pixels.
* 
* Using the Led class is very simple
* 1. Insert a new QWidget frame where you want the LED. 
* 2. Rename the QWidget frame to somthing other than myLed. 	
* 3. In the code create e new object call to the reference ui.myLed 
* 4. Make sure that you include Led.h 
*/
class Led
{
public:
	
	/**
     * \enum color
     * The color enum is define a specific color of the icon Led class.
     * An color may be either Green, Red or Yellow
     */
   enum color{	Green, Red, Yellow }; 
   
	 /**
	 * \enum mode
	 * The mode enum is define a specific state of color of the icon Led class.
     * An mode may be either On or Off
	 */
   enum mode{ On,Off };
   
   /** 
  	* Constructor.
  	* @param parent is the reference at the QWidget on the Dialog
  	* @param str is the string Label that describes the function of the Led. The default is empty.
  	* @param attrt is the color to creation the Led. The default is Led::Green.
  	* @param modt is the mode to creation the Led. The default is Led::On.
  	*/
	Led(QWidget * parent = 0,QString str="",color attrt=Green, mode modt=On);
	
	 /**
 	 * Destructor.
 	 */
	virtual ~Led();
	 
	/**
     * The setColor method attempts to set or reset the color of the Led.
     * @param attrt
     */
	void setColor(color attrt);
	 /**
     * The setMode method attempts to set or reset the mode of the Led. 
     * @param modt
     */
	void setMode(mode modt);
	
private:
	QLabel *lab;/*!< The pointer to icon QLabel of led object */
	QLabel *labT;/*!< The pointer to texst QLabel of led object */
	color attr;/*!< The color led object */
	mode mod;	/*!< The mode led object */
    QIcon *icon;/*!< The pointer to icon led object */
    QString text;/*!< The describe string led object */
};

#endif /*LED_H_*/
