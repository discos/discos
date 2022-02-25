/* ************************************************************************************************************* */
/* DISCOS project                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                    when            What                                                   */
/* Andrea Orlati (a.orlati@ira.inaf.it)   24/04/2015
*/


#ifndef SUMMARYSCHEMA_H_
#define SUMMARYSCHEMA_H_

#define SCHEMA_HEADER_ENTRY_NUMBER 45

#define SCHEMA_HEADER_ENTRY1 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"TELESCOP","Telescope name" )
#define SCHEMA_HEADER_ENTRY2 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"ReceiverCode","Receiver name" )
#define SCHEMA_HEADER_ENTRY3 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"BackendName","Backend name" )
#define SCHEMA_HEADER_ENTRY4 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"OBJECT","Target source name" )
#define SCHEMA_HEADER_ENTRY5 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"RightAscension","Target right ascension (radians)" )
#define SCHEMA_HEADER_ENTRY6 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"Declination","Target declination (radians)" )
#define SCHEMA_HEADER_ENTRY7 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"EQUINOX","Equinox of RA, Dec" )
#define SCHEMA_HEADER_ENTRY8 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"EXPTIME","Total integration time (seconds)" )
#define SCHEMA_HEADER_ENTRY9 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"PROJID","ProjectID" )
#define SCHEMA_HEADER_ENTRY10 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"LST","Local sidereal time (seconds)" )
#define SCHEMA_HEADER_ENTRY11 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"SCANTYPE","Scan astronomical type" )
#define SCHEMA_HEADER_ENTRY12 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"SCANMODE","Mapping mode" )
#define SCHEMA_HEADER_ENTRY13 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"SCANGEOM","Scan geometry" )
#define SCHEMA_HEADER_ENTRY14 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"SWTCHMOD","Switch mode" )
#define SCHEMA_HEADER_ENTRY15 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"ScheduleName","Name of the schedule" )
#define SCHEMA_HEADER_ENTRY16 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"LogFileName","Name of the log file" )
#define SCHEMA_HEADER_ENTRY17 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"SCANXVEL","Tracking rate (optional,OTF)" )
#define SCHEMA_HEADER_ENTRY18 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"WOBUSED","Wobbler used?" )
#define SCHEMA_HEADER_ENTRY19 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"OBSID","Observer or operator initials" )
#define SCHEMA_HEADER_ENTRY20 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"CREATOR","Software (incl. version)" )
#define SCHEMA_HEADER_ENTRY21 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"FITSVER","FITS version" )
#define SCHEMA_HEADER_ENTRY22 ( 0, _FILE_LONG_TYPE,_FILE_SINGLE_ENTRY,"NUSEBAND","Number of sections" )
#define SCHEMA_HEADER_ENTRY23 ( 0, _FILE_DOUBLE_TYPE,_FILE_MULTI_ENTRY,"FREQRESOL","Frequency resolution of the Nth section (MHz)" )
#define SCHEMA_HEADER_ENTRY24 ( 0, _FILE_DOUBLE_TYPE,_FILE_MULTI_ENTRY,"RESTFREQ","Rest frequency of the Nth section (MHz)" )
#define SCHEMA_HEADER_ENTRY25 ( 0, _FILE_DOUBLE_TYPE,_FILE_MULTI_ENTRY,"FREQ","Start frequency of the Nth section(MHz)" )
#define SCHEMA_HEADER_ENTRY26 ( 0, _FILE_DOUBLE_TYPE,_FILE_MULTI_ENTRY,"BWID","Bandwidth of the Nth section (MHz)" )
#define SCHEMA_HEADER_ENTRY27 ( 0, _FILE_LONG_TYPE,_FILE_MULTI_ENTRY,"FREQBINS","Number of spectral bins of the Nth section" )
#define SCHEMA_HEADER_ENTRY28 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"VRAD","Radial velocity" )
#define SCHEMA_HEADER_ENTRY29 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"VFRAME","Radial velocity reference frame" )
#define SCHEMA_HEADER_ENTRY30 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"VDEF","Radial velocity definition" )
#define SCHEMA_HEADER_ENTRY31 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"DATE-OBS","Observation time (YYYY-MM-DDThh:mm:ss.sss UT)" )
#define SCHEMA_HEADER_ENTRY32 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"PINAME","Name if the PI of the project" )
#define SCHEMA_HEADER_ENTRY33 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"WCSNAME","Astronomical basis frame" )
#define SCHEMA_HEADER_ENTRY34 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"SCANDIR","(Optional) scan direction" )
#define SCHEMA_HEADER_ENTRY35 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"DUTYCYC","(Optional) On-Off-Cal duty cycle" )
#define SCHEMA_HEADER_ENTRY36 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"USRFRAME","Description of the user frame" )
#define SCHEMA_HEADER_ENTRY37 ( 0, _FILE_STRING_TYPE,_FILE_MULTI_ENTRY,"POLATYPE","LCP, RCP, HLP, VLP, stokes type of the Nth section" )
#define SCHEMA_HEADER_ENTRY38 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"XMINDEG","Scan start x coordinate (deg, optional)")
#define SCHEMA_HEADER_ENTRY39 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"XMAXDEG","Scan end x coordinate (deg, optional)" )
#define SCHEMA_HEADER_ENTRY40 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"YMINDEG","Scan start y coordinate (deg, optional)" )
#define SCHEMA_HEADER_ENTRY41 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"YMAXDEG","Scan end y coordinate (deg, optional)" )
#define SCHEMA_HEADER_ENTRY42 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"SCANOFF","Scan offset (deg)" )
#define SCHEMA_HEADER_ENTRY43 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"SCANLEN","Scan span (deg)" )
#define SCHEMA_HEADER_ENTRY44 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"SCANSTART","Scan start (deg)" )
#define SCHEMA_HEADER_ENTRY45 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"TSYS","System temperature (K)" )

//#define SCHEMA_TABLE_NUMBER 1

//#define SCHEMA_TABLE0 ( "Data Table", 1 )
//#define SCHEMA_HEADER_ENTRY31 ( 1, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"test","test keyword" )

//#define SCHEMA_TABLE_COL1 ( 1,"azimuth", _FILE_DOUBLE_TYPE, _FILE_SINGLE_ENTRY, "radians" )
//#define SCHEMA_TABLE_COL2 ( 1,"elevation", _FILE_DOUBLE_TYPE, _FILE_SINGLE_ENTRY, "radians" )



//#define SCHEMA_TABLE2 ( "Antenna Temp", 2 )



#endif /* SUMMARYSCHEMA_H_ */
