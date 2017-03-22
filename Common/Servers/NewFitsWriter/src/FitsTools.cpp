#include "FitsTools.h"

using namespace FitsWriter_private;

bool CFitsTools::createFile(CCfits::FITS *& fits,const IRA::CString& fileName,IRA::CString& errorMessage)
{
	try {
		fits=new CCfits::FITS((const char *)fileName,CCfits::Write);
		fits->pHDU().writeDate();
	}
	catch (CCfits::FitsException& ex) {
		errorMessage=ex.message().c_str();
		return false;
	}
	return true;
}

void CFitsTools::closeFile(CCfits::FITS *& fits)
{
	IRA::CString err;
	if (fits!=NULL) {
		CFitsTools::flush(fits,err);
		delete fits;
		fits=NULL;
	}
}

bool CFitsTools::primaryHeaderComment(
		CCfits::FITS *const fits,
		const IRA::CString& comment,
		IRA::CString& errorMessage)
{
	if (!fits) {
		errorMessage="fits file is not created";
		return false;
	}
	try {
		fits->pHDU().writeComment((const char *)comment);
	}
	catch (CCfits::FitsException& ex) {
		errorMessage=ex.message().c_str();
		return false;
	}
	return true;
}

bool CFitsTools::primaryHeaderHistory(CCfits::FITS *const fits,const IRA::CString& history,
	IRA::CString& errorMessage)
{
	if (!fits) {
		errorMessage="fits file is not created";
		return false;
	}
	try {
		fits->pHDU().writeHistory((const char *)history);
	}
	catch (CCfits::FitsException& ex) {
		errorMessage=ex.message().c_str();
		return false;
	}
	return true;
}

bool CFitsTools::flush(CCfits::FITS *const fits,IRA::CString& errorMessage)
{
	if (!fits) {
		errorMessage = "fits file is not created";
		return false;
	}
	try{
		fits->flush();
		return true;
	}
	catch(CCfits::FitsException& fe){
		errorMessage = fe.message().c_str();
		return false;
	}
}

CCfits::Table *CFitsTools::createTable(CCfits::FITS *const fits,const IRA::CString& name,const std::vector<string>& colName,
		const std::vector<string>& colForm,const std::vector<string>& colUnit,IRA::CString& errorMessage)
{
	CCfits::Table *table;
	if (!fits) {
		errorMessage="fits file is not created";
		return NULL;
	}
	//for (int i=0;i<colName.size();i++) printf("nome colonne %d, %s %s %s\n",i,colName[i].c_str(),colForm[i].c_str(),colUnit[i].c_str());
	try {
		table=fits->addTable((const char *)name,0,colName,colForm,colUnit);
		if (!table) {
			errorMessage.Format("%s table could not be created",(const char *)name);
			return NULL;
		}
	}
	catch(CCfits::FitsException& fe){
		errorMessage=fe.message().c_str();
		return NULL;
	}
	return table;
}



