namespace NewFitsWriterTest {

#define ROW_NUMBER 20

class NewFitsWriter_FitsTools : public ::testing::Test {
public:
	::testing::AssertionResult FitsTools_filePrepare() {
		RecordProperty("description","check if a fits file can be created correctly");
		IRA::CString errorMsg,former,coler;
		CCfits::FITS *fits;
		CCfits::Table *table;
		std::vector<string> col,unit,form;
		std::vector< std::valarray< double > > data;
		std::vector< std::valarray< double > > meteo;
		std::valarray<double> dataTemp;
		std::valarray<double> meteoTemp;
		std::vector< double > scalarData1;
		std::vector< double > scalarData2;
		const long bins=1024;
		const long sections=4;
		const long spectrum=4;
		col.push_back("column1");
		col.push_back("column2");
		col.push_back("vector1");
		unit.push_back("dummy");
		unit.push_back("foo");
		unit.push_back("meteo");
		form.push_back("D");
		form.push_back("D");
		form.push_back("3D");
		double doubleValue=11.3455;
		double doubleValue1=342.2;
		std::string testString="test string";
		for (long kk=0;kk<sections;kk++) {
			coler.Format("Ch%ld",kk);
			col.push_back((const char *)coler);
			former.Format("%ldD",bins*spectrum);
			form.push_back((const char *)former);
			unit.push_back("");
		}
		/*data.resize(ROW_NUMBER);
		meteo.resize(ROW_NUMBER);
		scalarData1.resize(ROW_NUMBER);
		scalarData2.resize(ROW_NUMBER);*/
		meteoTemp.resize(3);
		meteoTemp[0]=0.1;
		meteoTemp[1]=0.2;
		meteoTemp[2]=0.3;
		dataTemp.resize(bins*spectrum);
		for (long nn=0;nn<ROW_NUMBER;nn++) {
			for (long kk=0;kk<spectrum;kk++) {
				for (long jj=0;jj<bins;jj++) {
					dataTemp[(kk*bins)+jj]=(((double)random())/RAND_MAX)+kk+nn;
				}
			}
			data.push_back(dataTemp);
			meteo.push_back(meteoTemp);
			scalarData1.push_back(doubleValue+nn);
			scalarData2.push_back(doubleValue1+nn);
		}
		if (!FitsWriter_private::CFitsTools::createFile(fits,fullName_file1,errorMsg)) {
			return ::testing::AssertionFailure() << (const char *) fullName_file1 << " creation failed with message " <<
					(const char *)errorMsg;
		}
		else {
			if (!IRA::CIRATools::fileExists(fullName_file1)) {
				return ::testing::AssertionFailure() << (const char *) fullName_file1 << " does not exists";
			}
		}
		table=FitsWriter_private::CFitsTools::createTable(fits,"TestTable",col,form,unit,errorMsg);
		if (!table) {
			return ::testing::AssertionFailure() << "table creation failed with message " << (const char *)errorMsg;
		}
		if (!FitsWriter_private::CFitsTools::setPrimaryHeaderKey(fits,"keyword",doubleValue,"Dummy keyword",errorMsg)) {
			return ::testing::AssertionFailure() << "cannot insert primary header keyword, message is " << (const char *)errorMsg;
		}
		if (!FitsWriter_private::CFitsTools::setPrimaryHeaderKey(fits,"TestMe",testString,"string keyword",errorMsg)) {
			return ::testing::AssertionFailure() << "cannot insert primary header keyword, message is " << (const char *)errorMsg;
		}
		if (!FitsWriter_private::CFitsTools::setTableHeaderKey(table,"keyword",doubleValue,"Dummy keyword",errorMsg)) {
			return ::testing::AssertionFailure() << "cannot insert table header keyword, message is " << (const char *)errorMsg;
		}
		if (!FitsWriter_private::CFitsTools::flush(fits,errorMsg)) {
			return ::testing::AssertionFailure() << "cannot flush file, message is " << (const char *)errorMsg;
		}

		if (!FitsWriter_private::CFitsTools::writeTable(table,"column1",scalarData1,1,errorMsg)) {
			return ::testing::AssertionFailure() << "cannot write column into table (column name access), message is " << (const char *)errorMsg;
		}
		if (!FitsWriter_private::CFitsTools::writeTable(table,2,scalarData2,1,errorMsg)) {
			return ::testing::AssertionFailure() << "cannot write into table (column ID access), message is " << (const char *)errorMsg;
		}
		if (!FitsWriter_private::CFitsTools::writeTable(table,3,meteo,1,errorMsg)) {
				return ::testing::AssertionFailure() << "cannot write into table (vector column), message is " << (const char *)errorMsg;
		}
		for (long kk=0;kk<sections;kk++) {
			if (!FitsWriter_private::CFitsTools::writeTable(table,4+kk,data,1,errorMsg)) {
				return ::testing::AssertionFailure() << "cannot write into table (data columns), message is " << (const char *)errorMsg;
			}
		}

		FitsWriter_private::CFitsTools::closeFile(fits);
		if (fits!=NULL) {
			return ::testing::AssertionFailure() << "file pointer should be null after closeup";
		}
		return ::testing::AssertionSuccess();
	}


protected:
	IRA::CString basePath;
	IRA::CString fullName_file1;
	virtual void SetUp() {
		char buff[256];
		getcwd(buff,256);
		basePath=buff;
		fullName_file1=basePath+"/testFits1.fits";
	}
	virtual void TearDown() {
		IRA::CString cleanCommand;
		cleanCommand="rm "+fullName_file1;
		system((const char *)cleanCommand);
	}
};

}
