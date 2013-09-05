#ifndef __H__Layout__
#define __H__Layout__

#include <map>
#include <string>

using std::map;
using std::string;

#include <iostream>
using std::cout;
using std::endl;

class Layout {
	public:
		Layout( const string& id_ );
		Layout( const Layout& layout_ );
		virtual ~Layout();

		virtual bool operator==( const Layout& layout_ ) const;
		virtual bool operator!=( const Layout& layout_ ) const;
		Layout& operator=( const Layout& layout_ );

		string getId() const;

		void insert( const string& label_, string& value_ );
		string find( const string& label_ ) const;

	private:
		typedef map<string, string>							LabelValue_m_t;
		typedef LabelValue_m_t::iterator				LabelValue_i_m_t;
		typedef LabelValue_m_t::const_iterator	LabelValue_ci_m_t;

		Layout();	// Not implemented

		string									m_id;
		Layout::LabelValue_m_t	m_labelsValues;
};

#endif // __H__Layout__
