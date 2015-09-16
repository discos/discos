
namespace NewFitsWriterTest {

class NewFitsWriter_File : public ::testing::Test {
public:
	::testing::AssertionResult File_opening() {
		IRA::CString errorMsg;
		std::vector<long> dim;
		RecordProperty("description","check if a fits file can be opened correctly (file + binary tables)");
		FitsWriter_private::CTable *tab=file->getTable(2,errorMsg);
		dim.push_back(4096);
		dim.push_back(1);
		dim.push_back(8192);
		if (!tab->defineDataColumn("Ch",dim,errorMsg)) {
			return ::testing::AssertionFailure() << "data column dimensioning failed with message: " << (const char *)errorMsg;
		}
		if (!file->open(errorMsg)) {
			return ::testing::AssertionFailure() << "file creation failed with message: " << (const char *)errorMsg;
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult File_creation() {
		IRA::CString errorMsg;
		RecordProperty("description","check if a fits file can be created correctly (keywords)");
		std::list<double> testList;
		testList.push_back(99.99);
		testList.push_back(109.99);
		testList.push_back(209.99);

		file->setKeyword<_FILE_STRING_TYPE>("TELESCOP","This_is_my_telescope",errorMsg);
		file->setKeyword<_FILE_STRING_TYPE>("ReceiverCode","KKC",errorMsg);
		file->setKeyword<_FILE_STRING_TYPE>("BackendName","TotalPower",errorMsg);
		file->setKeyword<_FILE_DOUBLE_TYPE>("epoch",1232323455.5333,errorMsg);
		file->setKeyword<_FILE_DOUBLE_TYPE>("test",testList,errorMsg);

		if (!file->create(errorMsg)) {
			return ::testing::AssertionFailure() << "file creation failed with message " << (const char *)errorMsg;
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult File_getTable() {
		IRA::CString errorMsg;
		RecordProperty("description","check if a binary table could be correctly retrieved");
		FitsWriter_private::CTable *tab1=file->getTable("DataTable",errorMsg);
		FitsWriter_private::CTable *tab2=file->getTable(1,errorMsg);
		if (!tab1) {
			return ::testing::AssertionFailure() << "table " << "DataTable" << " cannot be found, " << (const char *)errorMsg;
		}
		if (!tab2) {
			return ::testing::AssertionFailure() << "table " << 1 << " cannot be found, " << (const char *)errorMsg;
		}
		// check if it is detected a wrong retrieval request
		tab1=file->getTable("fooboo",errorMsg);
		if (tab1) {
			return ::testing::AssertionFailure() << "Not existing table returned with no error";
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult File_writeTableColumns() {
		IRA::CString errorMsg;
		RecordProperty("description","check if some data could be regularly into a binary table");
		FitsWriter_private::CTable *tab=file->getTable("DataTable",errorMsg);
		if (!tab) {
			return ::testing::AssertionFailure() << "table " << "DataTable" << " cannot be found, " << (const char *)errorMsg;
		}
		std::valarray<double> meteo(3);
		meteo[0]=10.4; meteo[1]=33.66; meteo[2]=55.45;

		if (!tab->setColumn<_FILE_DOUBLE_TYPE>("az",102.22,errorMsg)) {
			return ::testing::AssertionFailure() << "column " << "az" << " cannot be written, message is " << (const char *)errorMsg;
		}
		if (!tab->setColumn<_FILE_DOUBLE_TYPE>("el",44.123,errorMsg)) {
			return ::testing::AssertionFailure() << "column " << "el" << " cannot be written, message is " << (const char *)errorMsg;
		}
		if (!tab->setColumn<_FILE_LONG_TYPE>("flag",-1,errorMsg)) {
			return ::testing::AssertionFailure() << "column " << "flag" << " cannot be written, message is " << (const char *)errorMsg;
		}
		if (!tab->setColumn<_FILE_LONG_TYPE>("counter",15,errorMsg)) {
			return ::testing::AssertionFailure() << "column " << "counter" << " cannot be written, message is " << (const char *)errorMsg;
		}
		if (!tab->setColumn<_FILE_DOUBLE_TYPE>("weather",meteo,errorMsg)) {
			return ::testing::AssertionFailure() << "column " << "weather" << " cannot be written, message is " << (const char *)errorMsg;
		}
		//add a second row with just a value for the first column
		if (!tab->nextRow(errorMsg)) {
			return ::testing::AssertionFailure() << "cannot add a new row in table, message is " << (const char *)errorMsg;
		}
		if (!tab->setColumn<_FILE_DOUBLE_TYPE>("az",81.33,errorMsg)) {
			return ::testing::AssertionFailure() << "column " << "az" << " cannot be written, message is " << (const char *)errorMsg;
		}
		//try to add data for not existing column
		if (tab->setColumn<_FILE_DOUBLE_TYPE>("boofoo",22.343,errorMsg)) {
			return ::testing::AssertionFailure() << "writing into a not existing column, returned with no error";
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult File_writeTableColumnsFast() {
		IRA::CString errorMsg;
		long xdata=1;
		std::valarray<double> ydata;
		std::valarray<double> ch0data;
		double ch1data;
		std::valarray<double> ch2data;
		ch0data.resize(4096);
		ch1data=-1.2;
		ch2data.resize(8192);
		ydata.resize(2);
		RecordProperty("description","check if a binary table could be written with large amount of data");
		FitsWriter_private::CTable *tab=file->getTable("InfoTable",errorMsg);
		if (!tab) {
			return ::testing::AssertionFailure() << "table " << "InfoTable" << " cannot be found, " << (const char *)errorMsg;
		}
		for (unsigned k=0;k<120;k++) {
			if (!tab->setColumn<_FILE_LONG_TYPE>("X",xdata,errorMsg)) {
				return ::testing::AssertionFailure() << "column " << "X" << " cannot be written, message is " << (const char *)errorMsg;
			}
			ydata[0]=xdata+(rand()/*/RAND_MAX*/);
			ydata[1]=xdata*10+(rand()/*/RAND_MAX*/);
			for (unsigned y=0;y<4096;y++) ch0data[y]=xdata+y;
			for (unsigned y=0;y<8192;y++) ch2data[y]=xdata*y;
			if (!tab->setColumn<_FILE_DOUBLE_TYPE>("Y",ydata,errorMsg)) {
				return ::testing::AssertionFailure() << "column " << "Y" << " cannot be written, message is " << (const char *)errorMsg;
			}
			if (!tab->setDataColumn("Ch0",ch0data,errorMsg)) {
				return ::testing::AssertionFailure() << "column " << "Ch0" << " cannot be written, message is " << (const char *)errorMsg;
			}
			if (!tab->setDataColumn("Ch1",ch1data,errorMsg)) {
				return ::testing::AssertionFailure() << "column " << "Ch1" << " cannot be written, message is " << (const char *)errorMsg;
			}
			if (!tab->setDataColumn("Ch2",ch2data,errorMsg)) {
				return ::testing::AssertionFailure() << "column " << "Ch2" << " cannot be written, message is " << (const char *)errorMsg;
			}

			xdata++;
			//this will add a blank row at the end of the table when table is flushed at the end....but no issue for this test
			if (!tab->nextRow(errorMsg)) {
				return ::testing::AssertionFailure() << "cannot add row " << k << " in table, message is " << (const char *)errorMsg;
			}
		}
		for (unsigned k=0;k<84;k++) {
			if (!tab->setColumn<_FILE_LONG_TYPE>("X",xdata,errorMsg)) {
				return ::testing::AssertionFailure() << "column " << "X" << " cannot be written, message is " << (const char *)errorMsg;
			}
			ydata[0]=5.5;
			ydata[1]=7.8;
			if (!tab->setColumn<_FILE_DOUBLE_TYPE>("Y",ydata,errorMsg)) {
				return ::testing::AssertionFailure() << "column " << "Y" << " cannot be written, message is " << (const char *)errorMsg;
			}
			xdata++;
			//this will add a blank row at the end of the table when table is flushed at the end....but no issue for this test
			if (!tab->nextRow(errorMsg)) {
				return ::testing::AssertionFailure() << "cannot add row " << k << " in table, message is " << (const char *)errorMsg;
			}
		}
		return ::testing::AssertionSuccess();
	}

protected:
	static FitsWriter_private::CFile *file;

	static void TearDownTestCase() {
		IRA::CString cleanCommand;
		cleanCommand="rm "+file->getFileName();
		if (file) delete file;
		file=NULL;
		system((const char *)cleanCommand);
		//printf("%s \n",(const char *)cleanCommand);
	}
	static void SetUpTestCase() {
		file=new FitsWriter_private::CFile();
		#define _FILE_LOCAL_FILE file
		#include "TestSchemaCreator.h"
		#undef _FILE_LOCAL_FILE
		IRA::CString basePath;
		char buff[256];
		getcwd(buff,256);
		basePath=buff;
		file->setFileName(basePath+"/testFileIO.fits");
	}

	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};

FitsWriter_private::CFile *NewFitsWriterTest::NewFitsWriter_File::file=NULL;

}

