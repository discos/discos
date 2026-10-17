/* Functional tests of XEphemAstroLib: thread safety.
 *
 * Strategy: a reference table of positions is computed serially, in a single
 * thread, then many threads repeat the same computations concurrently, each
 * in a different order and with different preferences, so that the internal
 * caches are continuously invalidated by the other threads. Every concurrent
 * result must be bit-identical to the serial one.
 *
 * Run them also under ThreadSanitizer (make tsan) to detect data races that
 * do not happen to corrupt a result. XEPHEM_TEST_REDUCE=<n> divides the
 * number of iterations by n, to keep the instrumented run short.
 */

#include "gtest/gtest.h"
#include <atomic>
#include <cstdlib>
#include <map>
#include <random>
#include <thread>
#include <tuple>
#include "XEphemTestUtils.h"

using namespace XEphemTest;

namespace {

const int THREADS = 12;

/* number of iterations, reduced by the XEPHEM_TEST_REDUCE environment variable */
int loops(int n)
{
	const char* env = std::getenv("XEPHEM_TEST_REDUCE");
	int reduce = env ? std::atoi(env) : 1;
	if (reduce < 1) reduce = 1;
	return n / reduce > 0 ? n / reduce : 1;
}

const int ROUNDS = loops(6);

struct Site { double lon, lat, height; };

/* SRT, Medicina, Noto */
const Site SITES[] = {
	{degrad(9.245), degrad(39.493), 650.0},
	{degrad(11.647), degrad(44.521), 28.0},
	{degrad(14.989), degrad(36.876), 90.0}
};
const int NSITES = sizeof(SITES) / sizeof(SITES[0]);

struct Job {
	int body, moon, pref, site;
	double mjd;
};

std::vector<Job> makeJobs()
{
	std::vector<Job> jobs;
	const int prefs[] = {PREF_TOPO, PREF_GEO};
	for (int t = 0; t < 8; t++) {
		double mjd = 59000.0 + t * 137.37;
		for (int s = 0; s < NSITES; s++) {
			for (int p = 0; p < 2; p++) {
				for (int body = MERCURY; body < NOBJ; body++)
					jobs.push_back(Job{body, X_PLANET, prefs[p], s, mjd});
				for (int moon = J_IO; moon <= J_CALLISTO; moon++)
					jobs.push_back(Job{JUPITER, moon, prefs[p], s, mjd});
				jobs.push_back(Job{SATURN, S_TITAN, prefs[p], s, mjd});
			}
		}
	}
	return jobs;
}

int run(const Job& j, Position& pos)
{
	Now now = makeNow(j.mjd, SITES[j.site].lon, SITES[j.site].lat, SITES[j.site].height);
	return compute(now, j.body, j.pref, pos, j.moon);
}

/* run jobs[] in a random order and count results different from ref[] */
void worker(const std::vector<Job>& jobs, const std::vector<Position>& ref,
  unsigned seed, std::atomic<int>& mismatches, std::atomic<int>& failures)
{
	std::vector<size_t> order(jobs.size());
	for (size_t i = 0; i < order.size(); i++) order[i] = i;
	std::mt19937 rng(seed);
	for (int r = 0; r < ROUNDS; r++) {
		std::shuffle(order.begin(), order.end(), rng);
		for (size_t k = 0; k < order.size(); k++) {
			size_t i = order[k];
			Position p;
			if (run(jobs[i], p) != 0)
				failures++;
			else if (p != ref[i])
				mismatches++;
		}
	}
}

}

class XEphemAstroLib_functional : public ::testing::Test {
protected:
	std::vector<Job> jobs;
	std::vector<Position> ref;

	virtual void SetUp() {
		jobs = makeJobs();
		ref.resize(jobs.size());
		for (size_t i = 0; i < jobs.size(); i++)
			ASSERT_EQ(0, run(jobs[i], ref[i]));
	}
};

/* the serial reference itself must not depend on the call order */
TEST_F(XEphemAstroLib_functional, serialResultsIndependentFromOrder)
{
	std::vector<size_t> order(jobs.size());
	for (size_t i = 0; i < order.size(); i++) order[i] = order.size() - 1 - i;
	int mismatches = 0;
	for (size_t k = 0; k < order.size(); k++) {
		Position p;
		ASSERT_EQ(0, run(jobs[order[k]], p));
		if (p != ref[order[k]]) mismatches++;
	}
	EXPECT_EQ(0, mismatches);
}

/* many threads, mixed bodies, times, sites and preferences */
TEST_F(XEphemAstroLib_functional, concurrentComputationsMatchSerialReference)
{
	std::atomic<int> mismatches(0), failures(0);
	std::vector<std::thread> pool;
	for (int t = 0; t < THREADS; t++)
		pool.push_back(std::thread(worker, std::cref(jobs), std::cref(ref), 1000u + t,
		  std::ref(mismatches), std::ref(failures)));
	for (size_t t = 0; t < pool.size(); t++)
		pool[t].join();
	EXPECT_EQ(0, failures.load());
	EXPECT_EQ(0, mismatches.load()) << "out of " << THREADS * ROUNDS * jobs.size() << " computations";
}

/* all threads hammer the very same instant: exercises the cache-hit paths */
TEST_F(XEphemAstroLib_functional, concurrentSameInstantSameBody)
{
	const Job job{MOON, X_PLANET, PREF_TOPO, 0, 60123.456};
	Position expected;
	ASSERT_EQ(0, run(job, expected));
	std::atomic<int> mismatches(0);
	std::vector<std::thread> pool;
	for (int t = 0; t < THREADS; t++) {
		pool.push_back(std::thread([&]() {
			for (int i = 0; i < loops(2000); i++) {
				Position p;
				/* alternate with another instant to force cache refills */
				Job other = job;
				other.mjd += (i % 2) ? 0.0 : 0.5;
				run(other, p);
				run(job, p);
				if (p != expected) mismatches++;
			}
		}));
	}
	for (size_t t = 0; t < pool.size(); t++)
		pool[t].join();
	EXPECT_EQ(0, mismatches.load());
}

/* one half of the threads works topocentric, the other geocentric: a shared
 * preference table would make them overwrite each other's setting */
TEST_F(XEphemAstroLib_functional, concurrentPreferencesAreIsolated)
{
	const double mjd = 60321.123;
	Position topo, geo;
	ASSERT_EQ(0, compute(makeNow(mjd), MOON, PREF_TOPO, topo));
	ASSERT_EQ(0, compute(makeNow(mjd), MOON, PREF_GEO, geo));
	ASSERT_TRUE(topo != geo);

	std::atomic<int> wrong(0);
	std::vector<std::thread> pool;
	for (int t = 0; t < THREADS; t++) {
		const bool useTopo = (t % 2) == 0;
		pool.push_back(std::thread([&, useTopo]() {
			pref_set(PREF_EQUATORIAL, useTopo ? PREF_TOPO : PREF_GEO);
			for (int i = 0; i < loops(3000); i++) {
				Now now = makeNow(mjd);
				Obj obj = makeBody(MOON);
				/* no pref_set() here: the thread relies on its own setting */
				obj_cir(&now, &obj);
				Position p = {obj.s_ra, obj.s_dec, obj.s_az, obj.s_alt, obj.s_edist};
				if (p != (useTopo ? topo : geo)) wrong++;
			}
		}));
	}
	for (size_t t = 0; t < pool.size(); t++)
		pool[t].join();
	EXPECT_EQ(0, wrong.load());
}

/* earth satellites keep their whole state in file scope variables */
TEST_F(XEphemAstroLib_functional, concurrentEarthSatellites)
{
	Obj sat;
	ASSERT_TRUE(makeSatellite(sat));
	std::vector<double> ref;
	for (int i = 0; i < 50; i++) {
		Obj s = sat;
		Now now = makeNow(59389.0 + i * 0.01, SITES[i % NSITES].lon, SITES[i % NSITES].lat,
		  SITES[i % NSITES].height);
		ASSERT_EQ(0, obj_cir(&now, &s));
		ref.push_back(s.s_alt);
		ref.push_back(s.s_az);
		ref.push_back(s.s_range);
	}
	std::atomic<int> mismatches(0);
	std::vector<std::thread> pool;
	for (int t = 0; t < THREADS; t++) {
		pool.push_back(std::thread([&, t]() {
			for (int r = 0; r < loops(40); r++) {
				for (int k = 0; k < 50; k++) {
					int i = (k * 7 + t + r) % 50;
					Obj s = sat;
					Now now = makeNow(59389.0 + i * 0.01, SITES[i % NSITES].lon,
					  SITES[i % NSITES].lat, SITES[i % NSITES].height);
					obj_cir(&now, &s);
					if (s.s_alt != ref[3 * i] || s.s_az != ref[3 * i + 1] || s.s_range != ref[3 * i + 2])
						mismatches++;
				}
			}
		}));
	}
	for (size_t t = 0; t < pool.size(); t++)
		pool[t].join();
	EXPECT_EQ(0, mismatches.load());
}

/* low level, cache based functions called directly from many threads */
TEST_F(XEphemAstroLib_functional, concurrentLowLevelCaches)
{
	const int N = 64;
	std::vector<double> mj(N), eps(N), deps(N), dpsi(N), gst(N), ra(N), dec(N);
	for (int i = 0; i < N; i++) {
		mj[i] = J2000 + i * 91.3;
		obliquity(mj[i], &eps[i]);
		nutation(mj[i], &deps[i], &dpsi[i]);
		utc_gst(mj[i], 7.5, &gst[i]);
		ra[i] = degrad(83.6331); dec[i] = degrad(22.0145);
		precess(J2000, mj[i], &ra[i], &dec[i]);
	}
	std::atomic<int> mismatches(0);
	std::vector<std::thread> pool;
	for (int t = 0; t < THREADS; t++) {
		pool.push_back(std::thread([&, t]() {
			for (int r = 0; r < loops(200); r++) {
				int i = (r * 13 + t * 5) % N;
				double e, de, dp, g, a = degrad(83.6331), d = degrad(22.0145);
				obliquity(mj[i], &e);
				nutation(mj[i], &de, &dp);
				utc_gst(mj[i], 7.5, &g);
				precess(J2000, mj[i], &a, &d);
				if (e != eps[i] || de != deps[i] || dp != dpsi[i] || g != gst[i] || a != ra[i] || d != dec[i])
					mismatches++;
			}
		}));
	}
	for (size_t t = 0; t < pool.size(); t++)
		pool[t].join();
	EXPECT_EQ(0, mismatches.load());
}

/* setMoonDir() is process wide: changing it while other threads compute moons
 * must be safe. Directories without data make the library fall back to its
 * analytical model, so results must not change. */
TEST_F(XEphemAstroLib_functional, concurrentSetMoonDir)
{
	const Job job{JUPITER, J_EUROPA, PREF_TOPO, 0, 60000.5};
	Position expected;
	setMoonDir(NULL);
	ASSERT_EQ(0, run(job, expected));
	std::atomic<bool> stop(false);
	std::atomic<int> mismatches(0);
	std::thread setter([&]() {
		while (!stop) {
			setMoonDir(NULL);
			std::this_thread::yield();
		}
	});
	std::vector<std::thread> pool;
	for (int t = 0; t < THREADS; t++) {
		pool.push_back(std::thread([&]() {
			for (int i = 0; i < loops(500); i++) {
				Position p;
				Job other = job;
				other.mjd += (i % 2) ? 0.0 : 0.25;
				run(other, p);
				run(job, p);
				if (p != expected) mismatches++;
			}
		}));
	}
	for (size_t t = 0; t < pool.size(); t++)
		pool[t].join();
	stop = true;
	setter.join();
	EXPECT_EQ(0, mismatches.load());
}
