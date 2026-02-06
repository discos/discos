
#include <errno.h>
#include "IRA"
#include "DateTime.h"
#include <slamac.h>
#include <slalib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#include <boost/regex.hpp>

using namespace IRA;

void CIRATools::Wait(int seconds,long micro)
{
	struct timespec req,rem;
	req.tv_nsec=micro*1000;
	req.tv_sec=seconds;
	while (nanosleep(&req,&rem)!=0) {
		if (errno==EINTR) {
			req.tv_nsec=rem.tv_nsec;
			req.tv_sec=rem.tv_sec;
		}
		else {
			break;
		}
	}
}

void CIRATools::Wait(long long micro)
{
	long mm;
	int seconds;
	seconds=micro/1000000;
	mm=micro % 1000000;
	Wait(seconds,mm);
}

DDWORD CIRATools::timeMicroSeconds(TIMEDIFFERENCE& time)
{
	DDWORD tmp;
	tmp=((((time.day()*24+time.hour())*60LL+time.minute())*60+time.second())*1000000+time.microSecond());
	return tmp;
}

void CIRATools::getTime(TIMEVALUE& Now)
{
	Now.value(TimeUtil::ace2epoch(ACE_OS::gettimeofday()));
}

ACS::Time CIRATools::getACSTime()
{
	TIMEVALUE now;
	CIRATools::getTime(now);
	return now.value().value;
}

double CIRATools::getUNIXTime()
{
	return CIRATools::ACSTime2UNIXTime(CIRATools::getACSTime());
}

double CIRATools::ACSTime2UNIXTime(const ACS::Time& acs_time)
{
	return double(acs_time - ACSTIME2UNIXEPOCHOFFSET) / 10000000;
}

ACS::Time CIRATools::UNIXTime2ACSTime(const double& unix_epoch)
{
	return ACS::Time(long(unix_epoch * 10000000) + ACSTIME2UNIXEPOCHOFFSET);
}

std::string CIRATools::ACSTime2ISO8601(const ACS::Time& acs_time)
{
	ACS::Time unix_time = acs_time - ACSTIME2UNIXEPOCHOFFSET;
	std::time_t seconds = static_cast<std::time_t>(unix_time / 10000000);
	std::ostringstream mseconds;
	mseconds << std::setw(7) << std::setfill('0') << static_cast<unsigned int>(unix_time - seconds * 10000000);
	std::tm* tm = std::gmtime(&seconds);
	std::ostringstream oss;
	oss << std::put_time(tm, "%Y-%m-%dT%H:%M:%S") << "." << mseconds.str().substr(0, 3) << "Z";
	return oss.str();
}

void CIRATools::timeCopy(TIMEVALUE& dst,const TIMEVALUE& src)
{
	// EpochHelper.value() should have to have the 'const' qualifiers....that would result in a warning
	TIMEVALUE *tmp;
	tmp=const_cast<TIMEVALUE*>(&src);
	dst.value(tmp->value());
}

DDWORD CIRATools::timeDifference(TIMEVALUE& First,TIMEVALUE& Last)
{
	acstime::Duration diff;
	if (First>=Last.value()) {
		diff=First.difference(Last.value());
	}
	else {
		diff=Last.difference(First.value());
	}
	DurationHelper diffHelper(diff);
	return timeMicroSeconds(diffHelper);
}

long long CIRATools::timeSubtract(const TIMEVALUE& First,const TIMEVALUE& Second)
{
	TIMEVALUE Sec,Fir;
	timeCopy(Sec,Second);
	timeCopy(Fir,First);
	TIMEDIFFERENCE diff(Fir.difference(Sec.value())); //First-Second
	if (diff.positive()) {
		return timeMicroSeconds(diff);
	}
	else {
		return -1LL*timeMicroSeconds(diff);
	}
}

bool CIRATools::getDBValue(maci::ContainerServices *Services,CString fieldName,double &Val,CString Domain,CString name)
{
	CString fullName;
	if (name=="") fullName=Domain+Services->getName();
	else fullName=Domain+name;
	try {
		CDB::DAL_var Dal_p=Services->getCDB();
		CDB::DAO_var Dao_p=Dal_p->get_DAO_Servant((const char *)fullName);
		Val=Dao_p->get_double((const char *)fieldName);
	}
	catch (...) {
		return false;
	}
	return true;
}

bool CIRATools::getDBValue(maci::SimpleClient *client,CString fieldName,double &Val,CString Domain,CString name)
{
	CString fullName;
	fullName=Domain+name;
	try {
		CORBA::Object_var obj=client->getComponent("CDB",0,false);
		CDB::DAL_var Dal_p=CDB::DAL::_narrow(obj);
		CDB::DAO_var Dao_p=Dal_p->get_DAO_Servant((const char *)fullName);
		Val=Dao_p->get_double((const char *)fieldName);
	}
	catch (...) {
		return false;
	}
	return true;
}

bool CIRATools::getDBValue(maci::ContainerServices *Services,CString fieldName,long &Val,CString Domain,CString name)
{
	CString fullName;
	if (name=="") fullName=Domain+Services->getName();
	else fullName=Domain+name;
	try {
		CDB::DAL_var Dal_p=Services->getCDB();
		CDB::DAO_var Dao_p=Dal_p->get_DAO_Servant((const char *)fullName);
		Val=Dao_p->get_long((const char *)fieldName);
	}
	catch (...) {
		return false;
	}
	return true;
}

bool CIRATools::getDBValue(maci::SimpleClient *client,CString fieldName,long &Val,CString Domain,CString name)
{
	CString fullName;
	fullName=Domain+name;
	try {
		CORBA::Object_var obj=client->getComponent("CDB",0,false);
		CDB::DAL_var Dal_p=CDB::DAL::_narrow(obj);
		CDB::DAO_var Dao_p=Dal_p->get_DAO_Servant((const char *)fullName);
		Val=Dao_p->get_long((const char *)fieldName);
	}
	catch (...) {
		return false;
	}
	return true;
}

bool CIRATools::getDBValue(maci::ContainerServices *Services,CString fieldName,DWORD &Val,CString Domain,CString name)
{
	long app;
	CString fullName;
	if (name=="") fullName=Domain+Services->getName();
	else fullName=Domain+name;
	try {
		CDB::DAL_var Dal_p=Services->getCDB();
		CDB::DAO_var Dao_p=Dal_p->get_DAO_Servant((const char *)fullName);
		app=Dao_p->get_long((const char *)fieldName);
	}
	catch (...) {
		return false;
	}
	Val=(DWORD)app;
	return true;
}

bool CIRATools::getDBValue(maci::SimpleClient *client,CString fieldName,DWORD &Val,CString Domain,CString name)
{
	long app;
	CString fullName;
	fullName=Domain+name;
	try {
		CORBA::Object_var obj=client->getComponent("CDB",0,false);
		CDB::DAL_var Dal_p=CDB::DAL::_narrow(obj);
		CDB::DAO_var Dao_p=Dal_p->get_DAO_Servant((const char *)fullName);
		app=Dao_p->get_long((const char *)fieldName);
	}
	catch (...) {
		return false;
	}
	Val=(DWORD)app;
	return true;
}

bool CIRATools::getDBValue(maci::ContainerServices *Services,CString fieldName,CString &Val,CString Domain,CString name)
{
	CString fullName;
	if (name=="") fullName=Domain+Services->getName();
	else fullName=Domain+name;
	try {
		CDB::DAL_var Dal_p=Services->getCDB();
		CDB::DAO_var Dao_p=Dal_p->get_DAO_Servant((const char *)fullName);
		CORBA::String_var dbstr=Dao_p->get_string((const char *)fieldName);
		Val=CString((const char *)dbstr);
	}
	catch (...) {
		return false;
	}
	return true;
}

bool CIRATools::getDBValue(maci::SimpleClient *client,CString fieldName,CString &Val,CString Domain,CString name)
{
	CString fullName;
	fullName=Domain+name;
	try {
		CORBA::Object_var obj=client->getComponent("CDB",0,false);
		CDB::DAL_var Dal_p=CDB::DAL::_narrow(obj);
		CDB::DAO_var Dao_p=Dal_p->get_DAO_Servant((const char *)fullName);	
		CORBA::String_var dbstr=Dao_p->get_string((const char *)fieldName);
		Val=CString((const char *)dbstr);	
	}
	catch (...) {
		return false;
	}
	return true;
}

bool CIRATools::setDBValue(maci::ContainerServices* Services,CString fieldName,const double &Val,CString Domain,CString name)
{
	CString fullName;
	if (name=="") fullName=Domain+Services->getName();
	else fullName=Domain+name;
	try {
		CDB::DAL_var Dal_p=Services->getCDB();
		CDB::WDAL_var wDal_p=CDB::WDAL::_narrow(Dal_p);
		CDB::WDAO_var wDao_p=wDal_p->get_WDAO_Servant((const char *)fullName);
		wDao_p->set_double((const char *)fieldName,Val);
	}
	catch (...) {
		return false;
	}
	return true;
}

bool CIRATools::setDBValue(maci::ContainerServices* Services,CString fieldName,const long &Val,CString Domain,CString name)
{
	CString fullName;
	if (name=="") fullName=Domain+Services->getName();
	else fullName=Domain+name;
	try {
		CDB::DAL_var Dal_p=Services->getCDB();
		CDB::WDAL_var wDal_p=CDB::WDAL::_narrow(Dal_p);
		CDB::WDAO_var wDao_p=wDal_p->get_WDAO_Servant((const char *)fullName);
		wDao_p->set_long((const char *)fieldName,Val);
	}
	catch (...) {
		return false;
	}
	return true;
}

bool CIRATools::setDBValue(maci::ContainerServices* Services,CString fieldName,const DWORD &Val,CString Domain,CString name)
{
	CString fullName;
	if (name=="") fullName=Domain+Services->getName();
	else fullName=Domain+name;
	try {
		CDB::DAL_var Dal_p=Services->getCDB();
		CDB::WDAL_var wDal_p=CDB::WDAL::_narrow(Dal_p);
		CDB::WDAO_var wDao_p=wDal_p->get_WDAO_Servant((const char *)fullName);
		wDao_p->set_long((const char *)fieldName,Val);
	}
	catch (...) {
		return false;
	}
	return true;
}

bool CIRATools::setDBValue(maci::ContainerServices* Services,CString fieldName,const CString &Val,CString Domain,CString name)
{
	CString fullName;
	if (name=="") fullName=Domain+Services->getName();
	else fullName=Domain+name;
	try {
		CDB::DAL_var Dal_p=Services->getCDB();
		CDB::WDAL_var wDal_p=CDB::WDAL::_narrow(Dal_p);
		CDB::WDAO_var wDao_p=wDal_p->get_WDAO_Servant((const char *)fullName);
		wDao_p->set_string((const char *)fieldName,(const char*)Val);
	}
	catch (...) {
		return false;
	}
	return true;
}

double CIRATools::getHWAzimuth(const double& current,const double& dest,const double& minAz,const double& maxAz,const long& section,const double& cwLimit)
{
	//section == 0 : NEUTRAL
	//section > 0  : CW
	//section < 0  : CCW
	double tmpDest,tmpCurrent, tmpMinAz, tmpMaxAz;
	double d1,d2;
	double newOne;
	//makes sure the destination is in the range 0..360
	tmpDest=fmod(dest,360.0);
	if (tmpDest<0.0) tmpDest+=360.0;
	else if (tmpDest>360.0) tmpDest-=360.0;
	// makes sure the limits are a least 0..360 otherwise there may be angle that cannot be converted.
	if (minAz>0.0) tmpMinAz=0;
	else tmpMinAz=minAz;
	if (maxAz<360.0) tmpMaxAz=360.0;
	else tmpMaxAz=maxAz;
	// makes sure current position is inside the limits.....
	if (current<tmpMinAz) tmpCurrent=minAz;
	else if (current>tmpMaxAz) tmpCurrent=maxAz;
	else tmpCurrent=current;
	if (section==0) { //NEUTRAL
		if (tmpCurrent<=cwLimit) {
			d1=tmpDest-tmpCurrent;
			d2=360.0-d1;
		}
		else {
			d2=tmpCurrent-tmpDest;
			d1=360.0-d2;
		}
		if (d1<=d2) {
			newOne=tmpCurrent+d1;
			if ((newOne>tmpMaxAz) || (newOne<tmpMinAz)) newOne=tmpCurrent-d2;
		}
		else {
			newOne=tmpCurrent-d2;
			if ((newOne>tmpMaxAz) || (newOne<tmpMinAz)) newOne=tmpCurrent+d1;
		}
	}
	else if (section>0) { //CW (typically 180...450
			if (tmpDest>cwLimit) {
				newOne=tmpDest;
			}
			else if (tmpDest<=fmod(tmpMaxAz,360)) {
				newOne=tmpDest+360.0;
			}
			else {  // not reachable in this sector (typically 90..180)
				newOne=tmpDest;
			}
	}
	else { //(section==CCW) { typically -90...180
			double lower=fmod(tmpMinAz,360);
			lower+=360.0;
			if (tmpDest<=cwLimit) {
				newOne=tmpDest;
			}
			else if (tmpDest>lower) {
				newOne=tmpDest-360.0;
			}
			else {  // not reachable in this sector (typically 180..270
				newOne=tmpDest;
			}
	}
	return newOne;
}

bool CIRATools::calculateIFlimits(double LO_freq, 
                       double RF_min, double RF_max, 
                       long injection,
                       double& out_IF_min, double& out_IF_max,
                       double& out_start_freq, double& out_bandwidth)
{
    // Basic input validation: Min RF cannot be greater than Max RF
    if (RF_min>RF_max) return false;
    if (injection == 1) { //LSI
        out_IF_min = RF_min - LO_freq;
        out_IF_max = RF_max - LO_freq;
    } 
    else if (injection == -1) { //HSI
        // We calculate Min IF using Max RF.
        out_IF_min = LO_freq - RF_max;
        // We calculate Max IF using Min RF.
        out_IF_max = LO_freq - RF_min;
    } 
    else {
        // Invalid injection parameter provided
        return false;
    }
    // Take the absolute value to handle physical frequencies.
    // This manages cases where the user specifies a configuration 
    // that might cross DC (0 Hz) or if the LO/RF relationship 
    // differs from the specified injection type.
    out_IF_min = std::abs(out_IF_min);
    out_IF_max = std::abs(out_IF_max);

    // Ensure that output Min is strictly less than Output Max.
    // If negative frequencies were involved and abs() was applied, 
    // the order might need to be swapped.
    if (out_IF_min > out_IF_max) {
        std::swap(out_IF_min, out_IF_max);
    }
    // --- STEP 3: Compute Start Frequency and Bandwidth ---
    // The "Start Frequency" of a band is physically the lower limit.
    if (injection == -1) {
    	out_start_freq = - out_IF_min;
    }
    else {
    	out_start_freq = out_IF_min;
    }
    // Bandwidth is the span between the upper and lower limits.
    out_bandwidth = out_IF_max - out_IF_min; 
    return true;
}

DoubleConversionResult CIRATools::calculateDualConversion(double current_ol1, double current_ol2,
                                             double if2_min, double if2_max,
                                             double if1_min, double if1_max,
                                             long LO1_inj, long LO2_inj)
{
    DoubleConversionResult res;
    res.valid = true;
    res.error_msg = "OK";

    // --- A. Setup Parametri IF2 ---
    res.if2_min_freq = if2_min;
    res.if2_max_freq = if2_max;
    res.if2_bandwidth = if2_max - if2_min;

    // --- B. Calcolo Inversione e OL Equivalente ---
    int high_side_count = 0;
    if (LO1_inj == -1) high_side_count++;
    if (LO2_inj == -1) high_side_count++;
    
    res.spectrum_inverted = (high_side_count % 2 != 0);

    if (LO1_inj == 1) {
        res.eq_OL = current_ol1 + current_ol2;
    } else {
        res.eq_OL = current_ol1 - current_ol2;
    }

    // Gestione Zero Crossing / Frequenze Negative
    //bool eq_inverted_flag = false; // Flag locale per correggere la logica se ol_eq < 0
    if (res.eq_OL < 0) {
        res.eq_OL = std::abs(res.eq_OL);
        //eq_inverted_flag = true; 
        // Se l'OL eq cambia segno, l'inversione spettrale si inverte
        res.spectrum_inverted = !res.spectrum_inverted;
    }

    // --- C. Calcolo Frequenze RF ---
    // Formula base: IF2 = |RF - OL_eq| (con varianti di segno)
    // Ricaviamo RF da IF2:
    
    if (!res.spectrum_inverted) {
        // Spettro Dritto: RF = OL_eq + IF2
        res.rf_min_freq = res.eq_OL + if2_min;
        res.rf_max_freq = res.eq_OL + if2_max;
    } else {
        // Spettro Invertito: RF = OL_eq - IF2
        res.rf_min_freq = res.eq_OL - if2_max;
        res.rf_max_freq = res.eq_OL - if2_min;
        res.if2_min_freq= - res.if2_min_freq;
    }
    
    res.rf_center_freq = (res.rf_min_freq + res.rf_max_freq) / 2.0;
  
    double if1_range_start, if1_range_end;

    if (LO2_inj == 1) {
        // LSI (2° stadio): IF2 = IF1 - OL2  =>  IF1 = IF2 + OL2
        // Relazione lineare diretta (non inverte l'ordine locale)
        if1_range_start = if2_min + current_ol2;
        if1_range_end   = if2_max + current_ol2;
    } else {
        // HSI (2° stadio): IF2 = OL2 - IF1  =>  IF1 = OL2 - IF2
        // Relazione inversa: IF2_max genera IF1_min
        if1_range_start = current_ol2 - if2_max;
        if1_range_end   = current_ol2 - if2_min;
    }

    res.if1_min_generated = if1_range_start;
    res.if1_max_generated = if1_range_end;

    // --- E. Validazione Finale ---
    
    // 1. Controllo validità fisica IF1 (non può essere negativa)
    if (res.if1_min_generated < 0) {
        res.valid = false;
        res.error_msg = "First down conversion (IF1) is negative (impossible configuration)";
    }
    // 2. Controllo Limiti Filtro IF1 (Il segnale passa?)
    // Verifichiamo se l'INTERO range generato passa nel filtro
    else if (res.if1_min_generated < if1_min || res.if1_max_generated > if1_max) {
        res.valid = false;
        res.error_msg = "First conversion is outside the limits of the IF1 filter";
    }
    // 3. Controllo RF (non negativa)
    else if (res.rf_min_freq < 0) {
        res.valid = false;
        res.error_msg = "Resulting RF is negative";
    }
    return res;
}

DoubleConversionAnalysis CIRATools::analyzeDualConversion(double RF_min, double RF_max,
                                             double min_ol1, double max_ol1,
                                             double min_if1, double max_if1,
                                             double min_ol2, double max_ol2,
                                             double min_if2, double max_if2,
                                             long LO1_inj, long LO2_inj)
{
    DoubleConversionAnalysis res;
    res.valid_configuration = true;
    res.limited_by_IF1 = false;

    // 1. Calcolo del Range dell'OL Equivalente (Fisico dei Sintetizzatori)
    int high_side_count = 0;
    if (LO1_inj == -1) high_side_count++;
    if (LO2_inj == -1) high_side_count++;
    res.spectrum_inverted = (high_side_count % 2 != 0);

    if (LO1_inj == 1) { // LSI
        res.min_OL_eq = min_ol1 + min_ol2;
        res.max_OL_eq = max_ol1 + max_ol2;
    } else { // HSI
        res.min_OL_eq = min_ol1 - max_ol2; 
        res.max_OL_eq = max_ol1 - min_ol2; 
    }

    // 2. Calcolo Banda RF Teorica
    double theoretical_RF_min, theoretical_RF_max;
    if (!res.spectrum_inverted) {
        theoretical_RF_min = res.min_OL_eq + min_if2;
        theoretical_RF_max = res.max_OL_eq + max_if2;
    } else {
        theoretical_RF_min = res.min_OL_eq - max_if2; 
        theoretical_RF_max = res.max_OL_eq - min_if2;
    }

    // 3. Verifica IF1
    double stage1_RF_min, stage1_RF_max;
    if (LO1_inj == 1) { 
        stage1_RF_min = min_ol1 + min_if1;
        stage1_RF_max = max_ol1 + max_if1;
    } else { 
        stage1_RF_min = std::abs(min_ol1 - max_if1);
        stage1_RF_max = std::abs(max_ol1 - min_if1);
        if(stage1_RF_min > stage1_RF_max) std::swap(stage1_RF_min, stage1_RF_max);
    }

    // 4. Intersezione Finale (RF Utile)
    double final_min = std::max({RF_min, theoretical_RF_min, stage1_RF_min});
    double final_max = std::min({RF_max, theoretical_RF_max, stage1_RF_max});

    if (final_min > std::max(RF_min, theoretical_RF_min) || 
        final_max < std::min(RF_max, theoretical_RF_max)) {
        res.limited_by_IF1 = true;
    }

    if (final_min > final_max) {
        res.valid_configuration = false;
        res.min_RF_converted = 0; res.max_RF_converted = 0;
        res.min_OL_eq = 0; res.max_OL_eq = 0; // Reset anche OL
        return res;
    } 
    
    res.min_RF_converted = final_min;
    res.max_RF_converted = final_max;

    // --- FASE 5: RICALCOLO LIMITI OL (La correzione al tuo problema) ---
    // Ritagliamo il range OL affinché corrisponda solo alla zona dove RF Hardware e RF Teorica si sovrappongono.
    
    double useful_OL_min, useful_OL_max;

    // Calcoliamo quale range di OL è NECESSARIO per coprire i limiti hardware RF_min e RF_max
    // Consideriamo valido un OL se ALMENO UNA PARTE della sua banda IF cade nel range RF Hardware.
    
    if (!res.spectrum_inverted) {
        // Diritto: RF = OL + IF  ->  OL = RF - IF
        // Per toccare RF_min (dal basso), l'OL deve essere almeno RF_min - IF_max
        useful_OL_min = RF_min - max_if2;
        // Per toccare RF_max (dall'alto), l'OL deve essere al massimo RF_max - min_if2
        useful_OL_max = RF_max - min_if2;
    } else {
        // Invertito: RF = OL - IF  ->  OL = RF + IF
        // Per toccare RF_min (che corrisponde all'IF alta), OL >= RF_min + IF_min
        useful_OL_min = RF_min + min_if2;
        // Per toccare RF_max (che corrisponde all'IF bassa), OL <= RF_max + max_if2
        useful_OL_max = RF_max + max_if2;
    }

    // Ora intersechiamo questo range "Utile" con quello "Fisico" del sintetizzatore
    res.min_OL_eq = std::max(res.min_OL_eq, useful_OL_min);
    res.max_OL_eq = std::min(res.max_OL_eq, useful_OL_max);

    // Controllo finale di validità su OL
    if (res.min_OL_eq > res.max_OL_eq) {
        res.valid_configuration = false;
    }

    return res;
}

bool CIRATools::skyFrequency(const double& bf,const double& bbw,const double& rf,const double& rbw,double lo,
	double& iff,double& ifbw, double& RF1, double& RF2)
{
	bool inverted=false;	
	if ((bf*rf)<0) inverted=true; //this is a trick to check if the combination of the two bands gives an inversion or not
	double b1=std::abs(bf);	
	double b2=b1+bbw;
	double r1=std::abs(rf);
	double r2=r1+rbw;
	iff=MAX(b1,r1);
	double if2=MIN(b2,r2);
	ifbw=if2-iff;
	if (ifbw<=0) {
		iff=b1;
		ifbw=0;
		return false;
	}
	if (inverted) {
		iff=-iff; // change the sign to respect the assumption of the input parameters (negative means inverted band)
		RF1=lo+iff;
		RF2=RF1-ifbw;
	}
	else {
		RF1=lo+iff;
		RF2=RF1+ifbw;
	}
	return true;
}

bool CIRATools::skyFrequency(const double& bf,const double& bbw,const double& rf,const double& rbw,double& f,double& bw)
{
	bool bside,rside;
	double rf1,rf2,bf1,bf2;
	if (!bandLimits(bf,bbw,bf1,bf2,bside)) return false;
	if (!bandLimits(rf,rbw,rf1,rf2,rside)) return false;
	if (!mergeBands(rf1,rf2,rside,bf1,bf2,bside,f,bw)) return false;
	return true;
	/*double bs=bf+bbw;
	double rs=rf+rbw;
	f=MAX(bf,rf);
	double s=MIN(bs,rs);
	bw=s-f;
	if (bw<=0) {
		f=bf;
		bw=0;
		return false;
	}
	else {
		return true;
	}*/
}

bool CIRATools::getNextToken(const IRA::CString& str,int &start,const char& delimiter,IRA::CString &ret, const bool& stopAtFirst)
{
		return getNextToken(str, start, std::vector<char>{ delimiter }, ret, stopAtFirst);
}

bool CIRATools::getNextToken(const IRA::CString& str,int &start,const std::vector<char>& delimiters,IRA::CString &ret, const bool& stopAtFirst)
{
		int i;
		bool ok=false;
		char *token;
		i=str.GetLength();
		token=new char[i+1];
		i=0;
		while (str[start]!=0) {
			char c = str[start];
			ok=true;
			start++;
			if (std::any_of(delimiters.begin(), delimiters.end(), [c](char d){ return c == d; })) {
				// We still have not found any meaningful character, stop if stopAtFirst or continue skipping a delimiter
				if(!stopAtFirst && i==0)
					continue;
				break;
			}
			else {
				token[i]=c;
				i++;
			}
		}
		token[i]=0;
		ret=IRA::CString(token);
		delete []token;
		return ok;
}

double CIRATools::differenceBetweenAnglesRad(const double& a,const double& b)
{
	double diff=a-b;
	while(diff<-DPI) diff+=D2PI;
	while (diff>DPI) diff-=D2PI;
	return diff;
}

double CIRATools::differenceBetweenAnglesDeg(const double& a,const double& b)
{
	double diff=a-b;
	while(diff<-180.0) diff+=360.0;
	while (diff>180.0) diff-=360.0;
	return diff;
}

double CIRATools::latRangeRad(const double& lat)
{
	/*double res=lat;
	while (res<-DPIBY2) res+=DPIBY2;
	while (res>DPIBY2) res-=DPIBY2;
	return res;*/
	/*double w;
	w= slaDrange(lat);
	if (w<-DPIBY2) return -(DPI+w);
	else if (w>DPIBY2) return DPI-w;
	else return w;*/
	if (lat>DPIBY2) return DPIBY2;
	else if (lat<-DPIBY2) return -DPIBY2;
	else return lat;
}

double CIRATools::latRangeDeg(const double& lat)
{
	/*double w;
	w=slaDrange(lat*DD2R)*DR2D;
	if (w<-90.0) return -(180.0+w);
	else if (w>90.0) return 180.0-w;
	else return w;*/
	/*double res=lat;
	while (res<-90.0)  res+=90;
	while (res>90.0)  res-=90;
	return res;*/
	if (lat>90.0) return 90.0;
	else if (lat<-90.0) return -90.0;
	else return lat;
}

bool CIRATools::strToInterval(const IRA::CString& durationString,ACS::TimeInterval& interval,bool complete,char dateDelimiter,char timeDelimiter)
{
	int p,i=0,dCounter=0;
	long hour=0,minute=0,sec=0,usec=0;
	double second=0.0;
	IRA::CString timeToken[3],dayToken;
	bool day;
	long dayVal;
	TIMEDIFFERENCE period;
	interval=0;
	if (durationString.GetLength()==0) return false;
	// look if there is the day indication......
	p=durationString.Find(dateDelimiter,i);
	if (p>0) { dayToken=durationString.Mid(i,p-i); day=true; i=p+1; }
	else day=false;
	while (((p=durationString.Find(timeDelimiter,i))>0) && (dCounter<2)) {
		timeToken[dCounter]=durationString.Mid(i,p-i);
		dCounter++;
		i=p+1;
	}
	timeToken[dCounter]=durationString.Mid(i,durationString.GetLength()-i);
	dCounter++;
	if ((dCounter<3) && (complete)) return false;
	if (dCounter==1) {
		if (day) {
			hour=timeToken[0].ToLong();
			minute=0;
			second=0;
		}
		else {
			hour=0;
			minute=0;
			second=timeToken[0].ToDouble();
		}
	}
	else if (dCounter==2) {
		if (day) {
			hour=timeToken[0].ToLong();
			minute=timeToken[1].ToLong();
			second=0;
		}
		else {
			hour=0;
			minute=timeToken[0].ToLong();
			second=timeToken[1].ToDouble();
		}
	}
	else if (dCounter==3) {
		hour=timeToken[0].ToLong();
		minute=timeToken[1].ToLong();
		second=timeToken[2].ToDouble();
	}
	if ((hour<0) || (hour>23)) return false;
	if ((minute<0) || (minute>59)) return false;
	if ((second<0.0) || (second>=60.0)) return false;
	period.normalize(true);
	if (day) {
		dayVal=dayToken.ToLong();
		if (dayVal<0) return false;
		period.day(dayVal);
	}
	else {
		period.day(0);
	}
	sec=(long)floor(second);
	second=second-(double)sec;
	usec=(long)(second*1000000.0);
	period.hour(hour); period.minute(minute); period.second(sec); period.microSecond(usec);
	interval=period.value().value;
	return true;
}

bool CIRATools::intervalToStr(const ACS::TimeInterval& interval,IRA::CString& outString,char dateDelimiter,char timeDelimiter)
{
	TIMEDIFFERENCE timeD(interval);
	if (timeD.day()>0) {
		outString.Format("%03ld%c%02ld%c%02ld%c%02ld.%03lu",(long)timeD.day(),dateDelimiter,(long)timeD.hour(),timeDelimiter,(long)timeD.minute(),timeDelimiter,(long)timeD.second(),(unsigned long)timeD.microSecond()/1000);
	}
	else {
		outString.Format("%02ld%c%02ld%c%02ld.%03lu",(long)timeD.hour(),timeDelimiter,(long)timeD.minute(),timeDelimiter,(long)timeD.second(),(unsigned long)timeD.microSecond()/1000);
	}
	return true;
}

bool CIRATools::strToTime(const IRA::CString& timeString,ACS::Time& time,bool complete,char dateDelimiter,char timeDelimiter)
{
	long year=0,doy=0,hour=0,minute=0,sec=0,usec=0;
	double second=0.0;
	IRA::CString token,dateToken[2];
	TIMEVALUE now,epoch;
	IRA::CIRATools::getTime(now);
	int p,i=0,dCounter=0;
	IRA::CDateTime dt;

	while (((p=timeString.Find(dateDelimiter,i))>0) && (dCounter<2)) {
		dateToken[dCounter]=timeString.Mid(i,p-i);
		dCounter++;
		i=p+1;
	}
	if ((dCounter==0) && (complete)) return false;
	if (dCounter==0) {
		year=now.year();
		doy=now.dayOfYear();
	}
	else if (dCounter==1) {
		doy=dateToken[0].ToLong();
		year=now.year();
	}
	else {
		year=dateToken[0].ToLong();
		doy=dateToken[1].ToLong();
	}
	if (getNextToken(timeString,i,timeDelimiter,token)) {  //now read all three fields of the time representation
		hour=token.ToLong();
		if (getNextToken(timeString,i,timeDelimiter,token)) {
			minute=token.ToLong();
			if (getNextToken(timeString,i,timeDelimiter,token)) {
				second=token.ToDouble();
			}
			else {
				if (complete) return false;
				second=0.0;
			}
		}
		else {
			if (complete) return false;
			minute=0;
			second=0.0;
		}
	}
	else {
		return false;
	}
	// now check if the results are consistent
	if (year<0) return false;
	if (dt.isLeapYear(year)) {
		if ((doy<0) || (doy>366)) return false;
	}
	else {
		if ((doy<0) || (doy>365)) return false;
	}
	if ((hour<0) || (hour>23)) return false;
	if ((minute<0) || (minute>59)) return false;
	if ((second<0.0) || (second>=60)) return false;
	sec=(long)floor(second);
	second=second-(double)sec;
	usec=(long)(second*1000000.0);
	epoch.normalize(true);
	epoch.year(year); epoch.dayOfYear(doy); epoch.hour(hour); epoch.minute(minute);
	epoch.second(sec); epoch.microSecond(usec);
	time=epoch.value().value;
	return true;
}

bool CIRATools::timeToStr(const ACS::Time& time,IRA::CString& outString,char dateDelimiter,char timeDelimiter)
{
	TIMEVALUE timeE(time);
	outString.Format("%04lu%c%03ld%c%02ld%c%02ld%c%02ld.%03ld",(unsigned long)timeE.year(),dateDelimiter,(long)timeE.dayOfYear(),dateDelimiter,(long)timeE.hour(),
			timeDelimiter,(long)timeE.minute(),timeDelimiter,(long)timeE.second(),(long)timeE.microSecond()/1000);
	return true;
}

bool CIRATools::timeToStrExtended(const ACS::Time& time,IRA::CString& outString,char dateDelimiter,char timeDelimiter)
{
	TIMEVALUE timeE(time);
	outString.Format("%04lu%c%02ld%c%02ldT%02ld%c%02ld%c%02ld.%03ld",
			(unsigned long)timeE.year(),dateDelimiter,
			(long)timeE.month(),dateDelimiter,
			(long)timeE.day(),
			(long)timeE.hour(),timeDelimiter,
			(long)timeE.minute(),timeDelimiter,
			(long)timeE.second(),
			(long)timeE.microSecond()/1000);
	return true;
}

bool CIRATools::doubleSeqToStr(const ACS::doubleSeq& val,IRA::CString& outString,char delimiter)
{
	IRA::CString single;
	unsigned long len=val.length();
	outString="";
	for (unsigned i=0;i<len;i++) {
		single.Format("%lf",val[i]);
		outString+=single;
		if (i<len-1) outString+=delimiter;
	}
	return true;
}

bool CIRATools::longSeqToStr(const ACS::longSeq& val,IRA::CString& outString,char delimiter)
{
	IRA::CString single;
	unsigned long len=val.length();
	outString="";
	for (unsigned i=0;i<len;i++) {
		single.Format("%d",val[i]);
		outString+=single;
		if (i<len-1) outString+=delimiter;
	}
	return true;
}

bool CIRATools::hourAngleToRad(const IRA::CString& angle,double& rad,bool complete,char delimiter)
{
	int p,i=0,dCounter=0;
	long hour=0,minute=0;
	double second=0.0;
	bool sign;
	IRA::CString token[3];
	rad=0;
	if (angle.GetLength()==0) return false;
	while (((p=angle.Find(delimiter,i))>0) && (dCounter<2)) {
		token[dCounter]=angle.Mid(i,p-i);
		dCounter++;
		i=p+1;
	}
	token[dCounter]=angle.Mid(i,angle.GetLength()-i);
	dCounter++;
	if ((dCounter!=3) && (complete)) return false;
	if (dCounter==1) {
		hour=token[0].ToLong();
		minute=0;
		second=0.0;
	}
	else if (dCounter==2) {
		hour=token[0].ToLong();
		minute=token[1].ToLong();
		second=0.0;
	}
	else if (dCounter==3) {
		hour=token[0].ToLong();
		minute=token[1].ToLong();
		second=token[2].ToDouble();
	}
	if (token[0][0]=='-') {
		sign=true;
		hour=-hour;
	}
	else {
		sign=false;
	}
	if ((minute<0) || (minute>59)) return false;
	if ((second<0.0) || (second>=60.0)) return false;
	rad=hour*3600.0+minute*60.0+second;
	rad*=DS2R;  //convert seconds of time to radians
	rad=slaDranrm(rad);
	if (sign) {
		rad*=-1;
	}
	return true;
}

bool CIRATools::radToHourAngle(const double& rad,IRA::CString& outString,char delimiter)
{
	char sign;
	int hmsf[4];
	double ranged=dmod(rad,D2PI); // put the angle in the range -2PI..2PI
	slaDr2tf(3,ranged,&sign,hmsf); // convert it into hour minute second and fraction
	if (sign=='+') {
		outString.Format("%02d%c%02d%c%02d.%03d",hmsf[0],delimiter,hmsf[1],delimiter,hmsf[2],hmsf[3]);
	}
	else {
		outString.Format("%c%02d%c%02d%c%02d.%03d",sign,hmsf[0],delimiter,hmsf[1],delimiter,hmsf[2],hmsf[3]);
	}
	return true;
}

bool CIRATools::sexagesimalAngleToRad(const IRA::CString& angle,double& rad,bool complete,char delimiter)
{
	int p,i=0,dCounter=0;
	long deg=0,arcminute=0;
	double arcsecond=0.0;
	bool sign;
	IRA::CString token[3];
	rad=0;
	if (angle.GetLength()==0) return false;
	while (((p=angle.Find(delimiter,i))>0) && (dCounter<2)) {
		token[dCounter]=angle.Mid(i,p-i);
		dCounter++;
		i=p+1;
	}
	token[dCounter]=angle.Mid(i,angle.GetLength()-i);
	dCounter++;
	if ((dCounter!=3) && (complete)) return false;
	if (dCounter==1) {
		deg=token[0].ToLong();
		arcminute=0;
		arcsecond=0.0;
	}
	else if (dCounter==2) {
		deg=token[0].ToLong();
		arcminute=token[1].ToLong();;
		arcsecond=0.0;
	}
	else if (dCounter==3) {
		deg=token[0].ToLong();
		arcminute=token[1].ToLong();
		arcsecond=token[2].ToDouble();
	}
	//degrees could also be negative......
	if (token[0][0]=='-') {
		sign=true;
		deg*=-1;
	}
	else {
		sign=false;
	}
	if ((arcminute<0) || (arcminute>59)) return false;
	if ((arcsecond<0.0) || (arcsecond>=60.0)) return false;
	rad=DAS2R*(60.0*(60.0*(double)deg+(double)arcminute)+arcsecond);
	rad=slaDranrm(rad);
	if (sign) {
		rad*=-1;
	}
	return true;
}

bool CIRATools::radToSexagesimalAngle(const double& rad,IRA::CString& outString,char delimiter)
{
	char sign;
	int dmsf[4];
	double ranged=dmod(rad,D2PI); // put the angle in the range -2PI..2PI
	// convert it into hour minute second anf fraction..keeping track of the sign
	slaDr2af(3,ranged,&sign,dmsf);
	if (sign=='+') {
		outString.Format("%02d%c%02d%c%02d.%03d",dmsf[0],delimiter,dmsf[1],delimiter,dmsf[2],dmsf[3]);
	}
	else {
		outString.Format("%c%02d%c%02d%c%02d.%03d",sign,dmsf[0],delimiter,dmsf[1],delimiter,dmsf[2],dmsf[3]);
	}
	return true;
}

bool CIRATools::angleToRad(const IRA::CString& angle,double& rad)
{
	double deg=angle.ToDouble();
	rad=deg*DD2R;
	rad=dmod(rad,D2PI);
	return true;
}

bool CIRATools::radToAngle(const double& rad,IRA::CString& outString)
{
	double deg;
	deg=dmod(rad,D2PI);
	deg*=DR2D;
	outString.Format("%08.4lf",deg);
	return true;
}

bool CIRATools::longitudeToRad(const IRA::CString& lon,double& rad,bool complete,char delimiter)
{
	//long len=lon.GetLength();
	int len=lon.GetLength();
	bool res;
	if (len==0) return false;
	if (lon[len-1]=='d') {
		IRA::CString val=lon.Left(len-1);
		res=angleToRad(val,rad);
	}
	else {
		res=sexagesimalAngleToRad(lon,rad,complete,delimiter);
	}
	if (complete) {
		if ((rad<-DPI) || (rad>DPI)) return false;
		else return res;
	}
	else return res;
}


bool CIRATools::latitudeToRad(const IRA::CString& lat,double& rad,bool complete,char delimiter)
{
	//long len=lat.GetLength();
	int len=lat.GetLength();
	bool res;
	if (len==0) return false;
	if (lat[len-1]=='d') {
		IRA::CString val=lat.Left(len-1);
		res=angleToRad(val,rad);
	}
	else {
		res=sexagesimalAngleToRad(lat,rad,complete,delimiter);
	}
	if (complete) {
		if (rad<-DPIBY2 || rad>DPIBY2) return false;
		else return res;
	}
	else return res;
}

bool CIRATools::rightAscensionToRad(const IRA::CString& ra,double& rad,bool complete,char delimiter)
{
	//long len=ra.GetLength();
	int len=ra.GetLength();
	bool res;
	if (len==0) return false;
	if (ra[len-1]=='d') {
		IRA::CString val=ra.Left(len-1);
		res=angleToRad(val,rad);
	}
	else if (ra[len-1]=='h') {
		IRA::CString val=ra.Left(len-1);
		res=hourAngleToRad(val,rad,complete,delimiter);
	}
	else {
		res=sexagesimalAngleToRad(ra,rad,complete,delimiter);
	}
	if (complete) {
		if ((rad<0) || (rad>(D2PI))) return false;
		else return res;
	}
	else return res;
}

bool CIRATools::declinationToRad(const IRA::CString& dec,double& rad,bool complete,char delimiter)
{
	return latitudeToRad(dec,rad,complete,delimiter);
}

bool CIRATools::galLongitudeToRad(const IRA::CString& lon,double& rad,bool complete,char delimiter)
{
	//long len=lon.GetLength();
	int len=lon.GetLength();
	bool res;
	if (len==0) return false;
	if (lon[len-1]=='d') {
		IRA::CString val=lon.Left(len-1);
		res=angleToRad(val,rad);
	}
	else {
		res=sexagesimalAngleToRad(lon,rad,complete,delimiter);
	}
	if (complete) {
		if ((rad<0) || (rad>D2PI)) return false;
		else return res;
	}
	else return res;
}

bool CIRATools::galLatitudeToRad(const IRA::CString& lat,double& rad,bool complete,char delimiter)
{
	return latitudeToRad(lat,rad,complete,delimiter);
}

bool CIRATools::azimuthToRad(const IRA::CString& az,double& rad,bool complete)
{
	bool res=angleToRad(az,rad);
	if (complete) {
		if ((rad<0) || (rad>D2PI)) return false;
		else return res;
	}
	else return res;
}

bool CIRATools::elevationToRad(const IRA::CString& el,double& rad,bool complete)
{
	bool res=angleToRad(el,rad);
	if (complete) {
		if (rad<0 || rad>DPIBY2) return false;
		else return res;
	}
	else return res;
}

bool CIRATools::offsetToRad(const IRA::CString& offset,double& rad,char delimiter)
{
	//long len=offset.GetLength();
	int len=offset.GetLength();
	bool res;
	if (len==0) return false;
	if (offset[len-1]=='d') {
		IRA::CString val=offset.Left(len-1);
		res=angleToRad(val,rad);
	}
	else {
		res=sexagesimalAngleToRad(offset,rad,false,delimiter);
	}
	return res;
}

bool CIRATools::coordinateFrameToStr(const Antenna::TCoordinateFrame& frame,IRA::CString& str)
{
	if (frame==Antenna::ANT_EQUATORIAL) {
		str="EQ";
	}
	else if (frame==Antenna::ANT_GALACTIC) {
		str="GAL";
	}
	else {
		str="HOR";
	}
	return true;
}


bool CIRATools::strToCoordinateFrame(const IRA::CString& val,Antenna::TCoordinateFrame& frame)
{
	IRA::CString strVal(val);
	strVal.MakeUpper();
	if (strVal=="EQ") {
		frame=Antenna::ANT_EQUATORIAL;
	}
	else if (strVal=="GAL") {
		frame=Antenna::ANT_GALACTIC;
	}
	else if (strVal=="HOR") {
		frame=Antenna::ANT_HORIZONTAL;
	}
	else {
		return false;
	}
	return true;
}

bool CIRATools::strToReferenceFrame(const IRA::CString& val,Antenna::TReferenceFrame& frame)
{
	// ANT_UNDEF_FRAME is not considered to be  to a legal value for reference frame.
	// reported into antenna.xsd: TOPOCEN,BARY,LSRK,LSRD,GALCEN,LGRP,UNDEF"
	IRA::CString strVal(val);
	strVal.MakeUpper();
	if (strVal=="BARY") {
		frame=Antenna::ANT_BARY;
	}
	else if (strVal=="LSRK") {
		frame=Antenna::ANT_LSRK;
	}
	else if (strVal=="LSRD") {
		frame=Antenna::ANT_LSRD;
	}
	else if (strVal=="LGRP") {
		frame=Antenna::ANT_LGROUP;
	}
	else if (strVal=="GALCEN") {
		frame=Antenna::ANT_GALCEN;
	}
	else if (strVal=="TOPOCEN") {
		frame=Antenna::ANT_TOPOCEN;
	}
	else {
		return false;
	}
	return true;
}

bool CIRATools::referenceFrameToStr(const Antenna::TReferenceFrame& frame,IRA::CString& str)
{
	if (frame==Antenna::ANT_BARY) {
		str="BARY";
	}
	else if (frame==Antenna::ANT_LSRK) {
		str="LSRK";
	}
	else if (frame==Antenna::ANT_LSRD) {
		str="LSRD";
	}
	else if (frame==Antenna::ANT_LGROUP) {
		str="LGRP";
	}
	else if (frame==Antenna::ANT_GALCEN) {
		str="GALCEN";
	}
	else if (frame==Antenna::ANT_TOPOCEN) {
		str="TOPOCEN";
	}
	else {
		str="UNDEF";
	}
	return true;
}

bool CIRATools::strToVradDefinition(const IRA::CString& val,Antenna::TVradDefinition& frame)
{
	// ANT_UNDEF_DEF   is not considered to be  to a legal value for velocity definition
	// reported into antenna.xsd: RD,OP,Z,UNDEF
	IRA::CString strVal(val);
	strVal.MakeUpper();
	if (strVal=="RD") {
		frame=Antenna::ANT_RADIO;
	}
	else if (strVal=="OP") {
		frame=Antenna::ANT_OPTICAL;
	}
	else if (strVal=="Z") {
		frame=Antenna::ANT_REDSHIFT;
	}
	else {
		return false;
	}
	return true;
}

bool CIRATools::vradDefinitionToStr(const Antenna::TVradDefinition& frame,IRA::CString& str)
{
	if (frame==Antenna::ANT_RADIO) {
		str="RD";
	}
	else if (frame==Antenna::ANT_OPTICAL) {
		str="OP";
	}
	else if (frame==Antenna::ANT_REDSHIFT) {
		str="Z";
	}
	else {
		str="UNDEF";
	}
	return true;
}

bool CIRATools::equinoxToStr(const Antenna::TSystemEquinox& ep,IRA::CString& str)
{
	if (ep==Antenna::ANT_B1950) {
		str="B1950";
	}
	else if (ep==Antenna::ANT_J2000) {
		str="J2000";
	}
	else {
		str="APPARENT";
	}
	return true;
}

bool CIRATools::strToEquinox(const IRA::CString& str,Antenna::TSystemEquinox& ep)
{
	IRA::CString strVal(str);
	strVal.MakeUpper();
	if ((strVal=="1950") || (strVal=="1950.0") || (strVal=="B1950")) {
		ep=Antenna::ANT_B1950;
	}
	else if ((strVal=="2000") || (strVal=="2000.0") || (strVal=="J2000")) {
		ep=Antenna::ANT_J2000;
	}
	else if ((strVal=="APPARENT") || (strVal=="-1") ) {
		ep=Antenna::ANT_APPARENT;
	}
	else {
		return false;
	}
	return true;
}

bool CIRATools::makeDirectory(const IRA::CString& pathName)
{
	if (mkdir((const char *)pathName,0777) < 0) {
		if (errno==ENOENT) {
			int slash=pathName.RFind('/'); // look for the trailing slash
			if (slash!=-1) {  //
				IRA::CString subPath=pathName.Left(slash);
				if (!makeDirectory(subPath)) return false;
				if (mkdir((const char *)pathName,0777)<0) {
					return false;
				}
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}
	return true;
}

bool CIRATools::directoryExists(const IRA::CString& path)
{
	DIR *dir;
	bool exists=false;
	dir=opendir((const char *)path);
	if (dir!=NULL) {
		exists=true;
		closedir(dir);
	}
	return exists;
}

bool CIRATools::fileExists(const IRA::CString& file)
{
	struct stat buffer;
	return (stat((const char *)file,&buffer)==0);
}

bool CIRATools::createEmptyFile(const IRA::CString& file)
{
	bool ret;
	std::fstream fs;
	fs.open((const char *)file,ios::out);
	if (fs.fail()) ret=false;
	else ret=true;
	fs.close();
	return ret;
}

bool CIRATools::deleteFile(const IRA::CString& file)
{
	if(remove((const char *)file)==0) {
		return true;
	}
	else {
		return false;
	}
}

bool CIRATools::copyFile(const IRA::CString& src,const IRA::CString& dst)
{
	std::ifstream in ((const char *)src);
	if (in.fail()) return false;
	std::ofstream out ((const char *)dst);
	if (out.fail()) return false;
	out << in.rdbuf();
	out.close();
	in.close();
	return true;
}

bool CIRATools::extractFileName(const IRA::CString& fullPath,IRA::CString& baseDir,IRA::CString& baseName,
		 IRA::CString& extension)
{
	char *dirc, *basec;
	dirc=strdup((const char *)fullPath); // the APIs called below might change the content of the input string
	basec=strdup((const char *)fullPath);
	baseDir=IRA::CString(dirname(dirc));
	baseName=IRA::CString(basename(basec));
	if ((baseDir.GetLength()==0) || (baseName.GetLength()==0)) {
		return false;
	}
	int pos=baseName.Find('.');
	if (pos<0) { // not found
		extension="";
	}
	else {
		extension=baseName.Right(baseName.GetLength()-(pos+1));
		baseName=baseName.Mid(0,pos);
	}
	return true;
}

bool CIRATools::sendMail(const IRA::CString& subject,const IRA::CString& body,
  const IRA::CString& recipients, bool dryRun)
{
	IRA::CString command;
	command.Format("echo \"%s\" | mail -s \"%s\" ",(const char *)body,(const char *)subject);
	command=command+recipients;
	//cout << (const char *)command << endl;
	if (dryRun) return true;
	else if (system((const char *)command)<0) return false;
	else return true;
}

double CIRATools::roundNearest(const double& val,const long& decimals)
{
	long precision=(long)pow(10,decimals);
	return round(val*precision)/precision;
}

double CIRATools::roundDown(const double& val,const long& decimals)
{
	long precision=(long)pow(10,decimals);
	return floor(val*precision)/precision;
}

double CIRATools::roundUp(const double& val,const long& decimals)
{
	long precision=(long)pow(10,decimals);
	return ceil(val*precision)/precision;
}

double CIRATools::getMinimumValue(const ACS::doubleSeq& array,long& pos)
{
	if (array.length()>0) {
		double min=array[0];
		pos=0;
		for (unsigned i=1;i<array.length();i++) {
			if (array[i]<min) {
				min=array[i];
				pos=i;
			}
		}
		return min;
	}
	else {
		pos=-1;
		return 0.0;
	}
}

double CIRATools::getMaximumValue(const ACS::doubleSeq& array,long& pos)
{
	if (array.length()>0) {
		double max=array[0];
		pos=0;
		for (unsigned i=1;i<array.length();i++) {
			if (array[i]>max) {
				max=array[i];
				pos=i;
			}
		}
		return max;
	}
	else {
		pos=-1;
		return 0.0;
	}
}

bool CIRATools::matchRegExp(const IRA::CString& input,const IRA::CString& expr,std::vector<IRA::CString>& res)
{
	std::string::const_iterator start, end;
	std::string stream;
	boost::match_results<std::string::const_iterator> what;
	boost::match_flag_type flags = boost::match_default;
	stream=(const char *)input;
	start=stream.begin();
	end=stream.end();
	//boost::smatch what;
	boost::regex regExpr((const char *)expr);
	try {
		res.empty();
		while (boost::regex_search(start,end,what,regExpr,flags)) {
			//res.empty();
			for(unsigned i=0;i<what.size();i++) {
				std::string out(what[i].first,what[i].second);
				res.push_back(out.c_str());
			}
			start=what[0].second;
	  	//flags|=boost::match_prev_avail;
	  	//flags|=boost::match_not_bob;
		}
		return true;
	}
	catch (...) {
	}
	return false;
}

// *******************************//
// private:

bool CIRATools::bandLimits(const double&f,const double& w,double& f1,double& f2,bool& upper)
{
	/*if (w<0) {
		return false;
	}*/
	if (f>=0) {
		f1=f;
		f2=f+w;
		upper=true;
	}
	else {
		/*if (w>-f) {
			return false;
		}*/
		f1=f+w;
		f2=f;
		upper=false;
	}
	return true;
}

bool CIRATools::mergeBands(const double& rf1,const double& rf2,const bool& rside,const double& bf1,
		 const double& bf2,const bool& bside,double&f,double& w)
{
	double startF,stopF,bw;

	if ((rf1>rf2) || (bf1>bf2)) {
		return false;
	}
	if (rside && bside) { //UU
		startF=MAX(rf1,bf1);
		stopF=MIN(rf2,bf2);
		bw=stopF-startF;
		if (bw<=0) {
			f=rf1;
			w=0;
			return false;
		}
		else {
			f=startF;
			w=bw;
			return true;
		}
	}
	else if (rside && !bside) {  //UL
		startF=MAX(rf1,-bf2);
		stopF=MIN(rf2,-bf1);
		bw=stopF-startF;
		if (bw<=0) {
			f=rf1;
			w=0;
			return false;
		}
		else {
			f=-stopF;
			w=-bw;
			return true;
		}	
	}
	else if (!rside && bside) {  //LU
		startF=MAX(-rf2,bf1);
		stopF=MIN(-rf1,bf2);
		bw=stopF-startF;
		if (bw<=0) {
			f=rf1;
			w=0;
			return false;
		}
		else {
			f=-stopF;
			w=-bw;
			return true;
		}
	}
	else if (!rside && !bside) { //LL
		startF=MAX(-rf2,-bf2);
		stopF=MIN(-rf1,-bf1);
		bw=stopF-startF;
		if (bw<=0) {
			f=rf1;
			w=0;
			return false;
		}
		else {
			f=startF;
			w=bw;
			return true;
		}
	}
	return true;
}

