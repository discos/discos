#ifndef __H__Scan__
#define __H__Scan__

#include "Layout.h"

#include <map>
#include <string>

using std::map;
using std::string;

class Scan {
	public:
		typedef enum {
			typeUnknown,
			typePoint,
			typeFocus,
			typeCal,
			typeFluxCal,
			typeSkyDip,
			typeOn,
			typeOnOff,
			typeMap
		} Type_e;

		typedef enum {
			modeUnknown,
			modeSample,
			modeRaster,
			modeOTF,
			modeDrift,
			modeNone
		} Mode_e;

		typedef enum {
			geometryUnknown,
			geometrySingle,
			geometryLine,
			geometryCross,
			geometryRect,
			geometryQuad,
			geometryCirc,
			geometryCurve
		} Geometry_e;

		typedef enum {
			directionUndefined,
			directionUser,
			directionEquatorialLongitude,
			directionEquatorialLatitude,
			directionGalacticLongitude,
			directionGalacticLatitude,
			directionHorizontalLongitude,
			directionHorizontalLatitude,
		} Direction_e;

		static void initialize();

		static void terminate();

		static string getType( const Scan::Type_e	type_ );
		static string getMode( const Scan::Mode_e	mode_ );
		static string getGeometry( const Scan::Geometry_e	geometry_ );
		static string getDirection( const Scan::Direction_e direction_ );

		Scan();
		Scan( const Scan& scan_ );
		Scan( const Scan::Type_e			type_,
					const Scan::Mode_e			mode_,
					const Scan::Geometry_e	geometry_,
					const Scan::Direction_e	direction_,
					const long							line_,
					const long							repeats_,
					const double						length_,
					const double						xVelocity_,
					const double						time_,
					const double						xSpace_,
					const double						ySpace_,
					const double						skew_,
					const double						parameter1_,
					const double						parameter2_,
					const string&						croCycle_,
					const bool							zigZag_,
					const string&						cType1N_,
					const string&						cType2N_ );
		virtual ~Scan();

		virtual bool operator==( const Scan& scan_ ) const;
		virtual bool operator!=( const Scan& scan_ ) const;
		Scan& operator=( const Scan& scan_ );

		Scan::Type_e			getType()				const;
		Scan::Mode_e			getMode()				const;
		Scan::Geometry_e	getGeometry()		const;
		Scan::Direction_e	getDirection()	const;
		long							getLine()				const;
		long							getRepeats()		const;
		double						getLength()			const;
		double						getXVelocity()	const;
		double						getTime()				const;
		double						getXSpace()			const;
		double						getYSpace()			const;
		double						getSkew()				const;
		double						getParameter1() const;
		double						getParameter2() const;
		string						getCROCycle()		const;
		bool							getZigZag()			const;
		string						getCType1N()		const;
		string						getCType2N()		const;

		void setLayout( const Layout& layout_ );

	private:
		typedef map<Scan::Type_e, string>								TypeType_m_t;
		typedef TypeType_m_t::iterator									TypeType_i_m_t;
		typedef TypeType_m_t::const_iterator						TypeType_ci_m_t;

		typedef map<Scan::Mode_e, string>								ModeMode_m_t;
		typedef ModeMode_m_t::iterator									ModeMode_i_m_t;
		typedef ModeMode_m_t::const_iterator						ModeMode_ci_m_t;

		typedef map<Scan::Geometry_e, string>						GeometryGeometry_m_t;
		typedef GeometryGeometry_m_t::iterator					GeometryGeometry_i_m_t;
		typedef GeometryGeometry_m_t::const_iterator		GeometryGeometry_ci_m_t;

		typedef map<Scan::Direction_e, string>					DirectionDirection_m_t;
		typedef DirectionDirection_m_t::iterator				DirectionDirection_i_m_t;
		typedef DirectionDirection_m_t::const_iterator	DirectionDirection_ci_m_t;

		static Scan::TypeType_m_t						m_typesTypes;
		static Scan::ModeMode_m_t						m_modesModes;
		static Scan::GeometryGeometry_m_t		m_geometriesGeometries;
		static Scan::DirectionDirection_m_t	m_directionsDirections;

		Scan::Type_e			m_type;
		Scan::Mode_e			m_mode;
		Scan::Geometry_e	m_geometry;
		Scan::Direction_e	m_direction;
		long							m_line;
		long							m_repeats;
		double						m_length;
		double						m_xVelocity;
		double						m_time;
		double						m_xSpace;
		double						m_ySpace;
		double						m_skew;
		double						m_parameter1;
		double						m_parameter2;
		string						m_croCycle;
		bool							m_zigZag;

		string						m_cType1N;
		string						m_cType2N;
};

#endif // __H__Scan__
