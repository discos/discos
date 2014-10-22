#ifndef __H__SpectralAxis__
#define __H__SpectralAxis__

#include <string>

using std::string;

class SpectralAxis {
	public:
		SpectralAxis();
		SpectralAxis( const SpectralAxis& spectralAxis_ );
		SpectralAxis( const string&	axisName_,
									const string&	axisCol2_,
									const float		referenceChannel_,
									const double	referenceChannelValue_,
									const double	channelSeparation_,
									const string&	unit_,
									const string&	standardOfRest_,
									const string&	observingFrame_ );
		virtual ~SpectralAxis();

		virtual bool operator==( const SpectralAxis& spectralAxis_ ) const;
		virtual bool operator!=( const SpectralAxis& spectralAxis_ ) const;
		SpectralAxis& operator=( const SpectralAxis& spectralAxis_ );

		string	getAxisName()								const;
		string	getAxisCol2()								const;
		float		getReferenceChannel()				const;
		double	getReferenceChannelValue()	const;
		double	getChannelSeparation()			const;
		string	getUnit()										const;
		string	getStandardOfRest()					const;
		string	getObservingFrame()					const;

	private:
		string	m_axisName;
		string	m_axisCol2;
		float		m_referenceChannel;
		double	m_referenceChannelValue;
		double	m_channelSeparation;
		string	m_unit;
		string	m_standardOfRest;
		string	m_observingFrame;
};

#endif // __H__SpectralAxis__PECTRALAXIS__
