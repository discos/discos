#include "MBFitsWriterTable.h"

/*	// Not implemented
CMBFitsWriterTable::CMBFitsWriterTable() : CMBFitsWriter(),
																					 m_primaryGrouping_p(NULL),
																					 m_table_e(CMBFitsWriter::Undefined),
																					 m_fileOwner(false) {
}
*/

CMBFitsWriterTable::CMBFitsWriterTable( const CMBFitsWriterTable* mbFitsWriterTable_p_,
																				const CMBFitsWriter::Table_e table_e_ ) : CMBFitsWriter(),
																																									m_primaryGrouping_p(mbFitsWriterTable_p_),
																																									m_table_e(table_e_),
																																									m_fileOwner(!m_primaryGrouping_p || (CMBFitsWriter::Grouping == m_primaryGrouping_p->m_table_e)) {
}

/*	// Not implemented
CMBFitsWriterTable::CMBFitsWriterTable( const CMBFitsWriterTable& mbFitsWriterTable_ ) : CMBFitsWriter(mbFitsWriterTable_),
																																												 m_primaryGrouping_p(mbFitsWriterTable_.m_primaryGrouping_p),
																																												 m_table_e(mbFitsWriterTable_.m_table_e),
																																												 m_fileOwner(mbFitsWriterTable_.m_fileOwner) {
}
*/

CMBFitsWriterTable::~CMBFitsWriterTable() {
}

bool CMBFitsWriterTable::operator==( const CMBFitsWriterTable& mbFitsWriterTable_ ) const {
	return false;

/*	// Not implemented
	return CMBFitsWriter::operator==(mbFitsWriterTable_) &&
				 ( ((!m_primaryGrouping_p && !mbFitsWriterTable_.m_primaryGrouping_p) ||
						( m_primaryGrouping_p &&  mbFitsWriterTable_.m_primaryGrouping_p && (*m_primaryGrouping_p == *mbFitsWriterTable_.m_primaryGrouping_p))) &&
					 (m_table_e		== mbFitsWriterTable_.m_table_e		) &&
					 (m_fileOwner	== mbFitsWriterTable_.m_fileOwner	) );
*/
}

bool CMBFitsWriterTable::operator!=( const CMBFitsWriterTable& mbFitsWriterTable_ ) const {
	return !(*this == mbFitsWriterTable_);
}

/*	// Not implemented
CMBFitsWriterTable& CMBFitsWriterTable::operator=( const CMBFitsWriterTable& mbFitsWriterTable_ ) {
	// handle self assignment
	if ( *this != mbFitsWriterTable_ ) {
		CMBFitsWriter::operator=(mbFitsWriterTable_);

		m_primaryGrouping_p	= mbFitsWriterTable_.m_primaryGrouping_p;
		m_table_e						= mbFitsWriterTable_.m_table_e;
		m_fileOwner					= mbFitsWriterTable_.m_fileOwner;
	}

	//assignment operator
	return *this;
}
*/

void CMBFitsWriterTable::open() {
	if ( m_fileOwner ) {
		CMBFitsWriter::open();
	} else {
		CMBFitsWriter::m_mbFits_p = m_primaryGrouping_p->m_mbFits_p;
	}
}

void CMBFitsWriterTable::close() {
	if ( m_fileOwner ) {
		CMBFitsWriter::close();
	}
}

void CMBFitsWriterTable::addTable() {
	CMBFitsWriter::addTable(m_table_e);
}

void CMBFitsWriterTable::addTableColumns( const unsigned int iteration_ ) {
	CMBFitsWriter::addTableColumns(m_table_e, iteration_);
}

unsigned int CMBFitsWriterTable::getRowIndex() const {
	return CMBFitsWriter::getRowIndex(m_table_e);
}

void CMBFitsWriterTable::setRowIndex( const unsigned int rowIndex_ ) {
	CMBFitsWriter::setRowIndex(m_table_e, rowIndex_);
}

void CMBFitsWriterTable::insertRows( const unsigned int rowIndexStart_,
																		 const unsigned int rows_n_ ) {
	CMBFitsWriter::insertRows(m_table_e, rowIndexStart_, rows_n_);
}

string CMBFitsWriterTable::getExtName() const {
	return CMBFitsWriter::getExtName(m_table_e);
}
