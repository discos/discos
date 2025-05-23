#include "Schedule.h"
#include <slamac.h>
#include <ManagementModule.h>
#include <AntennaModule.h>
#include <iostream>

#define OFFFRAMEEQ "-EQOFFS"
#define OFFFRAMEHOR "-HOROFFS"
#define OFFFRAMEGAL "-GALOFFS"
#define RVEL "-RVEL"

#define SIDEREAL "SIDEREAL"
#define SUN "SUN"
#define MOON "MOON"
#define SATELLITE "SATELLITE"
#define PLANET "PLANET"
#define SOLARSYTEMBODY "SOLARSYTEMBODY"
#define OTF "OTF"
#define OTFC "OTFC"
#define	SKYDIP	"SKYDIP"
#define FOCUS "FOCUS"


using namespace std;
using namespace Schedule;



CScanList::CScanList(const IRA::CString& path,const IRA::CString& fileName): CBaseSchedule(path,fileName)
{
	m_schedule.clear();
}

CScanList::~CScanList()
{
	TIterator p;
	for(p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)!=NULL) {
			Antenna::TTrackingParameters *antenna=static_cast<Antenna::TTrackingParameters *>((*p)->primaryParameters);
			if (antenna) delete antenna;
			antenna=static_cast<Antenna::TTrackingParameters *>((*p)->secondaryParameters);
			if (antenna) delete antenna;
			MinorServo::MinorServoScan *servo=static_cast<MinorServo::MinorServoScan *>((*p)->servoParameters);
			if (servo) delete servo;
			Receivers::TReceiversParameters *receivers=static_cast<Receivers::TReceiversParameters *>((*p)->receieversParsmeters);
			if (receivers) delete receivers;
			Management::TSubScanConfiguration *subScanConf=(*p)->subScanConfiguration;
			if (subScanConf) delete subScanConf;
			delete (*p);
		}
	}
	m_schedule.clear();
}

bool CScanList::checkScan(const DWORD& id)
{
	TIterator i;
	for (i=m_schedule.begin();i<m_schedule.end();i++) {
		if ((*i)->id==id) {
			return true;
		}
	}
	return false;
}

bool CScanList::getScan(const DWORD&id,Management::TScanTypes& type,void *&prim,void *& sec,void *& servo,void *& recv,
		Management::TSubScanConfiguration *&subScanConf)
{
	TIterator i;
	for (i=m_schedule.begin();i<m_schedule.end();i++) {
		if ((*i)->id==id) {
			//target=(*i)->target;
			type=(*i)->type;
			prim=(*i)->primaryParameters;
			sec=(*i)->secondaryParameters;
			servo=(*i)->servoParameters;
			recv=(*i)->receieversParsmeters;
			subScanConf=(*i)->subScanConfiguration;
			return true;
		}
	}
	m_lastError.Format("Scan number %u is not known",id);
	return false;
}

bool CScanList::getScan(const DWORD& id,TRecord& rec)
{
	rec.id=id;	
	return getScan(rec.id,rec.type,rec.primaryParameters,rec.secondaryParameters,rec.servoParameters,rec.receieversParsmeters,
			rec.subScanConfiguration);
}

bool CScanList::printScan(const DWORD& id)
{
	TRecord rec;
	Antenna::TTrackingParameters *antenna;
	IRA::CString str;
	if (getScan(id,rec)) {
		antenna=static_cast<Antenna::TTrackingParameters *>(rec.primaryParameters);
		
		cout << "\t" << rec.id;	
		cout << " " << Antenna::Definitions::map(antenna->type);	
		cout << " " << antenna->targetName;		
		if ((antenna->paramNumber>0)) {
			for (long i=0;i<antenna->paramNumber;i++) {
				cout << "," << antenna->parameters[i];
			}
			CIRATools::equinoxToStr(antenna->equinox,str);		 
			cout << "," << Antenna::Definitions::map(antenna->frame) << "," << (const char *)str;
		}
		else cout << " --";
		cout << endl;
		cout << "\tOffsets: (" << Antenna::Definitions::map(antenna->offsetFrame) << ",";
		cout << antenna->latitudeOffset << "," << antenna->longitudeOffset << ")";
		cout << " RadialVel: (" << Antenna::Definitions::map(antenna->VradFrame) << ",";
		cout << Antenna::Definitions::map(antenna->VradDefinition) << ",";
		cout << antenna->RadialVelocity << ")";
		cout << endl;
		return true;
	}
	else return false;		
}

bool CScanList::checkConsistency(DWORD& line,IRA::CString& errMsg)
{
	 TIterator i,p;
	 for (i=m_schedule.begin();i<m_schedule.end();i++) {
		 for (p=i+1;p<m_schedule.end();p++) {
			 if ((*i)->id==(*p)->id) {
				 line=(*p)->line;
				 errMsg.Format("scan %d is duplicated",(*i)->id);
				 return false;
			 }
		 }
	 }
	 return true;
}

 /////////////////////////////////// PRIVATE ////////////////////////////////////////////////////////////

bool CScanList::parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errMsg)
{
	int start=0;
	IRA::CString ret;
	Management::TScanTypes type;
	// get the second item.....
	if (!IRA::CIRATools::getNextToken(line,start,SEPARATORS,ret,false)) {
		errMsg="format error";
		return false;
	}
	//...the scan type
	if (!IRA::CIRATools::getNextToken(line,start,SEPARATORS,ret,false)) {
		errMsg="could not read scan type";
		return false;
	}
	ret.MakeUpper();
	if(!Management::Definitions::map(ret,type)) {
	//if (!string2ScanType(ret,type)) {
		errMsg="unknown scan type";
		return false;
	}
	switch (type) {
		case Management::MNG_PEAKER: {
			CSubScanBinder binder(getConfiguration(),false);
			DWORD identifier;
			if (!parsePeaker(line,identifier,errMsg,binder)) {
				binder.dispose();
				return false;
			}
			TRecord *rec=new TRecord;
			rec->id=identifier;
			rec->type=type;
			rec->primaryParameters=(void *)binder.getPrimary();
			rec->secondaryParameters=(void *)binder.getSecondary();
			rec->receieversParsmeters=(void *)binder.getReceivers();
			rec->servoParameters=(void *)binder.getServo();
			rec->subScanConfiguration=binder.getSubScanConfiguration();
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Management::MNG_SIDEREAL: {
			CSubScanBinder binder(getConfiguration(),false);
			DWORD identifier;
			if (!parseSidereal2(line,identifier,errMsg,binder)) {
				binder.dispose();
				return false;
			}
			TRecord *rec=new TRecord;
			rec->id=identifier;
			rec->type=type;
			rec->primaryParameters=(void *)binder.getPrimary();
			rec->secondaryParameters=(void *)binder.getSecondary();
			rec->receieversParsmeters=(void *)binder.getReceivers();
			rec->servoParameters=(void *)binder.getServo();
			rec->subScanConfiguration=binder.getSubScanConfiguration();
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Management::MNG_SUN: {
	      DWORD id;
			Antenna::TTrackingParameters *prim=new Antenna::TTrackingParameters;
			if (!parseSun(line,prim,id,errMsg)) {
				if (prim) delete prim;
				return false; // errMsg already set by previous call
			}
			Antenna::TTrackingParameters *sec=new Antenna::TTrackingParameters;
			resetTrackingParameters(sec);
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->primaryParameters=(void *)prim;
			rec->secondaryParameters=(void *)sec;
			// **************************************
			// Da modificare come MNG_PEAKER
			CSubScanBinder binder(getConfiguration(),false);
			rec->receieversParsmeters=(void *)binder.getReceivers();
			rec->servoParameters=(void *)binder.getServo();
			rec->subScanConfiguration=binder.getSubScanConfiguration();
			// **************************************

			//rec->target="";
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Management::MNG_MOON: {
			DWORD id;
			Antenna::TTrackingParameters *prim=new Antenna::TTrackingParameters;
			if (!parseMoon(line,prim,id,errMsg)) {
				if (prim) delete prim;
				return false; // errMsg already set by previous call
			}
			Antenna::TTrackingParameters *sec=new Antenna::TTrackingParameters;
			resetTrackingParameters(sec);
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->primaryParameters=(void *)prim;
			rec->secondaryParameters=(void *)sec;
			// **************************************
			// Da modificare come MNG_PEAKER
			CSubScanBinder binder(getConfiguration(),false);
			rec->receieversParsmeters=(void *)binder.getReceivers();
			rec->servoParameters=(void *)binder.getServo();
			rec->subScanConfiguration=binder.getSubScanConfiguration();
			// **************************************

			//rec->target="";
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Management::MNG_OTFC: {
			CSubScanBinder binder(getConfiguration(),false);
			DWORD identifier;
			if (!parseOTFC(line,identifier,errMsg,binder)) {
				binder.dispose();
				return false;
			}
			TRecord *rec=new TRecord;
			rec->id=identifier;
			rec->type=type;
			rec->primaryParameters=(void *)binder.getPrimary();
			rec->secondaryParameters=(void *)binder.getSecondary();
			rec->receieversParsmeters=(void *)binder.getReceivers();
			rec->servoParameters=(void *)binder.getServo();
			rec->subScanConfiguration=binder.getSubScanConfiguration();
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Management::MNG_SKYDIP: {
			DWORD id;
			Antenna::TTrackingParameters *prim=new Antenna::TTrackingParameters;
			Antenna::TTrackingParameters *sec=new Antenna::TTrackingParameters;
			if (!parseSKYDIP(line,prim,sec,id,errMsg)) {
				if (prim) delete prim;
				if (dec) delete sec;
				return false; // errMsg already set by previous call
			}
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->primaryParameters=(void *)prim;
			rec->secondaryParameters=(void *)sec;
			// **************************************
			// Da modificare come MNG_PEAKER
			CSubScanBinder binder(getConfiguration(),false);
			rec->receieversParsmeters=(void *)binder.getReceivers();
			rec->servoParameters=(void *)binder.getServo();
			rec->subScanConfiguration=binder.getSubScanConfiguration();
			// **************************************

			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Management::MNG_SATELLITE: {
			break;
		}
		case Management::MNG_PLANET: {
			
          DWORD id;
			Antenna::TTrackingParameters *prim=new Antenna::TTrackingParameters;
			if (!parsePlanet(line,prim,id,errMsg)) {
				if (prim) delete prim;
				return false; // errMsg already set by previous call
			}
			Antenna::TTrackingParameters *sec=new Antenna::TTrackingParameters;
			resetTrackingParameters(sec);
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->primaryParameters=(void *)prim;
			rec->secondaryParameters=(void *)sec;
			// **************************************
			// Da modificare come MNG_PEAKER
			CSubScanBinder binder(getConfiguration(),false);
			rec->receieversParsmeters=(void *)binder.getReceivers();
			rec->servoParameters=(void *)binder.getServo();
			rec->subScanConfiguration=binder.getSubScanConfiguration();
			// **************************************

			//rec->target="";
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;			
			
			break;
		}
		case Management::MNG_OTF: {
			DWORD id;
			Antenna::TTrackingParameters *prim=new Antenna::TTrackingParameters;
			if (!parseOTF(line,prim,id,errMsg)) {
				if (prim) delete prim;
				return false; // errMsg already set by previous call
			}
			Antenna::TTrackingParameters *sec=new Antenna::TTrackingParameters;
			resetTrackingParameters(sec);
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->primaryParameters=(void *)prim;
			rec->secondaryParameters=(void *)sec;
			// **************************************
			// Da modificare come MNG_PEAKER
			CSubScanBinder binder(getConfiguration(),false);
			rec->receieversParsmeters=(void *)binder.getReceivers();
			rec->servoParameters=(void *)binder.getServo();
			rec->subScanConfiguration=binder.getSubScanConfiguration();
			// **************************************

			//rec->target="";
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		default: {
			errMsg="unknown scan type";
			return false;
		}
	}
	return true;
}

bool CScanList::parseMoon(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg)
{
	char type[32],offFrame[32],lonOff[32],latOff[32];
	long out;
	//double lonOff,latOff;
	out=sscanf((const char *)val,"%u\t%s\t%s\t%s\t%s",&id,type,offFrame,lonOff,latOff);
	if ((out!=2) && (out!=5)) {
		errMsg="invalid moon scan definition";
		return false;
	}
	scan->targetName=CORBA::string_dup("Moon");
	scan->type=Antenna::ANT_MOON;
	scan->paramNumber=0;
	scan->secondary=false;
	scan->VradFrame=Antenna::ANT_UNDEF_FRAME;
	scan->VradDefinition=Antenna::ANT_UNDEF_DEF;
	scan->RadialVelocity=0.0;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	scan->enableCorrection=true;
	if (out==5) {
		if (strcmp(offFrame,OFFFRAMEEQ)==0) {
			scan->offsetFrame=Antenna::ANT_EQUATORIAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid equatorial longitude offset";
				return false; //ra
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid equatorial latitude offset";
				return false;  //dec
			}
		}
		else if (strcmp(offFrame,OFFFRAMEHOR)==0) {
			scan->offsetFrame=Antenna::ANT_HORIZONTAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid horizontal longitude offset";
				return false;  //azimuth...since they are offsets negative values are valid
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid horizontal latitude offset";
				return false; //elevation
			}
		}
		else if (strcmp(offFrame,OFFFRAMEGAL)==0) {
			scan->offsetFrame=Antenna::ANT_GALACTIC;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid galactic longitude offset";
				return false;  //longitude
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid galactic latitude offset";
				return false; //latitude
			}
		}
		else {
			return false;
		}
		scan->applyOffsets=true;
	}
	else {
		scan->latitudeOffset=0.0;
		scan->longitudeOffset=0.0;
		scan->applyOffsets=false;
	}
	return true;
}

bool CScanList::parseSun(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg)
{
	char type[32],offFrame[32],lonOff[32],latOff[32];
	long out;
	//double lonOff,latOff;
	out=sscanf((const char *)val,"%u\t%s\t%s\t%s\t%s",&id,type,offFrame,lonOff,latOff);
	if ((out!=2) && (out!=5)) {
		errMsg="invalid sun scan definition";
		return false;
	}
	scan->targetName=CORBA::string_dup("Sun");
	scan->type=Antenna::ANT_SUN;
	scan->paramNumber=0;
	scan->secondary=false;
	scan->VradFrame=Antenna::ANT_UNDEF_FRAME;
	scan->VradDefinition=Antenna::ANT_UNDEF_DEF;
	scan->RadialVelocity=0.0;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	scan->enableCorrection=true;
	if (out==5) {
		if (strcmp(offFrame,OFFFRAMEEQ)==0) {
			scan->offsetFrame=Antenna::ANT_EQUATORIAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid equatorial longitude offset";
				return false; //ra
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid equatorial latitude offset";
				return false;  //dec
			}
		}
		else if (strcmp(offFrame,OFFFRAMEHOR)==0) {
			scan->offsetFrame=Antenna::ANT_HORIZONTAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid horizontal longitude offset";
				return false;  //azimuth...since they are offsets negative values are valid
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid horizontal latitude offset";
				return false; //elevation
			}
		}
		else if (strcmp(offFrame,OFFFRAMEGAL)==0) {
			scan->offsetFrame=Antenna::ANT_GALACTIC;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid galactic longitude offset";
				return false;  //longitude
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid galactic latitude offset";
				return false; //latitude
			}
		}
		else {
			return false;
		}
		scan->applyOffsets=true;
	}
	else {
		scan->latitudeOffset=0.0;
		scan->longitudeOffset=0.0;
		scan->applyOffsets=false;
	}
	return true;
}

bool CScanList::parsePlanet(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg)
{
	char type[32],offFrame[32],lonOff[32],latOff[32],planetName[32];
	long out;
	//double lonOff,latOff;
	out=sscanf((const char *)val,"%u\t%s\t%s\t%s\t%s\t%s",&id,type,planetName,offFrame,lonOff,latOff);
	if ((out!=3) && (out!=6)) {
		errMsg="invalid planet scan definition";
		return false;
	}
	scan->targetName=CORBA::string_dup(planetName);
	scan->type=Antenna::ANT_SOLARSYSTEMBODY;
	scan->paramNumber=0;
	scan->secondary=false;
	scan->VradFrame=Antenna::ANT_UNDEF_FRAME;
	scan->VradDefinition=Antenna::ANT_UNDEF_DEF;
	scan->RadialVelocity=0.0;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	scan->enableCorrection=true;
	if (out==6) {
 
		if (strcmp(offFrame,OFFFRAMEEQ)==0) {
			scan->offsetFrame=Antenna::ANT_EQUATORIAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid equatorial longitude offset";
				return false; //ra
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid equatorial latitude offset";
				return false;  //dec
			}
		}
		else if (strcmp(offFrame,OFFFRAMEHOR)==0) {
			scan->offsetFrame=Antenna::ANT_HORIZONTAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid horizontal longitude offset";
				return false;  //azimuth...since they are offsets negative values are valid
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid horizontal latitude offset";
				return false; //elevation
			}
		}
		else if (strcmp(offFrame,OFFFRAMEGAL)==0) {
			scan->offsetFrame=Antenna::ANT_GALACTIC;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid galactic longitude offset";
				return false;  //longitude
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid galactic latitude offset";
				return false; //latitude
			}
		}
		else {
			return false;
		}
		scan->applyOffsets=true;
	}
	else {
		scan->latitudeOffset=0.0;
		scan->longitudeOffset=0.0;
		scan->applyOffsets=false;
	}
	return true;
}










// void CSubScanBinder::sidereal(const char * targetName,const double& ra,const double& dec,const Antenna::TSystemEquinox& eq,const Antenna::TSections& section)
bool CScanList::parseSidereal2(const IRA::CString& val,DWORD& id,IRA::CString& errMsg,CSubScanBinder& binder)
{
	int start=0;
	IRA::CString token;
	Antenna::TCoordinateFrame frame;
	Antenna::TCoordinateFrame scanFrame=Antenna::ANT_EQUATORIAL;
	Antenna::TCoordinateFrame scanOffsetFrame=Antenna::ANT_EQUATORIAL;
	bool scanApplyOffsets=false;
	long scanParamNumber=0;
	IRA::CString name;
	double scanRadialVelocity;
	double scanLongitudeOffset,scanLatitudeOffset;
	Antenna::TReferenceFrame scanVradFrame;
	Antenna::TVradDefinition scanVradDefinition;
	double scanParameters[Antenna::ANTENNA_TRACKING_PARAMETER_NUMBER];
	Antenna::TSystemEquinox scanEquinox;

	// get the second item.....
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {  // id
		errMsg="cannot read scan identifier";
		return false;
	}
	id=token.ToLong();
	if (id==0) {
		errMsg="scan identifier cannot be zero";
		return false;
	}
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {  // type
		errMsg="cannot read scan type";
		return false;
	}
	//scan->type=Antenna::ANT_SIDEREAL;   //already know it is a sidereal
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {  // name
		errMsg="cannot read source name";
		return false;
	}
	//scan->targetName=CORBA::string_dup((const char *)token);
	name=token;
	bool frameOpen=false;
	bool offFrameOpen=false;
	long counter=0;
	long paramCounter=0;
	//scan->latitudeOffset=scan->longitudeOffset=0.0;
	//scan->applyOffsets=false;
	//scan->paramNumber=0;
	//scan->enableCorrection=true;
	//scan->secondary=false;
	//scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	//scan->VradFrame=Antenna::ANT_UNDEF_FRAME;
	//scan->VradDefinition=Antenna::ANT_UNDEF_DEF;
	//scan->RadialVelocity=0.0;
	frame=Antenna::ANT_EQUATORIAL;
	while (IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {  //get the next token...it represents the frame in which the coordinates are expressed
		bool ok=IRA::CIRATools::strToCoordinateFrame(token,frame);
		if ((frame==Antenna::ANT_EQUATORIAL) && (ok)) {
			if (frameOpen || offFrameOpen) {
				errMsg="wrong equatorial format";
				return false;  // if the frame has already been expressed, raise an error;
			}
			scanFrame=Antenna::ANT_EQUATORIAL;
			//scan->frame=Antenna::ANT_EQUATORIAL;
			frameOpen=true;
		}
		else if ((frame==Antenna::ANT_GALACTIC) && (ok)) {
			if (frameOpen || offFrameOpen) {
				errMsg="wrong galactic format";
				return false;  // if the frame has already been expressed, raise an error;
			}
			scanFrame=Antenna::ANT_GALACTIC;
			//scan->frame=Antenna::ANT_GALACTIC;
			frameOpen=true;
		}
		else if ((frame==Antenna::ANT_HORIZONTAL) && (ok)) {
			if (frameOpen || offFrameOpen) {
				errMsg="wrong horizontal format";
				return false;  // if the frame has already been expressed, raise an error;
			}
			scanFrame=Antenna::ANT_HORIZONTAL;
			//scan->frame=Antenna::ANT_HORIZONTAL;
			frameOpen=true;
		}
		else if (token==OFFFRAMEEQ) {
			if (frameOpen && (scanFrame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong equatorial offsets format";
					return false;
				}
				frameOpen=false;
				//scan->paramNumber=paramCounter;
				scanParamNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) {
				errMsg="wrong offsets format";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			//scan->offsetFrame=Antenna::ANT_EQUATORIAL;
			scanOffsetFrame=Antenna::ANT_EQUATORIAL;
			scanApplyOffsets=true;
			//scan->applyOffsets=true;
			offFrameOpen=true;
		}
		else if (token==OFFFRAMEGAL) {
			if (frameOpen && (scanFrame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong galactic offsets format";
					return false;
				}
				frameOpen=false;
				//scan->paramNumber=paramCounter;
				scanParamNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) {
				errMsg="wrong offsets format";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			//scan->offsetFrame=Antenna::ANT_GALACTIC;
			//scan->applyOffsets=true;
			scanOffsetFrame=Antenna::ANT_GALACTIC;
			scanApplyOffsets=true;
			offFrameOpen=true;
		}
		else if (token==OFFFRAMEHOR) {
			if (frameOpen && (scanFrame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong horizontal offsets format";
					return false;
				}
				frameOpen=false;
				//scan->paramNumber=paramCounter;
				scanParamNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) {
				errMsg="wrong offsets format";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			//scan->offsetFrame=Antenna::ANT_HORIZONTAL;
			//scan->applyOffsets=true;
			scanOffsetFrame=Antenna::ANT_HORIZONTAL;
			scanApplyOffsets=true;
			offFrameOpen=true;
		}
		else if (token==RVEL) {
			if (frameOpen && (scanFrame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong equatorial offsets format";
					return false;
				}
				frameOpen=false;
				//scan->paramNumber=paramCounter;
				scanParamNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) {
				errMsg="wrong scan definition";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			if (!parseVRADSwitch(val,start,scanRadialVelocity,scanVradFrame,scanVradDefinition,errMsg)) {
				return false;
			}
			//std::cout << "VRAD: " << (const char *) val << " " << scanRadialVelocity << endl;
			counter=0;
		}
		else {
			if (frameOpen) {
				if (counter==0) { //first two parameters are lon and lat
					if (scanFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::rightAscensionToRad(token,scanParameters[paramCounter],true)) {
							errMsg="right ascension format error";
							return false;
						}
					}
					else if (scanFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::azimuthToRad(token,scanParameters[paramCounter],true)) {
							errMsg="azimuth format error";
							return false;
						}
					}
					else if (scanFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::galLongitudeToRad(token,scanParameters[paramCounter],true)) {
							errMsg="galactic longitude format error";
							return false;
						}
					}
					paramCounter++;
					counter++;
				}
				else if (counter==1) {
					if (scanFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::declinationToRad(token,scanParameters[paramCounter],true)) {
							errMsg="declination format error";
							return false;
						}
					}
					else if (scanFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::elevationToRad(token,scanParameters[paramCounter],true)) {
							errMsg="elevation format error";
							return false;
						}
					}
					else if (scanFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::galLatitudeToRad(token,scanParameters[paramCounter],true)) {
							errMsg="galactic latitude format error";
							return false;
						}
					}
					if ((scanFrame==Antenna::ANT_HORIZONTAL) || (scanFrame==Antenna::ANT_GALACTIC)) {
						frameOpen=false;
						scanParamNumber=2;
						counter=0;
					}
					else {
						counter++;
						paramCounter++;
					}
				}
				else if ((counter==2) && (scanFrame==Antenna::ANT_EQUATORIAL)) {  // this should be the equinox, but only for equatorial frame
					if (!IRA::CIRATools::strToEquinox(token,scanEquinox)) {
						errMsg="invalid equinox";
						return false;
					}
					counter++;
				}
				else if ((counter<7) && (scanFrame==Antenna::ANT_EQUATORIAL)) { // if the frame is equatorial, up to 7 parameters are allowed
					scanParameters[paramCounter]=token.ToDouble();
					paramCounter++;
					counter++;
				}
				else {
					errMsg="invalid sidereal scan definition";
					return false;
				}
			}
			else if (offFrameOpen) {
				if (counter==0) {
					if (scanOffsetFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::offsetToRad(token,scanLongitudeOffset)) {
							errMsg="equatorial longitude offset format error";
							return false;
						}
					}
					else if (scanOffsetFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::offsetToRad(token,scanLongitudeOffset)) {
							errMsg="horizontal longitude offset format error";
							return false;
						}
					}
					else if (scanOffsetFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::offsetToRad(token,scanLongitudeOffset)) {
							errMsg="galactic longitude offset format error";
							return false;
						}
					}
					counter++;
				}
				else if (counter==1) {
					if (scanOffsetFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::offsetToRad(token,scanLatitudeOffset)) {
							errMsg="equatorial latitude offset format error";
							return false;
						}
					}
					else if (scanOffsetFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::offsetToRad(token,scanLatitudeOffset)) {
							errMsg="horizontal latitude offset format error";
							return false;
						}
					}
					else if (scanOffsetFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::offsetToRad(token,scanLatitudeOffset)) {
							errMsg="galactic latitude offset format error";
							return false;
						}
					}
					offFrameOpen=false;
					counter=0;
				}
				else {
					errMsg="too many parameters for offsets definition";
					return false;  // too many parameters in the offset specification
				}
			}
			else {
				errMsg="frame is missing";
				return false; // there is a parameter without having specified the frame
			}
		}
	}
	if (offFrameOpen) {
		errMsg="invalid sidereal scan definition";
		return false;
	}
	if ((frameOpen) && (counter<2)) {
		errMsg="invalid offsets definition";
		return false;
	}
	if (frameOpen && (scanFrame==Antenna::ANT_EQUATORIAL)) {
		if (counter<3) {
			errMsg="invalid equatorial coordinates definition";
			return false;
		}
		frameOpen=false;
		scanParamNumber=paramCounter;
		counter=0;
	}
	if (offFrameOpen || frameOpen) {
		errMsg="invalid sidereal scan definition";
		return false; // if the frame and frame offsets are not specified completely, raise an error
	}
	binder.sidereal(name,scanFrame,scanParameters,scanParamNumber,scanEquinox);
	if (scanApplyOffsets) {
		binder.addOffsets(scanLongitudeOffset,scanLatitudeOffset,scanOffsetFrame);
	}
	binder.addRadialvelocity(scanVradFrame,scanVradDefinition,scanRadialVelocity);
	return true;
}

/*bool CScanList::parseSidereal(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg)
{
	int start=0;
	IRA::CString token;
	Antenna::TCoordinateFrame frame;
	// get the second item.....
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {  // id
		errMsg="cannot read scan identifier";
		return false;
	}
	id=token.ToLong();
	if (id==0) {
		errMsg="scan identifier cannot be zero";
		return false;
	}
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {  // type
		errMsg="cannot read scan type";
		return false;
	}
	scan->type=Antenna::ANT_SIDEREAL;   //already know it is a sidereal
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {  // name
		errMsg="cannot read source name";
		return false;
	}
	scan->targetName=CORBA::string_dup((const char *)token);
	bool frameOpen=false;
	bool offFrameOpen=false;
	long counter=0;
	long paramCounter=0;
	scan->latitudeOffset=scan->longitudeOffset=0.0;
	scan->applyOffsets=false;
	scan->paramNumber=0;
	scan->enableCorrection=true;
	scan->secondary=false;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	scan->VradFrame=Antenna::ANT_UNDEF_FRAME;
	scan->VradDefinition=Antenna::ANT_UNDEF_DEF;
	scan->RadialVelocity=0.0;
	frame=Antenna::ANT_EQUATORIAL;
	while (IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {  //get the next token...it represents the frame in which the coordinates are expressed
		bool ok=IRA::CIRATools::strToCoordinateFrame(token,frame);
		if ((frame==Antenna::ANT_EQUATORIAL) && (ok)) {
			if (frameOpen || offFrameOpen) {
				errMsg="wrong equatorial format";
				return false;  // if the frame has already been expressed, raise an error;
			}
			scan->frame=Antenna::ANT_EQUATORIAL;
			frameOpen=true;
		}
		else if ((frame==Antenna::ANT_GALACTIC) && (ok)) {
			if (frameOpen || offFrameOpen) {
				errMsg="wrong galactic format";
				return false;  // if the frame has already been expressed, raise an error;
			}
			scan->frame=Antenna::ANT_GALACTIC;
			frameOpen=true;
		}
		else if ((frame==Antenna::ANT_HORIZONTAL) && (ok)) {
			if (frameOpen || offFrameOpen) {
				errMsg="wrong horizontal format";
				return false;  // if the frame has already been expressed, raise an error;
			}
			scan->frame=Antenna::ANT_HORIZONTAL;
			frameOpen=true;
		}
		else if (token==OFFFRAMEEQ) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong equatorial offsets format";
					return false;
				}
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) {
				errMsg="wrong offsets format";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			scan->offsetFrame=Antenna::ANT_EQUATORIAL;
			scan->applyOffsets=true;
			offFrameOpen=true;
		}
		else if (token==OFFFRAMEGAL) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong galactic offsets format";
					return false;
				}
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) {
				errMsg="wrong offsets format";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			scan->offsetFrame=Antenna::ANT_GALACTIC;
			scan->applyOffsets=true;
			offFrameOpen=true;
		}
		else if (token==OFFFRAMEHOR) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong horizontal offsets format";
					return false;
				}
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) {
				errMsg="wrong offsets format";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			scan->offsetFrame=Antenna::ANT_HORIZONTAL;
			scan->applyOffsets=true;
			offFrameOpen=true;
		}
		else if (token==RVEL) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong horizontal offsets format";
					return false;
				}
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) {
				errMsg="wrong scan definition";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			if (!parseVRADSwitch(val,start,scan->RadialVelocity,scan->VradFrame,scan->VradDefinition,errMsg)) {
				return false;
			}
			counter=0;
		}
		else {
			if (frameOpen) {
				if (counter==0) { //first two parameters are lon and lat
					if (scan->frame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::rightAscensionToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="right ascension format error";
							return false;
						}
					}
					else if (scan->frame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::azimuthToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="azimuth format error";
							return false;
						}
					}
					else if (scan->frame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::galLongitudeToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="galactic longitude format error";
							return false;
						}
					}
					paramCounter++;
					counter++;
				}
				else if (counter==1) {
					if (scan->frame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::declinationToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="declination format error";
							return false;
						}
					}
					else if (scan->frame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::elevationToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="elevation format error";
							return false;
						}
					}
					else if (scan->frame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::galLatitudeToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="galactic latitude format error";
							return false;
						}
					}
					if ((scan->frame==Antenna::ANT_HORIZONTAL) || (scan->frame==Antenna::ANT_GALACTIC)) {
						frameOpen=false;
						scan->paramNumber=2;
						counter=0;
					}
					else {
						counter++;
						paramCounter++;
					}
				}
				else if ((counter==2) && (scan->frame==Antenna::ANT_EQUATORIAL)) {  // this should be the equinox, but only for equatorial frame
					if (!IRA::CIRATools::strToEquinox(token,scan->equinox)) {
						errMsg="invalid equinox";
						return false;
					}
					counter++;
				}
				else if ((counter<7) && (scan->frame==Antenna::ANT_EQUATORIAL)) { // if the frame is equatorial, up to 7 parameters are allowed
					scan->parameters[paramCounter]=token.ToDouble();
					paramCounter++;
					counter++;
				}
				else {
					errMsg="invalid sidereal scan definition";
					return false;
				}
			}
			else if (offFrameOpen) {
				if (counter==0) {
					if (scan->offsetFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->longitudeOffset)) {
							errMsg="equatorial longitude offset format error";
							return false;
						}
					}
					else if (scan->offsetFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->longitudeOffset)) {
							errMsg="horizontal longitude offset format error";
							return false;
						}
					}
					else if (scan->offsetFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::offsetToRad(token,scan->longitudeOffset)) {
							errMsg="galactic longitude offset format error";
							return false;
						}
					}
					counter++;
				}
				else if (counter==1) {
					if (scan->offsetFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->latitudeOffset)) {
							errMsg="equatorial latitude offset format error";
							return false;
						}
					}
					else if (scan->offsetFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->latitudeOffset)) {
							errMsg="horizontal latitude offset format error";
							return false;
						}
					}
					else if (scan->offsetFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::offsetToRad(token,scan->latitudeOffset)) {
							errMsg="galactic latitude offset format error";
							return false;
						}
					}
					offFrameOpen=false;
					counter=0;
				}
				else {
					errMsg="too many parameters for offsets definition";
					return false;  // too many parameters in the offset specification
				}
			}
			else {
				errMsg="frame is missing";
				return false; // there is a parameter without having specified the frame
			}
		}
	}
	if (offFrameOpen) {
		errMsg="invalid sidereal scan definition";
		return false;
	}
	if ((frameOpen) && (counter<2)) {
		errMsg="invalid offsets definition";
		return false;
	}
	if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
		if (counter<3) {
			errMsg="invalid equatorial coordinates definition";
			return false;
		}
		frameOpen=false;
		scan->paramNumber=paramCounter;
		counter=0;
	}
	if (offFrameOpen || frameOpen) {
		errMsg="invalid sidereal scan definition";
		return false; // if the frame and frame offsets are not specified completely, raise an error
	}
	return true;
}*/

bool CScanList::parseOffsetSwitch(const IRA::CString& val,int& start,
  Antenna::TCoordinateFrame& offsetFrame,double& lonoff,double& latoff,bool& res,IRA::CString& errMsg)
{
	IRA::CString lontoken,lattoken,token;
	errMsg="";
	res=false;
	while (IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {
		if (strcmp(token,OFFFRAMEEQ)==0) {
			offsetFrame=Antenna::ANT_EQUATORIAL;
			if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,lontoken,false)) {
				errMsg="not enough parameters for equatorial offset switch";
				return false;
			}	
			if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,lattoken,false)) {
				errMsg="not enough parameters for equatorial offset switch";
				return false;
			}
			if (!IRA::CIRATools::offsetToRad(lontoken,lonoff)) {
				errMsg="invalid right ascension offset";
				return false;
			}
			if (!IRA::CIRATools::offsetToRad(lattoken,latoff)) {
				errMsg="invalid declination offset";
				return false; 
			}
			res=true;
			return true;
		}
		else if (strcmp(token,OFFFRAMEHOR)==0) {
			offsetFrame=Antenna::ANT_HORIZONTAL;
			if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,lontoken,false)) {
				errMsg="not enough parameters for horizontal offset switch";
				return false;
			}	
			if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,lattoken,false)) {
				errMsg="not enough parameters for horizontal offset switch";
				return false;
			}
			if (!IRA::CIRATools::offsetToRad(lontoken,lonoff)) {
				errMsg="invalid azimuth offset";
				return false;
			}
			if (!IRA::CIRATools::offsetToRad(lattoken,latoff)) {
				errMsg="invalid elevation offset";
				return false; 
			}
			res=true;
			return true;
		}
		else if (strcmp(token,OFFFRAMEGAL)==0) {
			offsetFrame=Antenna::ANT_GALACTIC;
			if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,lontoken,false)) {
				errMsg="not enough parameters for galactic offset switch";
				return false;
			}	
			if (!IRA::CIRATools::getNextToken(val,start,SEPARATORS,lattoken,false)) {
				errMsg="not enough parameters for galactic offset switch";
				return false;
			}
			if (!IRA::CIRATools::offsetToRad(lontoken,lonoff)) {
				errMsg="invalid longitude offset";
				return false;
			}
			if (!IRA::CIRATools::offsetToRad(lattoken,latoff)) {
				errMsg="invalid latitude offset";
				return false; 
			}
			res=true;
			return true;			
		}
	}
	return true;		
}

bool CScanList::parseVRADSwitch(const IRA::CString& val,int& start,double& vrad,
  Antenna::TReferenceFrame& frame,Antenna::TVradDefinition& ref,bool& result,IRA::CString& errMsg)
{
	IRA::CString token;
	errMsg="";
	result=false;
	while (IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {
		if (strcmp(token,RVEL)==0) {
			if (parseVRADSwitch(val,start,vrad,frame,ref,errMsg)) {
				result=true;
				return true;
			}
			else {
				return false;
			}
		}
	}
	return true;		
}

bool CScanList::parseVRADSwitch(const IRA::CString& val,int& start,double& vrad,Antenna::TReferenceFrame& frame,Antenna::TVradDefinition& ref,IRA::CString& errMsg)
{
	IRA::CString token;
	if (IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {
		vrad=token.ToDouble();
	}
	else {
		errMsg="not enough parameters for the radial velocity switch";
		return false;
	}
	if (IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {
		if (!Antenna::Definitions::map(token,frame)) {
			errMsg="the reference frame of the radial velocity is incorrect";
			return false;
		}
	}
	else {
		errMsg="not enough parameters for the radial velocity switch";
		return false;
	}
	if (IRA::CIRATools::getNextToken(val,start,SEPARATORS,token,false)) {
		if (!Antenna::Definitions::map(token,ref)) {
			errMsg="the radial velocity definition is incorrect";
			return false;
		}
	}
	else {
		errMsg="not enough parameters for the radial velocity switch";
		return false;
	}
	return true;
}

bool CScanList::parseSKYDIP(const IRA::CString& val,Antenna::TTrackingParameters *scan,Antenna::TTrackingParameters *secScan,DWORD& id,IRA::CString& errMsg)
{
	TRecord rec;
	char type[32],el1[32],el2[32],offFrame[32],lonOff[32],latOff[32];
	double duration;
	double dEl1,dEl2;
	DWORD referenceScan;
	long out;
	scan->paramNumber=0;
	scan->latitudeOffset=scan->longitudeOffset=0.0;
	scan->applyOffsets=false;
	scan->type=Antenna::ANT_OTF;
	scan->VradFrame=Antenna::ANT_UNDEF_FRAME;
	scan->VradDefinition=Antenna::ANT_UNDEF_DEF;
	scan->RadialVelocity=0.0;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	scan->secondary=true;
	scan->enableCorrection=false; // disable the pointing corrections
	out=sscanf((const char *)val,"%u\t%s\t%u\t%s\t%s\t%lf\t%s\t%s\t%s\t",&id,type,&referenceScan,el1,el2,&duration,offFrame,lonOff,latOff);
	if ((out!=6) && (out!=9)) {   //parameters are 9 for the OTF plus 3 (not mandatory) for the offsets...
		errMsg="invalid skydip scan definition";
		return false;
	}
	else {
		if (!IRA::CIRATools::elevationToRad(el1,dEl1,true)) {  //...need a complete check
			errMsg="first elevation limit has a format error in skydip definition";
			return false;
		}
		if (!IRA::CIRATools::elevationToRad(el2,dEl2,true)) {  //  ...need a complete check
			errMsg="second elevation limit has a format error in skydip definition";
			return false;
		}
		//description
		scan->otf.description=Antenna::SUBSCAN_STARTSTOP;
		//duration
		TIMEDIFFERENCE time;
		time.value((long double)duration);
		scan->otf.subScanDuration=time.value().value;
		//coordFrame
		scan->otf.coordFrame=Antenna::ANT_HORIZONTAL;
		//subScanFrame
		scan->otf.subScanFrame=Antenna::ANT_HORIZONTAL;
		//direction
		scan->otf.direction=Antenna::SUBSCAN_DECREASE;
		//geometry
		scan->otf.geometry=Antenna::SUBSCAN_CONSTLON;
		scan->otf.lon1=scan->otf.lon2=0.0; // these values will be decided runtime by the antenna subsystem considering the referenceScan
		scan->otf.lat1=GETMAX(dEl1,dEl2);
		scan->otf.lat2=GETMIN(dEl1,dEl2);
		if (out==9) {
			if (strcmp(offFrame,OFFFRAMEEQ)==0) {
				scan->offsetFrame=Antenna::ANT_EQUATORIAL;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
					errMsg="invalid right ascension offset";
					return false; //ra
				}
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
					errMsg="invalid declination offset";
					return false;  //dec
				}
			}
			else if (strcmp(offFrame,OFFFRAMEHOR)==0) {
				scan->offsetFrame=Antenna::ANT_HORIZONTAL;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
					errMsg="invalid azimuth offset";
					return false;  //azimuth...since they are offsets negative values are valid
				}
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
					errMsg="invalid elevation offset";
					return false;   //elevation
				}
			}
			else if (strcmp(offFrame,OFFFRAMEGAL)==0) {
				scan->offsetFrame=Antenna::ANT_GALACTIC;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
					errMsg="invalid galactic longitude offset";
					return false;  //longitude
				}
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
					errMsg="invalid galactic latitude offset";
					return false; //latitude
				}
			}
			else {
				errMsg="scan offset frame is unknown";
				return false;
			}
			scan->applyOffsets=true;
		}
		if (!getScan(referenceScan,rec)) {
			errMsg=getLastError();
			return false; // the secondary scan does not exist
		}
		Antenna::TTrackingParameters *tmp=static_cast<Antenna::TTrackingParameters *>(rec.primaryParameters);
		secScan->targetName=CORBA::string_dup(tmp->targetName);
		scan->targetName=CORBA::string_dup(tmp->targetName);
		secScan->type=tmp->type;
		for (long k=0;k<Antenna::ANTENNA_TRACKING_PARAMETER_NUMBER;k++) secScan->parameters[k]=tmp->parameters[k];
		secScan->paramNumber=tmp->paramNumber;
		secScan->frame=tmp->frame;
		secScan->equinox=tmp->equinox;
		secScan->longitudeOffset=0.0;
		secScan->latitudeOffset=0.0;
		secScan->applyOffsets=false;
		secScan->section=tmp->section;
		secScan->secondary=false;
		secScan->otf.lon1=tmp->otf.lon1;
		secScan->otf.lat1=tmp->otf.lat1;
		secScan->otf.lon2=tmp->otf.lon2;
		secScan->otf.lat2=tmp->otf.lat2;
		secScan->otf.coordFrame=tmp->otf.coordFrame;
		secScan->otf.geometry=tmp->otf.geometry;
		secScan->otf.subScanFrame=tmp->otf.subScanFrame;
		secScan->otf.description=tmp->otf.description;
		secScan->otf.direction=tmp->otf.direction;
		secScan->otf.subScanDuration=tmp->otf.subScanDuration;
		return true;
	}
}

bool CScanList::parsePeaker(const IRA::CString& line,DWORD& id,IRA::CString& errMsg,CSubScanBinder& binder)
{
	char type[32],axis[32];
	double span,seconds;
	DWORD scanTarget;
	TIMEDIFFERENCE duration;
	Management::TScanAxis ax=Management::MNG_NO_AXIS;
	TRecord rec;
	long out;
	out=sscanf((const char *)line,"%u\t%s\t%u\t%s\t%lf\t%lf",&id,type,&scanTarget,axis,&span,&seconds);
	if (id==0) {
		errMsg="scan identifier cannot be zero";
		return false;
	}
	if (!getScan(scanTarget,rec)) {
		errMsg=getLastError();
		return false; // the secondary scan does not exist
	}
	duration.value((long double)seconds);
	if (m_config!=0) {
		ax=m_config->getAxisFromServoName(axis);
		if (ax==Management::MNG_NO_AXIS) {
			errMsg="illegal or unknown axis name";
			return false;
		}
	}
	Antenna::TTrackingParameters * prim=static_cast<Antenna::TTrackingParameters *>(rec.primaryParameters);
	binder.peaker(axis,span,duration.value().value,prim);
	return true;
}

bool CScanList::parseOTFC(const IRA::CString& val,DWORD& id,IRA::CString& errMsg,CSubScanBinder& binder)
{
	TRecord rec;
	char coordFrame[32],geometry[32],subScanFrame[32],direction[32],type[32],span[32];
	DWORD scanCenter;
	double duration,sp;
	//Antenna::TCoordinateFrame frame;
	long out;
	int bMark;
	bool result;
	Antenna::TReferenceFrame VradFrame;
	Antenna::TVradDefinition VradDefinition;
	double RadialVelocity;

	Antenna::TCoordinateFrame cFrame,sSFrame;
	Antenna::TsubScanGeometry geo;
	Antenna::TsubScanDirection dir;
	Antenna::TCoordinateFrame scanOffsetFrame=Antenna::ANT_EQUATORIAL;
	double scanLongitudeOffset,scanLatitudeOffset;
	ACS::TimeInterval subScanDuration;
	
	//scan->targetName=CORBA::string_dup("");
	//scan->paramNumber=0;
	//scan->latitudeOffset=scan->longitudeOffset=0.0;
	//scan->applyOffsets=false;
	//scan->type=Antenna::ANT_OTF;
	//scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	//scan->secondary=true;
	//scan->enableCorrection=true;
	//scan->VradFrame=Antenna::ANT_UNDEF_FRAME;
	//scan->VradDefinition=Antenna::ANT_UNDEF_DEF;
	//scan->RadialVelocity=0.0;
	out=sscanf((const char *)val,"%u\t%s\t%u\t%s\t%s\t%s\t%s\t%s\t%lf",&id,type,&scanCenter,span,
	  coordFrame,subScanFrame,geometry,direction,&duration);
	if ((out!=9)) {   //parameters are 9 for the OTF plus 3 (not mandatory) for the offsets...
		errMsg="invalid on the fly scan definition";
		return false;
	}
	else {
		//Description
		//scan->otf.description=Antenna::SUBSCAN_CENTER;
		//scan->otf.lon1=scan->otf.lat1=0.0;
		//duration
		TIMEDIFFERENCE time;
		time.value((long double)duration);
		subScanDuration=time.value().value;
		//scan->otf.subScanDuration=time.value().value;
		//coordFrame
		if (!IRA::CIRATools::strToCoordinateFrame(coordFrame,cFrame)) {
			errMsg="the frame is not known";
			return false;
		}
		if (cFrame==Antenna::ANT_HORIZONTAL) {
			errMsg="horizontal frame is not supported as coordinate frame";
			return false;
		}
		//scan->otf.coordFrame=frame;
		//direction
		if (strcmp(direction,"INC")==0) dir=Antenna::SUBSCAN_INCREASE;
		else if (strcmp(direction,"DEC")==0) dir=Antenna::SUBSCAN_DECREASE;
		else {
			errMsg="invalid on the fly scan direction";
			return false;
		}
		//geometry
		if (strcmp(geometry,"LON")==0) geo=Antenna::SUBSCAN_CONSTLON;
		else if (strcmp(geometry,"LAT")==0) geo=Antenna::SUBSCAN_CONSTLAT; //GC not allowed
		else {
			errMsg="invalid on the fly scan geometry";
			return false;
		}
		//subScan Frame
		if (!IRA::CIRATools::strToCoordinateFrame(subScanFrame,sSFrame)) {
			errMsg="invalid on the fly scan frame";
			return false;
		}
		//scan->otf.subScanFrame=frame;
		//span
		//if (geo==Antenna::SUBSCAN_CONSTLON) {
		if (!IRA::CIRATools::offsetToRad(span,sp)) {
			errMsg="invalid on the fly scan span";
			return false;
		}
			//scan->otf.lon2=0.0;
		//}
		//else {
		//	if (!IRA::CIRATools::offsetToRad(span,scan->otf.lon2)) {
		//		errMsg="invalid on the fly scan longitude span";
		//		return false; //el span....no need of a check
		//	}
		//	scan->otf.lat2=0.0;
		//}		
	}
	if (!getScan(scanCenter,rec)) {
		errMsg=getLastError();
		return false; // the secondary scan does not exist
	}
	Antenna::TTrackingParameters *tmp=static_cast<Antenna::TTrackingParameters *>(rec.primaryParameters);
	binder.otfc(cFrame,geo,sSFrame,dir,sp,subScanDuration,tmp);
	bMark=0;
	if (!parseOffsetSwitch(val,bMark,scanOffsetFrame,scanLongitudeOffset,scanLatitudeOffset,result,errMsg)) {
		return false;
	}
	else if (result) {
		binder.addOffsets(scanLongitudeOffset,scanLatitudeOffset,scanOffsetFrame);
	}
	bMark=0;
	if (!parseVRADSwitch(val,bMark,RadialVelocity,VradFrame,VradDefinition,result,errMsg)) {
		return false;
	}
	else if (result) {
		binder.addRadialvelocity(VradFrame,VradDefinition,RadialVelocity);
	}	
	return true;
}

bool CScanList::parseOTF(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg)
{
	char coordFrame[32],geometry[32],subScanFrame[32],description[32],direction[32],type[32],lon1[32],lon2[32],lat1[32],lat2[32];
	char targetName[32];
	double duration;
	Antenna::TCoordinateFrame frame;
	long out;
	int bMark;
	bool result;
	IRA::CString token;
	Antenna::TReferenceFrame VradFrame;
	Antenna::TVradDefinition VradDefinition;
	double RadialVelocity;
	//scan->targetName="";
	scan->secondary=false;
	scan->paramNumber=0;
	scan->enableCorrection=true;
	scan->latitudeOffset=scan->longitudeOffset=0.0;
	scan->applyOffsets=false;
	scan->type=Antenna::ANT_OTF;
	scan->VradFrame=Antenna::ANT_UNDEF_FRAME;
	scan->VradDefinition=Antenna::ANT_UNDEF_DEF;
	scan->RadialVelocity=0.0;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	out=sscanf((const char *)val,"%u\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lf",&id,type,targetName,lon1,lat1,lon2,
			lat2,coordFrame,subScanFrame,geometry,description,direction,&duration);
	if ((out!=13)) {   //parameters are 12 for the OTF plus 3 (not mandatory) for the offsets...
		errMsg="invalid on the fly scan definition";
		return false;
	}
	else {
		TIMEDIFFERENCE time;
		time.value((long double)duration);
		scan->otf.subScanDuration=time.value().value;
		scan->targetName=CORBA::string_dup(targetName);
		if (strcmp(description,"SS")==0) scan->otf.description=Antenna::SUBSCAN_STARTSTOP;
		else if (strcmp(description,"CEN")==0) scan->otf.description=Antenna::SUBSCAN_CENTER;
		else {
			errMsg="invalid on the fly scan description";
			return false;
		}
		if (!IRA::CIRATools::strToCoordinateFrame(coordFrame,frame)) {
			errMsg="the coordinate frame is not known";
			return false;
		}
		if (frame==Antenna::ANT_EQUATORIAL) {
			scan->otf.coordFrame=Antenna::ANT_EQUATORIAL;
			if (scan->otf.description==Antenna::SUBSCAN_CENTER) {
				if (!IRA::CIRATools::rightAscensionToRad(lon1,scan->otf.lon1,true)) { //ra center ...need a complete check
					errMsg="central point right ascension format error";
					return false;
				}
				if (!IRA::CIRATools::declinationToRad(lat1,scan->otf.lat1,true)) { //dec center ...need a complete check
					errMsg="central point declination format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lon2,scan->otf.lon2)) { //ra span....no need of a check
					errMsg="right ascension span format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lat2,scan->otf.lat2)) { //dec span....no need of a check
					errMsg="declination span format error";
					return false;
				}
			}
			else {
				if (!IRA::CIRATools::rightAscensionToRad(lon1,scan->otf.lon1,true)) { //ra first point ...need a complete check
					errMsg="start point right ascension format error";
					return false;
				}
				if (!IRA::CIRATools::declinationToRad(lat1,scan->otf.lat1,true)) { //dec first point ...need a complete check
					errMsg="start point declination format error";
					return false;
				}
				if (!IRA::CIRATools::rightAscensionToRad(lon2,scan->otf.lon2,true)) {  //ra second point ...need a complete check
					errMsg="end point right ascension format error";
					return false;
				}
				if (!IRA::CIRATools::declinationToRad(lat2,scan->otf.lat2,true)) { //dec second point ...need a complete check
					errMsg="end point declination format error";
					return false;
				}
			}
		}
		else if (frame==Antenna::ANT_HORIZONTAL) {
			scan->otf.coordFrame=Antenna::ANT_HORIZONTAL;
			if (scan->otf.description==Antenna::SUBSCAN_CENTER) {
				if (!IRA::CIRATools::azimuthToRad(lon1,scan->otf.lon1,true)) { //az center ...need a complete check
					errMsg="central point azimuth format error";
					return false;
				}
				if (!IRA::CIRATools::elevationToRad(lat1,scan->otf.lat1,true)) {  //el center ...need a complete check
					errMsg="central point elevation format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lon2,scan->otf.lon2)) { //az span....no need of a check
					errMsg="azimuth span format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lat2,scan->otf.lat2)) { //el span....no need of a check
					errMsg="elevation span format error";
					return false;
				}
			}
			else {
				if (!IRA::CIRATools::azimuthToRad(lon1,scan->otf.lon1,true)) { //az first point ...need a complete check
					errMsg="start point azimuth format error";
					return false;
				}
				if (!IRA::CIRATools::elevationToRad(lat1,scan->otf.lat1,true)) { //el first point ...need a complete check
					errMsg="start point elevation format error";
					return false;
				}
				if (!IRA::CIRATools::azimuthToRad(lon2,scan->otf.lon2,true)) { //az second point ...need a complete check
					errMsg="end point azimuth format error";
					return false;
				}
				if (!IRA::CIRATools::elevationToRad(lat2,scan->otf.lat2,true)) { //el second point ...need a complete check
					errMsg="end point elevation format error";
					return false;
				}
			}
		}
		else if (frame==Antenna::ANT_GALACTIC) {
			scan->otf.coordFrame=Antenna::ANT_GALACTIC;
			if (scan->otf.description==Antenna::SUBSCAN_CENTER) {
				if (!IRA::CIRATools::galLongitudeToRad(lon1,scan->otf.lon1,true)) { //lon center ...need a complete check
					errMsg="central point galactic longitude format error";
					return false;
				}
				if (!IRA::CIRATools::galLatitudeToRad(lat1,scan->otf.lat1,true)) {  //lat center ...need a complete check
					errMsg="central point galactic latitude format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lon2,scan->otf.lon2)) { //lon span....no need of a check
					errMsg="galactic longitude span format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lat2,scan->otf.lat2)) {  //lat span....no need of a check
					errMsg="galactic latitude span format error";
					return false;
				}
			}
			else {
				if (!IRA::CIRATools::galLongitudeToRad(lon1,scan->otf.lon1,true)) { //lon first point ...need a complete check
					errMsg="start point galactic longitude format error";
					return false;
				}
				if (!IRA::CIRATools::galLatitudeToRad(lat1,scan->otf.lat1,true)) { //lat first point ...need a complete check
					errMsg="start point galactic latitude format error";
					return false;
				}
				if (!IRA::CIRATools::galLongitudeToRad(lon2,scan->otf.lon2,true)) {  //lon second point ...need a complete check
					errMsg="end point galactic longitude format error";
					return false;
				}
				if (!IRA::CIRATools::galLatitudeToRad(lat2,scan->otf.lat2,true)) {  //lat second point ...need a complete check
					errMsg="end point galactic latitude format error";
					return false;
				}
			}
		}
		if (!IRA::CIRATools::strToCoordinateFrame(subScanFrame,frame)) {
			errMsg="the scan frame is not known";
			return false;
		}
		scan->otf.subScanFrame=frame;
		if (strcmp(geometry,"LON")==0) scan->otf.geometry=Antenna::SUBSCAN_CONSTLON;
		else if (strcmp(geometry,"LAT")==0) scan->otf.geometry=Antenna::SUBSCAN_CONSTLAT;
		else if (strcmp(geometry,"GC")==0) scan->otf.geometry=Antenna::SUBSCAN_GREATCIRCLE;
		else {
			errMsg="invalid on the fly scan geometry";
			return false;
		}
		if (strcmp(direction,"INC")==0) scan->otf.direction=Antenna::SUBSCAN_INCREASE;
		else if (strcmp(direction,"DEC")==0) scan->otf.direction=Antenna::SUBSCAN_DECREASE;
		else {
			errMsg="invalid on the fly scan direction";
			return false;
		}
		bMark=0;
		if (!parseOffsetSwitch(val,bMark,scan->offsetFrame,scan->longitudeOffset,scan->latitudeOffset,result,errMsg)) {
			return false;
		}
		else if (result) {
			scan->applyOffsets=true;
		}
		bMark=0;
		if (!parseVRADSwitch(val,bMark,RadialVelocity,VradFrame,VradDefinition,result,errMsg)) {
			return false;
		}
		else if (result) {
			scan->VradFrame=VradFrame;
			scan->VradDefinition=VradDefinition;
			scan->RadialVelocity=RadialVelocity;
		}
		return true;
	}
}

void CScanList::resetTrackingParameters(Antenna::TTrackingParameters *scan)
{
	scan->secondary=false;
	scan->applyOffsets=false;
	scan->type=Antenna::ANT_NONE;
	scan->paramNumber=0;
	scan->enableCorrection=true;
}
