#include "FitsTools.h"

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

