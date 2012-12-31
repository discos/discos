#include "Scan.h"

#include <cstdlib>
#include <exception>

using std::exception;

Scan::TypeType_m_t						Scan::m_typesTypes						= Scan::TypeType_m_t();
Scan::ModeMode_m_t						Scan::m_modesModes						= Scan::ModeMode_m_t();
Scan::GeometryGeometry_m_t		Scan::m_geometriesGeometries	= Scan::GeometryGeometry_m_t();
Scan::DirectionDirection_m_t	Scan::m_directionsDirections	= Scan::DirectionDirection_m_t();

void Scan::initialize() {
	Scan::m_typesTypes.insert(Scan::TypeType_m_t::value_type(typeUnknown,	string("UNKNOWN")	));
	Scan::m_typesTypes.insert(Scan::TypeType_m_t::value_type(typePoint,		string("POINT")		));
	Scan::m_typesTypes.insert(Scan::TypeType_m_t::value_type(typeFocus,		string("FOCUS")		));
	Scan::m_typesTypes.insert(Scan::TypeType_m_t::value_type(typeCal,			string("CAL")			));
	Scan::m_typesTypes.insert(Scan::TypeType_m_t::value_type(typeFluxCal,	string("FLUXCAL")	));
	Scan::m_typesTypes.insert(Scan::TypeType_m_t::value_type(typeSkyDip,	string("SKYDIP")	));
	Scan::m_typesTypes.insert(Scan::TypeType_m_t::value_type(typeOn,			string("ON")			));
	Scan::m_typesTypes.insert(Scan::TypeType_m_t::value_type(typeOnOff,		string("ONOFF")		));
	Scan::m_typesTypes.insert(Scan::TypeType_m_t::value_type(typeMap,			string("MAP")			));

	Scan::m_modesModes.insert(Scan::ModeMode_m_t::value_type(modeUnknown,	string("UNKNOWN")	));
	Scan::m_modesModes.insert(Scan::ModeMode_m_t::value_type(modeSample,	string("SAMPLE")	));
	Scan::m_modesModes.insert(Scan::ModeMode_m_t::value_type(modeRaster,	string("RASTER")	));
	Scan::m_modesModes.insert(Scan::ModeMode_m_t::value_type(modeOTF,			string("OTF")			));
	Scan::m_modesModes.insert(Scan::ModeMode_m_t::value_type(modeDrift,		string("DRIFT")		));
	Scan::m_modesModes.insert(Scan::ModeMode_m_t::value_type(modeNone,		string("NONE")		));

	Scan::m_geometriesGeometries.insert(Scan::GeometryGeometry_m_t::value_type(geometryUnknown,	string("UNKNOWN")	));
	Scan::m_geometriesGeometries.insert(Scan::GeometryGeometry_m_t::value_type(geometrySingle,	string("SINGLE")	));
	Scan::m_geometriesGeometries.insert(Scan::GeometryGeometry_m_t::value_type(geometryLine,		string("LINE")		));
	Scan::m_geometriesGeometries.insert(Scan::GeometryGeometry_m_t::value_type(geometryCross,		string("CROSS")		));
	Scan::m_geometriesGeometries.insert(Scan::GeometryGeometry_m_t::value_type(geometryRect,		string("RECT")		));
	Scan::m_geometriesGeometries.insert(Scan::GeometryGeometry_m_t::value_type(geometryQuad,		string("QUAD")		));
	Scan::m_geometriesGeometries.insert(Scan::GeometryGeometry_m_t::value_type(geometryCirc,		string("CIRC")		));
	Scan::m_geometriesGeometries.insert(Scan::GeometryGeometry_m_t::value_type(geometryCurve,		string("CURVE")		));

	Scan::m_directionsDirections.insert(Scan::DirectionDirection_m_t::value_type(directionUndefined,							string("NDEF")));
	Scan::m_directionsDirections.insert(Scan::DirectionDirection_m_t::value_type(directionEquatorialLongitude,		string("RA")));
	Scan::m_directionsDirections.insert(Scan::DirectionDirection_m_t::value_type(directionEquatorialLatitude,			string("DEC")));
	Scan::m_directionsDirections.insert(Scan::DirectionDirection_m_t::value_type(directionGalacticLongitude,			string("GLON")));
	Scan::m_directionsDirections.insert(Scan::DirectionDirection_m_t::value_type(directionGalacticLatitude,				string("GLAT")));
	Scan::m_directionsDirections.insert(Scan::DirectionDirection_m_t::value_type(directionHorizontalLongitude,		string("ALON")));
	Scan::m_directionsDirections.insert(Scan::DirectionDirection_m_t::value_type(directionHorizontalLatitude,			string("ALAT")));
}

void Scan::terminate() {
	Scan::m_directionsDirections.clear();
	Scan::m_geometriesGeometries.clear();
	Scan::m_modesModes.clear();
	Scan::m_typesTypes.clear();
}

string Scan::getType( const Scan::Type_e type_ ) {
	Scan::TypeType_ci_m_t typeType_ci											= Scan::m_typesTypes.find(type_);
	if ( typeType_ci						== Scan::m_typesTypes.end()						) throw exception();

	return typeType_ci->second;
}

string Scan::getMode( const Scan::Mode_e mode_ ) {
	Scan::ModeMode_ci_m_t modeMode_ci											= Scan::m_modesModes.find(mode_);
	if ( modeMode_ci						== Scan::m_modesModes.end()						) throw exception();

	return modeMode_ci->second;
}

string Scan::getGeometry( const Scan::Geometry_e geometry_ ) {
	Scan::GeometryGeometry_ci_m_t geometryGeometry_ci			= Scan::m_geometriesGeometries.find(geometry_);
	if ( geometryGeometry_ci		== Scan::m_geometriesGeometries.end()	) throw exception();

	return geometryGeometry_ci->second;
}

string Scan::getDirection( const Scan::Direction_e direction_ ) {
	Scan::DirectionDirection_ci_m_t directionDirection_ci	= Scan::m_directionsDirections.find(direction_);
	if ( directionDirection_ci	== Scan::m_directionsDirections.end()	) throw exception();

	return directionDirection_ci->second;
}

Scan::Scan() : m_type(Scan::typeUnknown),
							 m_mode(Scan::modeUnknown),
							 m_geometry(Scan::geometryUnknown),
							 m_direction(Scan::directionUndefined),
							 m_line(0),
							 m_repeats(0),
							 m_length(0.0),
							 m_xVelocity(0.0),
							 m_time(0.0),
							 m_xSpace(0.0),
							 m_ySpace(0.0),
							 m_skew(0.0),
							 m_parameter1(0.0),
							 m_parameter2(0.0),
							 m_croCycle(string()),
							 m_zigZag(false),
							 m_cType1N(string()),
							 m_cType2N(string()) {
}

Scan::Scan( const Scan& scan_ ) : m_type(scan_.m_type),
																	m_mode(scan_.m_mode),
																	m_geometry(scan_.m_geometry),
																	m_direction(scan_.m_direction),
																	m_line(scan_.m_line),
																	m_repeats(scan_.m_repeats),
																	m_length(scan_.m_length),
																	m_xVelocity(scan_.m_xVelocity),
																	m_time(scan_.m_time),
																	m_xSpace(scan_.m_xSpace),
																	m_ySpace(scan_.m_ySpace),
																	m_skew(scan_.m_skew),
																	m_parameter1(scan_.m_parameter1),
																	m_parameter2(scan_.m_parameter2),
																	m_croCycle(scan_.m_croCycle),
																	m_zigZag(scan_.m_zigZag),
																	m_cType1N(scan_.m_cType1N),
																	m_cType2N(scan_.m_cType2N) {
}

Scan::Scan( const Scan::Type_e			type_,
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
						const string&						cType2N_) : m_type(type_),
																								m_mode(mode_),
																								m_geometry(geometry_),
																								m_direction(direction_),
																								m_line(line_),
																								m_repeats(repeats_),
																								m_length(length_),
																								m_xVelocity(xVelocity_),
																								m_time(time_),
																								m_xSpace(xSpace_),
																								m_ySpace(ySpace_),
																								m_skew(skew_),
																								m_parameter1(parameter1_),
																								m_parameter2(parameter2_),
																								m_croCycle(croCycle_),
																								m_zigZag(zigZag_),
																								m_cType1N(cType1N_),
																								m_cType2N(cType2N_) {
}

Scan::~Scan() {
}

bool Scan::operator==( const Scan& scan_ ) const {
	return ( (m_type				== scan_.m_type				) &&
					 (m_mode				== scan_.m_mode				) &&
					 (m_geometry		== scan_.m_geometry		) &&
					 (m_direction		== scan_.m_direction	) &&
					 (m_line				== scan_.m_line				) &&
					 (m_repeats			== scan_.m_repeats		) &&
					 (m_length			== scan_.m_length			) &&
					 (m_xVelocity		== scan_.m_xVelocity	) &&
					 (m_time				== scan_.m_time				) &&
					 (m_xSpace			== scan_.m_xSpace			) &&
					 (m_ySpace			== scan_.m_ySpace			) &&
					 (m_skew				== scan_.m_skew				) &&
					 (m_parameter1	== scan_.m_parameter1	) &&
					 (m_parameter2	== scan_.m_parameter2	) &&
					 (m_croCycle		== scan_.m_croCycle		) &&
					 (m_zigZag			== scan_.m_zigZag			) &&
					 (m_cType1N			== scan_.m_cType1N		) &&
					 (m_cType2N			== scan_.m_cType2N		) );
}

bool Scan::operator!=( const Scan& scan_ ) const {
	return !(*this == scan_);
}

Scan& Scan::operator=( const Scan& scan_ ) {
	// handle self assignment
	if ( *this != scan_ ) {
		m_type				= scan_.m_type;
		m_mode				= scan_.m_mode;
		m_geometry		= scan_.m_geometry;
		m_direction		= scan_.m_direction;
		m_line				= scan_.m_line;
		m_repeats			= scan_.m_repeats;
		m_length			= scan_.m_length;
		m_xVelocity		= scan_.m_xVelocity;
		m_time				= scan_.m_time;
		m_xSpace			= scan_.m_xSpace;
		m_ySpace			= scan_.m_ySpace;
		m_skew				= scan_.m_skew;
		m_parameter1	= scan_.m_parameter1;
		m_parameter2	= scan_.m_parameter2;
		m_croCycle		= scan_.m_croCycle;
		m_zigZag			= scan_.m_zigZag;
		m_cType1N			= scan_.m_cType1N;
		m_cType2N			= scan_.m_cType2N;
	}

	//assignment operator
	return *this;
}

Scan::Type_e			Scan::getType()				const {
	return m_type;
}

Scan::Mode_e			Scan::getMode()				const {
	return m_mode;
}

Scan::Geometry_e	Scan::getGeometry()		const {
	return m_geometry;
}

Scan::Direction_e	Scan::getDirection()	const {
	return m_direction;
}

long							Scan::getLine()				const {
	return m_line;
}

long							Scan::getRepeats()		const {
	return m_repeats;
}

double						Scan::getLength()			const {
	return m_length;
}

double						Scan::getXVelocity()	const {
	return m_xVelocity;
}

double						Scan::getTime()				const {
	return m_time;
}

double						Scan::getXSpace()			const {
	return m_xSpace;
}

double						Scan::getYSpace()			const {
	return m_ySpace;
}

double						Scan::getSkew()				const {
	return m_skew;
}

double						Scan::getParameter1() const {
	return m_parameter1;
}

double						Scan::getParameter2() const {
	return m_parameter2;
}

string						Scan::getCROCycle()		const {
	return m_croCycle;
}

bool							Scan::getZigZag()			const {
	return m_zigZag;
}

string						Scan::getCType1N()		const {
	return m_cType1N;
}

string						Scan::getCType2N()		const {
	return m_cType2N;
}

#define stringify( name ) # name

typedef enum {
	SCANTYPE,
	SCANMODE,
	SCANGEOM,
	SCANDIR,
	SCANLINE,
	SCANRPTS,
	SCANLEN,
	SCANXVEL,
	SCANTIME,
	SCANXSPC,
	SCANYSPC,
	SCANSKEW,
	SCANPAR1,
	SCANPAR2,
	CROCYCLE,
	ZIGZAG,
	CTYPE1N,
	CTYPE2N
} Label_e;

void Scan::setLayout( const Layout& layout_ ) {
	map<Label_e, string> labels;
	labels.insert(map<Label_e, string>::value_type(SCANTYPE,	stringify(SCANTYPE)));
	labels.insert(map<Label_e, string>::value_type(SCANMODE,	stringify(SCANMODE)));
	labels.insert(map<Label_e, string>::value_type(SCANGEOM,	stringify(SCANGEOM)));
	labels.insert(map<Label_e, string>::value_type(SCANDIR,		stringify(SCANDIR) ));
	labels.insert(map<Label_e, string>::value_type(SCANLINE,	stringify(SCANLINE)));
	labels.insert(map<Label_e, string>::value_type(SCANRPTS,	stringify(SCANRPTS)));
	labels.insert(map<Label_e, string>::value_type(SCANLEN,		stringify(SCANLEN) ));
	labels.insert(map<Label_e, string>::value_type(SCANXVEL,	stringify(SCANXVEL)));
	labels.insert(map<Label_e, string>::value_type(SCANTIME,	stringify(SCANTIME)));
	labels.insert(map<Label_e, string>::value_type(SCANXSPC,	stringify(SCANXSPC)));
	labels.insert(map<Label_e, string>::value_type(SCANYSPC,	stringify(SCANYSPC)));
	labels.insert(map<Label_e, string>::value_type(SCANSKEW,	stringify(SCANSKEW)));
	labels.insert(map<Label_e, string>::value_type(SCANPAR1,	stringify(SCANPAR1)));
	labels.insert(map<Label_e, string>::value_type(SCANPAR2,	stringify(SCANPAR2)));
	labels.insert(map<Label_e, string>::value_type(CROCYCLE,	stringify(CROCYCLE)));
	labels.insert(map<Label_e, string>::value_type(ZIGZAG,		stringify(ZIGZAG)	 ));

	labels.insert(map<Label_e, string>::value_type(CTYPE1N,		stringify(CTYPE1N) ));
	labels.insert(map<Label_e, string>::value_type(CTYPE2N,		stringify(CTYPE2N) ));

	string value;
	for ( map<Label_e, string>::const_iterator label_ci = labels.begin();
				label_ci != labels.end(); ++label_ci ) {
		value = layout_.find(label_ci->second);
		if ( value.empty() ) continue;

		switch( label_ci->first ) {
			case SCANTYPE:
				for ( TypeType_ci_m_t typeType_ci = m_typesTypes.begin();
							typeType_ci != m_typesTypes.end(); ++typeType_ci ) {
					if ( typeType_ci->second == value ) {
						m_type = typeType_ci->first;
						break;
					}
				}
				break;

			case SCANMODE:
				for ( ModeMode_ci_m_t modeMode_ci = m_modesModes.begin();
							modeMode_ci != m_modesModes.end(); ++modeMode_ci ) {
					if ( modeMode_ci->second == value ) {
						m_mode = modeMode_ci->first;
						break;
					}
				}
				break;

			case SCANGEOM:
				for ( GeometryGeometry_ci_m_t geometryGeometry_ci = m_geometriesGeometries.begin();
							geometryGeometry_ci != m_geometriesGeometries.end(); ++geometryGeometry_ci ) {
					if ( geometryGeometry_ci->second == value ) {
						m_geometry = geometryGeometry_ci->first;
						break;
					}
				}

			case SCANDIR:
				for ( DirectionDirection_ci_m_t directionDirection_ci = m_directionsDirections.begin(); directionDirection_ci != m_directionsDirections.end(); ++directionDirection_ci ) {
					if ( directionDirection_ci->second == value ) {
						m_direction = directionDirection_ci->first;
						break;
					}
				}

			case SCANLINE:	m_line			 = atoi(value.c_str());			break;

			case SCANRPTS:	m_repeats		 = atoi(value.c_str());			break;

			case SCANLEN:		m_length		 = atof(value.c_str());			break;

			case SCANXVEL:	m_xVelocity	 = atof(value.c_str());			break;

			case SCANTIME:	m_time			 = atof(value.c_str());			break;

			case SCANXSPC:	m_xSpace		 = atof(value.c_str());			break;

			case SCANYSPC:	m_ySpace		 = atof(value.c_str());			break;

			case SCANSKEW:	m_skew			 = atof(value.c_str());			break;

			case SCANPAR1:	m_parameter1 = atof(value.c_str());			break;

			case SCANPAR2:	m_parameter2 = atof(value.c_str());			break;

			case CROCYCLE:	m_croCycle	 = value;										break;

			case ZIGZAG:		m_zigZag		 = (value == string("1"));	break;

			case CTYPE1N:		m_cType1N		 = value;										break;
			case CTYPE2N:		m_cType2N		 = value;										break;

			default:
				labels.clear();

				throw exception();
		}
	}

	labels.clear();
}
