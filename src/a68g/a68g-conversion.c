//! @file a68g-conversion.c
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
//! Conversion tables for IEEE platforms.

#include "a68g.h"
#include "a68g-prelude.h"

// A list of 10 ^ 2 ^ n for conversion purposes on IEEE 754 platforms.

#if (A68_LEVEL >= 3)

//! @brief 10 ** expo

DOUBLE_T ten_up_double (int expo)
{
  static DOUBLE_T pow_10_double[] = {
    10.0q, 100.0q, 1.0e4q, 1.0e8q, 1.0e16q, 1.0e32q, 1.0e64q, 1.0e128q, 1.0e256q, 1.0e512q, 1.0e1024q, 1.0e2048q, 1.0e4096q
  };
// This appears sufficiently accurate.
  if (expo == 0) {
    return 1.0q;
  }
  BOOL_T neg_expo = (BOOL_T) (expo < 0);
  if (neg_expo) {
    expo = -expo;
  }
  if (expo > MAX_DOUBLE_EXPO) {
    expo = 0;
    errno = EDOM;
  }
  ABEND (expo > MAX_DOUBLE_EXPO, ERROR_INVALID_VALUE, __func__);
  DOUBLE_T dbl_expo = 1.0q;
  for (DOUBLE_T *dep = pow_10_double; expo != 0; expo >>= 1, dep++) {
    if (expo & 0x1) {
      dbl_expo *= *dep;
    }
  }
  return neg_expo ? 1.0q / dbl_expo : dbl_expo;
}

#endif

//! @brief 10 ** expo

REAL_T ten_up (int expo)
{
  static REAL_T pow_10[] = {
    10.0, 100.0, 1.0e4, 1.0e8, 1.0e16, 1.0e32, 1.0e64, 1.0e128, 1.0e256
  };
// This appears sufficiently accurate.
  BOOL_T neg_expo = (BOOL_T) (expo < 0);
  if (neg_expo) {
    expo = -expo;
  }
  ABEND (expo > MAX_REAL_EXPO, ERROR_INVALID_VALUE, __func__);
  REAL_T dbl_expo = 1.0;
  for (REAL_T *dep = pow_10; expo != 0; expo >>= 1, dep++) {
    if (expo & 0x1) {
      dbl_expo *= *dep;
    }
  }
  return neg_expo ? 1 / dbl_expo : dbl_expo;
}
