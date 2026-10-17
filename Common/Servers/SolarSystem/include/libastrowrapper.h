#ifndef LIBASTROWRAPPER_H
#define LIBASTROWRAPPER_H

/*
    C++ Wrapper for Xephem libastro.

    libastro keeps its caches and its preferences per thread, so different
    instances of these classes can be used concurrently from different threads
    without any global lock. A single instance is not synchronised: callers
    sharing one between threads must protect it (SolarSystemBodyImpl does it
    with its own mutex).
*/

#include <map>
#include <memory>
#include <string>
#include "astro.h"
#include "preferences.h"

namespace xephemlib
{

/* observing site and time, as required by libastro */
class Site: public Now {

public:

    Site();
    /* mj: UTC as Modified Julian Date; lon, lt: radians; height: meters */
    Site(double mj, double lon, double lt, double height);

    /* lon, lt: radians; height: meters above sea level */
    void setCoordinate(double lon, double lt, double height);
    /* mj: UTC as Modified Julian Date */
    void setTime(double mj);

    void stampa(void) const;
    double getMjd() const;

};

class SolarSystemBody {

public:

    SolarSystemBody();
    explicit SolarSystemBody(PLCode code);

    static std::string getPlanetNameFromCode(PLCode code);
    static PLCode getPlanetCodeFromName(const std::string& name);

    /* select the body; returns false if the code is not a valid body */
    bool setObject(PLCode code);
    PLCode getCode() const;

    /* compute the astrometric (J2000) topocentric position at site.
     * returns false if no valid body is set or libastro fails */
    bool compute(Site* site);

    Obj* getObject();
    void report() const;
    void getCoordinates(double& ra, double& dec, double& az, double& el, double& range) const;

    /* upper case body names to codes */
    static const std::map<std::string, PLCode> planet;

private:

    std::unique_ptr<Obj> obj;
    PLCode _code;
    double _ra, _dec, _range, _az, _el;

};

}

#endif /*LIBASTROWRAPPER_H*/
