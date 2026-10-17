#include "libastrowrapper.h"

#include <array>
#include <cctype>
#include <cstring>
#include <iostream>
#include <strings.h>

namespace xephemlib {

constexpr std::array<const char*, NOBJ> planetnames = {
    "Mercury", "Venus", "Mars", "Jupiter", "Saturn",
    "Uranus", "Neptune", "Pluto", "Sun", "Moon"
};

// xephem uses mjd 0 as 1899 12 31 at 12:00
constexpr double XEPHEM_MJD_OFFSET = 15019.5;

Site::Site()
{
    std::memset(static_cast<Now*>(this), 0, sizeof(Now));
    this->n_temp = 10.0;
    this->n_pressure = 0.0;     // no refraction, apparent places are computed by IRA::CSkySource
    this->n_epoch = J2000;      // astrometric J2000 coordinates
}

Site::Site(double mj, double lon, double lt, double height): Site()
{
    setCoordinate(lon, lt, height);
    setTime(mj);
}

void Site::setCoordinate(double lon, double lt, double height)
{
    this->n_lat = lt;
    this->n_lng = lon;
    this->n_elev = height / ERAD;   // libastro wants earth radii
}

void Site::setTime(double mj)
{
    this->n_mjd = mj - XEPHEM_MJD_OFFSET;
}

void Site::stampa(void) const
{
    std::cout << "mjd:" << getMjd() << std::endl;
    std::cout << "lon:" << raddeg(this->n_lng) << std::endl;
    std::cout << "lat:" << raddeg(this->n_lat) << std::endl;
    std::cout << "height:" << this->n_elev * ERAD << std::endl;
}

double Site::getMjd() const
{
    return this->n_mjd + XEPHEM_MJD_OFFSET;
}

SolarSystemBody::SolarSystemBody():
    obj(new Obj()),
    _code(NOBJ),
    _ra(0.0), _dec(0.0), _range(0.0), _az(0.0), _el(0.0)
{
}

SolarSystemBody::SolarSystemBody(PLCode code): SolarSystemBody()
{
    setObject(code);
}

bool SolarSystemBody::setObject(PLCode code)
{
    if (code < 0 || code >= NOBJ)
        return false;
    _code = code;
    // invariant data are set here, not in compute()
    std::memset(obj.get(), 0, sizeof(Obj));
    obj->any.co_type = PLANET;
    strncpy(obj->any.co_name, planetnames[_code], sizeof(obj->any.co_name) - 1);
    obj->pl.plo_code = _code;
    obj->pl.plo_moon = X_PLANET;
    return true;
}

PLCode SolarSystemBody::getCode() const
{
    return _code;
}

PLCode SolarSystemBody::getPlanetCodeFromName(const std::string& str)
{
    for (int i = 0; i < NOBJ; ++i) {
        if (strcasecmp(str.c_str(), planetnames[i]) == 0)
            return static_cast<PLCode>(i);
    }
    return NOBJ;
}

std::string SolarSystemBody::getPlanetNameFromCode(PLCode code)
{
    if (code < 0 || code >= NOBJ)
        return std::string();
    std::string name(planetnames[code]);
    for (auto& c : name)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return name;
}

void SolarSystemBody::getCoordinates(double& ra, double& dec, double& az, double& el, double& range) const
{
    ra = _ra;
    dec = _dec;
    az = _az;
    el = _el;
    range = _range;
}

bool SolarSystemBody::compute(Site* site)
{
    // obj_cir() aborts the process when called with an unset object
    if (site == nullptr || _code == NOBJ)
        return false;

    // preferences are per thread in libastro: set them in the calling thread
    pref_set(PREF_EQUATORIAL, PREF_TOPO);
    if (obj_cir(site, obj.get()) != 0)
        return false;

    _ra = obj->any.co_ra;
    _dec = obj->any.co_dec;
    _az = obj->any.co_az;
    _el = obj->any.co_alt;
    _range = obj->anyss.so_edist;
    return true;
}

Obj* SolarSystemBody::getObject()
{
    return obj.get();
}

void SolarSystemBody::report() const
{
    /* Astrometric right ascension and declination of the target center with
       respect to the observing site (coordinate origin) in the reference frame of
       the planetary ephemeris (ICRF). Compensated for down-leg light-time delay
       aberration. */
    std::cout << "Ra : " << _ra << std::endl;
    std::cout << "Dec: " << _dec << std::endl;
    std::cout << "Az : " << _az << std::endl;
    std::cout << "El : " << _el << std::endl;
    std::cout << "Range : " << _range << std::endl;
}

const std::map<std::string, PLCode> SolarSystemBody::planet = {
    {"MERCURY", MERCURY},
    {"VENUS", VENUS},
    {"MARS", MARS},
    {"JUPITER", JUPITER},
    {"SATURN", SATURN},
    {"URANUS", URANUS},
    {"NEPTUNE", NEPTUNE},
    {"PLUTO", PLUTO},
    {"SUN", SUN},
    {"MOON", MOON}
};

}
