#include "Baseband.h"

/*
Baseband::Baseband() : m_frequency(0.0),
											 m_bandWidth(0.0),
											 m_IFPolarization(0),
											 m_sections(Baseband::SectionIF_v_t()) {
}
*/

Baseband::Baseband( const Baseband& baseband_ ) : m_frequency(baseband_.m_frequency),
																									m_bandWidth(baseband_.m_bandWidth),
																									m_IFPolarization(baseband_.m_IFPolarization),
																									m_sections(baseband_.m_sections) {
}

Baseband::Baseband( const double frequency_,
										const double bandWidth_,
										const long IFPolarization_ ) : m_frequency(frequency_),
																									 m_bandWidth(bandWidth_),
																									 m_IFPolarization(IFPolarization_),
																									 m_sections(Baseband::Section::Section_v_t()) {
}

Baseband::~Baseband() {
}

bool Baseband::operator==( const Baseband& baseband_ ) const {
	return ( (m_frequency				== baseband_.m_frequency		 ) &&
					 (m_bandWidth				== baseband_.m_bandWidth		 ) &&
					 (m_IFPolarization	== baseband_.m_IFPolarization) &&
					 (m_sections				== baseband_.m_sections			 ) );
}

bool Baseband::operator!=( const Baseband& baseband_ ) const {
	return !(*this == baseband_);
}

Baseband& Baseband::operator=( const Baseband& baseband_ ) {
	if ( *this != baseband_ ) {
		m_frequency				= baseband_.m_frequency;
		m_bandWidth				= baseband_.m_bandWidth;
		m_IFPolarization	= baseband_.m_IFPolarization;

		m_sections.clear();
		m_sections				= baseband_.m_sections;
	}

	return *this;
}

bool Baseband::operator<( const Baseband& baseband_ ) const {
	return ( (m_frequency < baseband_.m_frequency) ||
					 ((fabs(m_frequency - baseband_.m_frequency) < numeric_limits<double>::epsilon()) &&
						(m_bandWidth < baseband_.m_bandWidth)) ||
					 ((fabs(m_frequency - baseband_.m_frequency) < numeric_limits<double>::epsilon()) &&
						(fabs(m_bandWidth - baseband_.m_bandWidth) < numeric_limits<double>::epsilon()) &&
						(m_IFPolarization < baseband_.m_IFPolarization)) );
}

bool Baseband::isSimilar( const Baseband& baseband_ ) const {
	return ( (fabs(m_frequency - baseband_.m_frequency) < numeric_limits<double>::epsilon()) &&
					 (fabs(m_bandWidth - baseband_.m_bandWidth) < numeric_limits<double>::epsilon()) &&
					 (m_IFPolarization	== baseband_.m_IFPolarization) );
}

void Baseband::insert( const Baseband::Section& section_ ) {
	m_sections.push_back(section_);
}

double Baseband::getBandWidth() const {
	return m_bandWidth;
}

void Baseband::getSections( Baseband::Section::Section_v_t& sections_ ) const {
	sections_ = m_sections;
}

// ******* class Section ******* //
/*
Baseband::Section::Section() : //m_sectionHeader(Backends::TSectionHeader{0, 0, BKND_LCP, 0.0, 0.0, [0.0, 0.0], 0.0, 0, 0, [0, 0]}), // TODO
															 m_indexInput(0) {
}
*/

Baseband::Section::Section( const Section& section_ ) : m_sectionHeader(section_.m_sectionHeader),
																												m_indexInput(section_.m_indexInput) {
}

Baseband::Section::Section( const Backends::TSectionHeader& sectionHeader_,
														const long indexInput_ ) : m_sectionHeader(sectionHeader_),
																											 m_indexInput(indexInput_) {
}

Baseband::Section::~Section() {
}

bool Baseband::Section::operator==( const Section& section_ ) const {
	return ( (m_sectionHeader.id								== section_.m_sectionHeader.id						) &&
					 (m_sectionHeader.bins							== section_.m_sectionHeader.bins					) &&
					 (m_sectionHeader.polarization			== section_.m_sectionHeader.polarization	) &&
					 (m_sectionHeader.bandWidth					== section_.m_sectionHeader.bandWidth			) &&
					 (m_sectionHeader.frequency					== section_.m_sectionHeader.frequency			) &&

					 (((m_sectionHeader.inputs					== section_.m_sectionHeader.inputs				) &&
						 (1																== m_sectionHeader.inputs									) &&
						 (m_sectionHeader.attenuation[0]	== section_.m_sectionHeader.attenuation[0])) ||
						((m_sectionHeader.inputs					== section_.m_sectionHeader.inputs				) &&
						 (2																== m_sectionHeader.inputs									) &&
						 (m_sectionHeader.attenuation[0]	== section_.m_sectionHeader.attenuation[0]) &&
						 (m_sectionHeader.attenuation[1]	== section_.m_sectionHeader.attenuation[1]))) &&

					 (m_sectionHeader.sampleRate				== section_.m_sectionHeader.sampleRate		) &&
					 (m_sectionHeader.feed							== section_.m_sectionHeader.feed					) &&
					 (m_sectionHeader.inputs						== section_.m_sectionHeader.inputs				) &&

					 (((m_sectionHeader.inputs					== section_.m_sectionHeader.inputs				) &&
						 (1																== m_sectionHeader.inputs									) &&
						 (m_sectionHeader.IF[0]						== section_.m_sectionHeader.IF[0]					)) ||
						((m_sectionHeader.inputs					== section_.m_sectionHeader.inputs				) &&
						 (2																== m_sectionHeader.inputs									) &&
						 (m_sectionHeader.IF[0]						== section_.m_sectionHeader.IF[0]					) &&
						 (m_sectionHeader.IF[1]						== section_.m_sectionHeader.IF[1]					))) &&

					 (m_indexInput											== section_.m_indexInput									) );
}

bool Baseband::Section::operator!=( const Section& section_ ) const {
	return !(*this == section_);
}

Baseband::Section& Baseband::Section::operator=( const Section& section_ ) {
	if ( *this != section_ ) {
		m_sectionHeader = section_.m_sectionHeader;
		m_indexInput		= section_.m_indexInput;
	}

	return *this;
}

long Baseband::Section::getID() const {
	return m_sectionHeader.id;
}

Backends::TPolarization Baseband::Section::getPolarization() const {
	return m_sectionHeader.polarization;
}

long Baseband::Section::getFeed()	const {
	return m_sectionHeader.feed;
}

long Baseband::Section::getInputs() const {
	return m_sectionHeader.inputs;
}

long Baseband::Section::getIDFeed()	const {
	return (m_sectionHeader.feed + 1) * 100 + m_indexInput + 1;
}
