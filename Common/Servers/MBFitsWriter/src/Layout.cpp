#include "Layout.h"

/*
Layout::Layout() : m_id(string()),
									 m_labelsValues(Layout::LabelValue_m_t()) {
}
*/

Layout::Layout( const string& id_ ) : m_id(id_),
																			m_labelsValues(Layout::LabelValue_m_t()) {
}

Layout::~Layout() {
	m_labelsValues.clear();
}

Layout::Layout( const Layout& layout_ ) : m_id(string()),
																					m_labelsValues(Layout::LabelValue_m_t()) {
	for ( Layout::LabelValue_ci_m_t labelValues_ci = layout_.m_labelsValues.begin();
				labelValues_ci != layout_.m_labelsValues.end(); ++labelValues_ci ) {
		m_labelsValues.insert(Layout::LabelValue_m_t::value_type(labelValues_ci->first, labelValues_ci->second));
	}
}

bool Layout::operator==( const Layout& layout_ ) const {
	if ( m_id != layout_.m_id ) return false;

	if ( m_labelsValues.size() != layout_.m_labelsValues.size() ) return false;

	for ( Layout::LabelValue_ci_m_t labelValues1_ci = m_labelsValues.begin(), labelValues2_ci = layout_.m_labelsValues.begin();
				(labelValues1_ci != m_labelsValues.end()) && (labelValues2_ci != layout_.m_labelsValues.end()); ++labelValues1_ci, ++labelValues2_ci ) {
		if ( labelValues1_ci->first	 != labelValues2_ci->first	) return false;
		if ( labelValues1_ci->second != labelValues2_ci->second ) return false;
	}

	return true;
}

bool Layout::operator!=( const Layout& layout_ ) const {
	return !(*this == layout_);
}

Layout& Layout::operator=( const Layout& layout_ ) {
	if ( *this != layout_ ) {
		m_id = layout_.m_id;

		m_labelsValues.clear();

		for ( Layout::LabelValue_ci_m_t labelValues_ci = layout_.m_labelsValues.begin();
					labelValues_ci != layout_.m_labelsValues.end(); ++labelValues_ci ) {
			m_labelsValues.insert(Layout::LabelValue_m_t::value_type(labelValues_ci->first, labelValues_ci->second));
		}
	}

	return *this;
}

string Layout::getId() const {
	return m_id;
}

void Layout::insert( const string& label_, string& value_ ) {
	m_labelsValues.insert(Layout::LabelValue_m_t::value_type(label_, value_));
}

string Layout::find( const string& label_ ) const {
	const Layout::LabelValue_ci_m_t labelValues_ci = m_labelsValues.find(label_);
	return (labelValues_ci != m_labelsValues.end()) ? labelValues_ci->second : string();
}
