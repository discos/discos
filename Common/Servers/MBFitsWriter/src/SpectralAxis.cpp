#include "SpectralAxis.h"

SpectralAxis::SpectralAxis() : m_axisName(string()),
															 m_axisCol2(string()),
															 m_referenceChannel(0.0),
															 m_referenceChannelValue(0.0),
															 m_channelSeparation(0.0),
															 m_unit(string()),
															 m_standardOfRest(string()),
															 m_observingFrame(string()) {
}

SpectralAxis::SpectralAxis( const SpectralAxis& spectralAxis_ ) : m_axisName(spectralAxis_.m_axisName),
																																	m_axisCol2(spectralAxis_.m_axisCol2),
																																	m_referenceChannel(spectralAxis_.m_referenceChannel),
																																	m_referenceChannelValue(spectralAxis_.m_referenceChannelValue),
																																	m_channelSeparation(spectralAxis_.m_channelSeparation),
																																	m_unit(spectralAxis_.m_unit),
																																	m_standardOfRest(spectralAxis_.m_standardOfRest),
																																	m_observingFrame(spectralAxis_.m_observingFrame) {
}

SpectralAxis::SpectralAxis( const string&	axisName_,
														const string&	axisCol2_,
														const float		referenceChannel_,
														const double	referenceChannelValue_,
														const double	channelSeparation_,
														const string&	unit_,
														const string&	standardOfRest_,
														const string&	observingFrame_ ) : m_axisName(axisName_),
																															m_axisCol2(axisCol2_),
																															m_referenceChannel(referenceChannel_),
																															m_referenceChannelValue(referenceChannelValue_),
																															m_channelSeparation(channelSeparation_),
																															m_unit(unit_),
																															m_standardOfRest(standardOfRest_),
																															m_observingFrame(observingFrame_) {
}

SpectralAxis::~SpectralAxis() {
}

bool SpectralAxis::operator==( const SpectralAxis& spectralAxis_ ) const {
	return ( (m_axisName							== spectralAxis_.m_axisName							) &&
					 (m_axisCol2							== spectralAxis_.m_axisCol2							) &&
					 (m_referenceChannel			== spectralAxis_.m_referenceChannel			) &&
					 (m_referenceChannelValue	== spectralAxis_.m_referenceChannelValue) &&
					 (m_channelSeparation			== spectralAxis_.m_channelSeparation		) &&
					 (m_unit									== spectralAxis_.m_unit									) &&
					 (m_standardOfRest				== spectralAxis_.m_standardOfRest				) &&
					 (m_observingFrame				== spectralAxis_.m_observingFrame				) );
}

bool SpectralAxis::operator!=( const SpectralAxis& spectralAxis_ ) const {
	return !(*this == spectralAxis_);
}

SpectralAxis& SpectralAxis::operator=( const SpectralAxis& spectralAxis_ ) {
	if ( *this != spectralAxis_ ) {
		m_axisName							= spectralAxis_.m_axisName;
		m_axisCol2							= spectralAxis_.m_axisCol2;
		m_referenceChannel			= spectralAxis_.m_referenceChannel;
		m_referenceChannelValue	= spectralAxis_.m_referenceChannelValue;
		m_channelSeparation			= spectralAxis_.m_channelSeparation;
		m_unit									= spectralAxis_.m_unit;
		m_standardOfRest				= spectralAxis_.m_standardOfRest;
		m_observingFrame				= spectralAxis_.m_observingFrame;
	}

	return *this;
}

string	SpectralAxis::getAxisName()								const {
	return m_axisName;
}

string	SpectralAxis::getAxisCol2()								const {
	return m_axisCol2;
}

float		SpectralAxis::getReferenceChannel()				const {
	return m_referenceChannel;
}

double	SpectralAxis::getReferenceChannelValue()	const {
	return m_referenceChannelValue;
}

double	SpectralAxis::getChannelSeparation()			const {
	return m_channelSeparation;
}

string	SpectralAxis::getUnit()										const {
	return m_unit;
}

string	SpectralAxis::getStandardOfRest()					const {
	return m_standardOfRest;
}

string	SpectralAxis::getObservingFrame()					const {
	return m_observingFrame;
}
