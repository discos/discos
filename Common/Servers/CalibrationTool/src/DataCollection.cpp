// $Id: DataCollection.cpp,v 1.2 2011-01-18 10:50:21 c.migoni Exp $

#include "DataCollection.h"
#include <SecureArea.h>

void CalibrationTool_private::getTsysFromBuffer(char *& buffer,const DWORD& channels ,double *tsys) {
	for (DWORD i=0;i<channels;i++) {
		tsys[i]=*((double *)buffer);
		buffer+=sizeof(double);
	}
}

using namespace CalibrationTool_private;

CDataCollection::CDataCollection()
{
	m_running=m_ready=m_start=m_stop=false;
    m_scanHeader=m_subScanHeader=false;
	m_sectionH=NULL;
	//m_fileName="";
	m_fileName=m_basePath=m_fullPath="";
	m_projectName="";
	m_observerName="";
	m_status=Management::MNG_OK;
	m_sourceName="";
	m_lonDone=m_latDone=false;
	m_focusDone=false;
	m_coordIndex=-1;
	m_lastScanPointing=false;
	m_lastScanFocus=false;
	m_errorDetected=false;
	m_lastTarget="";
	m_HPBW= m_amplitude=m_peakOffset=m_offSet=m_slope= m_sourceFlux=0.0;
	/*m_telecopeTacking=m_prevTelescopeTracking=false;
	m_telescopeTrackingTime=0;*/
}
	
CDataCollection::~CDataCollection()
{
	if (m_sectionH!=NULL)
        delete[] m_sectionH;
}

void CDataCollection::saveMainHeaders(Backends::TMainHeader const * h,
		Backends::TSectionHeader const * ch)
{
	memcpy(&m_mainH,h,sizeof(Backends::TMainHeader));
	if (m_sectionH!=NULL) delete[] m_sectionH;
	m_sectionH=new Backends::TSectionHeader[m_mainH.sections];
	memcpy(m_sectionH,ch,sizeof(Backends::TSectionHeader)*m_mainH.sections);
	m_ready=true; // main headers are saved....before that no activity can take place
}

bool CDataCollection::saveDump(char * memory)
{
	bool track;
	char *buffer;
	Backends::TDumpHeader *dh=(Backends::TDumpHeader *) memory;
	buffer=memory+sizeof(Backends::TDumpHeader); 
	/*if (dh->time>=m_telescopeTrackingTime) track=m_telecopeTacking;
	else track=m_prevTelescopeTracking;*/
	track=true;
	if ((!m_running) && (!m_start) && (!m_stop)) {
		m_start=true;
	}
	return m_dumpCollection.pushDump(dh->time,dh->calOn,memory,buffer,track,dh->dumpSize);
}

ACS::Time CDataCollection::getFirstDumpTime()
{
	return m_dumpCollection.getFirstTime();
}

long CDataCollection::getInputsNumber() const
{
	long sum=0;
	for (long i=0;i<m_mainH.sections;i++) {
		sum+=m_sectionH[i].inputs;
	}
	return sum;
}

bool CDataCollection::getDump(ACS::Time& time,bool& calOn,char *& memory,char *& buffer,bool& tracking,long& buffSize)
{
	return m_dumpCollection.popDump(time,calOn,memory,buffer,tracking,buffSize);
}

void CDataCollection::startStopStage()
{
	if (m_running) m_stop=true;
}

void CDataCollection::startRunnigStage()
{
    m_subScanHeader=false;
	m_running=true;
	m_start=false;
}

void CDataCollection::haltStopStage()
{
	m_running=false;
	m_stop=false;
}

void CDataCollection::haltResetStage()
{
	m_reset=false;
	m_errorDetected=false;
	m_lastScanPointing=m_lastScanFocus=false;
	m_lonDone=m_latDone=false;
	m_focusDone=false;
	m_status=Management::MNG_OK;
}

void CDataCollection::getFileName(IRA::CString& fileName,IRA::CString& fullPath) const
{
	fileName=m_fileName;
	fullPath=m_fullPath;
}

IRA::CString CDataCollection::getFileName() const
{
	return m_fullPath+m_fileName;
}

bool  CDataCollection::getMinorServoAxisPosition(const ACS::stringSeq& seq,unsigned& pos)
{
	pos=0;
	for (unsigned i=0;i<seq.length();i++) {
		IRA::CString name(seq[i]);
		if (m_minorServoNameForAxis==name) {
			pos=i;
			return true;
		}
	}
	return false;
}

bool CDataCollection::setScanSetup(const Management::TScanSetup& setup,bool& recording,bool& inconsistent)
{
	if (m_start && m_running) {
		recording=true;
		inconsistent =false;
		return false;
	}
	else {
        if (!m_scanHeader) {
            m_fileName=setup.baseName;
            m_projectName=setup.projectName;
            m_observerName=setup.observerName;
            m_deviceID=setup.device;
            m_basePath=setup.path;
        	IRA::CString temp;
        	temp=setup.baseName;
        	m_fileName=temp+".dat";
        	temp=setup.extraPath;
        	if (temp!="") {
        		m_fullPath=m_basePath+temp;
        	}
        	else {
        		m_fullPath=m_basePath;
        	}
            m_scanHeader=true;
            closePointingScan();
            closeFocusScan();
            return true;
        }
		else {
			recording=false;
			inconsistent =true;
			return false;
		}
	}
}

bool CDataCollection::setSubScanSetup(const Management::TSubScanSetup& setup,bool& recording,bool& inconsistent,bool& noScanAxis,bool& geometryWarning)
{
	geometryWarning=false;
	if (m_start && m_running) {
		recording=true;
		inconsistent =false;
		noScanAxis=false;
		return false;
	}
	else {
        if (m_scanHeader && !m_subScanHeader) {
        	IRA::CString currentTarget;
        	currentTarget=setup.targetID;
            m_scanAxis=setup.axis;
            m_minorServoNameForAxis=setup.minorServoNameForAxis;
            setCoordIndex();
            if (m_coordIndex<0) { // the subscan is not set to fit properly with the calibration tool.....
        		recording=false;
        		inconsistent =false;
        		noScanAxis=true;
            	return false;
            }
            if (m_lastTarget!=currentTarget) { // if a new target has been set, reset anything in any case
            	m_lastTarget=currentTarget;
            	closePointingScan();
            	closeFocusScan();
            }
            if (isPointingScan()) { // case of pointing scans....LON & LAT
            	if (m_lastScanFocus) { //avoid improper focus/pointing scan mixing
                	if (!isFocusScanClosed()) {
                		geometryWarning=true;
                	}
                	closePointingScan();
                	closeFocusScan();
            	}
            	m_lastScanPointing=true;
            	m_lastScanFocus=false;
            	if (isPointingScanDone()) { // if a full sequence LAT-LON has been completed, reset anything
            		closePointingScan();
            	}
            	else if (m_lonDone && !m_latDone) {    // avoid LON LAT sequence
            		geometryWarning=true;
            		closePointingScan();
            	}
            	else if (m_latDone && m_coordIndex==1) { // avoid LAT LAT sequence
            		geometryWarning=true;
            		closePointingScan();
            	}
            }
            else  if (isFocusScan()) { // case of focusing scans......
            	if (m_lastScanPointing) {
                	if (!isPointingScanClosed()) {
                		geometryWarning=true;
                	}
                	closePointingScan();
                	closeFocusScan();
            	}
            	m_lastScanPointing=false;
            	m_lastScanFocus=true;
            	if (isFocusScanDone()) {
            		closeFocusScan();
            	}
            }
            m_subScanHeader=true;
            m_errorDetected=false;
            return true;
        }
        else {
			recording=false;
			inconsistent =true;
			noScanAxis=false;
			return false;
		}
	}
}

bool CDataCollection::stopScan()
{
	m_ready=false;
    m_scanHeader=false;
    m_reset=true; // allow to close the file
	return true;
}

void CDataCollection::forceReset()
{
	m_running=m_ready=m_start=m_stop=false;
    m_scanHeader=m_subScanHeader=false;
	m_reset=true;
}

/*************** Private ***********************************************************/

void CDataCollection::setCoordIndex()
{
	switch (m_scanAxis) {
    	case Management::MNG_NO_AXIS: {
    		m_coordIndex = -1;
        	break;
    	}
        case Management::MNG_HOR_LON: {
	        m_coordIndex = 0;	// LON
	        break;
        }
        case Management::MNG_HOR_LAT: {
	        m_coordIndex = 1;	// LAT
	        break;
        }
        case Management::MNG_EQ_LON: {
	        m_coordIndex = 0;	// LON
            break;
        }
        case Management::MNG_EQ_LAT: {
	        m_coordIndex = 1;	// LAT
	        break;
        }
        case Management::MNG_GAL_LON: {
	        m_coordIndex = 0;	// LON
	        break;
        }
        case Management::MNG_GAL_LAT: {
	        m_coordIndex = 1;	// LAT
	        break;
        }
        case Management::MNG_SUBR_Z: {
        	m_coordIndex = 3; // ZS
        	break;
        }
        case Management::MNG_SUBR_X: {
        	m_coordIndex = -1;
        	break;
        }
        case Management::MNG_SUBR_Y: {
        	m_coordIndex = -1;
        	break;
        }
        case Management::MNG_PFP_Z: {
        	m_coordIndex = 2; // ZP
        	break;
        }
        case Management::MNG_PFP_Y: {
        	m_coordIndex = -1;
        	break;
        }
        case Management::MNG_BEAMPARK: {
        	m_coordIndex = -1;
        	break;
        }
        case Management::MNG_TRACK: {
        	m_coordIndex = -1;
        	break;
        }
        case Management::MNG_GCIRCLE: {
        	m_coordIndex = -1;
        	break;
        }
    }
}


