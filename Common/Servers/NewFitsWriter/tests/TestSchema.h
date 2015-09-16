/* ************************************************************************************************************* */
/* DISCOS project                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                    when            What                                                   */
/* Andrea Orlati (a.orlati@ira.inaf.it)   13/08/2015
*/

#ifndef SUMMARYSCHEMA_H_
#define SUMMARYSCHEMA_H_

#define SCHEMA_HEADER_ENTRY_NUMBER 5
#define SCHEMA_TABLE_NUMBER 2
#define SCHEMA_COLUMN_NUMBER 7
#define SCHEMA_DATA_COLUMN_NUMBER 1

#define SCHEMA_HEADER_ENTRY1 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"TELESCOP","Telescope name" )
#define SCHEMA_HEADER_ENTRY2 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"ReceiverCode","Receiver name" )
#define SCHEMA_HEADER_ENTRY3 ( 0, _FILE_STRING_TYPE,_FILE_SINGLE_ENTRY,"BackendName","Backend name" )
#define SCHEMA_HEADER_ENTRY4 ( 0, _FILE_DOUBLE_TYPE,_FILE_SINGLE_ENTRY,"epoch","epoch of the observation" )

#define SCHEMA_TABLE1 ( "DataTable", 1 )
#define SCHEMA_COLUMN1 ( 1, _FILE_DOUBLE_TYPE, 1, "az","radians" )
#define SCHEMA_COLUMN2 ( 1, _FILE_DOUBLE_TYPE, 1, "el", "radians" )
#define SCHEMA_COLUMN3 ( 1, _FILE_LONG_TYPE, 1, "flag", "boolean" )
#define SCHEMA_COLUMN4 ( 1, _FILE_LONG_TYPE, 1, "counter", "counter" )
#define SCHEMA_COLUMN5 ( 1, _FILE_DOUBLE_TYPE, 3, "weather", "temp/rh/press" )

#define SCHEMA_TABLE2 ( "InfoTable", 2 )
#define SCHEMA_COLUMN6 ( 2, _FILE_LONG_TYPE, 1, "X","sample number" )
#define SCHEMA_COLUMN7 ( 2, _FILE_DOUBLE_TYPE, 2, "Y", "kelvin" )
#define SCHEMA_DATA_COLUMN1 ( 2, "Ch", "section raw data")


#define SCHEMA_HEADER_ENTRY5 ( 1, _FILE_DOUBLE_TYPE,_FILE_MULTI_ENTRY,"test","test keyword" )


#endif /* SUMMARYSCHEMA_H_ */
