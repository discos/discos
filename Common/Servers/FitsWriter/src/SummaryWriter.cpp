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
}

CSummaryWriter::~CSummaryWriter()
{
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
	m_file=new FitsWriter_private::CFile();
	init();
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
	//m_file->begin();
	//CEntry *cc;
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

	return m_file->write(m_pFits,m_lastError);
	/*while ((cc=m_file->getNextEntry())!=NULL) {
		if (cc->getType()==_FILE_STRING_TYPE_S) {
			if (cc->isSimple()) {
				CHeaderEntry<_FILE_STRING_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,false> *>(cc);
				_FILE_STRING_TYPE::TEntryReference val=p->getValue();
				if (!CFitsTools::setPrimaryHeaderKey(m_pFits,p->getKeyword(),val,p->getDescription(),m_lastError)) return false;
			}
			else {
				CHeaderEntry<_FILE_STRING_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,true> *>(cc);
				std::list<_FILE_STRING_TYPE::TEntryReference> val=p->getValue();
				if (!CFitsTools::setPrimaryHeaderKey(m_pFits,p->getKeyword(),val,p->getDescription(),m_lastError)) return false;
			}
		} else if (cc->getType()==_FILE_DOUBLE_TYPE_S) {
			if (cc->isSimple()) {
				CHeaderEntry<_FILE_DOUBLE_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,false> *>(cc);
				_FILE_DOUBLE_TYPE::TEntryReference val=p->getValue();
				if (!CFitsTools::setPrimaryHeaderKey(m_pFits,p->getKeyword(),val,p->getDescription(),m_lastError)) return false;
			}
			else {
				CHeaderEntry<_FILE_DOUBLE_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,true> *>(cc);
				std::list<_FILE_DOUBLE_TYPE::TEntryReference> val=p->getValue();
				if (!CFitsTools::setPrimaryHeaderKey(m_pFits,p->getKeyword(),val,p->getDescription(),m_lastError)) return false;
			}
		} else {  // _FILE_LONG_TYPE_S
			if (cc->isSimple()) {
				CHeaderEntry<_FILE_LONG_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,false> *>(cc);
				_FILE_LONG_TYPE::TEntryReference val=p->getValue();
				if (!CFitsTools::setPrimaryHeaderKey(m_pFits,p->getKeyword(),val,p->getDescription(),m_lastError)) return false;
			}
			else {
				CHeaderEntry<_FILE_LONG_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,true> *>(cc);
				std::list<_FILE_LONG_TYPE::TEntryReference> val=p->getValue();
				if (!CFitsTools::setPrimaryHeaderKey(m_pFits,p->getKeyword(),val,p->getDescription(),m_lastError)) return false;
			}
		}
	}
	return true;*/


}


