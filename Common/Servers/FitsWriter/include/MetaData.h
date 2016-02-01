#ifndef CMETADATA_H_
#define CMETADATA_H_

/* ************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                                         */
/*                                                                                                                                                */
/* This code is under GNU General Public Licence (GPL).                                                    */
/*                                                                                                                                                */
/* Who                                                    	 When                      What                                      */
/* Andrea Orlati(aorlati@ira.inaf.it)          26/10/2013         Creation                                */

#include <IRA>
#include "FitsWriter.h"

namespace FitsWriter_private {

/**
 * This class stores all the meta data that are needed by the fits.This class is used internally by
 * the engine thread so does not need to be synchronized
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
  */
class CMetaData {

public:
	/**
	 * Constructor
	 */
	CMetaData();
	/**
	 * Destructor
	 */
	~CMetaData();

	/*
	 * It allows to set the current receiver code
	 */
	void setReceiverCode(const IRA::CString& recvCode) { m_receiverCode=recvCode; }
	
	/**
	 * @return the receiver code
	 */
	const IRA::CString& getReceiverCode() const { return m_receiverCode; }

	/**
	 * Saves the feed header
	*/
	void saveFeedHeader(CFitsWriter::TFeedHeader *fH,const WORD& number);

	/**
	 * @return the pointer to the array of the feed header
	 */
	CFitsWriter::TFeedHeader *getFeedHeader() const { return m_feeds; }

	/**
	 * @return the number of feeds, it is the dimension of the array returned with the call to
	 * <i> getfeedHeader() </i>.
	 */
	const WORD& getFeedNumber() const { return m_feedNumber; }

	/*
	 * It allows to read the sequence of identifiers of the backend sections
	 * @param sec used to return back the the array
	 */
	void getSectionsID(ACS::longSeq& sec) const { sec=m_sectionsID; }

	/*
	 * It allows to read the sequence of attenuations for each backend input
	 * @param att used to return back the the array
	*/
	void getBackendAttenuations(ACS::doubleSeq& att) const { att=m_backendAttenuations; }

	/*
	 * It allows to read the sequence of feeds identifiers for each backend input
	 * @param feed used to return back the the array
	*/
	void getFeedsID(ACS::longSeq& feed) const { feed=m_receiverFeedID; }

	/*
	 * It allows to read the sequence of ifs  identifiers for each backend input
	 * @param ifid used to return back the the array
	*/
	void getIFsID(ACS::longSeq& ifid) const { ifid=m_receiversIFID; }

	/*
	 * It allows to read the sequence of local oscillators
	 * @param lo used to return back the the array
	 */
	void getLocalOscillator(ACS::doubleSeq& lo) const { lo=m_localOscillator; }

	/*
	 * @return the arrays that contains the local oscillator values.
	 * @param len used to return back the size of the local oscillator array
	 */
	void getCalibrationMarks(ACS::doubleSeq& mark) const { mark=m_calibrationMarks; }

	/*
	 * allows to read the sequence of IFs polarization
	 * @param pol used to return back the the array
	 */
	void getReceiverPolarization(ACS::longSeq& pol) const { pol=m_receiverPolarization; }

	/**
	 * @param freq used to return the number of frequencies, one value for each section
	 */
	void getSkyFrequency(ACS::doubleSeq& freq) const { freq=m_skyFrequency; }

	/**
	 * @param freq used to return the number of bandwidths, one value for each section
	 */
	void getSkyBandwidth(ACS::doubleSeq& freq) const { freq=m_skyBandwidth; }

	/**
	 * allows to store the information about each of the backend inputs. The dimension of the sequences should be exactly the number returned by the method <i>getInputsNumber()</i>-
	 */
	void setInputsTable(const ACS::longSeq& sectionID,const ACS::longSeq& feed,const ACS::longSeq& ifs,const ACS::longSeq& pols,const ACS::doubleSeq& freq,const ACS::doubleSeq& bw,
			const ACS::doubleSeq& los,const ACS::doubleSeq& atts,const ACS::doubleSeq& mark)  {
		m_receiverPolarization=pols;
		m_receiversIFID=ifs;
		m_receiverFeedID=feed;
		m_calibrationMarks=mark;
		m_backendAttenuations=atts;
		m_skyFrequency=freq;
		m_skyBandwidth=bw;
		m_sectionsID=sectionID;
		m_localOscillator=los;
	}

	void setInputsTable()
	{
		m_receiverPolarization.length(0);
		m_receiversIFID.length(0);
		m_backendAttenuations.length(0);
		m_receiverFeedID.length(0);
		m_calibrationMarks.length(0);
		m_skyFrequency.length(0);
		m_skyBandwidth.length(0);
		m_sectionsID.length(0);
		m_localOscillator.length(0);
	}

	/**
	 * It allows to set the current source name and its salient parameters
	 */
	void setSource(const IRA::CString& source,const double& ra,const double& dec,const double& vlsr) {
		m_sourceName=source; m_sourceRa=ra; m_sourceDec=dec; m_sourceVlsr=vlsr;
	}

	/**
	 * @return the name of the source and its parameters
	 */
	void getSource(IRA::CString& source,double& ra,double& dec,double& vlsr) const {
		source=m_sourceName; ra=m_sourceRa; dec=m_sourceDec; vlsr=m_sourceVlsr;
	}

	/**
	 * It allows to set the antenna position offsets as directly returned by the Antenna subsystem
	*/
	void setAntennaOffsets(const double& azOff,const double& elOff,const double& raOff,const double& decOff,const double& lonOff,const double& latOff) {
		m_azOff=azOff; m_elOff=elOff; m_raOff=raOff; m_decOff=decOff; m_lonOff=lonOff; m_latOff=latOff;
	}

	/**
	 * @return the antenna position offsets
	 */
	void getAntennaOffsets(double& azOff,double& elOff,double& raOff,double& decOff,double& lonOff,double& latOff) const {
		azOff=m_azOff; elOff=m_elOff; raOff=m_raOff; decOff=m_decOff; lonOff=m_lonOff; latOff=m_latOff;
	}

	/**
	 * Stores the list of values corresponding to the estimated source flux of the source,
	 * @param fl list of fluxes, one value is  given corresponding to each input, but just one for each section is taken
	 */
	void setSourceFlux(const ACS::doubleSeq& fl) { m_sourceFlux=fl; }
	void setSourceFlux() { m_sourceFlux.length(0); }

	/**
	 * It returns the estimated source flux, one for each section.
	 */
	void getSourceFlux(ACS::doubleSeq& fl) const { fl=m_sourceFlux; }

	/**
	 * allows to store the information about each of the servo system axis
	 */
	void setServoAxis(const ACS::stringSeq& name, const ACS::stringSeq& unit) {
		m_servoAxisNames=name;
		m_servoAxisUnits=unit;
	}

	void setServoAxis() {
		m_servoAxisNames.length(0);
		m_servoAxisUnits.length(0);
	}

	void getServoAxisNames(ACS::stringSeq& names) const { names=m_servoAxisNames; }
	void getServoAxisUnits(ACS::stringSeq& units) const { units=m_servoAxisUnits; }

	void setDewarConfiguration(const Receivers::TDerotatorConfigurations& mod,const double& pos) {
		m_dewarMode=mod;
		m_dewarPos=pos;
	}
	void setDewarConfiguration() {
		m_dewarMode=Receivers::RCV_UNDEF_DEROTCONF;
		m_dewarPos=0.0;
	}

	void getDewarConfiguration(Receivers::TDerotatorConfigurations& mod,double& pos) const {
		mod=m_dewarMode;
		pos=m_dewarPos;
	}

	void setSubScanConf(const Management::TSubScanConfiguration& conf) {
		m_subScanConf=conf;
	}

	void setSubScanConf() {
		m_subScanConf.signal=Management::MNG_SIGNAL_NONE;
	}

	const Management::TSubScanConfiguration& getSubScanConf() {
		return m_subScanConf;
	}
private:

	/**
	 * Indicates the short name (code) of the receiver currently used
	 */
	IRA::CString m_receiverCode;

	/**
	 * array of structures that stores the data needed to fille the feed table
	 */
	CFitsWriter::TFeedHeader *m_feeds;
	/**
	 * Number of feed inside the table
	 */
	WORD m_feedNumber;

	/**
	 * stores the identifiers of the receiver IFs (one for each input defined by the backend)
	 */
	ACS::longSeq m_receiversIFID;
	/**
	 * array that contains the values of the local oscillator (one for each input defined by the backend)
	 */
	ACS::doubleSeq m_localOscillator;
	/**
	 * Stores the identifier od the baclend section (one for each input)
	 */
	ACS::longSeq m_sectionsID;
	/**
	 * stores the band width of the bands observed (projected to sky) (one for each input defined by the backend)
	 */
	ACS::doubleSeq m_skyBandwidth;
	/**
	 * stores the initial frequencies of the bands observed (projected to sky) (one for each input defined by the backend)
	 */
	ACS::doubleSeq m_skyFrequency;
	/**
	 * stores the value of the calibration marks, for each inputs from the backend
	 */
	ACS::doubleSeq m_calibrationMarks;
	/**
	 * stores the identifier of the receiver feeds ((one for each input defined by the backend)
	 */
	ACS::longSeq m_receiverFeedID;
	/**
	 * Stores the attenuations of each backend input
	 */
	ACS::doubleSeq m_backendAttenuations;
	/**
	 * array that contains the values of receiver polarization (one for each input defined by the backend)
	 */
	ACS::longSeq m_receiverPolarization;
	/**
	 * Name of the currently observed source
	 */
	IRA::CString m_sourceName;
	double m_sourceRa,m_sourceDec,m_sourceVlsr;
	/**
	 * antenna position offsets
	 */
	double m_azOff,m_elOff,m_raOff,m_decOff,m_lonOff,m_latOff;
	/**
	 * stores the  estimated source fluxes, one for each section.
	 */
	ACS::doubleSeq m_sourceFlux;
	/**
	 * Stores the description or name of each involved axis of the servo system
	 */
	ACS::stringSeq m_servoAxisNames;
	/**
	 * Stores the measurement unit of each involved axis of the servo system
	 */
	ACS::stringSeq m_servoAxisUnits;
	/**
	 * DewarPositioner configured mode
	 */
	Receivers::TDerotatorConfigurations m_dewarMode;
	/**
	 * Initial position of the dewar, it depends on the mode.
	 */
	double m_dewarPos;
	/**
	 * Sub scan configuration
	 */
	Management::TSubScanConfiguration m_subScanConf;
};

};

#endif /*CDATACOLLECTION_H_*/
