/* compute Obj fields for natural satellites.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "astro.h"
#include "preferences.h"

/* private cache of planet ephemerides and when they were computed
 * N.B. don't use ones in builtin[] -- they are the user's responsibility.
 */
static XE_TLS ObjPl plobj[NOBJ];
static XE_TLS Now plnow[NOBJ];
static XE_TLS int plpref[NOBJ];	/* PREF_EQUATORIAL used for plobj[] */

/* public builtin storage
 */
static XE_TLS Obj builtin[NBUILTIN];

/* process-wide setting shared by all threads, accessed atomically */
static char *moondir;

static void setMoon (Now *np, Obj *moonop, Obj *planop, MoonData *mdp);
static void init1BI (int idx, int pl, int moon, char *name);
static void initPlobj(void);
static void updatePlobj (Now *np, int code);
static void rotate (double a, double *x, double *y);

/* directory in which to look for auxil moon data files.
 * N.B. caller must supply persistent storage.
 */
void
setMoonDir (char *dir)
{
	__atomic_store_n (&moondir, dir, __ATOMIC_RELEASE);
}

/* return set of builtin objects.
 * caller can use this storage but should never try to free anything.
 */
int
getBuiltInObjs (Obj **opp)
{
	if (!builtin[MERCURY].o_name[0]) {
	    /* first time only */

	    init1BI (MERCURY,   MERCURY, X_PLANET,    "Mercury");

	    init1BI (VENUS,     VENUS,   X_PLANET,    "Venus");

	    init1BI (MARS,      MARS,    X_PLANET,    "Mars");
	    init1BI (PHOBOS,    MARS,    M_PHOBOS,    "Phobos");
	    init1BI (DEIMOS,    MARS,    M_DEIMOS,    "Deimos");

	    init1BI (JUPITER,   JUPITER, X_PLANET,    "Jupiter");
	    init1BI (IO,        JUPITER, J_IO,        "Io");
	    init1BI (EUROPA,    JUPITER, J_EUROPA,    "Europa");
	    init1BI (GANYMEDE,  JUPITER, J_GANYMEDE,  "Ganymede");
	    init1BI (CALLISTO,  JUPITER, J_CALLISTO,  "Callisto");

	    init1BI (SATURN,    SATURN,  X_PLANET,    "Saturn");
	    init1BI (MIMAS,     SATURN,  S_MIMAS,     "Mimas");
	    init1BI (ENCELADUS, SATURN,  S_ENCELADUS, "Enceladus");
	    init1BI (TETHYS,    SATURN,  S_TETHYS,    "Tethys");
	    init1BI (DIONE,     SATURN,  S_DIONE,     "Dione");
	    init1BI (RHEA,      SATURN,  S_RHEA,      "Rhea");
	    init1BI (TITAN,     SATURN,  S_TITAN,     "Titan");
	    init1BI (HYPERION,  SATURN,  S_HYPERION,  "Hyperion");
	    init1BI (IAPETUS,   SATURN,  S_IAPETUS,   "Iapetus");

	    init1BI (URANUS,    URANUS,  X_PLANET,    "Uranus");
	    init1BI (ARIEL,     URANUS,  U_ARIEL,     "Ariel");
	    init1BI (UMBRIEL,   URANUS,  U_UMBRIEL,   "Umbriel");
	    init1BI (TITANIA,   URANUS,  U_TITANIA,   "Titania");
	    init1BI (OBERON,    URANUS,  U_OBERON,    "Oberon");
	    init1BI (MIRANDA,   URANUS,  U_MIRANDA,   "Miranda");

	    init1BI (NEPTUNE,   NEPTUNE, X_PLANET,    "Neptune");

	    init1BI (PLUTO,     PLUTO,   X_PLANET,    "Pluto");

	    init1BI (SUN,       SUN,     X_PLANET,    "Sun");

	    init1BI (MOON,      MOON,    X_PLANET,    "Moon");
	}

	*opp = builtin;
	return (NBUILTIN);
}

static void
init1BI (int idx, int pl, int moon, char *name)
{
	strcpy (builtin[idx].o_name, name);
	builtin[idx].o_type = PLANET;
	builtin[idx].pl_code = pl;
	builtin[idx].pl_moon = moon;
}

/* find the circumstances for natural satellite object op at np.
 * TODO: distances and helio coords just copied from parent planet.
 */
int
plmoon_cir (Now *np, Obj *moonop)
{
	Obj *sunop = (Obj*)&plobj[SUN];
	char *mdir = __atomic_load_n (&moondir, __ATOMIC_ACQUIRE);
	MoonData md[X_MAXNMOONS];
	double sz, t1, t2;
	double pra, pdec;
	MoonData *mdp;
	Obj *planop;

	/* init plobj[] */
	if (!((Obj *)&plobj[0])->o_type)
	    initPlobj();

	/* get sun @ np */
	updatePlobj (np, SUN);

	/* get parent planet and moon info @ np */
	switch (moonop->pl_code) {

	case MARS:
	case PHOBOS:
	case DEIMOS:

	    planop = (Obj*)&plobj[MARS];

	    updatePlobj (np, MARS);

	    /* don't worry, this already caches based on the same inputs */
	    marsm_data (mjd, mdir, sunop, planop, &sz, &pra, &pdec, md);
	    mdp = &md[moonop->pl_moon];
	    break;

	case JUPITER:
	case IO:
	case EUROPA:
	case GANYMEDE:
	case CALLISTO:

	    planop = (Obj*)&plobj[JUPITER];

	    updatePlobj (np, JUPITER);

	    /* don't worry, this already caches based on the same inputs */
	    jupiter_data (mjd,mdir,sunop,planop,&sz,&t1,&t2,&pra,&pdec,md);
	    mdp = &md[moonop->pl_moon];
	    moonop->pl_aux1 = t1;
	    moonop->pl_aux2 = t2;
	    break;

	case SATURN:
	case MIMAS:
	case ENCELADUS:
	case TETHYS:
	case DIONE:
	case RHEA:
	case TITAN:
	case HYPERION:
	case IAPETUS:

	    planop = (Obj*)&plobj[SATURN];

	    updatePlobj (np, SATURN);

	    /* don't worry, this already caches based on the same inputs */
	    saturn_data (mjd,mdir,sunop,planop,&sz,&t1,&t2,&pra,&pdec,md);
	    mdp = &md[moonop->pl_moon];
	    moonop->pl_aux1 = t1;
	    moonop->pl_aux2 = t2;
	    break;

	case URANUS:
	case ARIEL:
	case UMBRIEL:
	case TITANIA:
	case OBERON:
	case MIRANDA:

	    planop = (Obj*)&plobj[URANUS];

	    updatePlobj (np, URANUS);

	    /* don't worry, this already caches based on the same inputs */
	    uranus_data (mjd, mdir, sunop, planop, &sz, &pra, &pdec, md);
	    mdp = &md[moonop->pl_moon];
	    break;

	default:

	    printf ("Called plmoon_cir with bad code: %d\n",moonop->pl_code);
	    return (-1);

	}

	/* set moonop */
	setMoon (np, moonop, planop, mdp);

	return (0);
}

/* bring the cached plobj[code] up to date with np and the current
 * PREF_EQUATORIAL preference, which obj_cir() also depends on.
 */
static void
updatePlobj (Now *np, int code)
{
	int pref = pref_get (PREF_EQUATORIAL);

	if (memcmp (&plnow[code], np, sizeof(Now)) || plpref[code] != pref) {
	    obj_cir (np, (Obj*)&plobj[code]);
	    memcpy (&plnow[code], np, sizeof(Now));
	    plpref[code] = pref;
	}
}

static void
initPlobj()
{
	int i;

	for (i = 0; i < NOBJ; i++) {
	    ((Obj*)&plobj[i])->o_type = PLANET;
	    ((Obj*)&plobj[i])->pl_code = i;
	}
}

/* set moonop->s_* fields.
 * np is needed to get local parallactic angle.
 */
static void
setMoon (Now *np, Obj *moonop, Obj *planop, MoonData *mdp)
{
	double pa, dra, ddec;

	/* just copy most fields from planet for now */
	moonop->s_gaera = planop->s_gaera;	/* TODO */
	moonop->s_gaedec = planop->s_gaedec;	/* TODO */
	moonop->s_elong = planop->s_elong;	/* TODO */
	moonop->s_size = 0;			/* TODO */
	moonop->s_sdist = planop->s_sdist;	/* TODO */
	moonop->s_edist = planop->s_edist;	/* TODO */
	moonop->s_hlat = planop->s_hlat;	/* TODO */
	moonop->s_hlong = planop->s_hlong;	/* TODO */
	moonop->s_phase = planop->s_phase;	/* TODO */

	/* new ra/dec directly from mdp */
	moonop->s_ra = mdp->ra;
	moonop->s_dec = mdp->dec;

	/* geoemtry info */
	moonop->pl_x = mdp->x;
	moonop->pl_y = mdp->y;
	moonop->pl_z = mdp->z;
	moonop->pl_evis = mdp->evis;
	moonop->pl_svis = mdp->svis;

	/* tweak alt/az by change in ra/dec rotated by pa */
	pa = parallacticLDA (lat, planop->s_dec, planop->s_alt);
	if (planop->s_az < PI)
	    pa = -pa;			/* rotation radec to altaz */
	dra = (moonop->s_ra - planop->s_ra)*cos(planop->s_dec);
	ddec = moonop->s_dec - planop->s_dec;
	rotate (pa, &dra, &ddec);
	moonop->s_alt = planop->s_alt + ddec;
	moonop->s_az = planop->s_az - dra/cos(planop->s_alt);

	/* new mag directly from mdp */
	set_smag (moonop, mdp->mag);

	/* name */
	strcpy (moonop->o_name, mdp->full);
}

/* rotate ccw by a */
static void
rotate (double a, double *x, double *y)
{
	double sa = sin(a);
	double ca = cos(a);
	double xp = (*x)*ca - (*y)*sa;
	double yp = (*x)*sa + (*y)*ca;
	*x = xp;
	*y = yp;
}
