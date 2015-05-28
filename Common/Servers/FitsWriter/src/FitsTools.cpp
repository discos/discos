#include "FitsTools.h"

using namespace FitsWriter_private;

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
};



