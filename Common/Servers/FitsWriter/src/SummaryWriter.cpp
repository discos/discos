#include "Version.h"
#include "SummaryWriter.h"
#include "FitsTools.h"
#include <Definitions.h>
#include <IRATools.h>

CSummaryWriter::CSummaryWriter() : m_lastError(""),m_filename("summary.fits"),m_basepath("~/"),m_pFits(NULL)
{
}

CSummaryWriter::~CSummaryWriter()
{
	if (m_pFits!=NULL) delete m_pFits;
}

void CSummaryWriter::setFileName(const char* name)
{
	m_filename=name;
}

void CSummaryWriter::setBasePath(const char* path)
{
	m_basepath=path;
}

bool CSummaryWriter::create()
{
	IRA::CString fullName=m_basepath+m_filename;
	try {
		m_pFits=new CCfits::FITS((const char *)fullName,CCfits::Write);
		m_pFits->pHDU().writeDate();
	}
	catch (CCfits::FitsException& ex) {
		m_lastError = ex.message().c_str();
		return false;
	}
	if (!CFitsTools::primaryHeaderComment(m_pFits,DEFAULT_COMMENT,m_lastError)) return false;
	if (!CFitsTools::primaryHeaderHistory(m_pFits,HISTORY1,m_lastError)) return false;
	if (!CFitsTools::primaryHeaderHistory(m_pFits,HISTORY2,m_lastError)) return false;
	if (!CFitsTools::primaryHeaderHistory(m_pFits,HISTORY3,m_lastError)) return false;
	if (!CFitsTools::primaryHeaderHistory(m_pFits,HISTORY4,m_lastError)) return false;
	if (!CFitsTools::primaryHeaderHistory(m_pFits,HISTORY5,m_lastError)) return false;
	if (!CFitsTools::primaryHeaderHistory(m_pFits,HISTORY6,m_lastError)) return false;
	if (!CFitsTools::primaryHeaderHistory(m_pFits,HISTORY7,m_lastError)) return false;
	return true;
};
