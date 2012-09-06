#include "LayoutCollection.h"

#include <exception>
using std::exception;

LayoutCollection::LayoutCollection() : m_IdsLayouts(LayoutCollection::IdLayout_p_m_t()) {
}

LayoutCollection::~LayoutCollection() {
	clear();
}

LayoutCollection::LayoutCollection( const LayoutCollection& layoutCollection_ ) : m_IdsLayouts(LayoutCollection::IdLayout_p_m_t()) {
	for ( LayoutCollection::IdLayout_p_ci_m_t idLayout_ci = layoutCollection_.m_IdsLayouts.begin();
				idLayout_ci != layoutCollection_.m_IdsLayouts.end(); ++idLayout_ci ) {
		m_IdsLayouts.insert(LayoutCollection::IdLayout_p_m_t::value_type(idLayout_ci->first, new Layout(*idLayout_ci->second)));
	}
}

bool LayoutCollection::operator==( const LayoutCollection& layoutCollection_ ) const {
	if ( m_IdsLayouts.size() != layoutCollection_.m_IdsLayouts.size() ) return false;

	for ( LayoutCollection::IdLayout_p_ci_m_t idLayout1_ci = m_IdsLayouts.begin(), idLayout2_ci = layoutCollection_.m_IdsLayouts.begin();
				(idLayout1_ci != m_IdsLayouts.end()) && (idLayout2_ci != m_IdsLayouts.end()); ++idLayout1_ci, ++idLayout2_ci ) {
		if ( idLayout1_ci->first	!= idLayout2_ci->first	) return false;

		if ( (idLayout1_ci->second != idLayout2_ci->second) &&
				 !idLayout1_ci->second && !idLayout2_ci->second &&
				 (*idLayout1_ci->second != *idLayout2_ci->second) ) return false;
	}

	return true;
}

bool LayoutCollection::operator!=( const LayoutCollection& layoutCollection_ ) const {
	return !(*this == layoutCollection_);
}

LayoutCollection& LayoutCollection::operator=( const LayoutCollection& layoutCollection_ ) {
	if ( *this != layoutCollection_ ) {
		m_IdsLayouts.clear();

		for ( LayoutCollection::IdLayout_p_ci_m_t idLayout_ci = layoutCollection_.m_IdsLayouts.begin();
					idLayout_ci != layoutCollection_.m_IdsLayouts.end(); ++idLayout_ci ) {
			m_IdsLayouts.insert(LayoutCollection::IdLayout_p_m_t::value_type(idLayout_ci->first, new Layout(*idLayout_ci->second)));
		}
	}

	return *this;
}

void LayoutCollection::clear() {
	for ( LayoutCollection::IdLayout_p_i_m_t idLayout_i = m_IdsLayouts.begin();
				idLayout_i != m_IdsLayouts.end(); ++idLayout_i ) {
		delete idLayout_i->second;
	}
	m_IdsLayouts.clear();
}

const Layout* const LayoutCollection::find( const string& id_ ) const {
	const LayoutCollection::IdLayout_p_ci_m_t idLayout_ci = m_IdsLayouts.find(id_);
	if ( idLayout_ci != m_IdsLayouts.end() ) return idLayout_ci->second;

	return NULL;
}

void LayoutCollection::insert( const Layout* const layout_p_ ) {
	LayoutCollection::IdLayout_p_i_m_t idLayout_i = m_IdsLayouts.find(layout_p_->getId());
	if ( idLayout_i != m_IdsLayouts.end() ) {
		delete idLayout_i->second;
		m_IdsLayouts.erase(idLayout_i);
	}

	m_IdsLayouts.insert(LayoutCollection::IdLayout_p_m_t::value_type(layout_p_->getId(), layout_p_));
}

void LayoutCollection::parse( const ACS::stringSeq& scanLayout_ ) {
	string scanLayoutText;
	size_t posBegin 				= string::npos;
	size_t posEnd						= string::npos;
	size_t posSeparator			= string::npos;
	Layout *layoutScan_p		= new Layout("SCAN");
	Layout *layoutSubScan_p	= NULL;
	Layout *layout_p				= layoutScan_p;

	clear();

	for ( unsigned long indexLayout = 0; indexLayout < scanLayout_.length(); ++indexLayout ) {
		scanLayoutText = string(scanLayout_[indexLayout]);

		if ( scanLayoutText.empty() ) continue;

		if ( string::npos != (posBegin = scanLayoutText.find('[')) ) {
			string id(scanLayoutText.substr(0, posBegin));
			layoutSubScan_p = new Layout(id);
			layout_p				= layoutSubScan_p;
			continue;
		}

		if ( string::npos != (posEnd = scanLayoutText.find(']')) ) {
			insert(layout_p);
			layout_p = NULL;
			continue;
		}

		if ( !layout_p ) {
			printf("Wrong syntax!\n");
			throw exception();
		}

		posSeparator = scanLayoutText.find('=');
		string label(scanLayoutText.substr(0, posSeparator));
		string value(scanLayoutText.substr(posSeparator + 1));
		layout_p->insert(label, value);
	}

	insert(layoutScan_p);
}
