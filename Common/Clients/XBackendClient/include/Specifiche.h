#ifndef SPECIFICHE_H
#define SPECIFICHE_H

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Liliana Branciforti(bliliana@arcetri.astro.it)  10/03/2009      Creation                                                  
 */

#include "headResult.h"
#include <vector>
#include <qstring.h>

using namespace std;


/**
  * class Specifiche
  * This class includes all information necessary to describe the specific 
  * of a integration 
  */

class Specifiche
{
public:
  /**
   * Constructor
   */
  Specifiche ( );
  
  /**
   * Destructor.
   */
  virtual ~Specifiche ( );
  int tempoEle; /*!<integration integration basic time*/
 
  /**
   * This method set the value of flag Modo8bit.
   * @param var is the value to set.
   */
  void SetModo8bit ( bool var )  { modo8bit = var; }
  
  /**
   * This method get the value of flag Modo8bit
   * @return a value of flag Modo8bit
   */
  bool GetModo8bit ( )  { return modo8bit;  }
  
  /**
   * This method set the value of integration time.
   * @param var is the value to set.
   */
  void SetTempoIntegrazione ( int var ) { tempoIntegrazione = var; }
  
  /**
   * This method get the value of integration time.
   * @return a value of integration time.
   */
  int GetTempoIntegrazione ( ) { return tempoIntegrazione;  }
  
  /**
   * This method set the value of remaining cycle of integration.
   * @param var is the value to set.
   */
  void SetNCicli ( int var ) {  nCicli = var;  }
  
  /**
   * This method get the value of remaining cycle of integration.
   * @return a value of remaining cycle of integration.
   */
  int GetNCicli ( ) { return nCicli;  }
  
  /**
   * This method setting each elemento of array SpecificaSezione at zero
   */
  void ResetArray ();
  
  /**
   * This method inset a new elemento on the array SpecificaSezione
   */
  void InsertElementArray ();

  /**
   * This method get the value of absolute Max Section.
   * @return a value of absolute Max Section.
   */
  int GetMaxSezioni ( ) {  return maxSezioni;  }
  
  /**
    * This method set the value of current Max Section.
    * @param var is the value to set.
    */
  void SetNSezioni ( int var ) { nSezioni = var; }
  
  /**
   * This method get the value of current Max Section.
   * @return a value of current Max Section.
   */ 
  int GetNSezioni ( ) {  return nSezioni;  }
  
   /**
    * The SetFunctionAdc method attempts to set the type of Function ADCMode
    * This methodn is only in available server version xarcos 
    * @param var is the value to set.
    */
  void SetFunctionAdc ( int new_var ) { if((new_var>=0)&&(new_var<6)) FunctionAdc = new_var;
  										else FunctionAdc=0;}
  /**
   * The GetFunctionAdc method attempts to get the setting type of Function ADCMode
   * This method is only in available server version xarcos 
   * @return a value of current setting of Function ADCMode.
   */
   int GetFunctionAdc ( ) {  return FunctionAdc;  }

public:
  vector<HeadResult> specificaSezione;  /*!<Sections array of object*/
private:
  int nSezioni;/*!< Max Section of current object*/
  int maxSezioni;/*!< absolute Max Section */
  bool modo8bit;/*!<flag Modo8bit of object*/
  int tempoIntegrazione;/*!<integration time of object*/
  int nCicli;/*!< cycle of integration of object*/
  int FunctionAdc;/*!<Function ADCMode of object*/

  /**
   * The initAttributes method attempts to initialize the object on create
   */
  void initAttributes ( ) ;

};

#endif // SPECIFICHE_H
