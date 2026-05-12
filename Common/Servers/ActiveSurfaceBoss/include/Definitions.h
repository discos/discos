#ifndef _ACTIVESURFACEBOSSDEFINITIONS_H_
#define _ACTIVESURFACEBOSSDEFINITIONS_H_

#define firstUSD 1
#define LOOPTIME 100000 // 0,10 sec
#define CDBPATH std::string(getenv("ACS_CDB")) + "/CDB/"
#define USDTABLE CDBPATH + "alma/AS/tab_convUSD.txt"
#define USDTABLECORRECTIONS CDBPATH + "alma/AS/act_rev02.txt"
#define MM2HSTEP    350 //(10500 HSTEP / 30 MM)
#define MM2STEP     1400 //(42000 STEP / 30 MM)
#define WARNINGUSDPERCENT 0.95
#define ERRORUSDPERCENT 0.90
#define THRESHOLDPOS 16 // 12 micron in step
#define NPOSITIONS 7
#define DELTAEL 15.0

// mask pattern for status
#define UNAV    0xFF000000
#define MRUN    0x000080
#define CAMM    0x000100
#define ENBL    0x002000
#define PAUT    0x000800
#define CAL     0x008000

#define _SET_CDB(PROP,LVAL,ROUTINE) {    \
        if (!CIRATools::setDBValue(m_services,#PROP,(const long&) LVAL)) \
        { ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,ROUTINE); \
            exImpl.setFieldName(#PROP); throw exImpl; \
        } \
}

#endif
