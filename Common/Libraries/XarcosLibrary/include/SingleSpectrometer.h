#ifndef SINGLESPECTROMETER_H
#define SINGLESPECTROMETER_H

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Liliana Branciforti(bliliana@arcetri.astro.it)  10/03/2009      Creation                                                  
 */

#include "BbcFilter.h"
#include "Spectrometer.h"
#include "AdcConditioner.h"
#include "Data.h"
#include "XaMutex.h"

/**
  * This object has the task of representing the interconnections real 
  * of the various elements that make up the HW 
  * This class regroups an object Spectrometer and two object BbcFilter
  * to form a single entity.
  */

class SingleSpectrometer
{
public:	  
  /**
   * Constructor
   * @param intf is pointer to object that manages the TCP Protocol with the HW 
   */	
  SingleSpectrometer(Cpld2Intf *intf);
  
  /**
   * Constructor
   * @param intf is the pointer to object that manages the TCP Protocol with the HW 
   * @param addrFFT is the physical address of elemet FFT
   * @param addrBBC1 is the physical address of elemet BBC1 on the FFT
   * @param addrBBC2 is the physical address of elemet BBC2 on the FFT
   */
  SingleSpectrometer(Cpld2Intf *intf,int addrFFT, int addrBBC1, int addrBBC2,int PuntiSpettro=4096);
  
  /**
   * Destructor.
   */
  virtual ~SingleSpectrometer ( );
   
  /**
   * This method initializes the HW and the object SingleSpectrometer
   * @param single is the spectrometer pointer of object
   * @param tmpD is the pointer of the values set 
   */
  void Init(SingleSpectrometer *single, Data *tmpD);
  
  /**
   * This method manages the recovery of the measures of TP 
   * @param single is the spectrometer pointer of object
   * @param tmpD is the pointer to the location of memory to the rescue by the data 
   * @param modo=true start the measure 
   * 	    modo=false retrieve the results of the measure 
   */
  void GetDataTp(SingleSpectrometer *single, Data *tmpD,bool modo);
  
  /**
   * This method manages the recovery of integration 
   * @param single is the spectrometer pointer of object
   * @param tmpD is the pointer to the location of memory to the rescue by the data
   */
  void GetData(SingleSpectrometer *single,Data *tmpD);

protected:
  Spectrometer *fft;/*!< is pointer to object FFT of object */
  BbcFilter *bbc1;/*!< is pointer to primary object BBC of object */
  BbcFilter *bbc2;/*!< is pointer to secondary object BBC of object */
  
public:  
  /**
	* This method get the spectrometer pointer of object 
	* @return a value of spectrometer pointer
	*/
  Spectrometer* GetFFT ( ) {  return fft; }
 
  /**
	* This method get the pointer to primary BBC of object
	* @return a value of BBC pointer
	*/
  BbcFilter* GetBbc1 ( ) {  return bbc1; }//Get the value of bbc1
  
  /**
	* This method get the pointer to secondary BBC of object
	* @return a value of BBC pointer
	*/
  BbcFilter *GetBbc2 ( ) { return bbc2; }//Get the value of bbc2   

private:
  bool modo8Bit;/*!< flag Modo8bit of object */
  bool modoPolarimetrico;/*!< Polarimetric mode of object */
  double tempoIntElementare;/*!< integration basic time of object */
  int numPuntiSpettro;/*!< Spectral number points  */
  double banda;/*!< banda of current object */
  double freqCentroBandaA;/*!< primary frequency of object */
  double freqCentroBandaB;/*!< secondary frequency of object */

public:
  /**
   * This method set the value of modo8Bit
   * @param new_var is the value to set
   */
  void setModo8Bit ( bool new_var ) { modo8Bit = new_var; }
  
  /**
	* This method get the value of modo8Bit
	* @return a value of modo8Bit
	*/
  bool getModo8Bit ( ) { return modo8Bit; }
  
  /**
   * This method set the value of modoPolarimetrico
   * @param new_var is the value to set
   */
  void setModoPolarimetrico ( bool new_var ) { modoPolarimetrico = new_var; }
  
  /**
	* This method get the value of modoPolarimetrico
	* @return a value of modoPolarimetrico
	*/
  bool getModoPolarimetrico ( ) { return modoPolarimetrico; }
  
  /**
   * This method set the value of integration basic time
   * @param new_var is the value to set
   */
  void setTempoIntElementare ( double new_var ) { tempoIntElementare = new_var; }
  
  /**
	* This method get the value of  integration basic time
	* @return a value of  integration basic time
	*/
  double getTempoIntElementare ( ) { return tempoIntElementare; }
  
  /**
   * This method set the value of spectral number points
   * @param new_var is the value to set
   */
  void setNumPuntiSpettro ( int new_var ) { 
	  if(new_var>4096) numPuntiSpettro = 4096;
	  if(new_var<=0) numPuntiSpettro=1;
	  numPuntiSpettro=new_var;
  }
  
  /**
	* This method get the value of spectral number points
	* @return a value of spectral number points
	*/
  int getNumPuntiSpettro ( ) { return numPuntiSpettro; }
  
  /**
   * This method set the value of banda
   * @param new_var is the value to set
   */
  void setBanda ( double new_var ) {  banda = new_var; }
  
  /**
	* This method get the value of banda
	* @return a value of banda
	*/
  double getBanda ( ) { return banda; }
  
  /**
   * This method set the value of freqCentroBandaA
   * @param new_var is the value to set
   */
  void setFreqCentroBandaA ( double new_var ) { freqCentroBandaA = new_var; }
  
  /**
	* This method get the value of freqCentroBandaA
	* @return a value of freqCentroBandaA
	*/
  double getFreqCentroBandaA ( ) { return freqCentroBandaA; }
  
  /**
   * This method set the value of freqCentroBandaB
   * @param new_var is the value to set
   */
  void setFreqCentroBandaB ( double new_var ) { freqCentroBandaB = new_var; }

  /**
	* This method get the value of freqCentroBandaB
	* @return a value of freqCentroBandaB
	*/
  double getFreqCentroBandaB ( ) { return freqCentroBandaB;}
  
  /**
   * This method septum this object with values OFF for HW 
   */
  void setOFF(int PuntiSpettro=4096);
  
  /**
   * This method septum this object with values ON for HW 
   */
  void setON(int PuntiSpettro=4096);
  
private:
 /**
   * The initAttributes method attempts to initialize the object on create
   * @param intf is pointer to object that manages the TCP Protocol with the HW 
   */
  void initAttributes (Cpld2Intf *intf ) ;

};

#endif // SINGLESPECTROMETER_H
