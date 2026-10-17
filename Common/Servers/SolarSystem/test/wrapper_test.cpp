/* Unit tests of the libastro C++ wrapper used by the SolarSystemBody component.
 * The wrapper no longer serialises the libastro calls with a global mutex, so
 * the concurrent test verifies that independent instances can be used from
 * different threads at the same time.
 */

#include "libastrowrapper.h"
#include <gtest/gtest.h>
#include <atomic>
#include <cmath>
#include <thread>
#include <vector>

using namespace xephemlib;

/* JPL Horizons, astrometric J2000, 2021 Jun 28 00:00 UTC, lon 9.5 E lat 39.5 N */
const double HORIZONS_MJD = 59393.0;

class SolarSystemBodyTest : public ::testing::Test {
protected:
    Site site;

    virtual void SetUp() {
        site.setCoordinate(degrad(9.5), degrad(39.5), 600.0);
        site.setTime(HORIZONS_MJD);
    }
};

TEST_F(SolarSystemBodyTest, siteDefaultIsInitialised)
{
    Site s;
    EXPECT_EQ(0.0, s.n_lat);
    EXPECT_EQ(0.0, s.n_lng);
    EXPECT_EQ(0.0, s.n_elev);
    EXPECT_EQ(0.0, s.n_pressure);
    EXPECT_EQ(J2000, s.n_epoch);
}

TEST_F(SolarSystemBodyTest, siteCoordinatesAndTime)
{
    Site s(60000.25, degrad(9.245), degrad(39.493), 650.0);
    EXPECT_DOUBLE_EQ(60000.25, s.getMjd());
    EXPECT_DOUBLE_EQ(degrad(9.245), s.n_lng);
    EXPECT_DOUBLE_EQ(degrad(39.493), s.n_lat);
    EXPECT_DOUBLE_EQ(650.0, s.n_elev * ERAD);
}

TEST_F(SolarSystemBodyTest, namesAndCodes)
{
    EXPECT_EQ(JUPITER, SolarSystemBody::getPlanetCodeFromName("jupiter"));
    EXPECT_EQ(SUN, SolarSystemBody::getPlanetCodeFromName("SUN"));
    EXPECT_EQ(MOON, SolarSystemBody::getPlanetCodeFromName("Moon"));
    EXPECT_EQ(NOBJ, SolarSystemBody::getPlanetCodeFromName("Vulcan"));
    EXPECT_EQ("JUPITER", SolarSystemBody::getPlanetNameFromCode(JUPITER));
    EXPECT_EQ("", SolarSystemBody::getPlanetNameFromCode(NOBJ));
    EXPECT_EQ(MARS, SolarSystemBody::planet.at("MARS"));
}

TEST_F(SolarSystemBodyTest, computeWithoutBodyFailsWithoutAborting)
{
    SolarSystemBody body;
    EXPECT_EQ(NOBJ, body.getCode());
    EXPECT_FALSE(body.compute(&site));
    EXPECT_FALSE(body.setObject(NOBJ));
    EXPECT_FALSE(body.compute(&site));
    EXPECT_FALSE(body.compute(nullptr));
}

TEST_F(SolarSystemBodyTest, jupiterPosition)
{
    SolarSystemBody p(JUPITER);
    ASSERT_TRUE(p.compute(&site));
    double ra, dec, az, el, range;
    p.getCoordinates(ra, dec, az, el, range);
    EXPECT_NEAR(334.167106906, raddeg(ra), 2.7e-4);
    EXPECT_NEAR(-11.764766054, raddeg(dec), 2.7e-4);
    EXPECT_NEAR(4.39448666972, range, 2.7e-5);
}

TEST_F(SolarSystemBodyTest, sunPosition)
{
    SolarSystemBody p(SUN);
    ASSERT_TRUE(p.compute(&site));
    double ra, dec, az, el, range;
    p.getCoordinates(ra, dec, az, el, range);
    EXPECT_NEAR(96.800863628, raddeg(ra), 2.7e-4);
    EXPECT_NEAR(23.287056282, raddeg(dec), 2.7e-4);
    EXPECT_NEAR(1.01658809888, range, 2.7e-6);
}

TEST_F(SolarSystemBodyTest, changeBody)
{
    SolarSystemBody p(SUN);
    ASSERT_TRUE(p.setObject(MARS));
    EXPECT_EQ(MARS, p.getCode());
    EXPECT_STREQ("Mars", p.getObject()->any.co_name);
    EXPECT_TRUE(p.compute(&site));
}

/* one component instance per thread, as in a container hosting several
 * SolarSystemBody components: results must equal the serial ones */
TEST_F(SolarSystemBodyTest, concurrentInstancesMatchSerialReference)
{
    const int THREADS = 8, STEPS = 200;
    struct Pos { double ra, dec, az, el, range; };
    std::vector<std::vector<Pos>> ref(NOBJ, std::vector<Pos>(STEPS));
    for (int b = 0; b < NOBJ; b++) {
        SolarSystemBody body(static_cast<PLCode>(b));
        Site s(site);
        for (int i = 0; i < STEPS; i++) {
            s.setTime(60000.0 + i * 0.01);
            ASSERT_TRUE(body.compute(&s));
            Pos& p = ref[b][i];
            body.getCoordinates(p.ra, p.dec, p.az, p.el, p.range);
        }
    }
    std::atomic<int> mismatches(0);
    std::vector<std::thread> pool;
    for (int t = 0; t < THREADS; t++) {
        pool.push_back(std::thread([&, t]() {
            const int b = t % NOBJ;
            SolarSystemBody body(static_cast<PLCode>(b));
            Site s(site);
            for (int r = 0; r < 5; r++) {
                for (int i = 0; i < STEPS; i++) {
                    s.setTime(60000.0 + i * 0.01);
                    Pos p;
                    if (!body.compute(&s)) { mismatches++; continue; }
                    body.getCoordinates(p.ra, p.dec, p.az, p.el, p.range);
                    const Pos& e = ref[b][i];
                    if (p.ra != e.ra || p.dec != e.dec || p.az != e.az || p.el != e.el || p.range != e.range)
                        mismatches++;
                }
            }
        }));
    }
    for (auto& th : pool)
        th.join();
    EXPECT_EQ(0, mismatches.load());
}
