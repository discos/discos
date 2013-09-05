/* matin.f -- translated by f2c (version 20031025).
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

/* Subroutine */ int matin_(integer *n, real *a, real *b, integer *msignl)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1;

    /* Local variables */
    static integer i__, j, k, l;
    static real t;
    static integer l1;
    static real det, swap;
    static integer irow, jrow;
    static real amaxi;
    static integer index[25]	/* was [5][5] */;
    static real pivot[5], determ;
    static integer icolum, jcolum, ipivot[5];


/*     MATRIX INVERSION ROUTINE 5 PARAMETERS. */


    /* Parameter adjustments */
    --b;
    a -= 6;

    /* Function Body */
    determ = (float)1.;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	ipivot[j - 1] = 0;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	amaxi = (float)0.;
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
	    if (ipivot[j - 1] == 1) {
		goto L105;
	    }
	    i__3 = *n;
	    for (k = 1; k <= i__3; ++k) {
		if (ipivot[k - 1] == 1) {
		    goto L100;
		}
		if (ipivot[k - 1] > 1) {
		    goto L730;
		}
		if (dabs(amaxi) >= (r__1 = a[j + k * 5], dabs(r__1))) {
		    goto L100;
		}
		irow = j;
		icolum = k;
		amaxi = a[j + k * 5];
L100:
		;
	    }
L105:
	    ;
	}

	++ipivot[icolum - 1];
	if (irow == icolum) {
	    goto L260;
	}
	determ = -determ;
	i__2 = *n;
	for (l = 1; l <= i__2; ++l) {
	    swap = a[irow + l * 5];
	    a[irow + l * 5] = a[icolum + l * 5];
	    a[icolum + l * 5] = swap;
	}
	swap = b[irow];
	b[irow] = b[icolum];
	b[icolum] = swap;
L260:
	index[i__ - 1] = irow;
	index[i__ + 4] = icolum;
	pivot[i__ - 1] = a[icolum + icolum * 5];
	if (pivot[i__ - 1] < (float)1e-30) {
	    goto L710;
	}
L320:
	det = determ * pivot[i__ - 1];
	if (det != (float)0.) {
	    goto L329;
	}
	*msignl = 0;
	determ *= (float)1e16;
	goto L320;
L329:
	determ = det;
	a[icolum + icolum * 5] = (float)1.;
	i__2 = *n;
	for (l = 1; l <= i__2; ++l) {
	    if (pivot[i__ - 1] < (r__1 = a[icolum + l * 5], dabs(r__1)) * 
		    1e-30) {
		goto L710;
	    }
	    a[icolum + l * 5] /= pivot[i__ - 1];
	}
	b[icolum] /= pivot[i__ - 1];
	i__2 = *n;
	for (l1 = 1; l1 <= i__2; ++l1) {
	    if (l1 == icolum) {
		goto L550;
	    }
	    t = a[l1 + icolum * 5];
	    a[l1 + icolum * 5] = (float)0.;
	    i__3 = *n;
	    for (l = 1; l <= i__3; ++l) {
		a[l1 + l * 5] -= a[icolum + l * 5] * t;
	    }
	    b[l1] -= b[icolum] * t;
L550:
	    ;
	}
    }

    i__2 = *n;
    for (i__ = 1; i__ <= i__2; ++i__) {
	l = *n + 1 - i__;
	if (index[l - 1] == index[l + 4]) {
	    goto L709;
	}
	jrow = index[l - 1];
	jcolum = index[l + 4];
	i__1 = *n;
	for (k = 1; k <= i__1; ++k) {
	    swap = a[k + jrow * 5];
	    a[k + jrow * 5] = a[k + jcolum * 5];
	    a[k + jcolum * 5] = swap;
	}
L709:
	;
    }
    *msignl = -1;
    return 0;

/*     MIXED DIMENSIONS */
L730:
    *msignl = 2;
    return 0;

/*     SINGULAR MATRIX */
L710:
    *msignl = 1;
    return 0;
} /* matin_ */

#ifdef __cplusplus
	}
#endif
