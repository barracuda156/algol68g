//! @file single-gamic.c
//! @author J. Marcel van der Veer

//! @section Copyright
//!
//! This file is part of Algol68G - an Algol 68 compiler-interpreter.
//! Copyright 2001-2024 J. Marcel van der Veer [algol68g@xs4all.nl].

//! @section License
//!
//! This program is free software; you can redistribute it and/or modify it 
//! under the terms of the GNU General Public License as published by the 
//! Free Software Foundation; either version 3 of the License, or 
//! (at your option) any later version.
//!
//! This program is distributed in the hope that it will be useful, but 
//! WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
//! or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
//! more details. You should have received a copy of the GNU General Public 
//! License along with this program. If not, see [http://www.gnu.org/licenses/].

//! @section Synopsis
//!
//! REAL generalised incomplete gamma function.

// Generalised incomplete gamma code in this file was downloaded from 
//
//   [http://helios.mi.parisdescartes.fr/~rabergel/]
//
// and adapted for Algol 68 Genie.
//
// Reference:
//   Rémy Abergel, Lionel Moisan. Fast and accurate evaluation of a
//   generalized incomplete gamma function. 2019. hal-01329669v2
//
// Original source code copyright and license:
//
// DELTAGAMMAINC Fast and Accurate Evaluation of a Generalized Incomplete Gamma
// Function. Copyright (C) 2016 Remy Abergel (remy.abergel AT gmail.com), Lionel
// Moisan (Lionel.Moisan AT parisdescartes.fr).
//
// This file is a part of the DELTAGAMMAINC software, dedicated to the
// computation of a generalized incomplete gammafunction. See the Companion paper
// for a complete description of the algorithm.
// ``Fast and accurate evaluation of a generalized incomplete gamma function''
// (Rémy Abergel, Lionel Moisan), preprint MAP5 nº2016-14, revision 1.
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see [http://www.gnu.org/licenses/].

// References
//   R. Abergel and L. Moisan. 2016. Fast and accurate evaluation of a
//   generalized incomplete gamma function, preprint MAP5 nº2016-14, revision 1
//   Rémy Abergel, Lionel Moisan. Fast and accurate evaluation of a
//   generalized incomplete gamma function. 2019. hal-01329669v2
//   F. W. J. Olver, D. W. Lozier, R. F. Boisvert, and C. W. Clark
//   (Eds.). 2010. NIST Handbook of Mathematical Functions. Cambridge University
//   Press. (see online version at [http://dlmf.nist.gov/])
//   W. H. Press, S. A. Teukolsky, W. T. Vetterling, and
//   B. P. Flannery. 1992. Numerical recipes in C: the art of scientific
//   computing (2nd ed.).
//   G. R. Pugh, 2004. An analysis of the Lanczos Gamma approximation (phd
//   thesis)

#include "a68g.h"
#include "a68g-genie.h"
#include "a68g-prelude.h"
#include "a68g-lib.h"
#include "a68g-mp.h"

#define ITMAX 1000000000        // Maximum allowed number of iterations
#define NITERMAX_ROMBERG 15     // Maximum allowed number of Romberg iterations
#define TOL_ROMBERG 0.1         // Tolerance factor used to stop the Romberg iterations
#define TOL_DIFF 0.2            // Tolerance factor used for the approximation of I_{x,y}^{mu,p} using differences

//! @brief compute plim (x), the limit of the partition of the domain (p,x)
// detailed in the paper.
//            |      x              if   0 < x
//            |
// plim (x) = <      0              if -9 <= x <= 0
//            |
//            | 5.*sqrt (|x|)-5.    otherwise

REAL_T plim (REAL_T x)
{
  return (x >= 0) ? x : ((x >= -9) ? 0 : 5 * sqrt (-x) - 5);
}

//! @brief compute G(p,x) in the domain x <= p using a continued fraction
// p >= 0
// x <= p

void G_cfrac_lower (REAL_T * Gcfrac, REAL_T p, REAL_T x)
{
// deal with special case
  if (x == 0) {
    *Gcfrac = 0;
    return;
  }
// Evaluate the continued fraction using Modified Lentz's method. However,
// as detailed in the paper, perform manually the first pass (n=1), of the
// initial Modified Lentz's method.
  REAL_T an = 1, bn = p, del;
  REAL_T f = an / bn, c = an / A68_REAL_MIN, d = 1 / bn;
  INT_T n = 2, k;
  do {
    k = n / 2;
    an = (n & 1 ? k : -(p - 1 + k)) * x;
    bn++;
    d = an * d + bn;
    if (d == 0) {
      d = A68_REAL_MIN;
    }
    c = bn + an / c;
    if (c == 0) {
      c = A68_REAL_MIN;
    }
    d = 1 / d;
    del = d * c;
    f *= del;
    n++;
  }
  while ((a68_abs_real (del - 1.0) >= A68_REAL_EPS) && (n < ITMAX));
  *Gcfrac = f;
}

//! @brief compute the G-function in the domain x < 0 and |x| < max (1,p-1)
// using a recursive integration by parts relation.
// This function cannot be used when mu > 0.
// p > 0, integer
// x < 0, |x| < max (1,p-1)

void G_ibp (REAL_T * Gibp, REAL_T p, REAL_T x)
{
  REAL_T t = a68_abs_real (x);
  REAL_T tt = 1 / (t * t);
  BOOL_T odd = (INT_T) a68_int_real (p) % 2 != 0;
  REAL_T c = 1 / t;
  REAL_T d = (p - 1);
  REAL_T s = c * (t - d);
  INT_T l = 0;
  BOOL_T stop;
  do {
    c *= d * (d - 1) * tt;
    d -= 2;
    REAL_T del = c * (t - d);
    s += del;
    l++;
    stop = a68_abs_real (del) < a68_abs_real (s) * A68_REAL_EPS;
  }
  while ((l < floor ((p - 2) / 2)) && !stop);
  if (odd && !stop) {
    s += d * c / t;
  }
  *Gibp = ((odd ? -1 : 1) * a68_exp_real (-t + lgamma (p) - (p - 1) * a68_ln_real (t)) + s) / t;
}

//! @brief compute the G-function in the domain x > p using a
// continued fraction.
// p > 0
// x > p, or x = +infinity

void G_cfrac_upper (REAL_T * Gcfrac, REAL_T p, REAL_T x)
{
// Special case
  if (a68_isinf_real (x)) {
    *Gcfrac = 0;
    return;
  }
// Evaluate the continued fraction using Modified Lentz's method. However,
// as detailed in the paper, perform manually the first pass (n=1), of the
// initial Modified Lentz's method.
  REAL_T an = 1, bn = x + 1 - p, c, d, del, f;
  BOOL_T t = (bn != 0);
  INT_T n;
  if (t) {
// b{1} is non-zero
    f = an / bn;
    c = an / A68_REAL_MIN;
    d = 1 / bn;
    n = 2;
  } else {
// b{1}=0 but b{2} is non-zero, compute Mcfrac = a{1}/f with f = a{2}/(b{2}+) a{3}/(b{3}+) ...
    an = -(1 - p);
    bn = x + 3 - p;
    f = an / bn;
    c = an / A68_REAL_MIN;
    d = 1 / bn;
    n = 3;
  }
  INT_T i = n - 1;
  do {
    an = -i * (i - p);
    bn += 2;
    d = an * d + bn;
    if (d == 0) {
      d = A68_REAL_MIN;
    }
    c = bn + an / c;
    if (c == 0) {
      c = A68_REAL_MIN;
    }
    d = 1 / d;
    del = d * c;
    f *= del;
    i++;
    n++;
  }
  while ((a68_abs_real (del - 1.0) >= A68_REAL_EPS) && (n < ITMAX));
  *Gcfrac = t ? f : 1 / f;
}

//! @brief compute G : (p,x) --> R defined as follows
// if x <= p:
//   G(p,x) = exp (x-p*ln (|x|)) * integral of s^{p-1} * exp (-sign (x)*s) ds from s = 0 to |x|
// otherwise:
//   G(p,x) = exp (x-p*ln (|x|)) * integral of s^{p-1} * exp (-s) ds from s = x to infinity
//   p > 0
//   x is a real number or +infinity.

void G_func (REAL_T * G, REAL_T p, REAL_T x)
{
  if (p >= plim (x)) {
    G_cfrac_lower (G, p, x);
  } else if (x < 0) {
    G_ibp (G, p, x);
  } else {
    G_cfrac_upper (G, p, x);
  }
}

//! @brief iteration of the Romberg approximation of I_{x,y}^{mu,p}

void romberg_iterations (REAL_T * R, REAL_T sigma, INT_T n, REAL_T x, REAL_T y, REAL_T mu, REAL_T p, REAL_T h, REAL_T pow2)
{
  INT_T adr0_prev = ((n - 1) * n) / 2;
  INT_T adr0 = (n * (n + 1)) / 2;
  REAL_T sum = 0, xx;
  for (int j = 1; j <= pow2; j++) {
    xx = x + ((y - x) * (2 * j - 1)) / (2 * pow2);
    sum += a68_exp_real (-mu * xx + (p - 1) * a68_ln_real (xx) - sigma);
  }
  R[adr0] = 0.5 * R[adr0_prev] + h * sum;
  REAL_T pow4 = 4;
  for (int m = 1; m <= n; m++) {
    R[adr0 + m] = (pow4 * R[adr0 + (m - 1)] - R[adr0_prev + (m - 1)]) / (pow4 - 1);
    pow4 *= 4;
  }
}

//! @ compute I_{x,y}^{mu,p} using a Romberg approximation.
// Compute rho and sigma so I_{x,y}^{mu,p} = rho * exp (sigma)

void romberg_estimate (REAL_T * rho, REAL_T * sigma, REAL_T x, REAL_T y, REAL_T mu, REAL_T p)
{
  REAL_T *R = (REAL_T *) get_heap_space (((NITERMAX_ROMBERG + 1) * (NITERMAX_ROMBERG + 2)) / 2 * sizeof (REAL_T));
  ASSERT (R != NULL);
// Initialization (n=1)
  *sigma = -mu * y + (p - 1) * a68_ln_real (y);
  R[0] = 0.5 * (y - x) * (a68_exp_real (-mu * x + (p - 1) * a68_ln_real (x) - (*sigma)) + 1);
// Loop for n > 0
  REAL_T relneeded = A68_REAL_EPS / TOL_ROMBERG;
  INT_T adr0 = 0, n = 1;
  REAL_T h = (y - x) / 2;       // n=1, h = (y-x)/2^n
  REAL_T pow2 = 1;              // n=1; pow2 = 2^(n-1)
  if (NITERMAX_ROMBERG >= 1) {
    REAL_T relerr;
    do {
      romberg_iterations (R, *sigma, n, x, y, mu, p, h, pow2);
      h /= 2;
      pow2 *= 2;
      adr0 = (n * (n + 1)) / 2;
      relerr = a68_abs_real ((R[adr0 + n] - R[adr0 + n - 1]) / R[adr0 + n]);
      n++;
    } while (n <= NITERMAX_ROMBERG && relerr > relneeded);
  }
// save Romberg estimate and free memory
  *rho = R[adr0 + (n - 1)];
  a68_free (R);
}

//! @brief compute generalized incomplete gamma function I_{x,y}^{mu,p}
//   I_{x,y}^{mu,p} = integral from x to y of s^{p-1} * exp (-mu*s) ds
// This procedure computes (rho, sigma) described below.
// The approximated value of I_{x,y}^{mu,p} is I = rho * exp (sigma)
//   mu is a real number non equal to zero 
//     (in general we take mu = 1 or -1 but any nonzero real number is allowed)
//   x, y are two numbers with 0 <= x <= y <= +infinity,
//     (the setting y=+infinity is allowed only when mu > 0)
//   p is a real number > 0, p must be an integer when mu < 0.

void deltagammainc (REAL_T * rho, REAL_T * sigma, REAL_T x, REAL_T y, REAL_T mu, REAL_T p)
{
// Particular cases
  if (a68_isinf_real (x) && a68_isinf_real (y)) {
    *rho = 0;
    *sigma = a68_neginf_double_real ();
    return;
  } else if (x == y) {
    *rho = 0;
    *sigma = a68_neginf_double_real ();
    return;
  }
  if (x == 0 && a68_isinf_real (y)) {
    *rho = 1;
    (*sigma) = lgamma (p) - p * a68_ln_real (mu);
    return;
  }
// Initialization
  REAL_T mA, mB, mx, my, nA, nB;
  G_func (&mx, p, mu * x);
  REAL_T nx = (a68_isinf_real (x) ? a68_neginf_double_real () : -mu * x + p * a68_ln_real (x));
  G_func (&my, p, mu * y);
  REAL_T ny = (a68_isinf_real (y) ? a68_neginf_double_real () : -mu * y + p * a68_ln_real (y));

// Compute (mA,nA) and (mB,nB) such as I_{x,y}^{mu,p} can be
// approximated by the difference A-B, where A >= B >= 0, A = mA*exp (nA) an 
// B = mB*exp (nB). When the difference involves more than one digit loss due to
// cancellation errors, the integral I_{x,y}^{mu,p} is evaluated using the
// Romberg approximation method.

  if (mu < 0) {
    mA = my;
    nA = ny;
    mB = mx;
    nB = nx;
  } else {
    if (p < plim (mu * x)) {
      mA = mx;
      nA = nx;
      mB = my;
      nB = ny;
    } else if (p < plim (mu * y)) {
      mA = 1;
      nA = lgamma (p) - p * a68_ln_real (mu);
      nB = fmax (nx, ny);
      mB = mx * a68_exp_real (nx - nB) + my * a68_exp_real (ny - nB);
    } else {
      mA = my;
      nA = ny;
      mB = mx;
      nB = nx;
    }
  }
// Compute (rho,sigma) such that rho*exp (sigma) = A-B
  *rho = mA - mB * a68_exp_real (nB - nA);
  *sigma = nA;
// If the difference involved a significant loss of precision, compute Romberg estimate.
  if (!a68_isinf_real (y) && ((*rho) / mA < TOL_DIFF)) {
    romberg_estimate (rho, sigma, x, y, mu, p);
  }
}

// A68G Driver routines

//! @brief PROC gamma inc g = (REAL p, x, y, mu) REAL

void genie_gamma_inc_g_real (NODE_T * n)
{
  A68_REAL x, y, mu, p;
  POP_OBJECT (n, &mu, A68_REAL);
  POP_OBJECT (n, &y, A68_REAL);
  POP_OBJECT (n, &x, A68_REAL);
  POP_OBJECT (n, &p, A68_REAL);
  REAL_T rho, sigma;
  deltagammainc (&rho, &sigma, VALUE (&x), VALUE (&y), VALUE (&mu), VALUE (&p));
  PUSH_VALUE (n, rho * a68_exp_real (sigma), A68_REAL);
}

//! @brief PROC gamma inc f = (REAL p, x) REAL

void genie_gamma_inc_f_real (NODE_T * n)
{
  A68_REAL x, p;
  POP_OBJECT (n, &x, A68_REAL);
  POP_OBJECT (n, &p, A68_REAL);
  REAL_T rho, sigma;
  deltagammainc (&rho, &sigma, VALUE (&x), a68_posinf_real (), 1, VALUE (&p));
  PUSH_VALUE (n, rho * a68_exp_real (sigma), A68_REAL);
}

//! @brief PROC gamma inc = (REAL p, x) REAL

void genie_gamma_inc_h_real (NODE_T * n)
{
#if (A68_LEVEL >= 3) && defined (HAVE_GNU_MPFR)
  genie_gamma_inc_real_mpfr (n);
#else
  genie_gamma_inc_f_real (n);
#endif
}

//! @brief PROC gamma inc gf = (REAL p, x) REAL

void genie_gamma_inc_gf_real (NODE_T * q)
{
// if x <= p: G(p,x) = exp (x-p*ln (|x|)) * integral over [0,|x|] of s^{p-1} * exp (-sign (x)*s) ds
// otherwise: G(p,x) = exp (x-p*ln (x)) * integral over [x,inf] of s^{p-1} * exp (-s) ds
  A68_REAL x, p;
  POP_OBJECT (q, &x, A68_REAL);
  POP_OBJECT (q, &p, A68_REAL);
  REAL_T G;
  G_func (&G, VALUE (&p), VALUE (&x));
  PUSH_VALUE (q, G, A68_REAL);
}
