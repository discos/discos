#include "Version.h"
#include "SummaryWriter.h"
#include "FitsTools.h"
#include <Definitions.h>
#include <IRATools.h>
// imports definition and the structure of the file
#include "SummarySchema.h"

using namespace FitsWriter_private;

void CSummaryWriter::init()
{
	#define _FILE_LOCAL_FILE m_file
	#include "FileCreator.h"
	#undef _FILE_LOCAL_FILE
	m_file->setKeyword("FITSVER",IRA::CString(CURRENT_VERSION));
}

CSummaryWriter::CSummaryWriter() : m_lastError(""),m_filename("summary.fits"),m_basepath("~/"),m_pFits(NULL), m_file(NULL)
{
	m_file=new FitsWriter_private::CFile();
	init();
}

CSummaryWriter::~CSummaryWriter()
{
	if (!m_fitsWritten) write();
	close();
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
	if (!CFitsTools::primaryHeaderHistory(m_pFits,HISTORY8,m_lastError)) return false;
	m_fitsWritten=false;
	return true;
};

bool CSummaryWriter::close()
{
	if (m_pFits!=NULL) {
		if (!CFitsTools::flush(m_pFits,m_lastError)) return false;
		delete m_pFits;
	}
	m_pFits=NULL;
	if (m_file!=NULL) delete m_file;
	m_file=NULL;
	return true;
}

bool CSummaryWriter::write()
{
	if (m_file && m_pFits) {
		m_fitsWritten=true;
		return m_file->write(m_pFits,m_lastError);
	}
	else {
		m_lastError="file not properly initialized";
		return false;
	}
}


