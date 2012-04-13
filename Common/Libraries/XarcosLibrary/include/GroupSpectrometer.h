#ifndef GROUPSPECTROMETER_H_
#define GROUPSPECTROMETER_H_

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Liliana Branciforti(bliliana@arcetri.astro.it)  10/03/2009      Creation                                                  
 */

#include "Cpld2IntfClient.h"
#include "Specifiche.h"
#include "DataIntegrazione.h"
#include "SingleSpectrometer.h"
#include <vector>

using namespace std;

/**
 * 
 * This object has the task of representing the HW real.
 * This class regroups array of object SingleSpectrometer and AdcConditioner
 *  to form a single entity.
 * Also it contains an object Specifiche and array of object Dato to manage 
 * the specific of integration, and array of object DataIntegrazione to store 
 * the results.
 *
 */

class GroupSpectrometer
{
public:
	Specifiche Xspec;/*!< Specifiche object */
	vector<DataIntegrazione> RisultatiIntegrazione;/*!< vector of DataIntegrazione */
	int tempoEle;/*!< integration basic time */
	vector<Data> dato;/*!< vector of dato */
	vector<SingleSpectrometer*> Spettrometro;/*!< vector of SingleSpectrometer */
	vector<AdcConditioner*> Adc;/*!< vector of AdcConditioner*/
	Cpld2IntfClient *intf;/*!<is pointer to object that manages the TCP Protocol with the HW */
	int countTp;/*!< counter of measures of TP */
	int countInt;/*!< counter of integration */
	bool hw,setting,active,abort,dataReady,overflowData;/*!< flag of state */
	char status;/*!< status of HW */
	int section;
	int input;
	vector<bool> enabled;
  /**
	* This method get the value of status of Backends
	* @return a value of status
	*/
	char getBackendStatus();/*!< */
	
	/**
	 * Constructor
	 */
	GroupSpectrometer(const char * a_server, int a_port, bool a_doppio);
	/**
	 * Destructor.
	 */
	virtual ~GroupSpectrometer();

    /**
     * This method get the value of spectrometer number of Backends
	 * @return a value of spectrometer number
	 */
	int GetNumSpec(){ return numSpec ; }
	
	/**
	 * This method get the value of spectral number points 
	 * @return a value of pectral number points
	 */
	int GetNmPuntiSpettro(){ return  numPuntiSpettro; }
	
	/**
	 * This method set the value of spectral number points 
	 * @param var is the value to set
	 */
	void SetNmPuntiSpettro(int var){ numPuntiSpettro=var; }
	
	/**
	 * This method prepares the specifications for the HW.
	 * This method work:
	 * 		make a copy of the carrier headers, 
	 * 		the carrier copy shall be ordered according to the chIn,
	 * 		also carrier copy shall be ordered according to the Polarimetric Mode
	 * 		generating the structure type:POL 0-3 NoPOL 0-7 POL4-7
	 * 		this structure is pass to method <i>Imposta()</i>
	 * @return a bolean for controll of error
	 */ 
    bool Setting();
    
    /**
	 * This method retrieve the partial results of an integration elementary
	 * @param nCicli is counter decrease of flus of data.
	 */ 
    bool GetData(int nCicli);
   
    /**
	 * This method retrieve the partial results of an elementary measure (200 millisecond)
     * @param modo=true wait 200 millisecond before take the measure tp
     * 	    modo=false instantly retrieve the results of the measure tp
     */
    bool GetDataTp(bool modo);
    bool GetDataTp(bool modo, vector<double> &tp);
    bool GetDataTpZero(bool modo, vector<double> &tp);
    
    /**
	 * This method terminating the flow of data and shall carry out the 
	 * standardisation compared to the real measures tp drawn 
	 */
    bool AbortDataTp();
    
    /**
	 * This method set the type of operating the ADC, specifically configuring 
	 * the distribution of bit on backplane 
	 * @param modo=true set ADC to the operation to 8 bit
     * 	    modo=false set ADC to the operation to 6 bit
	 */ 
    void AdcSetMode(bool modo);
    
    /**
	 * This method set the functionality of testing ADC 
	 * This method is only in available server version xarcos 
	 * @param func is a number that specifies the type of test function 
	 */
    void AdcSetFunction(int func);
   
    /**
	 * This method starts integration with the specific previously 
	 * initialize with the method <i>Init()</i>
	 */ 
	void StartInt();
	   
    /**
	 * This method abortice integration in progress 
	 */
	void AbortInt();
	   
    /**
	 * This method initialize the specifications in HW previously set with 
	 * the method <i>setting()</i>
	 */
	void Init();	
    /**
	 * This method check if the connection is active 
	 * This method is only in available server ACS version XBackends
	 */ 
	bool isConnected();
	bool reConnect();
	
private:
	int numPuntiSpettro;/*!< spectral number points */
	int numSpec;/*!< spectrometer number of Backends */ 
	bool doppio;
	/**
	 * The initAttributes method attempts to initialize the object on create
	 */
	bool initAttributes(const char * a_server, int a_port);
	
	/**
	 * This method and called up from <i>setting()</i> to set HW.
	 * This method modifier the vector dato.
	 * @param s is index of first spectrometer free 
	 * @param j is index of first header to consider 
	 * @param M is the first ADC of group
	 * @param spec is copy of list specific order 
	 * @return a bolean for controll of error
	 */ 
	bool Imposta(int &s, int &j, int M,vector<HeadResult> spec);
	
    void AdcReset(bool reset);
    void AdcIntTime(int ms);
    bool AdcGetData();

};

#endif /*GROUPSPECTROMETER_H_*/
