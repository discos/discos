// $Id: TestDBTable.cpp,v 1.4 2011-03-14 13:18:54 a.orlati Exp $

#include "DBTable.h"

using namespace maci;

int main(int argc, char *argv[])
{
	SimpleClient client;
	if (client.init(argc,argv)==0) {
		printf("Initialize error\n");
		return -1;
	}
	else {
		if (client.login()==0) {
			printf("Manager login error\n");
			return -1;
		}
	}
	IRA::CError err;
	IRA::CDBTable table(&client,"Source","DataBlock/SourceCatalog");
	
	err.Reset();
	if (!table.addField(err,"sourceName",CDataField::STRING)) {
		err.setExtra("Error adding field sourceName",0);
	}
	else if (!table.addField(err,"rightAscension",CDataField::STRING)) {
		err.setExtra("Error adding field rightAscension",0);
	}
	else if (!table.addField(err,"declination",CDataField::STRING)) {
		err.setExtra("Error adding field declination", 0);
	}
	else if (!table.addField(err,"epoch", CDataField::STRING)) {
		err.setExtra("Error adding field epoch", 0);
	}
	else if (!table.addField(err,"pmRA", CDataField::DOUBLE))	{
		err.setExtra("Error adding field pmRA", 0);
	}
	else if (!table.addField(err,"pmDec", CDataField::DOUBLE)) {
		err.setExtra("Error adding field pmDec", 0);
	}
	else if (!table.addField(err,"parallax", CDataField::DOUBLE))	{
		err.setExtra("Error adding field parallax", 0);
	}
	else if (!table.addField(err,"radialVelocity",CDataField::DOUBLE)) {
		err.setExtra("Error adding field radialVelocity", 0);
	}	
	if (!err.isNoError()) {
		printf("%s\n",(const char *)err.getFullDescription());
		return -1;
	}
	if (!table.openTable(err))	{
		printf("%s\n",(const char *)err.getFullDescription());
		return -1;
	}	
	table.First();
	for (unsigned i=0;i<table.recordCount();i++) {
		if (table["sourceName"]->isNull()) printf("Null\t");
	 	else printf("Source %s\t",(const char*)table["sourceName"]->asString());
		if (table["rightAscension"]->isNull()) printf("Null\t");
	 	else printf("%s\t",(const char*)table["rightAscension"]->asString());
		if (table["declination"]->isNull()) printf("Null\n");
	 	else printf("%s\n",(const char*)table["declination"]->asString());		
	 	table.Next();
	 }
	table.closeTable();
	
	IRA::CDBTable procTable(&client,"Procedure","Procedures/StationProcedures");
	err.Reset();
	if (!procTable.addField(err,"name",CDataField::STRING)) {
		err.setExtra("Error adding field sourceName",0);
	}
	else if (!procTable.addField(err,"body",CDataField::STRING)) {
		err.setExtra("Error adding field rightAscension",0);
	}
	if (!err.isNoError()) {
		printf("%s\n",(const char *)err.getFullDescription());
		return -1;
	}
	if (!procTable.openTable(err))	{
		printf("%s\n",(const char *)err.getFullDescription());
		return -1;
	}	
	procTable.First();
	for (unsigned i=0;i<procTable.recordCount();i++) {
		if (procTable["name"]->isNull()) printf("Null\n");
	 	else printf("Name %s\n",(const char*)procTable["name"]->asString());
		if (procTable["body"]->isNull()) printf("Null\n");
	 	else printf("%s\n",(const char*)procTable["body"]->asString());
		procTable.Next();
	}
	procTable.closeTable();
	
	client.logout();
	ACE_OS::sleep(1);
	printf("SHUTTING_DOWN\n");	
	return 0;	
}
