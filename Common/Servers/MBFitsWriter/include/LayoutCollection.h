#ifndef __H__LayoutCollection__
#define __H__LayoutCollection__

#include "Layout.h"

#include "CommonTools.h"

class LayoutCollection {
	public:
		LayoutCollection();
		LayoutCollection( const LayoutCollection& layoutCollection_ );
		virtual ~LayoutCollection();

		virtual bool operator==( const LayoutCollection& layoutCollection_ ) const;
		virtual bool operator!=( const LayoutCollection& layoutCollection_ ) const;
		LayoutCollection& operator=( const LayoutCollection& layoutCollection_ );

		const Layout* const find( const string& id_ ) const;
		void insert( const Layout* const layout_p_ );

		void parse( const ACS::stringSeq& scanLayout_ );

	private:
		typedef map<string, Layout*>						IdLayout_p_m_t;
		typedef IdLayout_p_m_t::iterator				IdLayout_p_i_m_t;
		typedef IdLayout_p_m_t::const_iterator	IdLayout_p_ci_m_t;

		void clear();

		LayoutCollection::IdLayout_p_m_t	m_IdsLayouts;
};

#endif // __H__LayoutCollection__
