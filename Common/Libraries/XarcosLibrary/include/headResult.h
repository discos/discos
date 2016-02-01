#ifndef HEADRESULT_H
#define HEADRESULT_H

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Liliana Branciforti(bliliana@arcetri.astro.it)  10/03/2009      Creation                                                  
 */

using namespace std;

/**
  * class HeadResult 
  * This class includes all information necessary to describe a section  
  */

class HeadResult
{
public:  
  /**
   * Constructor
   */
  HeadResult ( );
  
  /**
   * Destructor.
   */
  virtual ~HeadResult ( );
  
  /**
   * This method set the value of frequency.
   * @param var is the value to set.
   */
  void SetFlo (double var) { flo = var;}
  
  /**
   * This method get the value of frequency
   * @return a value of frequency
   */
  double GetFlo ( ) { return flo; }
  
  /**
   * This method set the value of band
   * @param var is the value to set. 
   */
  void SetBanda ( double var ) { banda = var; }
  
  /**
   * This method get the value of band
   * @return a value of band
   */
  double GetBanda ( ) { return banda; }
  
  /**
   * This method set the value of gain
   * @param var is the value to set. 
   */
  void SetGain ( double var ) { gain = var; }
  
  /**
   * This method get the value of gain
   * @return a value of gain
   */
  double GetGain ( ) { return gain; }
  
  /**
   * This method set the value of adc
   * @param var is the value to set.
   */
  void SetAdc ( int var ) { adc = var; }
  
  /**
   * This method get the value of adc
   * @return a value of adc
   */
  int GetAdc ( ) { return adc; }
  
  /**
   * This method set the value of index
   * @param var is the value to set.
   */
  void Setindex ( int var ) { index = var; }
  
  /**
   * This method get the value of index
   * @return a value of index
   */
  int Getindex ( ) { return index; }
  
  /**
    * This method set the value of modoPol
   * @param var is the value to set.
   */
  void SetModoPol ( bool var ) { modoPol = var; }
  
  /**
   * This method get the value of modoPol
   * @return a value of modoPol
   */
  bool GetModoPol ( ) { return modoPol;  }
  
  /**
   * This method setting each elemento of class at zero
   */
  void SetZeroHeadResult ( );
  
 /**
  * Operator
  */
  bool operator==( const HeadResult & second) const;
  bool operator!=( const HeadResult & second) const;
  
private:
  double flo;/*!< frequency of current object section  */
  double banda;/*!<banda of current object section  */
  double gain;/*!< gain of current object section   */
  int adc;/*!< Input of current object section   */
  bool modoPol;/*!<Polarimetric mode of current object section  */
  int index;/*!<index of current object section  */
  
  /**
   * The initAttributes method attempts to initialize the object on create
   */
  void initAttributes ( ) ;

};

#endif // HEADRESULT_H
