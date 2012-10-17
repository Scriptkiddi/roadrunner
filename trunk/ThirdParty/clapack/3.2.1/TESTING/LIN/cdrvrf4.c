/* cdrvrf4.f -- translated by f2c (version 20061008).
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

/* Common Block Declarations */

struct {
    char srnamt[32];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static integer c__2 = 2;
static integer c__4 = 4;
static integer c__1 = 1;

/* Subroutine */ int cdrvrf4_(integer *nout, integer *nn, integer *nval, real 
	*thresh, complex *c1, complex *c2, integer *ldc, complex *crf, 
	complex *a, integer *lda, real *s_work_clange__)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char uplos[1*2] = "U" "L";
    static char forms[1*2] = "N" "C";
    static char transs[1*2] = "N" "C";

    /* Format strings */
    static char fmt_9999[] = "(1x,\002 *** Error(s) or Failure(s) while test"
	    "ing CHFRK               ***\002)";
    static char fmt_9997[] = "(1x,\002     Failure in \002,a5,\002, CFORM="
	    "'\002,a1,\002',\002,\002 UPLO='\002,a1,\002',\002,\002 TRANS="
	    "'\002,a1,\002',\002,\002 N=\002,i3,\002, K =\002,i3,\002, test"
	    "=\002,g12.5)";
    static char fmt_9996[] = "(1x,\002All tests for \002,a5,\002 auxiliary r"
	    "outine passed the \002,\002threshold (\002,i5,\002 tests run)"
	    "\002)";
    static char fmt_9995[] = "(1x,a6,\002 auxiliary routine:\002,i5,\002 out"
	    " of \002,i5,\002 tests failed to pass the threshold\002)";

    /* System generated locals */
    integer a_dim1, a_offset, c1_dim1, c1_offset, c2_dim1, c2_offset, i__1, 
	    i__2, i__3, i__4, i__5, i__6, i__7;
    real r__1;
    complex q__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsle(cilist *), e_wsle(void), s_wsfe(cilist *), e_wsfe(void), 
	    do_fio(integer *, char *, ftnlen);

    /* Local variables */
    integer i__, j, k, n, iik, iin;
    real eps, beta;
    integer info;
    char uplo[1];
    integer nrun;
    real alpha;
    integer nfail, iseed[4];
    extern /* Subroutine */ int cherk_(char *, char *, integer *, integer *, 
	    real *, complex *, integer *, real *, complex *, integer *), chfrk_(char *, char *, char *, integer *, 
	    integer *, real *, complex *, integer *, real *, complex *);
    char cform[1];
    integer iform;
    real norma, normc;
    char trans[1];
    integer iuplo;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *);
    integer ialpha;
    extern /* Complex */ VOID clarnd_(complex *, integer *, integer *);
    extern doublereal slamch_(char *), slarnd_(integer *, integer *);
    integer itrans;
    extern /* Subroutine */ int ctfttr_(char *, char *, integer *, complex *, 
	    complex *, integer *, integer *), ctrttf_(char *, 
	    char *, integer *, complex *, integer *, complex *, integer *);
    real result[1];

    /* Fortran I/O blocks */
    static cilist io___28 = { 0, 0, 0, 0, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9995, 0 };



/*  -- LAPACK test routine (version 3.2.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2008 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CDRVRF4 tests the LAPACK RFP routines: */
/*      CHFRK */

/*  Arguments */
/*  ========= */

/*  NOUT          (input) INTEGER */
/*                The unit number for output. */

/*  NN            (input) INTEGER */
/*                The number of values of N contained in the vector NVAL. */

/*  NVAL          (input) INTEGER array, dimension (NN) */
/*                The values of the matrix dimension N. */

/*  THRESH        (input) REAL */
/*                The threshold value for the test ratios.  A result is */
/*                included in the output file if RESULT >= THRESH.  To have */
/*                every test ratio printed, use THRESH = 0. */

/*  C1            (workspace) COMPLEX array, dimension (LDC,NMAX) */

/*  C2            (workspace) COMPLEX array, dimension (LDC,NMAX) */

/*  LDC           (input) INTEGER */
/*                The leading dimension of the array A.  LDA >= max(1,NMAX). */

/*  CRF           (workspace) COMPLEX array, dimension ((NMAX*(NMAX+1))/2). */

/*  A             (workspace) COMPLEX array, dimension (LDA,NMAX) */

/*  LDA           (input) INTEGER */
/*                The leading dimension of the array A.  LDA >= max(1,NMAX). */

/*  S_WORK_CLANGE (workspace) REAL array, dimension (NMAX) */

/*  ===================================================================== */
/*     .. */
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
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Data statements .. */
    /* Parameter adjustments */
    --nval;
    c2_dim1 = *ldc;
    c2_offset = 1 + c2_dim1;
    c2 -= c2_offset;
    c1_dim1 = *ldc;
    c1_offset = 1 + c1_dim1;
    c1 -= c1_offset;
    --crf;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --s_work_clange__;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    nrun = 0;
    nfail = 0;
    info = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }
    eps = slamch_("Precision");

    i__1 = *nn;
    for (iin = 1; iin <= i__1; ++iin) {

	n = nval[iin];

	i__2 = *nn;
	for (iik = 1; iik <= i__2; ++iik) {

	    k = nval[iin];

	    for (iform = 1; iform <= 2; ++iform) {

		*(unsigned char *)cform = *(unsigned char *)&forms[iform - 1];

		for (iuplo = 1; iuplo <= 2; ++iuplo) {

		    *(unsigned char *)uplo = *(unsigned char *)&uplos[iuplo - 
			    1];

		    for (itrans = 1; itrans <= 2; ++itrans) {

			*(unsigned char *)trans = *(unsigned char *)&transs[
				itrans - 1];

			for (ialpha = 1; ialpha <= 4; ++ialpha) {

			    if (ialpha == 1) {
				alpha = 0.f;
				beta = 0.f;
			    } else if (ialpha == 1) {
				alpha = 1.f;
				beta = 0.f;
			    } else if (ialpha == 1) {
				alpha = 0.f;
				beta = 1.f;
			    } else {
				alpha = slarnd_(&c__2, iseed);
				beta = slarnd_(&c__2, iseed);
			    }

/*                       All the parameters are set: */
/*                          CFORM, UPLO, TRANS, M, N, */
/*                          ALPHA, and BETA */
/*                       READY TO TEST! */

			    ++nrun;

			    if (itrans == 1) {

/*                          In this case we are NOTRANS, so A is N-by-K */

				i__3 = k;
				for (j = 1; j <= i__3; ++j) {
				    i__4 = n;
				    for (i__ = 1; i__ <= i__4; ++i__) {
					i__5 = i__ + j * a_dim1;
					clarnd_(&q__1, &c__4, iseed);
					a[i__5].r = q__1.r, a[i__5].i = 
						q__1.i;
				    }
				}

				norma = clange_("I", &n, &k, &a[a_offset], 
					lda, &s_work_clange__[1]);

			    } else {

/*                          In this case we are TRANS, so A is K-by-N */

				i__3 = n;
				for (j = 1; j <= i__3; ++j) {
				    i__4 = k;
				    for (i__ = 1; i__ <= i__4; ++i__) {
					i__5 = i__ + j * a_dim1;
					clarnd_(&q__1, &c__4, iseed);
					a[i__5].r = q__1.r, a[i__5].i = 
						q__1.i;
				    }
				}

				norma = clange_("I", &k, &n, &a[a_offset], 
					lda, &s_work_clange__[1]);

			    }


/*                       Generate C1 our N--by--N Hermitian matrix. */
/*                       Make sure C2 has the same upper/lower part, */
/*                       (the one that we do not touch), so */
/*                       copy the initial C1 in C2 in it. */

			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
				i__4 = n;
				for (i__ = 1; i__ <= i__4; ++i__) {
				    i__5 = i__ + j * c1_dim1;
				    clarnd_(&q__1, &c__4, iseed);
				    c1[i__5].r = q__1.r, c1[i__5].i = q__1.i;
				    i__5 = i__ + j * c2_dim1;
				    i__6 = i__ + j * c1_dim1;
				    c2[i__5].r = c1[i__6].r, c2[i__5].i = c1[
					    i__6].i;
				}
			    }

/*                       (See comment later on for why we use CLANGE and */
/*                       not CLANHE for C1.) */

			    normc = clange_("I", &n, &n, &c1[c1_offset], ldc, 
				    &s_work_clange__[1]);

			    s_copy(srnamc_1.srnamt, "CTRTTF", (ftnlen)32, (
				    ftnlen)6);
			    ctrttf_(cform, uplo, &n, &c1[c1_offset], ldc, &
				    crf[1], &info);

/*                       call zherk the BLAS routine -> gives C1 */

			    s_copy(srnamc_1.srnamt, "CHERK ", (ftnlen)32, (
				    ftnlen)6);
			    cherk_(uplo, trans, &n, &k, &alpha, &a[a_offset], 
				    lda, &beta, &c1[c1_offset], ldc);

/*                       call zhfrk the RFP routine -> gives CRF */

			    s_copy(srnamc_1.srnamt, "CHFRK ", (ftnlen)32, (
				    ftnlen)6);
			    chfrk_(cform, uplo, trans, &n, &k, &alpha, &a[
				    a_offset], lda, &beta, &crf[1]);

/*                       convert CRF in full format -> gives C2 */

			    s_copy(srnamc_1.srnamt, "CTFTTR", (ftnlen)32, (
				    ftnlen)6);
			    ctfttr_(cform, uplo, &n, &crf[1], &c2[c2_offset], 
				    ldc, &info);

/*                       compare C1 and C2 */

			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
				i__4 = n;
				for (i__ = 1; i__ <= i__4; ++i__) {
				    i__5 = i__ + j * c1_dim1;
				    i__6 = i__ + j * c1_dim1;
				    i__7 = i__ + j * c2_dim1;
				    q__1.r = c1[i__6].r - c2[i__7].r, q__1.i =
					     c1[i__6].i - c2[i__7].i;
				    c1[i__5].r = q__1.r, c1[i__5].i = q__1.i;
				}
			    }

/*                       Yes, C1 is Hermitian so we could call CLANHE, */
/*                       but we want to check the upper part that is */
/*                       supposed to be unchanged and the diagonal that */
/*                       is supposed to be real -> CLANGE */

			    result[0] = clange_("I", &n, &n, &c1[c1_offset], 
				    ldc, &s_work_clange__[1]);
/* Computing MAX */
			    r__1 = dabs(alpha) * norma * norma + dabs(beta) * 
				    normc;
			    result[0] = result[0] / dmax(r__1,1.f) / max(n,1) 
				    / eps;

			    if (result[0] >= *thresh) {
				if (nfail == 0) {
				    io___28.ciunit = *nout;
				    s_wsle(&io___28);
				    e_wsle();
				    io___29.ciunit = *nout;
				    s_wsfe(&io___29);
				    e_wsfe();
				}
				io___30.ciunit = *nout;
				s_wsfe(&io___30);
				do_fio(&c__1, "CHFRK", (ftnlen)5);
				do_fio(&c__1, cform, (ftnlen)1);
				do_fio(&c__1, uplo, (ftnlen)1);
				do_fio(&c__1, trans, (ftnlen)1);
				do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&result[0], (ftnlen)
					sizeof(real));
				e_wsfe();
				++nfail;
			    }

/* L100: */
			}
/* L110: */
		    }
/* L120: */
		}
/* L130: */
	    }
/* L140: */
	}
/* L150: */
    }

/*     Print a summary of the results. */

    if (nfail == 0) {
	io___31.ciunit = *nout;
	s_wsfe(&io___31);
	do_fio(&c__1, "CHFRK", (ftnlen)5);
	do_fio(&c__1, (char *)&nrun, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	io___32.ciunit = *nout;
	s_wsfe(&io___32);
	do_fio(&c__1, "CHFRK", (ftnlen)5);
	do_fio(&c__1, (char *)&nfail, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nrun, (ftnlen)sizeof(integer));
	e_wsfe();
    }


    return 0;

/*     End of CDRVRF4 */

} /* cdrvrf4_ */
