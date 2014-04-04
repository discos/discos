/* fgaus.f -- translated by f2c (version 20031025).
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

doublereal fgaus_(integer *iwhich, real *x, real *x2, real *par)
{
    /* System generated locals */
    real ret_val, r__1;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real e, w;
    extern /* Subroutine */ int put_stderr__(char *, ftnlen);


/*     THREE-PARAMETER SINGLE GAUSSIAN + LINEAR FUNCTION IN 2ND COORDIANTE */


    /* Parameter adjustments */
    --par;

    /* Function Body */
    if (dabs(par[3]) < (r__1 = *x - par[2], dabs(r__1)) * 1e-30) {
/*	put_stderr__(" fp error gauss 1\n\000", (ftnlen)19);*/
	e = (float)0.;
    } else {
	e = (*x - par[2]) / par[3];
    }
    w = exp(e * (float)-2.7725887 * e);
    switch (*iwhich + 1) {
	case 1:  goto L20;
	case 2:  goto L40;
	case 3:  goto L60;
	case 4:  goto L80;
	case 5:  goto L100;
	case 6:  goto L120;
    }
L20:
    ret_val = par[1] * w + par[4] + par[5] * *x2;
    return ret_val;
L40:
    ret_val = w;
    return ret_val;
L60:
    ret_val = par[1] * (float)5.5451774 * e * w / par[3];
    return ret_val;
L80:
    ret_val = par[1] * (float)5.5451774 * e * e * w / par[3];
    return ret_val;
L100:
    ret_val = (float)1.;
    return ret_val;
L120:
    ret_val = *x2;
    return ret_val;
} /* fgaus_ */

#ifdef __cplusplus
	}
#endif
