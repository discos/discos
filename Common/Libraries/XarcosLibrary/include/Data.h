#ifndef DATA_H
#define DATA_H

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
#include <time.h>
#include <sys/timeb.h>


/**
  * This class includes all information necessary to administrate HW and specific
  * This class contain two object HeadResult, this permit to configure HW
  * in Full or single polarimetric Mode section and furthermore put together two section 
  * single polarimetric Mode.
  * It also maintain all information for single integration. 
  */

class Data 
{
public:
  /**
   * Constructor
   */
  Data ( );
  
  /**
   * Destructor.
   */
  virtual ~Data ( );
  
  /**
    * This method set primary header.
    * @param var is the value to set.
    */
  void SetIntestazioneCh1 ( HeadResult var ) { intestazioneCh1 = var; }
  
  /**
	* This method get the value of primary header
	* @return a value of primary header
	*/
  HeadResult GetIntestazioneCh1 ( ) { return intestazioneCh1; }
  
  /**
    * This method set the value of secondary header.
    * @param var is the value to set.
    */
  void SetIntestazioneCh2 ( HeadResult var ) { intestazioneCh2 = var; }
  
  /**
	* This method get the value of secondary header
	* @return a value of secondary header
	*/
  HeadResult GetIntestazioneCh2 ( ) { return intestazioneCh2; }
 
  /**
    * This method set the value of UT time beginning of integration.
    * @param var is the value to set.
    */
  void SetUtInizio ( timeb var ) { utInizio = var; }
  
  /**
	* This method get the value of UT time beginning of integration
	* @return a value of UT time beginning of integration
	*/
  timeb GetUtInizio ( ) { return utInizio; }//Get the value of utInizio
 
  /**
    * This method set the value of UT time ending of integration.
    * @param var is the value to set.
    */
  void SetUtFine ( timeb new_var ) { utFine = new_var; }
  
  /**
	* This method get the value of UT time ending of integration 
	* @return a value of UT time ending of integration
	*/
  timeb GetUtFine ( ) { return utFine; }

  /**
    * This method set the value of pointer of area of memory to store
    * results of integration
    * @param var is the value to set.
    */
  void SetBuffer ( long* var )  { buffer=var; }
  
  /**
	* This method get the value of pointer of area of memory to store
    * results of integration
	* @return a value of buffer is a pointer
	*/
  long* GetBuffer ( ) { return buffer; }
 
  /**
    * This method set the value of sizeBuff is the size of area of memory.
    * @param var is the value to set.
    */
  void SetSizeBuffer ( int var )  { sizeBuff=var; }
  
  /**
	* This method get the value of sizeBuff is the size of area of memory.
	* @return a value of sizeBuff is the size of area of memory.
	*/
  int GetSizeBuffer ( ) { return sizeBuff; }
  
  /**
    * This method set the value of flag Overflow.
    * @param var is the value to set.
    */
  void SetOverflow ( bool var )  { Overflow=var; }
  
  /**
	* This method get the value of flag Overflow
	* @return a value of flag Overflow
	*/
  bool GetOverflow ( ) { return Overflow; }

  /**
   * This method setting each elemento of class at zero
   */
  void SetZeroData ();

protected:
  long* buffer;/*!<pointer of results of integration  */
  int sizeBuff;/*!< size of area of memory pointint by buffer */
  bool Overflow;/*!< overflow flag of results of integrazion */

private:
  timeb utInizio;/*!< time beginning of integration */
  timeb utFine;/*!< time ending of integration */
  HeadResult intestazioneCh1;/*!< primary header of object */
  HeadResult intestazioneCh2;/*!< secondary header of object */
  
  /**
   * The initAttributes method attempts to initialize the object on create
   */
  void initAttributes ( ) ;

};

#endif // DATA_H
