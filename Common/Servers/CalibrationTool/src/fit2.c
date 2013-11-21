/* fit2.f -- translated by f2c (version 20031025).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "f2c.h"

/* Common Block Declarations */

struct {
    integer b_init__[2];
    real amptol, azhmwv[10], cpkmin, diftol, elhmwv[30], foroff[4]	/* 
	    was [2][2] */, gdytol, lvbosc_fs__[2], rxvfac[40], phatol, phjmax,
	     pi, pslope[4]	/* was [2][2] */, pvolt_fs__[30], revoff[4]	
	    /* was [2][2] */, rslope[4]	/* was [2][2] */, sof2hi, son2hi, 
	    son2lo, tmpk_fs__[30], rate0ti_fs__;
    integer span0ti_fs__, i20kch, i70kch, iacttp[2], ibdb, ilenal, ilents, 
	    ilvtl_fs__[2], imonds, intamp, intpha, itr2vc[112]	/* was [28][4]
	     */, lvsens, modsa, nhorwv, rxncodes, nprset, nrx_fs__, rpdt_fs__[
	    2], rpro_fs__[2], iswavif3_fs__, decoder4, pcalcntrl;
    logical kcheck, kadapt_fs__[2], kiwslw_fs__[2];
    shortint idevmat[32], idevdb[32], idevwand[32], rxlcode[120]	/* 
	    was [3][40] */, lalrm[3], ltsrs[5], model0ti_fs__, svermajor_fs__,
	     sverminor_fs__, sverpatch_fs__, iaxis[2], mk4dec_fs__, 
	    e_fill_init__[1];
    integer e_init__;
} fscom_init__;

#define fscom_init__1 fscom_init__

struct {
    integer b_quikr__[2];
    real arr1fx_fs__[6], arr2fx_fs__[6], arr3fx_fs__[6], arr4fx_fs__[6], 
	    arr5fx_fs__[6], arr6fx_fs__[6], beamsz_fs__[6], bm1nf_fs__, 
	    bm2nf_fs__, bmfp_fs__, cal1nf, cal2nf, calfp, caltmp[6], 
	    cor1fx_fs__, cor2fx_fs__, cor3fx_fs__, cor4fx_fs__, cor5fx_fs__, 
	    cor6fx_fs__, ctofnf, fastfw[4]	/* was [2][2] */, fastrv[4]	
	    /* was [2][2] */, fiwo_fs__[4]	/* was [2][2] */, flx1fx_fs__,
	     flx2fx_fs__, flx3fx_fs__, flx4fx_fs__, flx5fx_fs__, flx6fx_fs__, 
	    fowo_fs__[4]	/* was [2][2] */, fx1nf_fs__, fx2nf_fs__, 
	    fxfp_fs__, lmtn_fs__[4]	/* was [2][2] */, lmtp_fs__[4]	/* 
	    was [2][2] */, pethr[2], rbdread_fs__[2], rbdwrite_fs__[2], 
	    rnglc_fs__[2], rsdread_fs__[2], rsdwrite_fs__[2], rsread_fs__[2], 
	    rswrite_fs__[2], rv13_fs__[2], rv15flip_fs__[2], rv15for_fs__[2], 
	    rv15rev_fs__[2], rv15scale_fs__[2], rvrevw_fs__[2], rvw0_fs__[2], 
	    rvw8_fs__[2], siwo_fs__[4]	/* was [2][2] */, sowo_fs__[4]	/* 
	    was [2][2] */, stepfp, steplc_fs__[2], stepnf, tp1ifd, tp2ifd, 
	    tperer[2], tpsor[31], tpspc[31], tpzero[31], vadcrx, vadcst, 
	    vltpk_fs__[2], vminpk_fs__[2], slowfw[4]	/* was [2][2] */, 
	    slowrv[4]	/* was [2][2] */, tpidiff[32], caltemps[32], ssizfp;
    integer iadcrx, iadcst, iatlvc[15], iatuvc[15], ibugpc, ibwtap[2], ibxhrx,
	     ibxhst, ibypas[2], ibr4tap[2], ibyppc, ichand, ichper[2], 
	    iclwo_fs__[2], idchrx, idchst, idecpa_fs__[2], ieqtap[2], ieq4tap[
	    2], ierrdc_fs__, ifamrx[3], ifamst[3], ihdlc_fs__[2], ihdpk_fs__[
	    2], ihdwo_fs__[2], ilohst, ilowtp[2], imdl1fx_fs__, imdl2fx_fs__, 
	    imdl3fx_fs__, imdl4fx_fs__, imdl5fx_fs__, imdl6fx_fs__, imoddc, 
	    imodpe[2], inpfm, insper[2], intpfp, intpnf, iol1if_fs__, 
	    iol2if_fs__, ipaupc, iremif, iremvc[15], ireppc, irsttp[2], 
	    isethr[2], isynfm, iterpk_fs__[2], itrakaus_fs__[2], 
	    itrakbus_fs__[2], itrken[56]	/* was [28][2] */, 
	    itrkenus_fs__[56]	/* was [28][2] */, itrkpa[4]	/* was [2][2] 
	    */, itrkpc[28], itrper[2], lostrx, lostst, lswcal, ltrken[4], 
	    nblkpc, ncycpc, nptsfp, nrepfp, nrepnf, nsamplc_fs__[2], 
	    nsamppk_fs__[2], ichfp_fs__, ich1nf_fs__, ich2nf_fs__, 
	    iolif3_fs__, itapof[400]	/* was [200][2] */, iwtfp;
    logical kvrevw_fs__[2], kv15rev_fs__[2], kv15for_fs__[2], kv15scale_fs__[
	    2], kv13_fs__[2], kv15flip_fs__[2], kvw0_fs__[2], kvw8_fs__[2], 
	    ksread_fs__[2], kswrite_fs__[2], ksdread_fs__[2], ksdwrite_fs__[2]
	    , kbdwrite_fs__[2], kbdread_fs__[2], khecho_fs__, kpeakv_fs__[2], 
	    kposhd_fs__[4]	/* was [2][2] */, krdwo_fs__[2], kwrwo_fs__[2]
	    , kdoaux_fs__[2], krptp_fs__[2], kmvtp_fs__[2], kentp_fs__[2], 
	    kautohd_fs__[2], kldtp_fs__[2];
    shortint lauxfm[6], lauxfm4[4], laxfp[2], ldevfp, ldv1nf, ldv2nf, ltpchk[
	    4]	/* was [2][2] */, ltpnum[8]	/* was [4][2] */, loprid[6], 
	    qfill[1];
    integer e_quikr__;
} fscom_quikr__;

#define fscom_quikr__1 fscom_quikr__

struct {
    doublereal ra50, dec50, radat, decdat, alat, wlong, user_dev1_value__, 
	    user_dev2_value__;
    real azoff, decoff, eloff, istptp[2], itactp[2], raoff, xoff, yoff, 
	    ep1950, tempwx, preswx, humiwx, speedwx, cablev, systmp[32], 
	    epoch, height, diaman, slew1, slew2, lolim1, lolim2, uplim1, 
	    uplim2, horaz[30], horel[30], motorv[2], inscint[2], inscsl[2], 
	    outscint[2], outscsl[2], wrvolt[2], rateti_fs__, posnhd[4]	/* 
	    was [2][2] */, freqvc[15], motorv2[2], wrvolt2[2], wrvolt4[2], 
	    wrvolt42[2], extbwvc[15], cablevl;
    integer iclbox, iclopr, spanti_fs__, epochti_fs__, offsetti_fs__, 
	    secsoffti_fs__, ichfm_cn_tm__, cips[2], freqif3, fm4enable[2], 
	    tpivc[15], mifd_tpi__[3], bbc_tpi__[28]	/* was [2][14] */, 
	    vifd_tpi__[4], ifp_tpi__[4], logchg;
    logical khalt, kecho, kenastk[4]	/* was [2][2] */, klvdt_fs__[2], 
	    knewtape[2];
    integer icaptp[2], icheck[23], ilokvc[15], irdytp[2], irenvc, itraka[2], 
	    itrakb[2], ibmat, ibmcb, ionsor, ipashd[4]	/* was [2][2] */, 
	    ispeed[2], ienatp[2], idirtp[2], imodfm, iratfm, inp1if, inp2if, 
	    ndevlog, rack, drive[2], imaxtpsd[2], ichvlba[19], iskdtpsd[2], 
	    wx_met__, itpthick[2], vform_rate__, capstan[2], stchk[4], sterp, 
	    erchk, drive_type__[2], rack_type__, wrhd_fs__[2], vfm_xpnt__, 
	    ichs2, ibwvc[15], ifp2vc[16], vacsw[2], itpthick2[2], thin[2], 
	    vac4[2], imixif3, vfmenablehi, vfmenablelo, ihdmndel[2], iat1if, 
	    iat2if, iat3if, reccpu[2], select, rdhd_fs__[2], ifd_set__, 
	    if3_set__, imk4fmv, itpivc[15], iapdflg, iswif3_fs__[4], ipcalif3,
	     directionwx, ibds, ndas, idasfilt, idasbits, ichlba[4], 
	    mk4sync_dflt__, icomputer;
    shortint ilexper[4], illog[4], ilnewpr[4], ilnewsk[4], ilprc[4], ilskd[4],
	     ilstp[4], inext[3], lfeet_fs__[6]	/* was [3][2] */, lgen[4]	
	    /* was [2][2] */, lnaant[4], lsorna[5], lfreqv[45]	/* was [3][15]
	     */, idevant[32], idevgpib[32], idevlog[160]	/* was [5][32]
	     */, idevmcb[32], hwid, modelti_fs__, stcnm[4]	/* was [1][4] 
	    */, cwrap[4], user_dev1_name__[2], user_dev2_name__[2], idevds[32]
	    , hostpc_fs__[40];
} fscom_dum__;

#define fscom_dum__1 fscom_dum__

struct {
    char lexper[8], llog[8], lnewpr[8], lnewsk[8], lprc[8], lskd[8], lstp[8];
} fscom_dum2__;

#define fscom_dum2__1 fscom_dum2__

/* Table of constant values */

static integer c__0 = 0;

/* Subroutine */ int fit2_(real *x, real *y, real *x2, real *par, real *epar, 
	integer *npts, integer *npar, real *tol, integer *ntry, E_fp f, real *
	chi, integer *ierr)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static real a[25]	/* was [5][5] */, b[5];
    static integer i__, j, k;
    static real r__, aux[5], xsq;
    static integer itry;
    static real xsum;
    static integer mback;
    extern /* Subroutine */ int matin_(integer *, real *, real *, integer *);
    static integer nfree, iagain;


/*     LEAST SQUARES FITTING ROUTINE...FITS TO FUNCTION F */
/*     WHICH MUST BE DECLARED EXTERNAL IN THE CALLING ROUTINE. */
/*     MAXIMUM NUMBER OF PARAMETERS IS 5.  NOTE THAT AN */
/*     ERROR RESULTS IN A RETURN OF IERR <= 0.  THIS ROUTINE */
/*     REQUIRES SUBROUTINE MATINV. */



/* fscom.i */

/*  This include file includes all the sections of fscom. */
/*  By convention each section is a named common block with a name of the */
/*  form 'fscom_x' where 'x' is the name of the section, e.g. 'fscom_init' */
/*  for the initialization section. */

/*  Each part must have as its first and last variables, integers (no *) */
/*  with names of the form 'b_x' and 'e_x' respectively (b for begin, */
/*  e for end), where 'x' again is the name of the section. */

/*  See fscom_init.i for an example. */

/* params.i */
/* compile time parameters */

/* bits/int */
/* chars/int */
/* chars/int2 */
/* number of ints for maximum aligned unit */
/* maximum number of values in rxdiode.ctl */
/* Mark III comparison bit variable */
/* VLBA comparison bit variable */
/* Mark IV comparison bit variable */
/* S2 comparison bit variable */
/* K4 comparison bit variable */
/* K4MK4 comparison bit variable */
/* K4K3 comparison bit variable */
/* VLBA4 comparison bit variable */
/* VLBA geodesy rack */
/* VLBA drive type 2 */
/* VLBA drive type B */
/* Mark IV 1 head */
/* K41 comparison bit variable */
/* K41U comparison bit variable */
/* K42 comparison bit variable */
/* K42A comparison bit variable */
/* K42B comparison bit variable */
/* K42BU comparison bit variable */
/* K42C comparison bit variable */
/* K41DMS comparison bit variable */
/* K42DMS comparison bit variable */
/* VLBA42 comparison bit variable */
/* LBA comparison bit variable */
/* LBA4 comparison bit variable */
/* MK5 comparison bit variable */
/* MK5A comparison bit variable */
/* MK5A_BS comparison bit variable */
/* MK45 comparison bit variable */
/* VLBA45 comparison bit variable */
/* MK5B comparison bit variable */
/* MK5B_BS comparison bit variable */
/* Max number of LBA DAS allowed */
/* chekr        realtime priority */
/* clock func.  realtime priority */
/* Field System realtime priority */
/* Antenna      realtime priority */
/* Max number of entries in rxdef.ctl */
/* Max number of horizon mask entries */
/* MET Sensor with/without wind sensor */

/* maximum pointing model parameters */
/* maximum chars in a proc parameter */
/* max int words needed for a proc par */

/* max count of stack spack for proc p */

/* rack/drive */
/* rack/drive _types */

/* wx_met */



/* fscom_init.i */

/* See fscom.i for information on the structure */

/* fscom_quikr.i */

/* See fscom.i for information on the structure */


/* fscom_dum.i */

/* See fscom.i for information on the structure */







    /* Parameter adjustments */
    --epar;
    --par;
    --x2;
    --y;
    --x;

    /* Function Body */
    i__1 = *ntry;
    for (itry = 1; itry <= i__1; ++itry) {
	i__2 = *npar;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    b[i__ - 1] = (float)0.;
	    i__3 = *npar;
	    for (j = 1; j <= i__3; ++j) {
/* L20: */
		a[i__ + j * 5 - 6] = (float)0.;
	    }
	}
	i__3 = *npts;
	for (k = 1; k <= i__3; ++k) {
	    i__2 = *npar;
	    for (i__ = 1; i__ <= i__2; ++i__) {
/* L30: */
		aux[i__ - 1] = (*f)(&i__, &x[k], &x2[k], &par[1]);
	    }
	    r__ = y[k] - (*f)(&c__0, &x[k], &x2[k], &par[1]);
	    i__2 = *npar;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ - 1] += r__ * aux[i__ - 1];
		i__4 = *npar;
		for (j = 1; j <= i__4; ++j) {
/* L40: */
		    a[i__ + j * 5 - 6] += aux[i__ - 1] * aux[j - 1];
		}
	    }
	}

	matin_(npar, a, b, &mback);
	if (mback != -1) {
	    goto L200;
	}
	iagain = 0;
	i__4 = *npar;
	for (i__ = 1; i__ <= i__4; ++i__) {
	    if ((r__2 = b[i__ - 1] / sqrt((r__1 = a[i__ + i__ * 5 - 6], dabs(
		    r__1))), dabs(r__2)) > *tol) {
		iagain = 1;
	    }
/* L60: */
	    par[i__] += b[i__ - 1];
	}
	if (iagain == 0) {
	    goto L101;
	}
/* L100: */
    }
    *ierr = -2;
    goto L102;

L101:
    *ierr = itry;

L102:
    *chi = (float)0.;
    i__1 = *npar;
    for (i__ = 1; i__ <= i__1; ++i__) {
	epar[i__] = sqrt((r__1 = a[i__ + i__ * 5 - 6], dabs(r__1)));
/* L110: */
    }
    nfree = *npts - *npar;
    if (nfree <= 0) {
	return 0;
    }
    xsum = (float)0.;
    xsq = (float)0.;
    i__1 = *npts;
    for (i__ = 1; i__ <= i__1; ++i__) {
	r__ = y[i__] - (*f)(&c__0, &x[i__], &x2[i__], &par[1]);
/* L120: */
	xsq += r__ * r__;
    }
    *chi = sqrt(xsq / nfree);
    i__1 = *npar;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L140: */
	epar[i__] = *chi * sqrt((r__1 = a[i__ + i__ * 5 - 6], dabs(r__1)));
    }
    return 0;
L200:
    *ierr = -1;
    return 0;
} /* fit2_ */

#ifdef __cplusplus
	}
#endif
