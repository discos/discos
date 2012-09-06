#ifndef __H__Baseband__
#define __H__Baseband__

#include <BackendsDefinitionsC.h>

#include <limits>
#include <map>
#include <set>
#include <vector>

using std::numeric_limits;
using std::map;
using std::pair;
using std::set;
using std::vector;

class Baseband {
	public:
		class Section {
			public:
				typedef vector<Section>									Section_v_t;
				typedef Section_v_t::iterator						Section_i_v_t;
				typedef Section_v_t::const_iterator			Section_ci_v_t;

				Section( const Section& section_ );
				Section( const Backends::TSectionHeader& sectionHeader_,
								 const long indexInput_ );
				virtual ~Section();

				virtual bool operator==( const Section& section_ ) const;
				virtual bool operator!=( const Section& section_ ) const;
				Section& operator=( const Section& section_ );

				long getID() const;

				Backends::TPolarization getPolarization() const;
				long getFeed()	 const;
				long getInputs() const;

				long getIDFeed() const;

			private:
				Section();	// Not implemented

				Backends::TSectionHeader	m_sectionHeader;
				long											m_indexInput;
		};

		typedef set<Baseband>									Baseband_s_t;
		typedef Baseband_s_t::iterator				Baseband_i_s_t;
		typedef Baseband_s_t::const_iterator	Baseband_ci_s_t;

		Baseband( const Baseband& baseband_ );
		Baseband( const double frequency_,
							const double bandWidth_,
							const long	 IFPolarization_ );
		virtual ~Baseband();

		virtual bool operator==( const Baseband& baseband_ ) const;
		virtual bool operator!=( const Baseband& baseband_ ) const;
		Baseband& operator=( const Baseband& baseband_ );

		virtual bool operator<( const Baseband& baseband_ ) const;

		virtual bool isSimilar( const Baseband& baseband_ ) const;

		void insert( const Baseband::Section& section_ );

		double getBandWidth() const;
		void getSections( Baseband::Section::Section_v_t& sections_ ) const;

	private:
		Baseband();		// Not implemented

		double			m_frequency;
		double			m_bandWidth;
		long				m_IFPolarization;

		Baseband::Section::Section_v_t m_sections;
};

#endif // __H__Baseband__
