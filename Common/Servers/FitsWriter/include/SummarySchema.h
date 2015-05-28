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

#define SCHEMA_ENTRY_NUMBER 30

#define SCHEMA_ENTRY1 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"TELESCOP","Telescope name" )
#define SCHEMA_ENTRY2 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"ReceiverCode","Receiver name" )
#define SCHEMA_ENTRY3 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"BackendName","Backend name" )
#define SCHEMA_ENTRY4 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"OBJECT","Target source name" )
#define SCHEMA_ENTRY5 ( _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"RightAscension","Target right ascension (radians)" )
#define SCHEMA_ENTRY6 ( _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"Declination","Target declination (radians)" )
#define SCHEMA_ENTRY7 ( _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"EQUINOX","Equinox of RA, Dec" )
#define SCHEMA_ENTRY8 ( _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"EXPTIME","Total integration time (seconds)" )
#define SCHEMA_ENTRY9 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"PROJID","ProjectID" )
#define SCHEMA_ENTRY10 ( _FILE_LONG_TYPE,_FILE_SINGLE_ENTRY,"LST","Local sidereal time" )
#define SCHEMA_ENTRY11 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"SCANTYPE","Scan astronomical type" )
#define SCHEMA_ENTRY12 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"SCANMODE","Mapping mode" )
#define SCHEMA_ENTRY13 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"SCANGEOM","Scan geometry" )
#define SCHEMA_ENTRY14 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"SWTCHMOD","Switch mode" )
#define SCHEMA_ENTRY15 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"ScheduleName","Name of the schedule" )
#define SCHEMA_ENTRY16 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"LogFileName","Name of the log file" )
#define SCHEMA_ENTRY17 ( _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"SCANXVEL","Tracking rate (optional,OTF)" )
#define SCHEMA_ENTRY18 ( _FILE_LONG_TYPE,_FILE_SINGLE_ENTRY,"WOBUSED","Wobbler used?" )
#define SCHEMA_ENTRY19 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"OBSID","Observer or operator initials" )
#define SCHEMA_ENTRY20 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"CREATOR","Software (incl. version)" )
#define SCHEMA_ENTRY21 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"FITSVER","FITS version" )
#define SCHEMA_ENTRY22 ( _FILE_LONG_TYPE,_FILE_SINGLE_ENTRY,"NUSEBANDS","Number of sections" )
#define SCHEMA_ENTRY23 ( _FILE_DOUBLE_TYPE,_FILE_MULTI_ENTRY,"FREQRES","Frequency resolution (MHz)" )
#define SCHEMA_ENTRY24 ( _FILE_DOUBLE_TYPE,_FILE_MULTI_ENTRY,"RESTFREQ","Rest frequency (MHz)" )
#define SCHEMA_ENTRY25 ( _FILE_DOUBLE_TYPE,_FILE_MULTI_ENTRY,"FREQ","frequency of the Nth section(MHz)" )
#define SCHEMA_ENTRY26 ( _FILE_DOUBLE_TYPE,_FILE_MULTI_ENTRY,"BWID","bandwidth of the Nth section (MHz)" )
#define SCHEMA_ENTRY27 ( _FILE_LONG_TYPE,_FILE_MULTI_ENTRY,"CHAN","Number of spectral bins of the Nth section" )

#define SCHEMA_ENTRY28 ( _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"VRAD","Radial velocity" )
#define SCHEMA_ENTRY29 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"VFRAME","Radial velocity reference frame" )
#define SCHEMA_ENTRY30 ( _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"VDEF","Radial velocity definition" )


#endif /* SUMMARYSCHEMA_H_ */
