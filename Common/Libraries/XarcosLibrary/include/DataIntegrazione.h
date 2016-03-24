#ifndef DATAINTEGRAZIONE_H
#define DATAINTEGRAZIONE_H

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Liliana Branciforti(bliliana@arcetri.astro.it)  10/03/2009      Creation                                                  
 */

#include <vector>
#include "headResult.h"
#include "Specifiche.h"
#include "Data.h"
#include <time.h>
#include <sys/timeb.h>
#include <complex>

using namespace std;

/**
  * This class includes all information of each single integration 
  * and the results store in the vectore struct.
  */

class DataIntegrazione
{
public:
  /**
   * Constructor
   */
  DataIntegrazione ( );
  
  /**
   * Destructor.
   */
  virtual ~DataIntegrazione ( );

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
    * This method set header.
    * @param var is the value to set.
    */
  void SetHead ( HeadResult var ) { head = var; }
  
  /**
	* This method get the value of header
	* @return a value of header
	*/
  HeadResult GetHead ( ) {  return head; }
  
  /**
   * This method set the value of integration time.
   * @param var is the value to set.
   */
  void SetTempoIntegrazione ( int var ) { tempoIntegrazione = var; }
  
  /**
   * This method get the value of integration time.
   * @return a value of integration time.
   */
  int GetTempoIntegrazione ( ) { return tempoIntegrazione; }
  
  /**
   * This method set the value of flag Modo8bit.
   * @param var is the value to set.
   */
  void SetModo8Bit ( bool var ) { modo8Bit = var; }
  
  /**
   * This method get the value of flag Modo8bit
   * @return a value of flag Modo8bit
   */
  bool GetModo8Bit ( ) { return modo8Bit; }

  /**
    * This method set the value of UT time beginning of integration
    * it take by system clock 
    */
  void SetUtInizio ( ) {
	  timeb timebuffer;
	  ftime( &timebuffer );
	  utInizio = timebuffer; }

  /**
	* This method get the value of UT time beginning of integration
	* @return a value of UT time beginning of integration
	*/
  timeb GetUtInizio ( ) { return utInizio; }
  
  /**
    * This method set the value of UT time ending of integration.
    * it take by system clock
    */
  void SetUtFine ( ) {
	  timeb timebuffer;
	  ftime( &timebuffer );
	  utFine = timebuffer;
	  }
 
  /**
	* This method get the value of UT time ending of integration 
	* @return a value of UT time ending of integration
	*/
  timeb GetUtFine ( ) { return utFine; }
  
  /**
   * This method set the value of flag ValoriValidi
   * @return a value of flag ValoriValidi
   */
  void SetValoriValidi ( bool new_var ) { ValoriValidi = new_var; }
 
  /**
   * This method get the value of flag ValoriValidi
   * @return a value of flag ValoriValidi
   */
  bool GetValoriValidi ( ) { return ValoriValidi; }
  
  /**
   * This method set the value of NArrayDati
   * @param var is the value to set.
   */
  void SetNArrayDati ( int var ) { nArrayDati = var; }
  
  /**
   * This method get the value of NArrayDati 
   * @return a value of NArrayDati
   */
  int GetNArrayDati ( ) { return nArrayDati; }
  
  /**
    * This method set the value of lungArrayDati.
    * @param var is the value to set.
    */
  void SetLungArrayDati ( int var ) { lungArrayDati = var; }
  
  /**
	* This method get the value of lungArrayDati
	* @return a value of lungArrayDati
	*/
  int GetLungArrayDati ( ) { return lungArrayDati; }
  
  /**
   * This method get the value of vector xx 
   * @return a value of vector xx
   */
  vector<double> GetXx ( ) { return xx; }
  /**
   * This method get the value of vector yy
   * @return a value of vector yy
   */
  vector<double> GetYy ( ) { return yy; }
  /**
   * This method get the value of vector xy 
   * @return a value of vector xy
   */
  vector<complex< double > > GetXy ( ) { return xy; }
  /**
   * This method get the value of vector tp 
   * @return a value of vector tp
   */
  vector<double> GetTp( ) { return tp; }
  
  /**
   * This method setting each elemento of class at zero
   */
  void SetZeroDataIntegratione ( );
  
  /**
    * This method set the value of each elemnto of vector xx a zero
    */
  void SetXxZero ( );
 
  /**
    * This method set the value of each elemnto of vector yy a zero
    */
  void SetYyZero ( );
 
  /**
    * This method set the value of each element of vector xy a zero
    */
  void SetXyZero ( );
 
  /**
    * This method set the value of each element of vector tp a zero
    */
  void SetTpZero ( );
  
  /**
    * This method set the value of each element of vector xx
    */
  void SetXx ( long *buff);
 
  /**
    * This method set the value of each element of vector xx
    * This method is used when two section single polarimetric mode are unifid
    */
  void SetXx2( long *buff);
  
  /**
    * This method set the value of each element of vector yy
    */
  void SetYy ( long *buff);
  
  /**
    * This method set the value of each element of vector xy
    */
  void SetXy ( long *buff);
  
  /**
    * This method set the value of each element of vector tp a zero
    * @param buff is a pointer of area of memory that contain data
    * @param size is the dimention of buffer
    * @param modo specify the type of data
    */
  void SetTp ( long *buff, int size, bool modo);
  
  vector<double> xx;/*!< data of integration single and full mode*/
  vector<double> yy;/*!< data of integration single mode*/
  vector<complex< double > > xy;/*!< data of integration full mode*/
  vector<double> tp;/*!< data of total power*/
  HeadResult head;/*!< header of object */
  
private:
  int tempoIntegrazione;/*!< time of integration*/
  bool modo8Bit;/*!< type of integration*/
  timeb utInizio;/*!< UT time of beginning integration*/
  timeb utFine;/*!< UT time of ending integration*/
  bool ValoriValidi;/*!< flag that confirm the validity of data */
  int nArrayDati;/*!< number of type of data */
  int lungArrayDati;/*!< size of data store*/
  bool Overflow;/*!< overflow flag of results of integrazion*/
	  
  /**
   * The initAttributes method attempts to initialize the object on create
   */
  void initAttributes ( ) ;

};

#endif // DATAINTEGRAZIONE_H


