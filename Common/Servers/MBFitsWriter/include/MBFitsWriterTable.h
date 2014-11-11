#ifndef __H__CMBFitsWriterTable__
#define __H__CMBFitsWriterTable__

#include "MBFitsWriter.h"

class CMBFitsWriterTable : public CMBFitsWriter {
	public:
		CMBFitsWriterTable( const CMBFitsWriterTable* mbFitsWriterTable_p_,
												const CMBFitsWriter::Table_e table_e_ );

		/** Default destructor */
		virtual ~CMBFitsWriterTable();

		/**
		 * Open file
		 */
		virtual void open();

		/**
		 * Close file
		 */
		virtual void close();

		virtual void addTable();
		virtual void addTableColumns( const unsigned int iteration_ = 0 );

		template<typename T>
		void getKeywordValue( const string& keyword_,
													T& value_ );

		template<typename T>
		void setKeywordValue( const string& keyword_,
													const T value_,
													const string& comment_ = string(),
													const string& referenceKeyword_ = string(),
													const string& format_ = string() );

/*
		template<typename T>
		void setKeywordValue( const CMBFitsWriter::Table_e table_e_,
													const string& keyword_,
													const T value_,
													const string& comment_ = string(),
													const string& referenceKeyword_ = string(),
													const string& format_ = string() );
*/

		template<typename T>
		void setColumnValue( const string& columnName_,
												 const T value_ );

		template<typename T>
		void setColumnValue( const string& columnName_,
												 const vector<T>& value_ );

		virtual unsigned int getRowIndex() const;
		virtual void				 setRowIndex( const unsigned int rowIndex_ );
		virtual void				 insertRows( const unsigned int rowIndexStart_,
																		 const unsigned int rows_n_ );

		string getExtName() const;

	private:
		/** Default constructor */
		CMBFitsWriterTable();																														// Not implemented

		/** Copy constructor
		 *  \param mbFitsWriterTable_ Object to copy from
		 */
		CMBFitsWriterTable( const CMBFitsWriterTable& mbFitsWriterTable_ );							// Not implemented

		/** Equal operator
		 *  \param mbFitsWriterTable_ Object to assign from
		 *  \return Equal true/false
		 */
		virtual bool operator==( const CMBFitsWriterTable& mbFitsWriterTable_ ) const;	// Not implemented

		/** notEqual operator
		 *  \param mbFitsWriterTable_ Object to assign from
		 *  \return NotEqual true/false
		 */
		virtual bool operator!=( const CMBFitsWriterTable& mbFitsWriterTable_ ) const;	// Not implemented

		/** Assignment operator
		 *  \param mbFitsWriterTable_ Object to assign from
		 *  \return A reference to this
		 */
		CMBFitsWriterTable& operator=( const CMBFitsWriterTable& mbFitsWriterTable_ );	// Not implemented

		const CMBFitsWriterTable*			m_primaryGrouping_p;
		const CMBFitsWriter::Table_e	m_table_e;
		const bool										m_fileOwner;
};

template<typename T>
void CMBFitsWriterTable::getKeywordValue( const string& keyword_,
																					T& value_ ) {
	CMBFitsWriter::getKeywordValue(m_table_e,
																 keyword_,
																 value_);
}

template<typename T>
void CMBFitsWriterTable::setKeywordValue( const string& keyword_,
																					const T value_,
																					const string& comment_,
																					const string& referenceKeyword_,
																					const string& format_ ) {
	CMBFitsWriter::setKeywordValue(m_table_e,
																 keyword_,
																 value_,
																 comment_,
																 referenceKeyword_,
																 format_);
}

/*
template<typename T>
void CMBFitsWriterTable::setKeywordValue( const CMBFitsWriter::Table_e table_e_,
																					const string& keyword_,
																					const T value_,
																					const string& comment_,
																					const string& referenceKeyword_,
																					const string& format_ ) {
	CMBFitsWriter::setKeywordValue(table_e_,
																 keyword_,
																 value_,
																 comment_,
																 referenceKeyword_,
																 format_);
}
*/

template<typename T>
void CMBFitsWriterTable::setColumnValue( const string& columnName_,
																				 const T value_ ) {
	CMBFitsWriter::setColumnValue(m_table_e,
																columnName_,
																value_);
}

template<typename T>
void CMBFitsWriterTable::setColumnValue( const string& columnName_,
																				 const vector<T>& value_ ) {
	CMBFitsWriter::setColumnValue(m_table_e,
																columnName_,
																value_);
}

#endif // __H__CMBFitsWriterTable__
