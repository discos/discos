/* Unit tests of XEphemAstroLib.
 * Single threaded checks of the numerical results, of the internal caches and
 * of the per-thread semantic introduced to make the library thread safe.
 */

#include "gtest/gtest.h"
#include <thread>
#include "XEphemTestUtils.h"

using namespace XEphemTest;

class XEphemAstroLib_unit : public ::testing::Test {
protected:
	virtual void SetUp() {
		pref_set(PREF_EQUATORIAL, PREF_TOPO);
	}
};

/* ---------------------------------------------------------------- time */

TEST_F(XEphemAstroLib_unit, cal_mjd_J2000Epoch)
{
	double m;
	cal_mjd(1, 1.5, 2000, &m);
	EXPECT_DOUBLE_EQ(J2000, m);
	EXPECT_DOUBLE_EQ(51544.5, m + MJD_TO_XEPHEM);
}

TEST_F(XEphemAstroLib_unit, mjd_cal_roundTrip)
{
	const int months[] = {1, 2, 3, 7, 12};
	const double days[] = {1.0, 15.25, 28.5, 31.75, 29.0};
	const int years[] = {1999, 2004, 2021, 2026, 2100};
	for (int i = 0; i < 5; i++) {
		double m, dy;
		int mn, yr;
		cal_mjd(months[i], days[i], years[i], &m);
		mjd_cal(m, &mn, &dy, &yr);
		EXPECT_EQ(months[i], mn);
		EXPECT_NEAR(days[i], dy, 1e-9);
		EXPECT_EQ(years[i], yr);
	}
}

TEST_F(XEphemAstroLib_unit, mjd_year_middleOfYear)
{
	double m, yr;
	cal_mjd(7, 2.5, 2021, &m);   /* day 183.5 of 365 */
	mjd_year(m, &yr);
	EXPECT_NEAR(2021.5, yr, 1e-3);
}

TEST_F(XEphemAstroLib_unit, utc_gst_inverseOfgst_utc)
{
	double m, gst, utc;
	cal_mjd(3, 20.0, 2024, &m);
	for (double hr = 0.5; hr < 24.0; hr += 3.25) {
		utc_gst(m, hr, &gst);
		EXPECT_GE(gst, 0.0);
		EXPECT_LT(gst, 24.0);
		gst_utc(m, gst, &utc);
		EXPECT_NEAR(hr, utc, 1e-6);
	}
}

TEST_F(XEphemAstroLib_unit, utc_gst_knownValueAtJ2000)
{
	/* GMST at 2000 Jan 1, 12h UT is 18.697374558 h */
	double gst;
	utc_gst(J2000 - 0.5, 12.0, &gst);
	EXPECT_NEAR(18.697374558, gst, 1e-5);
}

/* ---------------------------------------------------------- math utils */

TEST_F(XEphemAstroLib_unit, range_wrapsAngle)
{
	double v = -1.0;
	range(&v, 2 * PI);
	EXPECT_NEAR(2 * PI - 1.0, v, 1e-12);
	v = 7 * PI;
	range(&v, 2 * PI);
	EXPECT_NEAR(PI, v, 1e-12);
}

/* --------------------------------------------------- earth orientation */

TEST_F(XEphemAstroLib_unit, obliquity_atJ2000)
{
	double eps;
	obliquity(J2000, &eps);
	EXPECT_NEAR(23.4392911, raddeg(eps), 1e-6);
}

TEST_F(XEphemAstroLib_unit, nutation_withinPhysicalBounds)
{
	double deps, dpsi;
	nutation(J2000, &deps, &dpsi);
	/* amplitudes are ~9" in obliquity and ~17" in longitude */
	EXPECT_LT(fabs(raddeg(deps) * 3600.0), 10.0);
	EXPECT_LT(fabs(raddeg(dpsi) * 3600.0), 20.0);
}

TEST_F(XEphemAstroLib_unit, precess_roundTrip)
{
	double ra = degrad(83.6331), dec = degrad(22.0145);  /* Crab, J2000 */
	double ra0 = ra, dec0 = dec;
	double m2026;
	cal_mjd(1, 1.0, 2026, &m2026);
	precess(J2000, m2026, &ra, &dec);
	EXPECT_GT(separation(ra, dec, ra0, dec0), degrad(0.1));   /* it moved */
	precess(m2026, J2000, &ra, &dec);
	EXPECT_NEAR(0.0, raddeg(separation(ra, dec, ra0, dec0)) * 3600.0, 1e-3);
}

/* -------------------------------------------------------------- bodies */

TEST_F(XEphemAstroLib_unit, sunpos_distanceAtPerihelion)
{
	double lsn, rsn, bsn, m;
	cal_mjd(1, 3.0, 2021, &m);   /* perihelion 2021 Jan 2 */
	sunpos(m, &lsn, &rsn, &bsn);
	EXPECT_NEAR(0.98326, rsn, 1e-4);
	EXPECT_LT(fabs(bsn), degrad(0.001));
}

/* Reference values from JPL Horizons: astrometric J2000 coordinates,
 * 2021 Jun 28 00:00 UTC (JD 2459393.5, MJD 59393.0), site lon 9.5 E lat 39.5 N.
 */
TEST_F(XEphemAstroLib_unit, obj_cir_jupiterAgainstHorizons)
{
	Position p;
	Now now = makeNow(59393.0, degrad(9.5), degrad(39.5), 600.0);
	ASSERT_EQ(0, compute(now, JUPITER, PREF_TOPO, p));
	EXPECT_NEAR(334.167106906, raddeg(p.ra), 2.7e-4);
	EXPECT_NEAR(-11.764766054, raddeg(p.dec), 2.7e-4);
	EXPECT_NEAR(4.39448666972, p.edist, 2.7e-5);
}

TEST_F(XEphemAstroLib_unit, obj_cir_sunAgainstHorizons)
{
	Position p;
	Now now = makeNow(59393.0, degrad(9.5), degrad(39.5), 600.0);
	ASSERT_EQ(0, compute(now, SUN, PREF_TOPO, p));
	EXPECT_NEAR(96.800863628, raddeg(p.ra), 2.7e-4);
	EXPECT_NEAR(23.287056282, raddeg(p.dec), 2.7e-4);
	EXPECT_NEAR(1.01658809888, p.edist, 2.7e-6);
}

TEST_F(XEphemAstroLib_unit, obj_cir_allBodiesSucceed)
{
	Now now = makeNow(61000.25);
	for (int code = MERCURY; code < NOBJ; code++) {
		Position p;
		EXPECT_EQ(0, compute(now, code, PREF_TOPO, p)) << "body " << code;
		EXPECT_GE(p.ra, 0.0);
		EXPECT_LT(p.ra, 2 * PI);
		EXPECT_LE(fabs(p.dec), PI / 2);
		EXPECT_GT(p.edist, 0.0);
	}
}

TEST_F(XEphemAstroLib_unit, obj_cir_moonTopocentricParallax)
{
	/* the Moon's horizontal parallax is ~1 deg: topocentric and geocentric
	 * positions must differ by a fraction of a degree, Saturn (~9.5 AU) by
	 * less than its 0.93 arcsec horizontal parallax */
	Position topo, geo;
	Now now = makeNow(61000.25);
	ASSERT_EQ(0, compute(now, MOON, PREF_TOPO, topo));
	ASSERT_EQ(0, compute(now, MOON, PREF_GEO, geo));
	double d = raddeg(separation(topo.ra, topo.dec, geo.ra, geo.dec));
	EXPECT_GT(d, 0.05);
	EXPECT_LT(d, 1.1);
	ASSERT_EQ(0, compute(now, SATURN, PREF_TOPO, topo));
	ASSERT_EQ(0, compute(now, SATURN, PREF_GEO, geo));
	d = raddeg(separation(topo.ra, topo.dec, geo.ra, geo.dec)) * 3600.0;
	EXPECT_GT(d, 0.0);
	EXPECT_LT(d, 0.95);
}

TEST_F(XEphemAstroLib_unit, plmoon_cir_galileanMoonsCloseToJupiter)
{
	Now now = makeNow(60500.0);
	Position jup;
	ASSERT_EQ(0, compute(now, JUPITER, PREF_TOPO, jup));
	for (int moon = J_IO; moon <= J_CALLISTO; moon++) {
		Position p;
		ASSERT_EQ(0, compute(now, JUPITER, PREF_TOPO, p, moon));
		/* Callisto elongation never exceeds ~0.18 deg */
		EXPECT_LT(raddeg(separation(p.ra, p.dec, jup.ra, jup.dec)), 0.2) << "moon " << moon;
	}
}

TEST_F(XEphemAstroLib_unit, getBuiltInObjs_namesAndCodes)
{
	Obj* op = NULL;
	int n = getBuiltInObjs(&op);
	ASSERT_EQ(NBUILTIN, n);
	ASSERT_TRUE(op != NULL);
	EXPECT_STREQ("Jupiter", op[JUPITER].o_name);
	EXPECT_STREQ("Io", op[IO].o_name);
	EXPECT_EQ(JUPITER, op[IO].pl_code);
	EXPECT_STREQ("Moon", op[MOON].o_name);
}

TEST_F(XEphemAstroLib_unit, db_tle_earthSatellite)
{
	Obj sat;
	ASSERT_TRUE(makeSatellite(sat));
	Now now = makeNow(59389.0);
	ASSERT_EQ(0, obj_cir(&now, &sat));
	/* ISS orbits at ~420 km: the slant range from a ground site is between
	 * the orbit height and the Earth diameter plus the orbit height (m) */
	EXPECT_GT(sat.s_range, 3.5e5);
	EXPECT_LT(sat.s_range, 2 * ERAD + 5.0e5);
	if (sat.s_alt > 0)
		EXPECT_LT(sat.s_range, 2.5e6);    /* above the horizon */
}

/* -------------------------------------------------------------- caches */

/* every cache keyed on the time must return the same value it computes from
 * scratch, whatever the history of previous calls */
TEST_F(XEphemAstroLib_unit, caches_resultIndependentFromCallHistory)
{
	Now t1 = makeNow(59393.5), t2 = makeNow(60200.75);
	Position a, b, a2, b2;
	ASSERT_EQ(0, compute(t1, MARS, PREF_TOPO, a));
	ASSERT_EQ(0, compute(t2, MARS, PREF_TOPO, b));
	ASSERT_EQ(0, compute(t1, MARS, PREF_TOPO, a2));   /* cache refilled */
	ASSERT_EQ(0, compute(t1, MARS, PREF_TOPO, a2));   /* cache hit */
	ASSERT_EQ(0, compute(t2, MARS, PREF_TOPO, b2));
	EXPECT_TRUE(a == a2);
	EXPECT_TRUE(b == b2);
	EXPECT_TRUE(a != b);
}

TEST_F(XEphemAstroLib_unit, caches_nutationRepeatable)
{
	double d1, p1, d2, p2, d3, p3;
	nutation(J2000, &d1, &p1);
	nutation(J2000 + 1000.0, &d2, &p2);
	nutation(J2000, &d3, &p3);
	EXPECT_EQ(d1, d3);
	EXPECT_EQ(p1, p3);
	EXPECT_NE(d1, d2);
}

/* --------------------------------------------------------- preferences */

TEST_F(XEphemAstroLib_unit, pref_set_returnsPriorValue)
{
	EXPECT_EQ(PREF_TOPO, pref_set(PREF_EQUATORIAL, PREF_GEO));
	EXPECT_EQ(PREF_GEO, pref_get(PREF_EQUATORIAL));
	EXPECT_EQ(PREF_GEO, pref_set(PREF_EQUATORIAL, PREF_TOPO));
	EXPECT_EQ(PREF_TOPO, pref_get(PREF_EQUATORIAL));
}

TEST_F(XEphemAstroLib_unit, pref_set_isPerThread)
{
	pref_set(PREF_EQUATORIAL, PREF_GEO);
	int seen = -1, afterSet = -1;
	std::thread t([&]() {
		seen = pref_get(PREF_EQUATORIAL);        /* default of a new thread */
		pref_set(PREF_EQUATORIAL, PREF_GEO);
		afterSet = pref_get(PREF_EQUATORIAL);
		pref_set(PREF_EQUATORIAL, PREF_TOPO);
	});
	t.join();
	EXPECT_EQ(PREF_TOPO, seen);
	EXPECT_EQ(PREF_GEO, afterSet);
	EXPECT_EQ(PREF_GEO, pref_get(PREF_EQUATORIAL)); /* untouched by the thread */
}

TEST_F(XEphemAstroLib_unit, getBuiltInObjs_isPerThread)
{
	Obj* mine = NULL;
	Obj* other = NULL;
	getBuiltInObjs(&mine);
	std::thread t([&]() { getBuiltInObjs(&other); });
	t.join();
	EXPECT_TRUE(mine != other);
}

/* -------------------------------------------------- process wide state */

TEST_F(XEphemAstroLib_unit, cns_figure_badIdAndNotLoaded)
{
	double ra[40], dec[40];
	int dcodes[40];
	EXPECT_EQ(-1, cns_figure(-1, J2000, ra, dec, dcodes));
	EXPECT_EQ(-1, cns_figure(10000, J2000, ra, dec, dcodes));
	EXPECT_EQ(0, cns_figure(0, J2000, ra, dec, dcodes));  /* nothing loaded */
}
