#ifndef __H__CMBFitsWriter__
#define __H__CMBFitsWriter__

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: CMBFitsWriter.h,v 0.01 2011-05-26 15:45 paolo.libardi Exp $																							 */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  When                       What													 */
/* ------------------------------------------------------------------------------------------------------------- */
/* Paolo Libardi																				26/05/2011								 Creation											 */
/* ************************************************************************************************************* */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <CCfits>
#include <cmath>
#include <exception>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using CCfits::FITS;
using CCfits::FitsException;
using CCfits::String;
using CCfits::Table;

using std::auto_ptr;
using std::exception;
using std::map;
using std::ofstream;
using std::string;
using std::stringstream;
using std::vector;

class CMBFitsWriter {
	public:
		typedef enum {
			Undefined,
			Primary,
			Scan,
			FebePar,
			ArrayData,
			Monitor,
			DataPar,
			Grouping
		} Table_e;

		static void initialize();

		static void terminate();

		/** Default constructor */
		CMBFitsWriter();

		/** Default destructor */
		virtual ~CMBFitsWriter();

		/**
		 * Get the file name.
		 *  \return the filename
		 */
		string getFileName() const;

		/**
		 * Set the file name.
		 * @param filename_ contains the file name
		 */
		void setFileName( const string& fileName_ );

		/**
		 * Get the path.
		 *  \return the path
		 */
		string getPath() const;

		/**
		 * Set the path of the file
		 * @param path_ contains the path
		 */
		void setPath( const string& path_ );

		/**
		 * Get the path+file name.
		 *  \return the path+filename
		 */
		string getFullName() const;

		/**
		 * Open file
		 */
		virtual void open();

		/**
		 * Close file
		 */
		virtual void close();

		virtual void addTable( const CMBFitsWriter::Table_e table_e_ );
		virtual void addTableColumns( const CMBFitsWriter::Table_e table_e_,
																	const int iteration_ = 0 );

		template<typename T>
		void getKeywordValue( const CMBFitsWriter::Table_e table_e_,
													const string& keyword_,
													T& value_ );

		template<typename T>
		void setKeywordValue( const CMBFitsWriter::Table_e table_e_,
													const string& keyword_,
													const T value_,
													const string& comment_ = string(),
													const string& referenceKeyword_ = string(),
													const string& format_ = string() );

		template<typename T>
		void getColumnValue( const CMBFitsWriter::Table_e table_e_,
												 const string& columnName_,
												 const unsigned int rowIndex_,
												 T& value_ );

		template<typename T>
		void getColumnValue( const CMBFitsWriter::Table_e table_e_,
												 const string& columnName_,
												 const unsigned int rowIndex_,
												 vector<T>& value_ );

		template<typename T>
		void getColumnValue( const CMBFitsWriter::Table_e table_e_,
												 const string& columnName_,
												 const unsigned int rowIndex_,
												 valarray<T>& value_ );

		template<typename T>
		void setColumnValue( const CMBFitsWriter::Table_e table_e_,
												 const string& columnName_,
												 const T value_ );

		template<typename T>
		void setColumnValue( const CMBFitsWriter::Table_e table_e_,
												 const string& columnName_,
												 const vector<T>& value_ );

		virtual void setColumnUnits( const CMBFitsWriter::Table_e table_e_,
																 const string& keyword_ );

		virtual unsigned int getRowIndex( const CMBFitsWriter::Table_e table_e_ ) const;
		virtual void				 setRowIndex( const CMBFitsWriter::Table_e table_e_,
																			const unsigned int rowIndex_ );
		virtual void				 insertRows( const CMBFitsWriter::Table_e table_e_,
																		 const unsigned int rowIndexStart_,
																		 const unsigned int rows_n_ );

		static string getExtName( const CMBFitsWriter::Table_e table_e_ );

	private:
		static bool directoryExists( const string& path_ );
		static void makeDirectory( const string& path_ );

		/** Copy constructor
		 *  \param mbFitsWriter_ Object to copy from
		 */
		CMBFitsWriter( const CMBFitsWriter& mbFitsWriter_ );										// Not implemented

		/** Equal operator
		 *  \param mbFitsWriter_ Object to assign from
		 *  \return Equal true/false
		 */
		virtual bool operator==( const CMBFitsWriter& mbFitsWriter_ ) const;		// Not implemented

		/** notEqual operator
		 *  \param mbFitsWriter_ Object to assign from
		 *  \return NotEqual true/false
		 */
		virtual bool operator!=( const CMBFitsWriter& mbFitsWriter_ ) const;		// Not implemented

		/** Assignment operator
		 *  \param mbFitsWriter_ Object to assign from
		 *  \return A reference to this
		 */
		CMBFitsWriter& operator=( const CMBFitsWriter& mbFitsWriter_ );					// Not implemented

		bool directoryExists() const;
		void makeDirectory() const;

	protected:
		FITS*		m_mbFits_p;

	private:
		class Keyword {
			public:
				typedef map<string, Keyword>								KeywordKeyword_m_t;
				typedef KeywordKeyword_m_t::iterator				KeywordKeyword_i_m_t;
				typedef KeywordKeyword_m_t::const_iterator	KeywordKeyword_ci_m_t;

				Keyword();
				Keyword( const Keyword& keyword_ );
				Keyword( const string& keyword_, const string& format_, const string& units_);
				virtual ~Keyword();

				bool operator==( const Keyword& keyword_ ) const;
				bool operator!=( const Keyword& keyword_ ) const;
				Keyword& operator=( const Keyword& keyword_ );

				string getKeyword() const;
				string getFormat()	const;
				string getUnits()		const;

			private:
				string m_keyword;
				string m_format;
				string m_units;
		};

		typedef map<CMBFitsWriter::Table_e, string>							TableName_m_t;
		typedef TableName_m_t::iterator						      				TableName_i_m_t;
		typedef TableName_m_t::const_iterator										TableName_ci_m_t;

		typedef map<CMBFitsWriter::Table_e, Table*>							TableTable_m_p_t;
		typedef TableTable_m_p_t::iterator											TableTable_i_m_p_t;
		typedef TableTable_m_p_t::const_iterator								TableTable_ci_m_p_t;

		typedef map<CMBFitsWriter::Table_e, unsigned int>				TableRowIndex_m_t;
		typedef TableRowIndex_m_t::iterator											TableRowIndex_i_m_t;
		typedef TableRowIndex_m_t::const_iterator								TableRowIndex_ci_m_t;

		typedef map<string, string>															KeywordFormat_m_t;
		typedef KeywordFormat_m_t::iterator											KeywordFormat_i_m_t;
		typedef KeywordFormat_m_t::const_iterator								KeywordFormat_ci_m_t;

		typedef map<CMBFitsWriter::Table_e, KeywordFormat_m_t>	TableKeywordFormat_m_t;
		typedef TableKeywordFormat_m_t::iterator								TableKeywordFormat_i_m_t;
		typedef TableKeywordFormat_m_t::const_iterator					TableKeywordFormat_ci_m_t;

		static CMBFitsWriter::TableName_m_t											m_tablesNames;

		static CMBFitsWriter::TableKeywordFormat_m_t						m_tablesKeywordsFormats;
		static CMBFitsWriter::Keyword::KeywordKeyword_m_t				m_keywords;

		string getKeywordFormat( const CMBFitsWriter::Table_e table_e_,
														 const string& keyword_ );

		template<typename T>
		void setKeyword( const CMBFitsWriter::Table_e table_e_,
										 const string& keyword_,
										 const T value_,
										 const string& comment_ );

		void getColumns( const CMBFitsWriter::Table_e table_e_,
                     vector<string>& columnNames_,
                     vector<string>& columnFormats_,
                     vector<string>& columnUnits_,
                     const unsigned int iteration_ = 0 );

		string	m_fileName;
		string	m_path;

		CMBFitsWriter::TableTable_m_p_t  m_tablesTables;
		CMBFitsWriter::TableRowIndex_m_t m_tablesRowIndices;
};

template<typename T>
void CMBFitsWriter::getKeywordValue( const CMBFitsWriter::Table_e table_e_,
																		 const string& keyword_,
																		 T& value_ ) {
	if ( CMBFitsWriter::Undefined != table_e_ ) {
		const string extName(CMBFitsWriter::getExtName(table_e_));

		try {
		  if ( Primary == table_e_ ) {
		    m_mbFits_p->pHDU().keyWord(keyword_).value(value_);
		  } else {
        m_mbFits_p->extension(extName).keyWord(keyword_).value(value_);
		  }
		} catch( FitsException& exception_ ) {
			throw exception_;
		}
	}
}

template<typename T>
void CMBFitsWriter::setKeywordValue( const CMBFitsWriter::Table_e table_e_,
																		 const string& keyword_,
																		 const T value_,
																		 const string& comment_,
																		 const string& referenceKeyword_,
																		 const string& format_ ) {
	if ( CMBFitsWriter::Undefined != table_e_ ) {
		try {
			string format(!referenceKeyword_.empty() ? CMBFitsWriter::getKeywordFormat(table_e_, referenceKeyword_)
																							 : (!format_.empty() ? format_ : CMBFitsWriter::getKeywordFormat(table_e_, keyword_)));
			// TODO - segnalare correttamente l'errore - gestire il caso di nuove keyword non previste dallo standard
			if ( format.empty() ) throw exception();

      int		 typeCode = 0;
      long	 repeat   = 0;
      long	 width    = 0;
      int		 status   = 0;

			fits_binary_tform(const_cast<char*>(format.c_str()), &typeCode, &repeat, &width, &status);

			// typecode - datatype code of the table column.
			// The negative of the value indicates a variable length array column.
			switch( typeCode ) {
				case   1: {		// X - bit
						// TODO - set value of correct type
						// TODO - se utilizzo mask per ottenere il valore di un singolo bit ATTENZIONE a low-order/high-order -> non voglio che il codice dipenda dall'architettura in modo statico
						// TODO - warn for incorrect type passed as argument
						setKeyword(table_e_, keyword_, value_, comment_);
					}
					break;
				case  11: {		// B - byte
						// TODO - set value of correct type
						// TODO - se utilizzo mask per ottenere il valore di un singolo byte ATTENZIONE a low-order/high-order -> non voglio che il codice dipenda dall'architettura in modo statico
						// TODO - warn for incorrect type passed as argument
						setKeyword(table_e_, keyword_, value_, comment_);
					}
					break;
				case  14: {		// L - logical
						// TODO - set value of correct type
						// TODO - warn for incorrect type passed as argument
						setKeyword(table_e_, keyword_, value_, comment_);
					}
					break;
				case  16: {		// A - ASCII character
						stringstream valueStr;
						valueStr << value_;
						string value(valueStr.str().substr(0, repeat));

						// TODO - warn for incorrect type passed as argument
						if ( value != valueStr.str() ) { /* TODO */ }

						setKeyword(table_e_, keyword_, value, comment_);
					}
					break;
				case  21: {		// I - short integer
						// TODO - set value of correct type
						// TODO - warn for incorrect type passed as argument
						setKeyword(table_e_, keyword_, value_, comment_);
					}
					break;
				case  41: {		// J - integer
						// TODO - set value of correct type
						// TODO - warn for incorrect type passed as argument
						setKeyword(table_e_, keyword_, value_, comment_);
					}
					break;
				case  42: {		// E - real
						// TODO - set value of correct type
						// TODO - warn for incorrect type passed as argument
						setKeyword(table_e_, keyword_, value_, comment_);
					}
					break;
				case  82: {		// D - double precision
						// TODO - set value of correct type
						// TODO - warn for incorrect type passed as argument
						setKeyword(table_e_, keyword_, value_, comment_);
					}
					break;
				case  83: {		// C - complex
						// TODO - set value of correct type
						// TODO - warn for incorrect type passed as argument
						setKeyword(table_e_, keyword_, value_, comment_);
					}
					break;
				case 163: {		// M - double complex
						// TODO - set value of correct type
						// TODO - warn for incorrect type passed as argument
						setKeyword(table_e_, keyword_, value_, comment_);
					}
					break;
			}
		} catch( FitsException& exception_ ) {
			throw exception_;
		}
	}
}

template<typename T>
void CMBFitsWriter::setKeyword( const CMBFitsWriter::Table_e table_e_,
																const string& keyword_,
																const T value_,
																const string& comment_ ) {
	if ( CMBFitsWriter::Undefined != table_e_ ) {
		const string extName(CMBFitsWriter::getExtName(table_e_));

		try {
			if ( Primary == table_e_ ) {
				m_mbFits_p->pHDU().addKey(keyword_, value_, comment_);
			} else {
				m_mbFits_p->extension(extName).addKey(keyword_, value_, comment_);
			}
		} catch( FitsException& exception_ ) {
			throw exception_;
		}
	}
}

template<typename T>
void CMBFitsWriter::getColumnValue( const CMBFitsWriter::Table_e table_e_,
																		const string& columnName_,
																		const unsigned int rowIndex_,
																		T& value_ ) {
	if ( CMBFitsWriter::Undefined != table_e_ ) {
		const string extName(CMBFitsWriter::getExtName(table_e_));

		vector<T> values;

		try {
		  if ( Primary == table_e_ ) {
		    // TODO

        throw exception();
		  } else {
        m_mbFits_p->extension(extName).column(columnName_).read(value_, rowIndex_, rowIndex_ + 1);
			}
		} catch( FitsException& exception_ ) {
			values.clear();

			throw exception_;
		}

		values.clear();
	}
}

template<typename T>
void CMBFitsWriter::getColumnValue( const CMBFitsWriter::Table_e table_e_,
																		const string& columnName_,
																		const unsigned int rowIndex_,
																		vector<T>& value_ ) {
	if ( CMBFitsWriter::Undefined != table_e_ ) {
		const string extName(CMBFitsWriter::getExtName(table_e_));

		valarray<T> values;

		try {
		  if ( Primary == table_e_ ) {
		    // TODO

        throw exception();
		  } else {
        m_mbFits_p->extension(extName).column(columnName_).read(values, rowIndex_);
				for ( size_t index = 0; index < values.size(); ++index ) {
					value_.push_back(values[index]);
				}
			}
		} catch( FitsException& exception_ ) {
			throw exception_;
		}
	}
}

template<typename T>
void CMBFitsWriter::getColumnValue( const CMBFitsWriter::Table_e table_e_,
																		const string& columnName_,
																		const unsigned int rowIndex_,
																		valarray<T>& value_ ) {
	if ( CMBFitsWriter::Undefined != table_e_ ) {
		const string extName(CMBFitsWriter::getExtName(table_e_));

		try {
		  if ( Primary == table_e_ ) {
		    // TODO

        throw exception();
		  } else {
		  	if ( (0 > m_mbFits_p->extension(extName).column(columnName_).type()) ||
						 (1 < m_mbFits_p->extension(extName).column(columnName_).repeat() ) ) {
					m_mbFits_p->extension(extName).column(columnName_).read(value_, rowIndex_);
				} else {
					m_mbFits_p->extension(extName).column(columnName_).read(value_, rowIndex_, rowIndex_ + 1);
				}
			}
		} catch( FitsException& exception_ ) {
			throw exception_;
		}
	}
}

template<typename T>
void CMBFitsWriter::setColumnValue( const CMBFitsWriter::Table_e table_e_,
																	  const string& columnName_,
																	  const T value_ ) {
	if ( CMBFitsWriter::Undefined != table_e_ ) {
		const string extName(CMBFitsWriter::getExtName(table_e_));

		CMBFitsWriter::TableRowIndex_ci_m_t	tableRowIndex_ci = m_tablesRowIndices.find(table_e_);
		if ( tableRowIndex_ci == m_tablesRowIndices.end()					 ) throw exception();

		vector<T> values;

		try {
			values.push_back(value_);

		  if ( Primary == table_e_ ) {
		    // TODO
        values.clear();

        printf("setColumnValue - 1: %s, %s\n", extName.c_str(), columnName_.c_str());
        throw exception();
		  } else {
        m_mbFits_p->extension(extName).column(columnName_).write(values, tableRowIndex_ci->second);
		  }
		} catch( FitsException& exception_ ) {
			values.clear();

			printf("setColumnValue - 2: %s, %s\n", extName.c_str(), columnName_.c_str());
			throw exception_;
		}

		values.clear();
	}
}

template<typename T>
void CMBFitsWriter::setColumnValue( const CMBFitsWriter::Table_e table_e_,
																	  const string& columnName_,
																	  const vector<T>& value_ ) {
	// TODO - gestire eccezione in caso di elenco di valori empty
	if ( CMBFitsWriter::Undefined != table_e_ ) {
		CMBFitsWriter::TableName_ci_m_t			tableName_ci		 = CMBFitsWriter::m_tablesNames.find(table_e_);
		if ( tableName_ci			== CMBFitsWriter::m_tablesNames.end() ) throw exception();

		CMBFitsWriter::TableRowIndex_ci_m_t	tableRowIndex_ci = m_tablesRowIndices.find(table_e_);
		if ( tableRowIndex_ci == m_tablesRowIndices.end()						) throw exception();

		std::vector<long> vectorLengths;

		try {
			vectorLengths.push_back(value_.size());

		  if ( Primary == table_e_ ) {
		    // TODO
        vectorLengths.clear();

        printf("setColumnValue - 3: %d, %s\n", table_e_, columnName_.c_str());
        throw exception();
		  } else {
		  	if ( (0 > m_mbFits_p->extension(tableName_ci->second).column(columnName_).type()) ||
						 (1 < m_mbFits_p->extension(tableName_ci->second).column(columnName_).repeat() ) ) {
					m_mbFits_p->extension(tableName_ci->second).column(columnName_).write(value_, vectorLengths, tableRowIndex_ci->second);
				} else {
					setColumnValue(table_e_, columnName_, value_.front());
				}
		  }
		} catch( FitsException& exception_ ) {
			vectorLengths.clear();

       printf("setColumnValue - 4: %d, %s\n", table_e_, columnName_.c_str());
			throw exception_;
		}

		vectorLengths.clear();
	}
}

#endif // __H__CMBFitsWriter__
