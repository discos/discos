#ifndef XEPHEM_TEST_UTILS_H
#define XEPHEM_TEST_UTILS_H

/* Helpers shared by the XEphemAstroLib unit and functional tests. */

#include <cmath>
#include <cstring>
#include <vector>
#include "astro.h"
#include "preferences.h"

namespace XEphemTest {

/* offset between a standard MJD and the xephem day number (0 = 1899 Dec 31.5) */
const double MJD_TO_XEPHEM = 15019.5;

/* SRT site, used as reference observatory */
const double SRT_LON = degrad(9.245);
const double SRT_LAT = degrad(39.493);
const double SRT_HEIGHT = 650.0;          /* m */

struct Position {
	double ra, dec, az, alt, edist;
	bool operator==(const Position& o) const {
		/* bit-exact: same inputs must give the very same numbers */
		return ra == o.ra && dec == o.dec && az == o.az && alt == o.alt && edist == o.edist;
	}
	bool operator!=(const Position& o) const { return !(*this == o); }
};

/* build a fully initialised Now; mjd is a standard MJD (UTC) */
inline Now makeNow(double mjd, double lon = SRT_LON, double lat = SRT_LAT,
  double height = SRT_HEIGHT, double epoch = J2000)
{
	Now now;
	std::memset(&now, 0, sizeof(now));
	now.n_mjd = mjd - MJD_TO_XEPHEM;
	now.n_lng = lon;
	now.n_lat = lat;
	now.n_elev = height / ERAD;
	now.n_temp = 10.0;
	now.n_pressure = 0.0;      /* no refraction */
	now.n_epoch = epoch;
	return now;
}

/* build a planet/Sun/Moon or a builtin moon object */
inline Obj makeBody(int code, int moon = X_PLANET)
{
	Obj obj;
	std::memset(&obj, 0, sizeof(obj));
	obj.o_type = PLANET;
	obj.pl_code = (PLCode)code;
	obj.pl_moon = (MCode)moon;
	return obj;
}

/* compute the position of a body; returns obj_cir()/plmoon_cir() status */
inline int compute(Now now, int code, int prefEquatorial, Position& pos, int moon = X_PLANET)
{
	Obj obj = makeBody(code, moon);
	pref_set(PREF_EQUATORIAL, prefEquatorial);
	int ret = (moon == X_PLANET) ? obj_cir(&now, &obj) : plmoon_cir(&now, &obj);
	pos.ra = obj.s_ra;
	pos.dec = obj.s_dec;
	pos.az = obj.s_az;
	pos.alt = obj.s_alt;
	pos.edist = obj.s_edist;
	return ret;
}

/* angular separation between two equatorial positions, radians */
inline double separation(double ra1, double dec1, double ra2, double dec2)
{
	double c = sin(dec1) * sin(dec2) + cos(dec1) * cos(dec2) * cos(ra1 - ra2);
	if (c > 1.0) c = 1.0;
	if (c < -1.0) c = -1.0;
	return acos(c);
}

/* rewrite the last digit of a TLE line with its modulo 10 checksum */
inline void fixTleChecksum(char* line)
{
	int sum = 0;
	for (int i = 0; i < 68; i++) {
		if (line[i] >= '0' && line[i] <= '9')
			sum += line[i] - '0';
		else if (line[i] == '-')
			sum += 1;
	}
	line[68] = (char)('0' + sum % 10);
}

/* an ISS two line element set (epoch 2021 Jun 23) */
inline bool makeSatellite(Obj& obj)
{
	char name[] = "ISS (ZARYA)";
	char l1[] = "1 25544U 98067A   21174.51875000  .00001264  00000-0  31614-4 0  9990";
	char l2[] = "2 25544  51.6439 307.4436 0002411  38.7516  52.4583 15.48848812289610";
	fixTleChecksum(l1);
	fixTleChecksum(l2);
	std::memset(&obj, 0, sizeof(obj));
	return db_tle(name, l1, l2, &obj) == 0;
}

}

#endif
