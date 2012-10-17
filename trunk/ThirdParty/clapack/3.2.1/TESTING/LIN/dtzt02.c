/* dtzt02.f -- translated by f2c (version 20061008).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__7 = 7;
static doublereal c_b5 = 0.;
static doublereal c_b6 = 1.;

doublereal dtzt02_(integer *m, integer *n, doublereal *af, integer *lda, 
	doublereal *tau, doublereal *work, integer *lwork)
{
    /* System generated locals */
    integer af_dim1, af_offset, i__1, i__2;
    doublereal ret_val;

    /* Local variables */
    integer i__;
    doublereal rwork[1];
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    xerbla_(char *, integer *), dlatzm_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTZT02 returns */
/*       || I - Q'*Q || / ( M * eps) */
/*  where the matrix Q is defined by the Householder transformations */
/*  generated by DTZRQF. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix AF. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix AF. */

/*  AF      (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The output of DTZRQF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array AF. */

/*  TAU     (input) DOUBLE PRECISION array, dimension (M) */
/*          Details of the Householder transformations as returned by */
/*          DTZRQF. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          length of WORK array. Must be >= N*N+N */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    af_dim1 = *lda;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    --tau;
    --work;

    /* Function Body */
    ret_val = 0.;

    if (*lwork < *n * *n + *n) {
	xerbla_("DTZT02", &c__7);
	return ret_val;
    }

/*     Quick return if possible */

    if (*m <= 0 || *n <= 0) {
	return ret_val;
    }

/*     Q := I */

    dlaset_("Full", n, n, &c_b5, &c_b6, &work[1], n);

/*     Q := P(1) * ... * P(m) * Q */

    for (i__ = *m; i__ >= 1; --i__) {
	i__1 = *n - *m + 1;
	dlatzm_("Left", &i__1, n, &af[i__ + (*m + 1) * af_dim1], lda, &tau[
		i__], &work[i__], &work[*m + 1], n, &work[*n * *n + 1]);
/* L10: */
    }

/*     Q := P(m) * ... * P(1) * Q */

    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *n - *m + 1;
	dlatzm_("Left", &i__2, n, &af[i__ + (*m + 1) * af_dim1], lda, &tau[
		i__], &work[i__], &work[*m + 1], n, &work[*n * *n + 1]);
/* L20: */
    }

/*     Q := Q - I */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	work[(i__ - 1) * *n + i__] += -1.;
/* L30: */
    }

    ret_val = dlange_("One-norm", n, n, &work[1], n, rwork) / (
	    dlamch_("Epsilon") * (doublereal) max(*m,*n));
    return ret_val;

/*     End of DTZT02 */

} /* dtzt02_ */
