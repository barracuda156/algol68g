//! @file rts-unformatted.c
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
//! Unformatted transput.

#include "a68g.h"
#include "a68g-genie.h"
#include "a68g-prelude.h"
#include "a68g-mp.h"
#include "a68g-double.h"
#include "a68g-transput.h"

//! @brief Skip new-lines and form-feeds.

void skip_nl_ff (NODE_T * p, int *ch, A68_REF ref_file)
{
  A68_FILE *f = FILE_DEREF (&ref_file);
  while ((*ch) != EOF_CHAR && IS_NL_FF (*ch)) {
    A68_BOOL *z = (A68_BOOL *) STACK_TOP;
    ADDR_T pop_sp = A68_SP;
    unchar_scanner (p, f, (char) (*ch));
    if (*ch == NEWLINE_CHAR) {
      on_event_handler (p, LINE_END_MENDED (f), ref_file);
      A68_SP = pop_sp;
      if (VALUE (z) == A68_FALSE) {
        PUSH_REF (p, ref_file);
        genie_new_line (p);
      }
    } else if (*ch == FORMFEED_CHAR) {
      on_event_handler (p, PAGE_END_MENDED (f), ref_file);
      A68_SP = pop_sp;
      if (VALUE (z) == A68_FALSE) {
        PUSH_REF (p, ref_file);
        genie_new_page (p);
      }
    }
    (*ch) = char_scanner (f);
  }
}

//! @brief Scan an int from file.

void scan_integer (NODE_T * p, A68_REF ref_file)
{
  A68_FILE *f = FILE_DEREF (&ref_file);
  reset_transput_buffer (INPUT_BUFFER);
  int ch = char_scanner (f);
  while (ch != EOF_CHAR && (IS_SPACE (ch) || IS_NL_FF (ch))) {
    if (IS_NL_FF (ch)) {
      skip_nl_ff (p, &ch, ref_file);
    } else {
      ch = char_scanner (f);
    }
  }
  if (ch != EOF_CHAR && (ch == '+' || ch == '-')) {
    plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
    ch = char_scanner (f);
  }
  while (ch != EOF_CHAR && IS_DIGIT (ch)) {
    plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
    ch = char_scanner (f);
  }
  if (ch != EOF_CHAR) {
    unchar_scanner (p, f, (char) ch);
  }
}

//! @brief Scan a real from file.

void scan_real (NODE_T * p, A68_REF ref_file)
{
  A68_FILE *f = FILE_DEREF (&ref_file);
  char x_e = EXPONENT_CHAR;
  reset_transput_buffer (INPUT_BUFFER);
  int ch = char_scanner (f);
  while (ch != EOF_CHAR && (IS_SPACE (ch) || IS_NL_FF (ch))) {
    if (IS_NL_FF (ch)) {
      skip_nl_ff (p, &ch, ref_file);
    } else {
      ch = char_scanner (f);
    }
  }
  if (ch != EOF_CHAR && (ch == '+' || ch == '-')) {
    plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
    ch = char_scanner (f);
  }
  while (ch != EOF_CHAR && IS_DIGIT (ch)) {
    plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
    ch = char_scanner (f);
  }
  if (ch == EOF_CHAR || !(ch == POINT_CHAR || TO_UPPER (ch) == TO_UPPER (x_e))) {
    goto salida;
  }
  if (ch == POINT_CHAR) {
    plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
    ch = char_scanner (f);
    while (ch != EOF_CHAR && IS_DIGIT (ch)) {
      plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
      ch = char_scanner (f);
    }
  }
  if (ch == EOF_CHAR || TO_UPPER (ch) != TO_UPPER (x_e)) {
    goto salida;
  }
  if (TO_UPPER (ch) == TO_UPPER (x_e)) {
    plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
    ch = char_scanner (f);
    while (ch != EOF_CHAR && ch == BLANK_CHAR) {
      ch = char_scanner (f);
    }
    if (ch != EOF_CHAR && (ch == '+' || ch == '-')) {
      plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
      ch = char_scanner (f);
    }
    while (ch != EOF_CHAR && IS_DIGIT (ch)) {
      plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
      ch = char_scanner (f);
    }
  }
salida:if (ch != EOF_CHAR) {
    unchar_scanner (p, f, (char) ch);
  }
}

//! @brief Scan a bits from file.

void scan_bits (NODE_T * p, A68_REF ref_file)
{
  A68_FILE *f = FILE_DEREF (&ref_file);
  reset_transput_buffer (INPUT_BUFFER);
  int ch = char_scanner (f);
  while (ch != EOF_CHAR && (IS_SPACE (ch) || IS_NL_FF (ch))) {
    if (IS_NL_FF (ch)) {
      skip_nl_ff (p, &ch, ref_file);
    } else {
      ch = char_scanner (f);
    }
  }
  while (ch != EOF_CHAR && (ch == FLIP_CHAR || ch == FLOP_CHAR)) {
    plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
    ch = char_scanner (f);
  }
  if (ch != EOF_CHAR) {
    unchar_scanner (p, f, (char) ch);
  }
}

//! @brief Scan a char from file.

void scan_char (NODE_T * p, A68_REF ref_file)
{
  A68_FILE *f = FILE_DEREF (&ref_file);
  reset_transput_buffer (INPUT_BUFFER);
  int ch = char_scanner (f);
  skip_nl_ff (p, &ch, ref_file);
  if (ch != EOF_CHAR) {
    plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
  }
}

//! @brief Scan a string from file.

void scan_string (NODE_T * p, char *term, A68_REF ref_file)
{
  A68_FILE *f = FILE_DEREF (&ref_file);
  if (END_OF_FILE (f)) {
    reset_transput_buffer (INPUT_BUFFER);
    end_of_file_error (p, ref_file);
  } else {
    reset_transput_buffer (INPUT_BUFFER);
    int ch = char_scanner (f);
    BOOL_T siga = A68_TRUE;
    while (siga) {
      if (ch == EOF_CHAR || END_OF_FILE (f)) {
        if (get_transput_buffer_index (INPUT_BUFFER) == 0) {
          end_of_file_error (p, ref_file);
        }
        siga = A68_FALSE;
      } else if (IS_NL_FF (ch)) {
        ADDR_T pop_sp = A68_SP;
        unchar_scanner (p, f, (char) ch);
        if (ch == NEWLINE_CHAR) {
          on_event_handler (p, LINE_END_MENDED (f), ref_file);
        } else if (ch == FORMFEED_CHAR) {
          on_event_handler (p, PAGE_END_MENDED (f), ref_file);
        }
        A68_SP = pop_sp;
        siga = A68_FALSE;
      } else if (term != NO_TEXT && strchr (term, ch) != NO_TEXT) {
        siga = A68_FALSE;
        unchar_scanner (p, f, (char) ch);
      } else {
        plusab_transput_buffer (p, INPUT_BUFFER, (char) ch);
        ch = char_scanner (f);
      }
    }
  }
}

//! @brief Make temp file name.

BOOL_T a68_mkstemp (char *fn, int flags, mode_t permissions)
{
// "tmpnam" is not safe, "mkstemp" is Unix, so a68g brings its own tmpnam.
#define TMP_SIZE 32
#define TRIALS 32
  BUFFER tfilename;
  char *letters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int len = (int) strlen (letters);
  BOOL_T good_file = A68_FALSE;
// Next are prefixes to try.
// First we try /tmp, and if that won't go, the current dir.
  char *prefix[] = { "/tmp/a68_", "./a68_", NO_TEXT };
  for (int i = 0; prefix[i] != NO_TEXT; i++) {
    for (int k = 0; k < TRIALS && good_file == A68_FALSE; k++) {
      a68_bufcpy (tfilename, prefix[i], BUFFER_SIZE);
      for (int j = 0; j < TMP_SIZE; j++) {
        int cindex;
        do {
          cindex = (int) (a68_unif_rand () * len);
        } while (cindex < 0 || cindex >= len);
        char chars[2];
        chars[0] = letters[cindex];
        chars[1] = NULL_CHAR;
        a68_bufcat (tfilename, chars, BUFFER_SIZE);
      }
      a68_bufcat (tfilename, ".tmp", BUFFER_SIZE);
      errno = 0;
      FILE_T fd = open (tfilename, flags | O_EXCL, permissions);
      good_file = (BOOL_T) (fd != A68_NO_FILE && errno == 0);
      if (good_file) {
        (void) close (fd);
      }
    }
  }
  if (good_file) {
    a68_bufcpy (fn, tfilename, BUFFER_SIZE);
    return A68_TRUE;
  } else {
    return A68_FALSE;
  }
#undef TMP_SIZE
#undef TRIALS
}

//! @brief Open a file, or establish it.

FILE_T open_physical_file (NODE_T * p, A68_REF ref_file, int flags, mode_t permissions)
{
  BOOL_T reading = (flags & ~O_BINARY) == A68_READ_ACCESS;
  BOOL_T writing = (flags & ~O_BINARY) == A68_WRITE_ACCESS;
  ABEND (reading == writing, ERROR_INTERNAL_CONSISTENCY, __func__);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (!IS_NIL (STRING (file))) {
    if (writing) {
      A68_REF z = *DEREF (A68_REF, &STRING (file));
      A68_ARRAY *arr; A68_TUPLE *tup;
      GET_DESCRIPTOR (arr, tup, &z);
      UPB (tup) = LWB (tup) - 1;
    }
// Associated file.
    TRANSPUT_BUFFER (file) = get_unblocked_transput_buffer (p);
    reset_transput_buffer (TRANSPUT_BUFFER (file));
    END_OF_FILE (file) = A68_FALSE;
    FILE_ENTRY (file) = -1;
    return FD (file);
  } else if (IS_NIL (IDENTIFICATION (file))) {
// No identification, so generate a unique identification..
    if (reading) {
      return A68_NO_FILE;
    } else {
      BUFFER tfilename;
      int len;
      BUFCLR (tfilename);
      if (!a68_mkstemp (tfilename, flags, permissions)) {
        diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_NO_TEMP);
        exit_genie (p, A68_RUNTIME_ERROR);
      }
      FD (file) = open (tfilename, flags, permissions);
      len = 1 + (int) strlen (tfilename);
      IDENTIFICATION (file) = heap_generator (p, M_C_STRING, len);
      BLOCK_GC_HANDLE (&(IDENTIFICATION (file)));
      a68_bufcpy (DEREF (char, &IDENTIFICATION (file)), tfilename, len);
      TRANSPUT_BUFFER (file) = get_unblocked_transput_buffer (p);
      reset_transput_buffer (TRANSPUT_BUFFER (file));
      END_OF_FILE (file) = A68_FALSE;
      TMP_FILE (file) = A68_TRUE;
      FILE_ENTRY (file) = store_file_entry (p, FD (file), tfilename, TMP_FILE (file));
      return FD (file);
    }
  } else {
// Opening an identified file.
    A68_REF ref_filename = IDENTIFICATION (file);
    CHECK_REF (p, ref_filename, M_ROWS);
    char *filename = DEREF (char, &ref_filename);
    if (OPEN_EXCLUSIVE (file)) {
// Establishing requires that the file does not exist.
      if (flags == (A68_WRITE_ACCESS)) {
        flags |= O_EXCL;
      }
      OPEN_EXCLUSIVE (file) = A68_FALSE;
    }
    FD (file) = open (filename, flags, permissions);
    TRANSPUT_BUFFER (file) = get_unblocked_transput_buffer (p);
    reset_transput_buffer (TRANSPUT_BUFFER (file));
    END_OF_FILE (file) = A68_FALSE;
    FILE_ENTRY (file) = store_file_entry (p, FD (file), filename, TMP_FILE (file));
    return FD (file);
  }
}

//! @brief Call PROC (REF FILE) VOID during transput.

void genie_call_proc_ref_file_void (NODE_T * p, A68_REF ref_file, A68_PROCEDURE z)
{
  ADDR_T pop_sp = A68_SP, pop_fp = A68_FP;
  MOID_T *u = M_PROC_REF_FILE_VOID;
  PUSH_REF (p, ref_file);
  genie_call_procedure (p, MOID (&z), u, u, &z, pop_sp, pop_fp);
  A68_SP = pop_sp;              // Voiding
}

// Unformatted transput.

//! @brief Hexadecimal value of digit.

int char_value (int ch)
{
  switch (ch) {
  case '0': {
      return 0;
    }
  case '1': {
      return 1;
    }
  case '2': {
      return 2;
    }
  case '3': {
      return 3;
    }
  case '4': {
      return 4;
    }
  case '5': {
      return 5;
    }
  case '6': {
      return 6;
    }
  case '7': {
      return 7;
    }
  case '8': {
      return 8;
    }
  case '9': {
      return 9;
    }
  case 'A':
  case 'a': {
      return 10;
    }
  case 'B':
  case 'b': {
      return 11;
    }
  case 'C':
  case 'c': {
      return 12;
    }
  case 'D':
  case 'd': {
      return 13;
    }
  case 'E':
  case 'e': {
      return 14;
    }
  case 'F':
  case 'f': {
      return 15;
    }
  default: {
      return -1;
    }
  }
}

//! @brief INT value of BITS denotation

UNSIGNED_T bits_to_int (NODE_T * p, char *str)
{
  errno = 0;
  char *radix = NO_TEXT, *end = NO_TEXT;
  int base = (int) a68_strtou (str, &radix, 10);
  if (radix != NO_TEXT && TO_UPPER (radix[0]) == TO_UPPER (RADIX_CHAR) && errno == 0) {
    UNSIGNED_T bits = 0;
    if (base < 2 || base > 16) {
      diagnostic (A68_RUNTIME_ERROR, p, ERROR_INVALID_RADIX, base);
      exit_genie (p, A68_RUNTIME_ERROR);
    }
    bits = a68_strtou (&(radix[1]), &end, base);
    if (end != NO_TEXT && end[0] == NULL_CHAR && errno == 0) {
      return bits;
    }
  }
  diagnostic (A68_RUNTIME_ERROR, p, ERROR_IN_DENOTATION, M_BITS);
  exit_genie (p, A68_RUNTIME_ERROR);
  return 0;
}

//! @brief Convert string to required mode and store.

BOOL_T genie_string_to_value_internal (NODE_T * p, MOID_T * m, char *a, BYTE_T * item)
{
  errno = 0;
// strto.. does not mind empty strings.
  if (strlen (a) == 0) {
    return A68_FALSE;
  }
  if (m == M_INT) {
    A68_INT *z = (A68_INT *) item;
    char *end;
    VALUE (z) = (INT_T) a68_strtoi (a, &end, 10);
    if (end[0] == NULL_CHAR && errno == 0) {
      STATUS (z) = INIT_MASK;
      return A68_TRUE;
    } else {
      return A68_FALSE;
    }
  }
  if (m == M_REAL) {
    A68_REAL *z = (A68_REAL *) item;
    char *end;
    VALUE (z) = strtod (a, &end);
    if (end[0] == NULL_CHAR && errno == 0) {
      STATUS (z) = INIT_MASK;
      return A68_TRUE;
    } else {
      return A68_FALSE;
    }
  }
#if (A68_LEVEL >= 3)
  if (m == M_LONG_INT) {
    A68_LONG_INT *z = (A68_LONG_INT *) item;
    if (string_to_double_int (p, z, a) == A68_FALSE) {
      return A68_FALSE;
    }
    STATUS (z) = INIT_MASK;
    return A68_TRUE;
  }
  if (m == M_LONG_REAL) {
    A68_LONG_REAL *z = (A68_LONG_REAL *) item;
    char *end;
//  VALUE (z).f = strtoflt128 (a, &end);
    VALUE (z).f = string_to_double (a, &end);
    MATH_RTE (p, errno != 0, M_LONG_REAL, ERROR_MATH);
    if (end[0] == NULL_CHAR && errno == 0) {
      STATUS (z) = INIT_MASK;
      return A68_TRUE;
    } else {
      return A68_FALSE;
    }
  }
  if (m == M_LONG_BITS) {
    A68_LONG_BITS *z = (A68_LONG_BITS *) item;
    int ret = A68_TRUE;
    DOUBLE_NUM_T b;
    set_lw (b, 0x0);
    if (a[0] == FLIP_CHAR || a[0] == FLOP_CHAR) {
// [] BOOL denotation is "TTFFFFTFT ...".
      if (strlen (a) > (size_t) A68_LONG_BITS_WIDTH) {
        errno = ERANGE;
        ret = A68_FALSE;
      } else {
        int n = 1;
        UNSIGNED_T k = 0x1;
        for (int j = (int) strlen (a) - 1; j >= 0; j--) {
          if (a[j] == FLIP_CHAR) {
            if (n <= A68_LONG_BITS_WIDTH / 2) {
              LW (b) |= k;
            } else {
              HW (b) |= k;
            }
          } else if (a[j] != FLOP_CHAR) {
            ret = A68_FALSE;
          }
          k <<= 1;
        }
      }
      VALUE (z) = b;
    } else {
// BITS denotation.
      VALUE (z) = double_strtou (p, a);
    }
    return ret;
  }
#else
  if (m == M_LONG_BITS || m == M_LONG_LONG_BITS) {
    int digits = DIGITS (m);
    int status = A68_TRUE;
    ADDR_T pop_sp = A68_SP;
    MP_T *z = (MP_T *) item;
    if (a[0] == FLIP_CHAR || a[0] == FLOP_CHAR) {
// [] BOOL denotation is "TTFFFFTFT ...".
      if (strlen (a) > (size_t) A68_BITS_WIDTH) {
        errno = ERANGE;
        status = A68_FALSE;
      } else {
        MP_T *w = lit_mp (p, 1, 0, digits);
        SET_MP_ZERO (z, digits);
        for (int j = (int) strlen (a) - 1; j >= 0; j--) {
          if (a[j] == FLIP_CHAR) {
            (void) add_mp (p, z, z, w, digits);
          } else if (a[j] != FLOP_CHAR) {
            status = A68_FALSE;
          }
          (void) mul_mp_digit (p, w, w, (MP_T) 2, digits);
        }
      }
    } else {
// BITS denotation is also allowed.
      mp_strtou (p, z, a, m);
    }
    A68_SP = pop_sp;
    if (errno != 0 || status == A68_FALSE) {
      return A68_FALSE;
    }
    MP_STATUS (z) = (MP_T) INIT_MASK;
    return A68_TRUE;
  }
#endif
  if (m == M_LONG_INT || m == M_LONG_LONG_INT) {
    int digits = DIGITS (m);
    MP_T *z = (MP_T *) item;
    if (strtomp (p, z, a, digits) == NaN_MP) {
      return A68_FALSE;
    }
    if (!check_mp_int (z, m)) {
      errno = ERANGE;
      return A68_FALSE;
    }
    MP_STATUS (z) = (MP_T) INIT_MASK;
    return A68_TRUE;
  }
  if (m == M_LONG_REAL || m == M_LONG_LONG_REAL) {
    int digits = DIGITS (m);
    MP_T *z = (MP_T *) item;
    if (strtomp (p, z, a, digits) == NaN_MP) {
      return A68_FALSE;
    }
    MP_STATUS (z) = (MP_T) INIT_MASK;
    return A68_TRUE;
  }
  if (m == M_BOOL) {
    A68_BOOL *z = (A68_BOOL *) item;
    char q = a[0], flip = FLIP_CHAR, flop = FLOP_CHAR;
    if (q == flip || q == flop) {
      VALUE (z) = (BOOL_T) (q == flip);
      STATUS (z) = INIT_MASK;
      return A68_TRUE;
    } else {
      return A68_FALSE;
    }
  }
  if (m == M_BITS) {
    A68_BITS *z = (A68_BITS *) item;
    int status = A68_TRUE;
    if (a[0] == FLIP_CHAR || a[0] == FLOP_CHAR) {
// [] BOOL denotation is "TTFFFFTFT ...".
      if (strlen (a) > (size_t) A68_BITS_WIDTH) {
        errno = ERANGE;
        status = A68_FALSE;
      } else {
        UNSIGNED_T k = 0x1;
        VALUE (z) = 0;
        for (int j = (int) strlen (a) - 1; j >= 0; j--) {
          if (a[j] == FLIP_CHAR) {
            VALUE (z) += k;
          } else if (a[j] != FLOP_CHAR) {
            status = A68_FALSE;
          }
          k <<= 1;
        }
      }
    } else {
// BITS denotation is also allowed.
      VALUE (z) = bits_to_int (p, a);
    }
    if (errno != 0 || status == A68_FALSE) {
      return A68_FALSE;
    }
    STATUS (z) = INIT_MASK;
    return A68_TRUE;
  }
  return A68_FALSE;
}

//! @brief Convert string in input buffer to value of required mode.

void genie_string_to_value (NODE_T * p, MOID_T * mode, BYTE_T * item, A68_REF ref_file)
{
  char *str = get_transput_buffer (INPUT_BUFFER);
  errno = 0;
// end string, just in case.
  plusab_transput_buffer (p, INPUT_BUFFER, NULL_CHAR);
  if (mode == M_INT) {
    if (genie_string_to_value_internal (p, mode, str, item) == A68_FALSE) {
      value_error (p, mode, ref_file);
    }
  } else if (mode == M_LONG_INT || mode == M_LONG_LONG_INT) {
    if (genie_string_to_value_internal (p, mode, str, item) == A68_FALSE) {
      value_error (p, mode, ref_file);
    }
  } else if (mode == M_REAL) {
    if (genie_string_to_value_internal (p, mode, str, item) == A68_FALSE) {
      value_error (p, mode, ref_file);
    }
  } else if (mode == M_LONG_REAL || mode == M_LONG_LONG_REAL) {
    if (genie_string_to_value_internal (p, mode, str, item) == A68_FALSE) {
      value_error (p, mode, ref_file);
    }
  } else if (mode == M_BOOL) {
    if (genie_string_to_value_internal (p, mode, str, item) == A68_FALSE) {
      value_error (p, mode, ref_file);
    }
  } else if (mode == M_BITS) {
    if (genie_string_to_value_internal (p, mode, str, item) == A68_FALSE) {
      value_error (p, mode, ref_file);
    }
  } else if (mode == M_LONG_BITS || mode == M_LONG_LONG_BITS) {
    if (genie_string_to_value_internal (p, mode, str, item) == A68_FALSE) {
      value_error (p, mode, ref_file);
    }
  } else if (mode == M_CHAR) {
    A68_CHAR *z = (A68_CHAR *) item;
    if (str[0] == NULL_CHAR) {
//      value_error (p, mode, ref_file);.
      VALUE (z) = NULL_CHAR;
      STATUS (z) = INIT_MASK;
    } else {
      int len = (int) strlen (str);
      if (len == 0 || len > 1) {
        value_error (p, mode, ref_file);
      }
      VALUE (z) = str[0];
      STATUS (z) = INIT_MASK;
    }
  } else if (mode == M_STRING) {
    A68_REF z;
    z = c_to_a_string (p, str, get_transput_buffer_index (INPUT_BUFFER) - 1);
    *(A68_REF *) item = z;
  }
  if (errno != 0) {
    transput_error (p, ref_file, mode);
  }
}

//! @brief Read object from file.

void genie_read_standard (NODE_T * p, MOID_T * mode, BYTE_T * item, A68_REF ref_file)
{
  A68_FILE *f = FILE_DEREF (&ref_file);
  errno = 0;
  if (END_OF_FILE (f)) {
    end_of_file_error (p, ref_file);
  }
  if (mode == M_PROC_REF_FILE_VOID) {
    genie_call_proc_ref_file_void (p, ref_file, *(A68_PROCEDURE *) item);
  } else if (mode == M_FORMAT) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_UNDEFINED_TRANSPUT, M_FORMAT);
    exit_genie (p, A68_RUNTIME_ERROR);
  } else if (mode == M_REF_SOUND) {
    read_sound (p, ref_file, DEREF (A68_SOUND, (A68_REF *) item));
  } else if (IS_REF (mode)) {
    CHECK_REF (p, *(A68_REF *) item, mode);
    genie_read_standard (p, SUB (mode), ADDRESS ((A68_REF *) item), ref_file);
  } else if (mode == M_INT || mode == M_LONG_INT || mode == M_LONG_LONG_INT) {
    scan_integer (p, ref_file);
    genie_string_to_value (p, mode, item, ref_file);
  } else if (mode == M_REAL || mode == M_LONG_REAL || mode == M_LONG_LONG_REAL) {
    scan_real (p, ref_file);
    genie_string_to_value (p, mode, item, ref_file);
  } else if (mode == M_BOOL) {
    scan_char (p, ref_file);
    genie_string_to_value (p, mode, item, ref_file);
  } else if (mode == M_CHAR) {
    scan_char (p, ref_file);
    genie_string_to_value (p, mode, item, ref_file);
  } else if (mode == M_BITS || mode == M_LONG_BITS || mode == M_LONG_LONG_BITS) {
    scan_bits (p, ref_file);
    genie_string_to_value (p, mode, item, ref_file);
  } else if (mode == M_STRING) {
    char *term = DEREF (char, &TERMINATOR (f));
    scan_string (p, term, ref_file);
    genie_string_to_value (p, mode, item, ref_file);
  } else if (IS_STRUCT (mode)) {
    for (PACK_T *q = PACK (mode); q != NO_PACK; FORWARD (q)) {
      genie_read_standard (p, MOID (q), &item[OFFSET (q)], ref_file);
    }
  } else if (IS_UNION (mode)) {
    A68_UNION *z = (A68_UNION *) item;
    if (!(STATUS (z) | INIT_MASK) || VALUE (z) == NULL) {
      diagnostic (A68_RUNTIME_ERROR, p, ERROR_EMPTY_VALUE, mode);
      exit_genie (p, A68_RUNTIME_ERROR);
    }
    genie_read_standard (p, (MOID_T *) (VALUE (z)), &item[A68_UNION_SIZE], ref_file);
  } else if (IS_ROW (mode) || IS_FLEX (mode)) {
    MOID_T *deflexed = DEFLEX (mode);
    A68_ARRAY *arr;
    A68_TUPLE *tup;
    CHECK_INIT (p, INITIALISED ((A68_REF *) item), mode);
    GET_DESCRIPTOR (arr, tup, (A68_REF *) item);
    if (get_row_size (tup, DIM (arr)) > 0) {
      BYTE_T *base_addr = DEREF (BYTE_T, &ARRAY (arr));
      BOOL_T done = A68_FALSE;
      initialise_internal_index (tup, DIM (arr));
      while (!done) {
        ADDR_T a68_index = calculate_internal_index (tup, DIM (arr));
        ADDR_T elem_addr = ROW_ELEMENT (arr, a68_index);
        genie_read_standard (p, SUB (deflexed), &base_addr[elem_addr], ref_file);
        done = increment_internal_index (tup, DIM (arr));
      }
    }
  }
  if (errno != 0) {
    transput_error (p, ref_file, mode);
  }
}

//! @brief PROC ([] SIMPLIN) VOID read

void genie_read (NODE_T * p)
{
  A68_REF row;
  POP_REF (p, &row);
  genie_stand_in (p);
  PUSH_REF (p, row);
  genie_read_file (p);
}

//! @brief Open for reading.

void open_for_reading (NODE_T * p, A68_REF ref_file)
{
  A68_FILE *file = FILE_DEREF (&ref_file);
  if (!OPENED (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_NOT_OPEN);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (DRAW_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "draw");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (WRITE_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "write");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!GET (&CHANNEL (file))) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_CHANNEL_DOES_NOT_ALLOW, "getting");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!READ_MOOD (file) && !WRITE_MOOD (file)) {
    if (IS_NIL (STRING (file))) {
      if ((FD (file) = open_physical_file (p, ref_file, A68_READ_ACCESS, 0)) == A68_NO_FILE) {
        open_error (p, ref_file, "getting");
      }
    } else {
      FD (file) = open_physical_file (p, ref_file, A68_READ_ACCESS, 0);
    }
    DRAW_MOOD (file) = A68_FALSE;
    READ_MOOD (file) = A68_TRUE;
    WRITE_MOOD (file) = A68_FALSE;
    CHAR_MOOD (file) = A68_TRUE;
  }
  if (!CHAR_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "binary");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
}

//! @brief PROC (REF FILE, [] SIMPLIN) VOID get

void genie_read_file (NODE_T * p)
{
  A68_REF row; A68_ARRAY *arr; A68_TUPLE *tup;
  POP_REF (p, &row);
  CHECK_REF (p, row, M_ROW_SIMPLIN);
  GET_DESCRIPTOR (arr, tup, &row);
  int elems = ROW_SIZE (tup);
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  open_for_reading (p, ref_file);
// Read.
  if (elems <= 0) {
    return;
  }
  BYTE_T *base_address = DEREF (BYTE_T, &ARRAY (arr));
  int elem_index = 0;
  for (int k = 0; k < elems; k++) {
    A68_UNION *z = (A68_UNION *) & base_address[elem_index];
    MOID_T *mode = (MOID_T *) (VALUE (z));
    BYTE_T *item = (BYTE_T *) & base_address[elem_index + A68_UNION_SIZE];
    genie_read_standard (p, mode, item, ref_file);
    elem_index += SIZE (M_SIMPLIN);
  }
}

//! @brief Convert value to string.

void genie_value_to_string (NODE_T * p, MOID_T * moid, BYTE_T * item, int mod)
{
  if (moid == M_INT) {
    A68_INT *z = (A68_INT *) item;
    PUSH_UNION (p, M_INT);
    PUSH_VALUE (p, VALUE (z), A68_INT);
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_INT)));
    if (mod == FORMAT_ITEM_G) {
      PUSH_VALUE (p, A68_INT_WIDTH + 1, A68_INT);
      genie_whole (p);
    } else if (mod == FORMAT_ITEM_H) {
      PUSH_VALUE (p, A68_REAL_WIDTH + A68_EXP_WIDTH + 4, A68_INT);
      PUSH_VALUE (p, A68_REAL_WIDTH - 1, A68_INT);
      PUSH_VALUE (p, A68_EXP_WIDTH + 1, A68_INT);
      PUSH_VALUE (p, 3, A68_INT);
      genie_real (p);
    }
    return;
  }
#if (A68_LEVEL >= 3)
  if (moid == M_LONG_INT) {
    A68_LONG_INT *z = (A68_LONG_INT *) item;
    PUSH_UNION (p, M_LONG_INT);
    PUSH (p, z, SIZE (M_LONG_INT));
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_LONG_INT)));
    if (mod == FORMAT_ITEM_G) {
      PUSH_VALUE (p, A68_LONG_WIDTH + 1, A68_INT);
      genie_whole (p);
    } else if (mod == FORMAT_ITEM_H) {
      PUSH_VALUE (p, A68_LONG_REAL_WIDTH + A68_LONG_EXP_WIDTH + 4, A68_INT);
      PUSH_VALUE (p, A68_LONG_REAL_WIDTH - 1, A68_INT);
      PUSH_VALUE (p, A68_LONG_EXP_WIDTH + 1, A68_INT);
      PUSH_VALUE (p, 3, A68_INT);
      genie_real (p);
    }
    return;
  }
  if (moid == M_LONG_REAL) {
    A68_LONG_REAL *z = (A68_LONG_REAL *) item;
    PUSH_UNION (p, M_LONG_REAL);
    PUSH_VALUE (p, VALUE (z), A68_LONG_REAL);
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_LONG_REAL)));
    PUSH_VALUE (p, A68_LONG_REAL_WIDTH + A68_LONG_EXP_WIDTH + 4, A68_INT);
    PUSH_VALUE (p, A68_LONG_REAL_WIDTH - 1, A68_INT);
    PUSH_VALUE (p, A68_LONG_EXP_WIDTH + 1, A68_INT);
    if (mod == FORMAT_ITEM_G) {
      genie_float (p);
    } else if (mod == FORMAT_ITEM_H) {
      PUSH_VALUE (p, 3, A68_INT);
      genie_real (p);
    }
    return;
  }
  if (moid == M_LONG_BITS) {
    A68_LONG_BITS *z = (A68_LONG_BITS *) item;
    char *s = stack_string (p, 8 + A68_LONG_BITS_WIDTH);
    int n = 0;
    for (int w = 0; w <= 1; w++) {
      UNSIGNED_T bit = D_SIGN;
      for (int j = 0; j < A68_BITS_WIDTH; j++) {
        if (w == 0) {
          s[n] = (char) ((HW (VALUE (z)) & bit) ? FLIP_CHAR : FLOP_CHAR);
        } else {
          s[n] = (char) ((LW (VALUE (z)) & bit) ? FLIP_CHAR : FLOP_CHAR);
        }
        bit >>= 1;
        n++;
      }
    }
    s[n] = NULL_CHAR;
    return;
  }
#else
  if (moid == M_LONG_BITS || moid == M_LONG_LONG_BITS) {
    int bits = get_mp_bits_width (moid), word = get_mp_bits_words (moid);
    int pos = bits;
    char *str = stack_string (p, 8 + bits);
    ADDR_T pop_sp = A68_SP;
    unt *row = stack_mp_bits (p, (MP_T *) item, moid);
    str[pos--] = NULL_CHAR;
    while (pos >= 0) {
      unt bit = 0x1;
      for (int j = 0; j < MP_BITS_BITS && pos >= 0; j++) {
        str[pos--] = (char) ((row[word - 1] & bit) ? FLIP_CHAR : FLOP_CHAR);
        bit <<= 1;
      }
      word--;
    }
    A68_SP = pop_sp;
    return;
  }
#endif
  if (moid == M_LONG_INT) {
    MP_T *z = (MP_T *) item;
    PUSH_UNION (p, M_LONG_INT);
    PUSH (p, z, SIZE (M_LONG_INT));
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_LONG_INT)));
    if (mod == FORMAT_ITEM_G) {
      PUSH_VALUE (p, A68_LONG_WIDTH + 1, A68_INT);
      genie_whole (p);
    } else if (mod == FORMAT_ITEM_H) {
      PUSH_VALUE (p, A68_LONG_REAL_WIDTH + A68_LONG_EXP_WIDTH + 4, A68_INT);
      PUSH_VALUE (p, A68_LONG_REAL_WIDTH - 1, A68_INT);
      PUSH_VALUE (p, A68_LONG_EXP_WIDTH + 1, A68_INT);
      PUSH_VALUE (p, 3, A68_INT);
      genie_real (p);
    }
    return;
  }
  if (moid == M_LONG_LONG_INT) {
    MP_T *z = (MP_T *) item;
    PUSH_UNION (p, M_LONG_LONG_INT);
    PUSH (p, z, SIZE (M_LONG_LONG_INT));
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_LONG_LONG_INT)));
    if (mod == FORMAT_ITEM_G) {
      PUSH_VALUE (p, A68_LONG_LONG_WIDTH + 1, A68_INT);
      genie_whole (p);
    } else if (mod == FORMAT_ITEM_H) {
      PUSH_VALUE (p, A68_LONG_LONG_REAL_WIDTH + A68_LONG_LONG_EXP_WIDTH + 4, A68_INT);
      PUSH_VALUE (p, A68_LONG_LONG_REAL_WIDTH - 1, A68_INT);
      PUSH_VALUE (p, A68_LONG_LONG_EXP_WIDTH + 1, A68_INT);
      PUSH_VALUE (p, 3, A68_INT);
      genie_real (p);
    }
    return;
  }
  if (moid == M_REAL) {
    A68_REAL *z = (A68_REAL *) item;
    PUSH_UNION (p, M_REAL);
    PUSH_VALUE (p, VALUE (z), A68_REAL);
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_REAL)));
    PUSH_VALUE (p, A68_REAL_WIDTH + A68_EXP_WIDTH + 4, A68_INT);
    PUSH_VALUE (p, A68_REAL_WIDTH - 1, A68_INT);
    PUSH_VALUE (p, A68_EXP_WIDTH + 1, A68_INT);
    if (mod == FORMAT_ITEM_G) {
      genie_float (p);
    } else if (mod == FORMAT_ITEM_H) {
      PUSH_VALUE (p, 3, A68_INT);
      genie_real (p);
    }
    return;
  }
  if (moid == M_LONG_REAL) {
    MP_T *z = (MP_T *) item;
    PUSH_UNION (p, M_LONG_REAL);
    PUSH (p, z, (int) SIZE (M_LONG_REAL));
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_LONG_REAL)));
    PUSH_VALUE (p, A68_LONG_REAL_WIDTH + A68_LONG_EXP_WIDTH + 4, A68_INT);
    PUSH_VALUE (p, A68_LONG_REAL_WIDTH - 1, A68_INT);
    PUSH_VALUE (p, A68_LONG_EXP_WIDTH + 1, A68_INT);
    if (mod == FORMAT_ITEM_G) {
      genie_float (p);
    } else if (mod == FORMAT_ITEM_H) {
      PUSH_VALUE (p, 3, A68_INT);
      genie_real (p);
    }
    return;
  }
  if (moid == M_LONG_LONG_REAL) {
    MP_T *z = (MP_T *) item;
    PUSH_UNION (p, M_LONG_LONG_REAL);
    PUSH (p, z, (int) SIZE (M_LONG_LONG_REAL));
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_LONG_LONG_REAL)));
    PUSH_VALUE (p, A68_LONG_LONG_REAL_WIDTH + A68_LONG_LONG_EXP_WIDTH + 4, A68_INT);
    PUSH_VALUE (p, A68_LONG_LONG_REAL_WIDTH - 1, A68_INT);
    PUSH_VALUE (p, A68_LONG_LONG_EXP_WIDTH + 1, A68_INT);
    if (mod == FORMAT_ITEM_G) {
      genie_float (p);
    } else if (mod == FORMAT_ITEM_H) {
      PUSH_VALUE (p, 3, A68_INT);
      genie_real (p);
    }
    return;
  }
  if (moid == M_BITS) {
    A68_BITS *z = (A68_BITS *) item;
    char *str = stack_string (p, 8 + A68_BITS_WIDTH);
    UNSIGNED_T bit = 0x1;
    int j;
    for (j = 1; j < A68_BITS_WIDTH; j++) {
      bit <<= 1;
    }
    for (j = 0; j < A68_BITS_WIDTH; j++) {
      str[j] = (char) ((VALUE (z) & bit) ? FLIP_CHAR : FLOP_CHAR);
      bit >>= 1;
    }
    str[j] = NULL_CHAR;
    return;
  }
}

//! @brief Print object to file.

void genie_write_standard (NODE_T * p, MOID_T * mode, BYTE_T * item, A68_REF ref_file)
{
  errno = 0;
  ABEND (mode == NO_MOID, ERROR_INTERNAL_CONSISTENCY, __func__);
  if (mode == M_PROC_REF_FILE_VOID) {
    genie_call_proc_ref_file_void (p, ref_file, *(A68_PROCEDURE *) item);
  } else if (mode == M_FORMAT) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_UNDEFINED_TRANSPUT, M_FORMAT);
    exit_genie (p, A68_RUNTIME_ERROR);
  } else if (mode == M_SOUND) {
    write_sound (p, ref_file, (A68_SOUND *) item);
  } else if (mode == M_INT || mode == M_LONG_INT || mode == M_LONG_LONG_INT) {
    genie_value_to_string (p, mode, item, FORMAT_ITEM_G);
    add_string_from_stack_transput_buffer (p, UNFORMATTED_BUFFER);
  } else if (mode == M_REAL || mode == M_LONG_REAL || mode == M_LONG_LONG_REAL) {
    genie_value_to_string (p, mode, item, FORMAT_ITEM_G);
    add_string_from_stack_transput_buffer (p, UNFORMATTED_BUFFER);
  } else if (mode == M_BOOL) {
    A68_BOOL *z = (A68_BOOL *) item;
    char flipflop = (char) (VALUE (z) == A68_TRUE ? FLIP_CHAR : FLOP_CHAR);
    plusab_transput_buffer (p, UNFORMATTED_BUFFER, flipflop);
  } else if (mode == M_CHAR) {
    A68_CHAR *ch = (A68_CHAR *) item;
    plusab_transput_buffer (p, UNFORMATTED_BUFFER, (char) VALUE (ch));
  } else if (mode == M_BITS || mode == M_LONG_BITS || mode == M_LONG_LONG_BITS) {
    char *str = (char *) STACK_TOP;
    genie_value_to_string (p, mode, item, FORMAT_ITEM_G);
    add_string_transput_buffer (p, UNFORMATTED_BUFFER, str);
  } else if (mode == M_ROW_CHAR || mode == M_STRING) {
// Handle these separately since this is faster than straightening.
    add_a_string_transput_buffer (p, UNFORMATTED_BUFFER, item);
  } else if (IS_UNION (mode)) {
    A68_UNION *z = (A68_UNION *) item;
    genie_write_standard (p, (MOID_T *) (VALUE (z)), &item[A68_UNION_SIZE], ref_file);
  } else if (IS_STRUCT (mode)) {
    for (PACK_T *q = PACK (mode); q != NO_PACK; FORWARD (q)) {
      BYTE_T *elem = &item[OFFSET (q)];
      genie_check_initialisation (p, elem, MOID (q));
      genie_write_standard (p, MOID (q), elem, ref_file);
    }
  } else if (IS_ROW (mode) || IS_FLEX (mode)) {
    MOID_T *deflexed = DEFLEX (mode);
    A68_ARRAY *arr;
    A68_TUPLE *tup;
    CHECK_INIT (p, INITIALISED ((A68_REF *) item), M_ROWS);
    GET_DESCRIPTOR (arr, tup, (A68_REF *) item);
    if (get_row_size (tup, DIM (arr)) > 0) {
      BYTE_T *base_addr = DEREF (BYTE_T, &ARRAY (arr));
      BOOL_T done = A68_FALSE;
      initialise_internal_index (tup, DIM (arr));
      while (!done) {
        ADDR_T a68_index = calculate_internal_index (tup, DIM (arr));
        ADDR_T elem_addr = ROW_ELEMENT (arr, a68_index);
        BYTE_T *elem = &base_addr[elem_addr];
        genie_check_initialisation (p, elem, SUB (deflexed));
        genie_write_standard (p, SUB (deflexed), elem, ref_file);
        done = increment_internal_index (tup, DIM (arr));
      }
    }
  }
  if (errno != 0) {
    ABEND (IS_NIL (ref_file), ERROR_ACTION, error_specification ());
    transput_error (p, ref_file, mode);
  }
}

//! @brief PROC ([] SIMPLOUT) VOID print, write

void genie_write (NODE_T * p)
{
  A68_REF row;
  POP_REF (p, &row);
  genie_stand_out (p);
  PUSH_REF (p, row);
  genie_write_file (p);
}

//! @brief Open for writing.

void open_for_writing (NODE_T * p, A68_REF ref_file)
{
  A68_FILE *file = FILE_DEREF (&ref_file);
  if (!OPENED (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_NOT_OPEN);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (DRAW_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "draw");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (READ_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "read");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!PUT (&CHANNEL (file))) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_CHANNEL_DOES_NOT_ALLOW, "putting");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!READ_MOOD (file) && !WRITE_MOOD (file)) {
    if (IS_NIL (STRING (file))) {
      if ((FD (file) = open_physical_file (p, ref_file, A68_WRITE_ACCESS, A68_PROTECTION)) == A68_NO_FILE) {
        open_error (p, ref_file, "putting");
      }
    } else {
      FD (file) = open_physical_file (p, ref_file, A68_WRITE_ACCESS, 0);
    }
    DRAW_MOOD (file) = A68_FALSE;
    READ_MOOD (file) = A68_FALSE;
    WRITE_MOOD (file) = A68_TRUE;
    CHAR_MOOD (file) = A68_TRUE;
  }
  if (!CHAR_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "binary");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
}

//! @brief PROC (REF FILE, [] SIMPLOUT) VOID put

void genie_write_file (NODE_T * p)
{
  A68_REF row; A68_ARRAY *arr; A68_TUPLE *tup;
  POP_REF (p, &row);
  CHECK_REF (p, row, M_ROW_SIMPLOUT);
  GET_DESCRIPTOR (arr, tup, &row);
  int elems = ROW_SIZE (tup);
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  open_for_writing (p, ref_file);
// Write.
  if (elems <= 0) {
    return;
  }
  BYTE_T *base_address = DEREF (BYTE_T, &ARRAY (arr));
  int elem_index = 0;
  for (int k = 0; k < elems; k++) {
    A68_UNION *z = (A68_UNION *) & (base_address[elem_index]);
    MOID_T *mode = (MOID_T *) (VALUE (z));
    BYTE_T *item = (BYTE_T *) & base_address[elem_index + A68_UNION_SIZE];
    reset_transput_buffer (UNFORMATTED_BUFFER);
    genie_write_standard (p, mode, item, ref_file);
    write_purge_buffer (p, ref_file, UNFORMATTED_BUFFER);
    elem_index += SIZE (M_SIMPLOUT);
  }
}

//! @brief Read object binary from file.

void genie_read_bin_standard (NODE_T * p, MOID_T * mode, BYTE_T * item, A68_REF ref_file)
{
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *f = FILE_DEREF (&ref_file);
  errno = 0;
  if (END_OF_FILE (f)) {
    end_of_file_error (p, ref_file);
  }
  if (mode == M_PROC_REF_FILE_VOID) {
    genie_call_proc_ref_file_void (p, ref_file, *(A68_PROCEDURE *) item);
  } else if (mode == M_FORMAT) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_UNDEFINED_TRANSPUT, M_FORMAT);
    exit_genie (p, A68_RUNTIME_ERROR);
  } else if (mode == M_REF_SOUND) {
    read_sound (p, ref_file, (A68_SOUND *) ADDRESS ((A68_REF *) item));
  } else if (IS_REF (mode)) {
    CHECK_REF (p, *(A68_REF *) item, mode);
    genie_read_bin_standard (p, SUB (mode), ADDRESS ((A68_REF *) item), ref_file);
  } else if (mode == M_INT) {
    A68_INT *z = (A68_INT *) item;
    ASSERT (io_read (FD (f), &(VALUE (z)), sizeof (VALUE (z))) != -1);
    STATUS (z) = INIT_MASK;
  } else if (mode == M_LONG_INT) {
#if (A68_LEVEL >= 3)
    A68_LONG_INT *z = (A68_LONG_INT *) item;
    ASSERT (io_read (FD (f), &(VALUE (z)), sizeof (VALUE (z))) != -1);
    STATUS (z) = INIT_MASK;
#else
    MP_T *z = (MP_T *) item;
    ASSERT (io_read (FD (f), z, (size_t) SIZE (mode)) != -1);
    MP_STATUS (z) = (MP_T) INIT_MASK;
#endif
  } else if (mode == M_LONG_LONG_INT) {
    MP_T *z = (MP_T *) item;
    ASSERT (io_read (FD (f), z, (size_t) SIZE (mode)) != -1);
    MP_STATUS (z) = (MP_T) INIT_MASK;
  } else if (mode == M_REAL) {
    A68_REAL *z = (A68_REAL *) item;
    ASSERT (io_read (FD (f), &(VALUE (z)), sizeof (VALUE (z))) != -1);
    STATUS (z) = INIT_MASK;
  } else if (mode == M_LONG_REAL) {
#if (A68_LEVEL >= 3)
    A68_LONG_REAL *z = (A68_LONG_REAL *) item;
    ASSERT (io_read (FD (f), &(VALUE (z)), sizeof (VALUE (z))) != -1);
    STATUS (z) = INIT_MASK;
#else
    MP_T *z = (MP_T *) item;
    ASSERT (io_read (FD (f), z, (size_t) SIZE (mode)) != -1);
    MP_STATUS (z) = (MP_T) INIT_MASK;
#endif
  } else if (mode == M_LONG_LONG_REAL) {
    MP_T *z = (MP_T *) item;
    ASSERT (io_read (FD (f), z, (size_t) SIZE (mode)) != -1);
    MP_STATUS (z) = (MP_T) INIT_MASK;
  } else if (mode == M_BOOL) {
    A68_BOOL *z = (A68_BOOL *) item;
    ASSERT (io_read (FD (f), &(VALUE (z)), sizeof (VALUE (z))) != -1);
    STATUS (z) = INIT_MASK;
  } else if (mode == M_CHAR) {
    A68_CHAR *z = (A68_CHAR *) item;
    ASSERT (io_read (FD (f), &(VALUE (z)), sizeof (VALUE (z))) != -1);
    STATUS (z) = INIT_MASK;
  } else if (mode == M_BITS) {
    A68_BITS *z = (A68_BITS *) item;
    ASSERT (io_read (FD (f), &(VALUE (z)), sizeof (VALUE (z))) != -1);
    STATUS (z) = INIT_MASK;
  } else if (mode == M_LONG_BITS) {
#if (A68_LEVEL >= 3)
    A68_LONG_BITS *z = (A68_LONG_BITS *) item;
    ASSERT (io_read (FD (f), &(VALUE (z)), sizeof (VALUE (z))) != -1);
    STATUS (z) = INIT_MASK;
#else
    MP_T *z = (MP_T *) item;
    ASSERT (io_read (FD (f), z, (size_t) SIZE (mode)) != -1);
    MP_STATUS (z) = (MP_T) INIT_MASK;
#endif
  } else if (mode == M_LONG_LONG_BITS) {
    MP_T *z = (MP_T *) item;
    ASSERT (io_read (FD (f), z, (size_t) SIZE (mode)) != -1);
    MP_STATUS (z) = (MP_T) INIT_MASK;
  } else if (mode == M_ROW_CHAR || mode == M_STRING) {
    int len;
    ASSERT (io_read (FD (f), &(len), sizeof (len)) != -1);
    reset_transput_buffer (UNFORMATTED_BUFFER);
    for (int k = 0; k < len; k++) {
      char ch;
      ASSERT (io_read (FD (f), &(ch), sizeof (char)) != -1);
      plusab_transput_buffer (p, UNFORMATTED_BUFFER, ch);
    }
    *(A68_REF *) item = c_to_a_string (p, get_transput_buffer (UNFORMATTED_BUFFER), DEFAULT_WIDTH);
  } else if (IS_UNION (mode)) {
    A68_UNION *z = (A68_UNION *) item;
    if (!(STATUS (z) | INIT_MASK) || VALUE (z) == NULL) {
      diagnostic (A68_RUNTIME_ERROR, p, ERROR_EMPTY_VALUE, mode);
      exit_genie (p, A68_RUNTIME_ERROR);
    }
    genie_read_bin_standard (p, (MOID_T *) (VALUE (z)), &item[A68_UNION_SIZE], ref_file);
  } else if (IS_STRUCT (mode)) {
    for (PACK_T *q = PACK (mode); q != NO_PACK; FORWARD (q)) {
      genie_read_bin_standard (p, MOID (q), &item[OFFSET (q)], ref_file);
    }
  } else if (IS_ROW (mode) || IS_FLEX (mode)) {
    MOID_T *deflexed = DEFLEX (mode);
    A68_ARRAY *arr; A68_TUPLE *tup;
    CHECK_INIT (p, INITIALISED ((A68_REF *) item), M_ROWS);
    GET_DESCRIPTOR (arr, tup, (A68_REF *) item);
    if (get_row_size (tup, DIM (arr)) > 0) {
      BYTE_T *base_addr = DEREF (BYTE_T, &ARRAY (arr));
      BOOL_T done = A68_FALSE;
      initialise_internal_index (tup, DIM (arr));
      while (!done) {
        ADDR_T a68_index = calculate_internal_index (tup, DIM (arr));
        ADDR_T elem_addr = ROW_ELEMENT (arr, a68_index);
        genie_read_bin_standard (p, SUB (deflexed), &base_addr[elem_addr], ref_file);
        done = increment_internal_index (tup, DIM (arr));
      }
    }
  }
  if (errno != 0) {
    transput_error (p, ref_file, mode);
  }
}

//! @brief PROC ([] SIMPLIN) VOID read bin

void genie_read_bin (NODE_T * p)
{
  A68_REF row;
  POP_REF (p, &row);
  genie_stand_back (p);
  PUSH_REF (p, row);
  genie_read_bin_file (p);
}

//! @brief PROC (REF FILE, [] SIMPLIN) VOID get bin

void genie_read_bin_file (NODE_T * p)
{
  A68_REF row; A68_ARRAY *arr; A68_TUPLE *tup;
  POP_REF (p, &row);
  CHECK_REF (p, row, M_ROW_SIMPLIN);
  GET_DESCRIPTOR (arr, tup, &row);
  int elems = ROW_SIZE (tup);
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  ref_file = *(A68_REF *) STACK_TOP;
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (!OPENED (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_NOT_OPEN);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (DRAW_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "draw");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (WRITE_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "write");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!GET (&CHANNEL (file))) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_CHANNEL_DOES_NOT_ALLOW, "getting");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!BIN (&CHANNEL (file))) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_CHANNEL_DOES_NOT_ALLOW, "binary getting");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!READ_MOOD (file) && !WRITE_MOOD (file)) {
    if ((FD (file) = open_physical_file (p, ref_file, A68_READ_ACCESS | O_BINARY, 0)) == A68_NO_FILE) {
      open_error (p, ref_file, "binary getting");
    }
    DRAW_MOOD (file) = A68_FALSE;
    READ_MOOD (file) = A68_TRUE;
    WRITE_MOOD (file) = A68_FALSE;
    CHAR_MOOD (file) = A68_FALSE;
  }
  if (CHAR_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "text");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
// Read.
  if (elems <= 0) {
    return;
  }
  int elem_index = 0;
  BYTE_T *base_address = DEREF (BYTE_T, &ARRAY (arr));
  for (int k = 0; k < elems; k++) {
    A68_UNION *z = (A68_UNION *) & base_address[elem_index];
    MOID_T *mode = (MOID_T *) (VALUE (z));
    BYTE_T *item = (BYTE_T *) & base_address[elem_index + A68_UNION_SIZE];
    genie_read_bin_standard (p, mode, item, ref_file);
    elem_index += SIZE (M_SIMPLIN);
  }
}

//! @brief Write object binary to file.

void genie_write_bin_standard (NODE_T * p, MOID_T * mode, BYTE_T * item, A68_REF ref_file)
{
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *f = FILE_DEREF (&ref_file);
  errno = 0;
  if (mode == M_PROC_REF_FILE_VOID) {
    genie_call_proc_ref_file_void (p, ref_file, *(A68_PROCEDURE *) item);
  } else if (mode == M_FORMAT) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_UNDEFINED_TRANSPUT, M_FORMAT);
    exit_genie (p, A68_RUNTIME_ERROR);
  } else if (mode == M_SOUND) {
    write_sound (p, ref_file, (A68_SOUND *) item);
  } else if (mode == M_INT) {
    ASSERT (io_write (FD (f), &(VALUE ((A68_INT *) item)), sizeof (VALUE ((A68_INT *) item))) != -1);
  } else if (mode == M_LONG_INT) {
#if (A68_LEVEL >= 3)
    ASSERT (io_write (FD (f), &(VALUE ((A68_LONG_INT *) item)), sizeof (VALUE ((A68_LONG_INT *) item))) != -1);
#else
    ASSERT (io_write (FD (f), (MP_T *) item, (size_t) SIZE (mode)) != -1);
#endif
  } else if (mode == M_LONG_LONG_INT) {
    ASSERT (io_write (FD (f), (MP_T *) item, (size_t) SIZE (mode)) != -1);
  } else if (mode == M_REAL) {
    ASSERT (io_write (FD (f), &(VALUE ((A68_REAL *) item)), sizeof (VALUE ((A68_REAL *) item))) != -1);
  } else if (mode == M_LONG_REAL) {
#if (A68_LEVEL >= 3)
    ASSERT (io_write (FD (f), &(VALUE ((A68_LONG_REAL *) item)), sizeof (VALUE ((A68_LONG_REAL *) item))) != -1);
#else
    ASSERT (io_write (FD (f), (MP_T *) item, (size_t) SIZE (mode)) != -1);
#endif
  } else if (mode == M_LONG_LONG_REAL) {
    ASSERT (io_write (FD (f), (MP_T *) item, (size_t) SIZE (mode)) != -1);
  } else if (mode == M_BOOL) {
    ASSERT (io_write (FD (f), &(VALUE ((A68_BOOL *) item)), sizeof (VALUE ((A68_BOOL *) item))) != -1);
  } else if (mode == M_CHAR) {
    ASSERT (io_write (FD (f), &(VALUE ((A68_CHAR *) item)), sizeof (VALUE ((A68_CHAR *) item))) != -1);
  } else if (mode == M_BITS) {
    ASSERT (io_write (FD (f), &(VALUE ((A68_BITS *) item)), sizeof (VALUE ((A68_BITS *) item))) != -1);
  } else if (mode == M_LONG_BITS) {
#if (A68_LEVEL >= 3)
    ASSERT (io_write (FD (f), &(VALUE ((A68_LONG_BITS *) item)), sizeof (VALUE ((A68_LONG_BITS *) item))) != -1);
#else
    ASSERT (io_write (FD (f), (MP_T *) item, (size_t) SIZE (mode)) != -1);
#endif
  } else if (mode == M_LONG_LONG_BITS) {
    ASSERT (io_write (FD (f), (MP_T *) item, (size_t) SIZE (mode)) != -1);
  } else if (mode == M_ROW_CHAR || mode == M_STRING) {
    reset_transput_buffer (UNFORMATTED_BUFFER);
    add_a_string_transput_buffer (p, UNFORMATTED_BUFFER, item);
    int len = get_transput_buffer_index (UNFORMATTED_BUFFER);
    ASSERT (io_write (FD (f), &(len), sizeof (len)) != -1);
    WRITE (FD (f), get_transput_buffer (UNFORMATTED_BUFFER));
  } else if (IS_UNION (mode)) {
    A68_UNION *z = (A68_UNION *) item;
    genie_write_bin_standard (p, (MOID_T *) (VALUE (z)), &item[A68_UNION_SIZE], ref_file);
  } else if (IS_STRUCT (mode)) {
    for (PACK_T *q = PACK (mode); q != NO_PACK; FORWARD (q)) {
      BYTE_T *elem = &item[OFFSET (q)];
      genie_check_initialisation (p, elem, MOID (q));
      genie_write_bin_standard (p, MOID (q), elem, ref_file);
    }
  } else if (IS_ROW (mode) || IS_FLEX (mode)) {
    MOID_T *deflexed = DEFLEX (mode);
    A68_ARRAY *arr; A68_TUPLE *tup;
    CHECK_INIT (p, INITIALISED ((A68_REF *) item), M_ROWS);
    GET_DESCRIPTOR (arr, tup, (A68_REF *) item);
    if (get_row_size (tup, DIM (arr)) > 0) {
      BYTE_T *base_addr = DEREF (BYTE_T, &ARRAY (arr));
      BOOL_T done = A68_FALSE;
      initialise_internal_index (tup, DIM (arr));
      while (!done) {
        ADDR_T a68_index = calculate_internal_index (tup, DIM (arr));
        ADDR_T elem_addr = ROW_ELEMENT (arr, a68_index);
        BYTE_T *elem = &base_addr[elem_addr];
        genie_check_initialisation (p, elem, SUB (deflexed));
        genie_write_bin_standard (p, SUB (deflexed), elem, ref_file);
        done = increment_internal_index (tup, DIM (arr));
      }
    }
  }
  if (errno != 0) {
    transput_error (p, ref_file, mode);
  }
}

//! @brief PROC ([] SIMPLOUT) VOID write bin, print bin

void genie_write_bin (NODE_T * p)
{
  A68_REF row;
  POP_REF (p, &row);
  genie_stand_back (p);
  PUSH_REF (p, row);
  genie_write_bin_file (p);
}

//! @brief PROC (REF FILE, [] SIMPLOUT) VOID put bin

void genie_write_bin_file (NODE_T * p)
{
  A68_REF row; A68_ARRAY *arr; A68_TUPLE *tup;
  POP_REF (p, &row);
  CHECK_REF (p, row, M_ROW_SIMPLOUT);
  GET_DESCRIPTOR (arr, tup, &row);
  int elems = ROW_SIZE (tup);
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  ref_file = *(A68_REF *) STACK_TOP;
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (!OPENED (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_NOT_OPEN);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (DRAW_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "draw");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (READ_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "read");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!PUT (&CHANNEL (file))) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_CHANNEL_DOES_NOT_ALLOW, "putting");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!BIN (&CHANNEL (file))) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_CHANNEL_DOES_NOT_ALLOW, "binary putting");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!READ_MOOD (file) && !WRITE_MOOD (file)) {
    if ((FD (file) = open_physical_file (p, ref_file, A68_WRITE_ACCESS | O_BINARY, A68_PROTECTION)) == A68_NO_FILE) {
      open_error (p, ref_file, "binary putting");
    }
    DRAW_MOOD (file) = A68_FALSE;
    READ_MOOD (file) = A68_FALSE;
    WRITE_MOOD (file) = A68_TRUE;
    CHAR_MOOD (file) = A68_FALSE;
  }
  if (CHAR_MOOD (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_WRONG_MOOD, "text");
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (elems <= 0) {
    return;
  }
  BYTE_T *base_address = DEREF (BYTE_T, &ARRAY (arr));
  int elem_index = 0;
  for (int k = 0; k < elems; k++) {
    A68_UNION *z = (A68_UNION *) & base_address[elem_index];
    MOID_T *mode = (MOID_T *) (VALUE (z));
    BYTE_T *item = (BYTE_T *) & base_address[elem_index + A68_UNION_SIZE];
    genie_write_bin_standard (p, mode, item, ref_file);
    elem_index += SIZE (M_SIMPLOUT);
  }
}

// Next are formatting routines "whole", "fixed" and "float" for mode
// INT, LONG INT and LONG LONG INT, and REAL, LONG REAL and LONG LONG REAL.
// They are direct implementations of the routines described in the
// Revised Report, although those were only meant as a specification.
// The rest of Algol68G should only reference "genie_whole", "genie_fixed"
// or "genie_float" since internal routines like "sub_fixed" may leave the
// stack corrupted when called directly.

//! @brief Generate a string of error chars.

char *error_chars (char *s, int n)
{
  int k = (n != 0 ? ABS (n) : 1);
  s[k] = NULL_CHAR;
  while (--k >= 0) {
    s[k] = ERROR_CHAR;
  }
  return s;
}

//! @brief Convert temporary C string to A68 string.

A68_REF tmp_to_a68_string (NODE_T * p, char *temp_string)
{
// no compaction allowed since temp_string might be up for garbage collecting ...
  return c_to_a_string (p, temp_string, DEFAULT_WIDTH);
}

//! @brief Add c to str, assuming that "str" is large enough.

char *plusto (char c, char *str)
{
  MOVE (&str[1], &str[0], (unt) (strlen (str) + 1));
  str[0] = c;
  return str;
}

//! @brief Add c to str, assuming that "str" is large enough.

char *string_plusab_char (char *str, char c, int strwid)
{
  char z[2];
  z[0] = c;
  z[1] = NULL_CHAR;
  a68_bufcat (str, z, strwid);
  return str;
}

//! @brief Add leading spaces to str until length is width.

char *leading_spaces (char *str, int width)
{
  int j = width - (int) strlen (str);
  while (--j >= 0) {
    (void) plusto (BLANK_CHAR, str);
  }
  return str;
}

//! @brief Convert int to char using a table.

char digchar (int k)
{
  char *s = "0123456789abcdefghijklmnopqrstuvwxyz";
  if (k >= 0 && k < (int) strlen (s)) {
    return s[k];
  } else {
    return ERROR_CHAR;
  }
}

//! @brief Formatted string for HEX_NUMBER.

char *bits_to_string (NODE_T * p)
{
  A68_INT width, base;
  POP_OBJECT (p, &base, A68_INT);
  POP_OBJECT (p, &width, A68_INT);
  DECREMENT_STACK_POINTER (p, SIZE (M_HEX_NUMBER));
  CHECK_INT_SHORTEN (p, VALUE (&base));
  CHECK_INT_SHORTEN (p, VALUE (&width));
  MOID_T *mode = (MOID_T *) (VALUE ((A68_UNION *) STACK_TOP));
  int length = ABS (VALUE (&width)), radix = ABS (VALUE (&base));
  if (radix < 2 || radix > 16) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_INVALID_RADIX, radix);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  reset_transput_buffer (EDIT_BUFFER);
#if (A68_LEVEL <= 2)
  (void) mode;
  (void) length;
  (void) error_chars (get_transput_buffer (EDIT_BUFFER), VALUE (&width));
#else
  {
    BOOL_T ret = A68_TRUE;
    if (mode == M_BOOL) {
      UNSIGNED_T z = VALUE ((A68_BOOL *) (STACK_OFFSET (A68_UNION_SIZE)));
      ret = convert_radix (p, (UNSIGNED_T) z, radix, length);
    } else if (mode == M_CHAR) {
      INT_T z = VALUE ((A68_CHAR *) (STACK_OFFSET (A68_UNION_SIZE)));
      ret = convert_radix (p, (UNSIGNED_T) z, radix, length);
    } else if (mode == M_INT) {
      INT_T z = VALUE ((A68_INT *) (STACK_OFFSET (A68_UNION_SIZE)));
      ret = convert_radix (p, (UNSIGNED_T) z, radix, length);
    } else if (mode == M_REAL) {
// A trick to copy a REAL into an unt without truncating
      UNSIGNED_T z;
      memcpy (&z, (void *) &VALUE ((A68_REAL *) (STACK_OFFSET (A68_UNION_SIZE))), 8);
      ret = convert_radix (p, z, radix, length);
    } else if (mode == M_BITS) {
      UNSIGNED_T z = VALUE ((A68_BITS *) (STACK_OFFSET (A68_UNION_SIZE)));
      ret = convert_radix (p, (UNSIGNED_T) z, radix, length);
    } else if (mode == M_LONG_INT) {
      DOUBLE_NUM_T z = VALUE ((A68_LONG_INT *) (STACK_OFFSET (A68_UNION_SIZE)));
      ret = convert_radix_double (p, z, radix, length);
    } else if (mode == M_LONG_REAL) {
      DOUBLE_NUM_T z = VALUE ((A68_LONG_REAL *) (STACK_OFFSET (A68_UNION_SIZE)));
      ret = convert_radix_double (p, z, radix, length);
    } else if (mode == M_LONG_BITS) {
      DOUBLE_NUM_T z = VALUE ((A68_LONG_BITS *) (STACK_OFFSET (A68_UNION_SIZE)));
      ret = convert_radix_double (p, z, radix, length);
    }
    if (ret == A68_FALSE) {
      errno = EDOM;
      PRELUDE_ERROR (A68_TRUE, p, ERROR_OUT_OF_BOUNDS, mode);
    }
  }
#endif
  return get_transput_buffer (EDIT_BUFFER);
}

//! @brief Standard string for LONG INT.

#if (A68_LEVEL >= 3)
char *long_sub_whole_double (NODE_T * p, DOUBLE_NUM_T n, int width)
{
  char *s = stack_string (p, 8 + width);
  DOUBLE_NUM_T ten;
  set_lw (ten, 10);
  s[0] = NULL_CHAR;
  int len = 0;
  do {
    if (len < width) {
      DOUBLE_NUM_T w = double_udiv (p, M_LONG_INT, n, ten, 1);
      (void) plusto (digchar (LW (w)), s);
    }
    len++;
    n = double_udiv (p, M_LONG_INT, n, ten, 0);
  } while (!D_ZERO (n));
  if (len > width) {
    (void) error_chars (s, width);
  }
  return s;
}
#endif

char *long_sub_whole (NODE_T * p, MP_T * m, int digits, int width)
{
  int len = 0;
  char *s = stack_string (p, 8 + width);
  s[0] = NULL_CHAR;
  ADDR_T pop_sp = A68_SP;
  MP_T *n = nil_mp (p, digits);
  (void) move_mp (n, m, digits);
  do {
    if (len < width) {
// Sic transit gloria mundi.
      int n_mod_10 = (MP_INT_T) MP_DIGIT (n, (int) (1 + MP_EXPONENT (n))) % 10;
      (void) plusto (digchar (n_mod_10), s);
    }
    len++;
    (void) over_mp_digit (p, n, n, (MP_T) 10, digits);
  } while (MP_DIGIT (n, 1) > 0);
  if (len > width) {
    (void) error_chars (s, width);
  }
  A68_SP = pop_sp;
  return s;
}

//! @brief Standard string for INT.

char *sub_whole (NODE_T * p, INT_T n, int width)
{
  char *s = stack_string (p, 8 + width);
  int len = 0;
  s[0] = NULL_CHAR;
  do {
    if (len < width) {
      (void) plusto (digchar (n % 10), s);
    }
    len++;
    n /= 10;
  } while (n != 0);
  if (len > width) {
    (void) error_chars (s, width);
  }
  return s;
}

//! @brief Formatted string for NUMBER.

char *whole (NODE_T * p)
{
  A68_INT width;
  POP_OBJECT (p, &width, A68_INT);
  CHECK_INT_SHORTEN (p, VALUE (&width));
  int arg_sp = A68_SP;
  DECREMENT_STACK_POINTER (p, SIZE (M_NUMBER));
  MOID_T *mode = (MOID_T *) (VALUE ((A68_UNION *) STACK_TOP));
  if (mode == M_INT) {
    INT_T x = VALUE ((A68_INT *) (STACK_OFFSET (A68_UNION_SIZE)));
    INT_T n = ABS (x);
    int length = ABS (VALUE (&width)) - (x < 0 || VALUE (&width) > 0 ? 1 : 0);
    int size = (x < 0 ? 1 : (VALUE (&width) > 0 ? 1 : 0));
    if (VALUE (&width) == 0) {
      INT_T m = n;
      length = 0;
      while ((m /= 10, length++, m != 0)) {
        ;
      }
    }
    size += length;
    size = 8 + (size > VALUE (&width) ? size : VALUE (&width));
    char *s = stack_string (p, size);
    a68_bufcpy (s, sub_whole (p, n, length), size);
    if (length == 0 || strchr (s, ERROR_CHAR) != NO_TEXT) {
      (void) error_chars (s, VALUE (&width));
    } else {
      if (x < 0) {
        (void) plusto ('-', s);
      } else if (VALUE (&width) > 0) {
        (void) plusto ('+', s);
      }
      if (VALUE (&width) != 0) {
        (void) leading_spaces (s, ABS (VALUE (&width)));
      }
    }
    return s;
  }
#if (A68_LEVEL >= 3)
  if (mode == M_LONG_INT) {
    DOUBLE_NUM_T x = VALUE ((A68_LONG_INT *) (STACK_OFFSET (A68_UNION_SIZE))), n, ten;
    set_lw (ten, 10);
    n = abs_double_int (x);
    int length = ABS (VALUE (&width)) - (D_NEG (x) || VALUE (&width) > 0 ? 1 : 0);
    int size = (D_NEG (x) ? 1 : (VALUE (&width) > 0 ? 1 : 0));
    if (VALUE (&width) == 0) {
      DOUBLE_NUM_T m = n;
      length = 0;
      while ((m = double_udiv (p, M_LONG_INT, m, ten, 0), length++, !D_ZERO (m))) {
        ;
      }
    }
    size += length;
    size = 8 + (size > VALUE (&width) ? size : VALUE (&width));
    char *s = stack_string (p, size);
    a68_bufcpy (s, long_sub_whole_double (p, n, length), size);
    if (length == 0 || strchr (s, ERROR_CHAR) != NO_TEXT) {
      (void) error_chars (s, VALUE (&width));
    } else {
      if (D_NEG (x)) {
        (void) plusto ('-', s);
      } else if (VALUE (&width) > 0) {
        (void) plusto ('+', s);
      }
      if (VALUE (&width) != 0) {
        (void) leading_spaces (s, ABS (VALUE (&width)));
      }
    }
    return s;
  }
#endif
  if (mode == M_LONG_INT || mode == M_LONG_LONG_INT) {
    int digits = DIGITS (mode);
    MP_T *n = (MP_T *) (STACK_OFFSET (A68_UNION_SIZE));
    A68_SP = arg_sp;            // We keep the mp where it's at
    if (MP_EXPONENT (n) >= (MP_T) digits) {
      int max_length = (mode == M_LONG_INT ? A68_LONG_INT_WIDTH : A68_LONG_LONG_INT_WIDTH);
      int length = (VALUE (&width) == 0 ? max_length : VALUE (&width));
      char *s = stack_string (p, 1 + length);
      (void) error_chars (s, length);
      return s;
    }
    BOOL_T ltz = (BOOL_T) (MP_DIGIT (n, 1) < 0);
    int length = ABS (VALUE (&width)) - (ltz || VALUE (&width) > 0 ? 1 : 0);
    int size = (ltz ? 1 : (VALUE (&width) > 0 ? 1 : 0));
    MP_DIGIT (n, 1) = ABS (MP_DIGIT (n, 1));
    if (VALUE (&width) == 0) {
      MP_T *m = nil_mp (p, digits);
      (void) move_mp (m, n, digits);
      length = 0;
      while ((over_mp_digit (p, m, m, (MP_T) 10, digits), length++, MP_DIGIT (m, 1) != 0)) {
        ;
      }
    }
    size += length;
    size = 8 + (size > VALUE (&width) ? size : VALUE (&width));
    char *s = stack_string (p, size);
    a68_bufcpy (s, long_sub_whole (p, n, digits, length), size);
    if (length == 0 || strchr (s, ERROR_CHAR) != NO_TEXT) {
      (void) error_chars (s, VALUE (&width));
    } else {
      if (ltz) {
        (void) plusto ('-', s);
      } else if (VALUE (&width) > 0) {
        (void) plusto ('+', s);
      }
      if (VALUE (&width) != 0) {
        (void) leading_spaces (s, ABS (VALUE (&width)));
      }
    }
    return s;
  }
  if (mode == M_REAL || mode == M_LONG_REAL || mode == M_LONG_LONG_REAL) {
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER));
    PUSH_VALUE (p, VALUE (&width), A68_INT);
    PUSH_VALUE (p, 0, A68_INT);
    return fixed (p);
  }
  return NO_TEXT;
}

//! @brief Fetch next digit from LONG.

char long_choose_dig (NODE_T * p, MP_T * y, int digits)
{
// Assuming positive "y".
  ADDR_T pop_sp = A68_SP;
  (void) mul_mp_digit (p, y, y, (MP_T) 10, digits);
  int c = MP_EXPONENT (y) == 0 ? (MP_INT_T) MP_DIGIT (y, 1) : 0;
  if (c > 9) {
    c = 9;
  }
  MP_T *t = lit_mp (p, c, 0, digits);
  (void) sub_mp (p, y, y, t, digits);
// Reset the stack to prevent overflow, there may be many digits.
  A68_SP = pop_sp;
  return digchar (c);
}

//! @brief Standard string for LONG.

char *long_sub_fixed (NODE_T * p, MP_T * x, int digits, int width, int after)
{
  ADDR_T pop_sp = A68_SP;
  MP_T *y = nil_mp (p, digits);
  MP_T *s = nil_mp (p, digits);
  MP_T *t = nil_mp (p, digits);
  (void) ten_up_mp (p, t, -after, digits);
  (void) half_mp (p, t, t, digits);
  (void) add_mp (p, y, x, t, digits);
  int before = 0;
// Not RR - argument reduction.
  while (MP_EXPONENT (y) > 1) {
    int k = (int) round (MP_EXPONENT (y) - 1);
    MP_EXPONENT (y) -= k;
    before += k * LOG_MP_RADIX;
  }
// Follow RR again.
  SET_MP_ONE (s, digits);
  while ((sub_mp (p, t, y, s, digits), MP_DIGIT (t, 1) >= 0)) {
    before++;
    (void) div_mp_digit (p, y, y, (MP_T) 10, digits);
  }
// Compose the number.
  if (before + after + (after > 0 ? 1 : 0) > width) {
    char *str = stack_string (p, width + 1);
    (void) error_chars (str, width);
    A68_SP = pop_sp;
    return str;
  }
  int strwid = 8 + before + after;
  char *str = stack_string (p, strwid);
  str[0] = NULL_CHAR;
  int len = 0;
  for (int j = 0; j < before; j++) {
    char ch = (char) (len < A68_LONG_LONG_REAL_WIDTH ? long_choose_dig (p, y, digits) : '0');
    (void) string_plusab_char (str, ch, strwid);
    len++;
  }
  if (after > 0) {
    (void) string_plusab_char (str, POINT_CHAR, strwid);
  }
  for (int j = 0; j < after; j++) {
    char ch = (char) (len < A68_LONG_LONG_REAL_WIDTH ? long_choose_dig (p, y, digits) : '0');
    (void) string_plusab_char (str, ch, strwid);
    len++;
  }
  if ((int) strlen (str) > width) {
    (void) error_chars (str, width);
  }
  A68_SP = pop_sp;
  return str;
}

#if (A68_LEVEL >= 3)

//! @brief Fetch next digit from REAL.

char choose_dig_double (DOUBLE_T * y)
{
// Assuming positive "y".
  int c = (int) (*y *= 10);
  if (c > 9) {
    c = 9;
  }
  *y -= c;
  return digchar (c);
}

#endif

#if (A68_LEVEL >= 3)

//! @brief Standard string for REAL.

char *sub_fixed_double (NODE_T * p, DOUBLE_T x, int width, int after, int precision)
{
  ABEND (x < 0, ERROR_INTERNAL_CONSISTENCY, __func__);
// Round and scale. 
  DOUBLE_T z = x + 0.5q * ten_up_double (-after);
  DOUBLE_T y = z;
  int before = 0;
// Not according RR - argument reduction to avoid long division loop.
  if (z >= 1.0e10q) {          // Arbitrary, log10 must be worthwhile.
    before = (int) floor_double (log10_double (z)) - 1;
    z /= ten_up_double (before);
  }
// Follow RR again.
  while (z >= 1.0q) {
    before++;
    z /= 10.0q;
  }
// Scale number.
  y /= ten_up_double (before);
// Put digits, prevent garbage from overstretching precision.
// Many languages produce garbage when specifying more decimals 
// than the type actually has. A68G pads '0's in this case.
// That is just as arbitrary, but at least recognisable.
  int strwid = 8 + before + after;      // A bit too long.
  char *str = stack_string (p, strwid);
  int len = 0;
  for (int j = 0; j < before; j++) {
    char ch = (char) (len < precision ? choose_dig_double (&y) : '0');
    (void) string_plusab_char (str, ch, strwid);
    len++;
  }
  if (after > 0) {
    (void) string_plusab_char (str, POINT_CHAR, strwid);
  }
  for (int j = 0; j < after; j++) {
    char ch = (char) (len < precision ? choose_dig_double (&y) : '0');
    (void) string_plusab_char (str, ch, strwid);
    len++;
  }
  if ((int) strlen (str) > width) {
    (void) error_chars (str, width);
  }
  return str;
}

//! @brief Standard string for REAL.

char *sub_fixed (NODE_T * p, REAL_T x, int width, int after)
{
// Better precision than the REAL only routine
  return sub_fixed_double (p, (DOUBLE_T) x, width, after, A68_REAL_WIDTH);
}

#else

//! @brief Fetch next digit from REAL.

char choose_dig (REAL_T * y)
{
// Assuming positive "y".
  int c = (int) (*y *= 10);
  if (c > 9) {
    c = 9;
  }
  *y -= c;
  return digchar (c);
}

//! @brief Standard string for REAL.

char *sub_fixed (NODE_T * p, REAL_T x, int width, int after)
{
  ABEND (x < 0, ERROR_INTERNAL_CONSISTENCY, __func__);
// Round and scale. 
  REAL_T z = x + 0.5 * ten_up (-after);
  REAL_T y = z;
  int before = 0;
// Not according RR - argument reduction to avoid long division loop.
  if (z >= 1.0e10) {            // Arbitrary, log10 must be worthwhile.
    before = (int) floor (log10 (z)) - 1;
    z /= ten_up (before);
  }
// Follow RR again.
  while (z >= 1.0) {
    before++;
    z /= 10.0;
  }
// Scale number.
  y /= ten_up (before);
// Put digits, prevent garbage from overstretching precision.
// Many languages produce garbage when specifying more decimals 
// than the type actually has. A68G pads '0's in this case.
// That is just as arbitrary, but at least recognisable.
  int strwid = 8 + before + after;      // A bit too long.
  char *str = stack_string (p, strwid);
  int len = 0;
  for (int j = 0; j < before; j++) {
    char ch = (char) (len < A68_REAL_WIDTH ? choose_dig (&y) : '0');
    (void) string_plusab_char (str, ch, strwid);
    len++;
  }
  if (after > 0) {
    (void) string_plusab_char (str, POINT_CHAR, strwid);
  }
  for (int j = 0; j < after; j++) {
    char ch = (char) (len < A68_REAL_WIDTH ? choose_dig (&y) : '0');
    (void) string_plusab_char (str, ch, strwid);
    len++;
  }
  if ((int) strlen (str) > width) {
    (void) error_chars (str, width);
  }
  return str;
}

#endif

//! @brief Formatted string for NUMBER.

char *fixed (NODE_T * p)
{
  A68_INT width, after;
  POP_OBJECT (p, &after, A68_INT);
  POP_OBJECT (p, &width, A68_INT);
  CHECK_INT_SHORTEN (p, VALUE (&after));
  CHECK_INT_SHORTEN (p, VALUE (&width));
  ADDR_T arg_sp = A68_SP;
  DECREMENT_STACK_POINTER (p, SIZE (M_NUMBER));
  MOID_T *mode = (MOID_T *) (VALUE ((A68_UNION *) STACK_TOP));
  ADDR_T pop_sp = A68_SP;
  if (mode == M_REAL) {
    REAL_T x = VALUE ((A68_REAL *) (STACK_OFFSET (A68_UNION_SIZE)));
    int length = ABS (VALUE (&width)) - (x < 0 || VALUE (&width) > 0 ? 1 : 0);
    CHECK_REAL (p, x);
    A68_SP = arg_sp;
    if (VALUE (&after) >= 0 && (length > VALUE (&after) || VALUE (&width) == 0)) {
      REAL_T y = ABS (x);
      if (VALUE (&width) == 0) {
        length = (VALUE (&after) == 0 ? 1 : 0);
        REAL_T z0 = ten_up (-VALUE (&after)), z1 = ten_up (length);
        while (y + 0.5 * z0 > z1) {
          length++;
          z1 *= 10.0;
        }
        length += (VALUE (&after) == 0 ? 0 : VALUE (&after) + 1);
      }
      char *s = sub_fixed (p, y, length, VALUE (&after));
      if (strchr (s, ERROR_CHAR) == NO_TEXT) {
        if (length > (int) strlen (s) && (s[0] != NULL_CHAR ? s[0] == POINT_CHAR : A68_TRUE) && y < 1.0) {
          (void) plusto ('0', s);
        }
        if (x < 0) {
          (void) plusto ('-', s);
        } else if (VALUE (&width) > 0) {
          (void) plusto ('+', s);
        }
        if (VALUE (&width) != 0) {
          (void) leading_spaces (s, ABS (VALUE (&width)));
        }
        return s;
      } else if (VALUE (&after) > 0) {
        A68_SP = arg_sp;
        PUSH_VALUE (p, VALUE (&width), A68_INT);
        PUSH_VALUE (p, VALUE (&after) - 1, A68_INT);
        return fixed (p);
      } else {
        return error_chars (s, VALUE (&width));
      }
    } else {
      char *s = stack_string (p, 8 + ABS (VALUE (&width)));
      return error_chars (s, VALUE (&width));
    }
  }
#if (A68_LEVEL >= 3)
  if (mode == M_LONG_REAL) {
    DOUBLE_T x = VALUE ((A68_LONG_REAL *) (STACK_OFFSET (A68_UNION_SIZE))).f;
    int length = ABS (VALUE (&width)) - (x < 0 || VALUE (&width) > 0 ? 1 : 0);
    CHECK_DOUBLE_REAL (p, x);
    A68_SP = arg_sp;
    if (VALUE (&after) >= 0 && (length > VALUE (&after) || VALUE (&width) == 0)) {
      DOUBLE_T y = ABS (x);
      if (VALUE (&width) == 0) {
        length = (VALUE (&after) == 0 ? 1 : 0);
        DOUBLE_T z0 = ten_up_double (-VALUE (&after)), z1 = ten_up_double (length);
        while (y + 0.5 * z0 > z1) {
          length++;
          z1 *= 10.0;
        }
        length += (VALUE (&after) == 0 ? 0 : VALUE (&after) + 1);
      }
      char *s = sub_fixed_double (p, y, length, VALUE (&after), A68_LONG_REAL_WIDTH);
      if (strchr (s, ERROR_CHAR) == NO_TEXT) {
        if (length > (int) strlen (s) && (s[0] != NULL_CHAR ? s[0] == POINT_CHAR : A68_TRUE) && y < 1.0) {
          (void) plusto ('0', s);
        }
        if (x < 0) {
          (void) plusto ('-', s);
        } else if (VALUE (&width) > 0) {
          (void) plusto ('+', s);
        }
        if (VALUE (&width) != 0) {
          (void) leading_spaces (s, ABS (VALUE (&width)));
        }
        return s;
      } else if (VALUE (&after) > 0) {
        A68_SP = arg_sp;
        PUSH_VALUE (p, VALUE (&width), A68_INT);
        PUSH_VALUE (p, VALUE (&after) - 1, A68_INT);
        return fixed (p);
      } else {
        return error_chars (s, VALUE (&width));
      }
    } else {
      char *s = stack_string (p, 8 + ABS (VALUE (&width)));
      return error_chars (s, VALUE (&width));
    }
  }
#endif
  if (mode == M_LONG_REAL || mode == M_LONG_LONG_REAL) {
    int digits = DIGITS (mode);
    MP_T *x = (MP_T *) (STACK_OFFSET (A68_UNION_SIZE));
    A68_SP = arg_sp;
    BOOL_T ltz = (BOOL_T) (MP_DIGIT (x, 1) < 0);
    MP_DIGIT (x, 1) = ABS (MP_DIGIT (x, 1));
    int length = ABS (VALUE (&width)) - (ltz || VALUE (&width) > 0 ? 1 : 0);
    if (VALUE (&after) >= 0 && (length > VALUE (&after) || VALUE (&width) == 0)) {
      MP_T *z0 = nil_mp (p, digits);
      MP_T *z1 = nil_mp (p, digits);
      MP_T *t = nil_mp (p, digits);
      if (VALUE (&width) == 0) {
        length = (VALUE (&after) == 0 ? 1 : 0);
        (void) set_mp (z0, (MP_T) (MP_RADIX / 10), -1, digits);
        (void) set_mp (z1, (MP_T) 10, 0, digits);
        (void) pow_mp_int (p, z0, z0, VALUE (&after), digits);
        (void) pow_mp_int (p, z1, z1, length, digits);
        while ((div_mp_digit (p, t, z0, (MP_T) 2, digits), add_mp (p, t, x, t, digits), sub_mp (p, t, t, z1, digits), MP_DIGIT (t, 1) > 0)) {
          length++;
          (void) mul_mp_digit (p, z1, z1, (MP_T) 10, digits);
        }
        length += (VALUE (&after) == 0 ? 0 : VALUE (&after) + 1);
      }
//    char *s = stack_string (p, 8 + length);
      char *s = long_sub_fixed (p, x, digits, length, VALUE (&after));
      if (strchr (s, ERROR_CHAR) == NO_TEXT) {
        if (length > (int) strlen (s) && (s[0] != NULL_CHAR ? s[0] == POINT_CHAR : A68_TRUE) && (MP_EXPONENT (x) < 0 || MP_DIGIT (x, 1) == 0)) {
          (void) plusto ('0', s);
        }
        if (ltz) {
          (void) plusto ('-', s);
        } else if (VALUE (&width) > 0) {
          (void) plusto ('+', s);
        }
        if (VALUE (&width) != 0) {
          (void) leading_spaces (s, ABS (VALUE (&width)));
        }
        return s;
      } else if (VALUE (&after) > 0) {
        A68_SP = arg_sp;
        MP_DIGIT (x, 1) = ltz ? -ABS (MP_DIGIT (x, 1)) : ABS (MP_DIGIT (x, 1));
        PUSH_VALUE (p, VALUE (&width), A68_INT);
        PUSH_VALUE (p, VALUE (&after) - 1, A68_INT);
        return fixed (p);
      } else {
        return error_chars (s, VALUE (&width));
      }
    } else {
      char *s = stack_string (p, 8 + ABS (VALUE (&width)));
      return error_chars (s, VALUE (&width));
    }
  }
  if (mode == M_INT) {
    int x = VALUE ((A68_INT *) (STACK_OFFSET (A68_UNION_SIZE)));
    PUSH_UNION (p, M_REAL);
    PUSH_VALUE (p, (REAL_T) x, A68_REAL);
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_REAL)));
    PUSH_VALUE (p, VALUE (&width), A68_INT);
    PUSH_VALUE (p, VALUE (&after), A68_INT);
    return fixed (p);
  }
  if (mode == M_LONG_INT || mode == M_LONG_LONG_INT) {
    A68_SP = pop_sp;
    if (mode == M_LONG_INT) {
      VALUE ((A68_UNION *) STACK_TOP) = (void *) M_LONG_REAL;
    } else {
      VALUE ((A68_UNION *) STACK_TOP) = (void *) M_LONG_LONG_REAL;
    } INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER));
    PUSH_VALUE (p, VALUE (&width), A68_INT);
    PUSH_VALUE (p, VALUE (&after), A68_INT);
    return fixed (p);
  }
  return NO_TEXT;
}

//! @brief Scale LONG for formatting.

void long_standardise (NODE_T * p, MP_T * y, int digits, int before, int after, int *q)
{
  ADDR_T pop_sp = A68_SP;
  MP_T *f = nil_mp (p, digits);
  MP_T *g = nil_mp (p, digits);
  MP_T *h = nil_mp (p, digits);
  MP_T *t = nil_mp (p, digits);
  ten_up_mp (p, g, before, digits);
  (void) div_mp_digit (p, h, g, (MP_T) 10, digits);
// Speed huge exponents.
  if ((MP_EXPONENT (y) - MP_EXPONENT (g)) > 1) {
    (*q) += LOG_MP_RADIX * ((int) MP_EXPONENT (y) - (int) MP_EXPONENT (g) - 1);
    MP_EXPONENT (y) = MP_EXPONENT (g) + 1;
  }
  while ((sub_mp (p, t, y, g, digits), MP_DIGIT (t, 1) >= 0)) {
    (void) div_mp_digit (p, y, y, (MP_T) 10, digits);
    (*q)++;
  }
  if (MP_DIGIT (y, 1) != 0) {
// Speed huge exponents.
    if ((MP_EXPONENT (y) - MP_EXPONENT (h)) < -1) {
      (*q) -= LOG_MP_RADIX * ((int) MP_EXPONENT (h) - (int) MP_EXPONENT (y) - 1);
      MP_EXPONENT (y) = MP_EXPONENT (h) - 1;
    }
    while ((sub_mp (p, t, y, h, digits), MP_DIGIT (t, 1) < 0)) {
      (void) mul_mp_digit (p, y, y, (MP_T) 10, digits);
      (*q)--;
    }
  }
  ten_up_mp (p, f, -after, digits);
  (void) div_mp_digit (p, t, f, (MP_T) 2, digits);
  (void) add_mp (p, t, y, t, digits);
  (void) sub_mp (p, t, t, g, digits);
  if (MP_DIGIT (t, 1) >= 0) {
    (void) move_mp (y, h, digits);
    (*q)++;
  }
  A68_SP = pop_sp;
}

#if (A68_LEVEL >= 3)

//! @brief Scale REAL for formatting.

void standardise_double (DOUBLE_T * y, int before, int after, int *p)
{
//int g = 1.0q; for (int j = 0; j < before; j++) { g *= 10.0q; }
  DOUBLE_T g = ten_up_double (before);
  DOUBLE_T h = g / 10.0q;
  while (*y >= g) {
    *y *= 0.1q;
    (*p)++;
  }
  if (*y != 0.0q) {
    while (*y < h) {
      *y *= 10.0q;
      (*p)--;
    }
  }
//f = 1.0q; for (int j = 0; j < after; j++) { f *= 0.1q; }
  DOUBLE_T f = ten_up_double (-after);
  if (*y + 0.5q * f >= g) {
    *y = h;
    (*p)++;
  }
}

//! @brief Scale REAL for formatting.

void standardise (REAL_T * y, int before, int after, int *p)
{
// Better precision than the REAL only routine
  DOUBLE_T z = (DOUBLE_T) * y;
  standardise_double (&z, before, after, p);
  *y = (REAL_T) z;
}

#else

//! @brief Scale REAL for formatting.

void standardise (REAL_T * y, int before, int after, int *p)
{
// This according RR, but for REAL the last digits are approximate.
// A68G 3 uses DOUBLE precision version.
//int g = 1.0; for (int j = 0; j < before; j++) { g *= 10.0; }
  REAL_T g = ten_up (before);
  REAL_T h = g / 10.0;
  while (*y >= g) {
    *y *= 0.1;
    (*p)++;
  }
  if (*y != 0.0) {
    while (*y < h) {
      *y *= 10.0;
      (*p)--;
    }
  }
//f = 1.0; for (int j = 0; j < after; j++) { f *= 0.1; }
  REAL_T f = ten_up (-after);
  if (*y + 0.5 * f >= g) {
    *y = h;
    (*p)++;
  }
}

#endif

//! @brief Formatted string for NUMBER.

char *real (NODE_T * p)
{
// POP arguments.
  A68_INT width, after, expo, frmt;
  POP_OBJECT (p, &frmt, A68_INT);
  POP_OBJECT (p, &expo, A68_INT);
  POP_OBJECT (p, &after, A68_INT);
  POP_OBJECT (p, &width, A68_INT);
  CHECK_INT_SHORTEN (p, VALUE (&frmt));
  CHECK_INT_SHORTEN (p, VALUE (&expo));
  CHECK_INT_SHORTEN (p, VALUE (&after));
  CHECK_INT_SHORTEN (p, VALUE (&width));
  ADDR_T arg_sp = A68_SP;
  DECREMENT_STACK_POINTER (p, SIZE (M_NUMBER));
  MOID_T *mode = (MOID_T *) (VALUE ((A68_UNION *) STACK_TOP));
  ADDR_T pop_sp = A68_SP;
  if (mode == M_REAL) {
    REAL_T x = VALUE ((A68_REAL *) (STACK_OFFSET (A68_UNION_SIZE)));
    int before = ABS (VALUE (&width)) - ABS (VALUE (&expo)) - (VALUE (&after) != 0 ? VALUE (&after) + 1 : 0) - 2;
    A68_SP = arg_sp;
    CHECK_REAL (p, x);
    if (SIGN (before) + SIGN (VALUE (&after)) > 0) {
      REAL_T y = ABS (x);
      int q = 0;
      standardise (&y, before, VALUE (&after), &q);
      if (VALUE (&frmt) > 0) {
        while (q % VALUE (&frmt) != 0) {
          y *= 10;
          q--;
          if (VALUE (&after) > 0) {
            VALUE (&after)--;
          }
        }
      } else {
        REAL_T upb = ten_up (-VALUE (&frmt)), lwb = ten_up (-VALUE (&frmt) - 1);
        while (y < lwb) {
          y *= 10;
          q--;
          if (VALUE (&after) > 0) {
            VALUE (&after)--;
          }
        }
        while (y > upb) {
          y /= 10;
          q++;
          if (VALUE (&after) > 0) {
            VALUE (&after)++;
          }
        }
      }
      PUSH_UNION (p, M_REAL);
      PUSH_VALUE (p, SIGN (x) * y, A68_REAL);
      INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_REAL)));
      PUSH_VALUE (p, SIGN (VALUE (&width)) * (ABS (VALUE (&width)) - ABS (VALUE (&expo)) - 1), A68_INT);
      PUSH_VALUE (p, VALUE (&after), A68_INT);
      char *t1 = fixed (p);
      PUSH_UNION (p, M_INT);
      PUSH_VALUE (p, q, A68_INT);
      INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_INT)));
      PUSH_VALUE (p, VALUE (&expo), A68_INT);
      char *t2 = whole (p);
      int strwid = 8 + (int) strlen (t1) + 1 + (int) strlen (t2);
      char *s = stack_string (p, strwid);
      a68_bufcpy (s, t1, strwid);
      (void) string_plusab_char (s, EXPONENT_CHAR, strwid);
      a68_bufcat (s, t2, strwid);
      if (VALUE (&expo) == 0 || strchr (s, ERROR_CHAR) != NO_TEXT) {
        A68_SP = arg_sp;
        PUSH_VALUE (p, VALUE (&width), A68_INT);
        PUSH_VALUE (p, VALUE (&after) != 0 ? VALUE (&after) - 1 : 0, A68_INT);
        PUSH_VALUE (p, VALUE (&expo) > 0 ? VALUE (&expo) + 1 : VALUE (&expo) - 1, A68_INT);
        PUSH_VALUE (p, VALUE (&frmt), A68_INT);
        return real (p);
      } else {
        return s;
      }
    } else {
      char *s = stack_string (p, 8 + ABS (VALUE (&width)));
      return error_chars (s, VALUE (&width));
    }
  }
#if (A68_LEVEL >= 3)
  if (mode == M_LONG_REAL) {
    DOUBLE_T x = VALUE ((A68_LONG_REAL *) (STACK_OFFSET (A68_UNION_SIZE))).f;
    int before = ABS (VALUE (&width)) - ABS (VALUE (&expo)) - (VALUE (&after) != 0 ? VALUE (&after) + 1 : 0) - 2;
    CHECK_DOUBLE_REAL (p, x);
    A68_SP = arg_sp;
    if (SIGN (before) + SIGN (VALUE (&after)) > 0) {
      DOUBLE_T y = (x >= 0.0q ? x : -x);
      int q = 0;
      standardise_double (&y, before, VALUE (&after), &q);
      if (VALUE (&frmt) > 0) {
        while (q % VALUE (&frmt) != 0) {
          y *= 10.0q;
          q--;
          if (VALUE (&after) > 0) {
            VALUE (&after)--;
          }
        }
      } else {
        DOUBLE_T upb = ten_up_double (-VALUE (&frmt)), lwb = ten_up_double (-VALUE (&frmt) - 1);
        while (y < lwb) {
          y *= 10.0q;
          q--;
          if (VALUE (&after) > 0) {
            VALUE (&after)--;
          }
        }
        while (y > upb) {
          y /= 10.0q;
          q++;
          if (VALUE (&after) > 0) {
            VALUE (&after)++;
          }
        }
      }
      PUSH_UNION (p, M_LONG_REAL);
      {
        DOUBLE_NUM_T d;
        d.f = (x >= 0.0q ? y : -y);
        PUSH_VALUE (p, d, A68_LONG_REAL);
      }
      INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_LONG_REAL)));
      PUSH_VALUE (p, SIGN (VALUE (&width)) * (ABS (VALUE (&width)) - ABS (VALUE (&expo)) - 1), A68_INT);
      PUSH_VALUE (p, VALUE (&after), A68_INT);
      char *t1 = fixed (p);
      PUSH_UNION (p, M_INT);
      PUSH_VALUE (p, q, A68_INT);
      INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_INT)));
      PUSH_VALUE (p, VALUE (&expo), A68_INT);
      char *t2 = whole (p);
      int strwid = 8 + (int) strlen (t1) + 1 + (int) strlen (t2);
      char *s = stack_string (p, strwid);
      a68_bufcpy (s, t1, strwid);
      (void) string_plusab_char (s, EXPONENT_CHAR, strwid);
      a68_bufcat (s, t2, strwid);
      if (VALUE (&expo) == 0 || strchr (s, ERROR_CHAR) != NO_TEXT) {
        A68_SP = arg_sp;
        PUSH_VALUE (p, VALUE (&width), A68_INT);
        PUSH_VALUE (p, VALUE (&after) != 0 ? VALUE (&after) - 1 : 0, A68_INT);
        PUSH_VALUE (p, VALUE (&expo) > 0 ? VALUE (&expo) + 1 : VALUE (&expo) - 1, A68_INT);
        PUSH_VALUE (p, VALUE (&frmt), A68_INT);
        return real (p);
      } else {
        return s;
      }
    } else {
      char *s = stack_string (p, 8 + ABS (VALUE (&width)));
      return error_chars (s, VALUE (&width));
    }
  }
#endif
  if (mode == M_LONG_REAL || mode == M_LONG_LONG_REAL) {
    int digits = DIGITS (mode);
    int before;
    MP_T *x = (MP_T *) (STACK_OFFSET (A68_UNION_SIZE));
    CHECK_LONG_REAL (p, x, mode);
    BOOL_T ltz = (BOOL_T) (MP_DIGIT (x, 1) < 0);
    A68_SP = arg_sp;
    MP_DIGIT (x, 1) = ABS (MP_DIGIT (x, 1));
    before = ABS (VALUE (&width)) - ABS (VALUE (&expo)) - (VALUE (&after) != 0 ? VALUE (&after) + 1 : 0) - 2;
    if (SIGN (before) + SIGN (VALUE (&after)) > 0) {
      int q = 0;
      size_t N_mp = SIZE_MP (digits);
      MP_T *z = nil_mp (p, digits);
      (void) move_mp (z, x, digits);
      long_standardise (p, z, digits, before, VALUE (&after), &q);
      if (VALUE (&frmt) > 0) {
        while (q % VALUE (&frmt) != 0) {
          (void) mul_mp_digit (p, z, z, (MP_T) 10, digits);
          q--;
          if (VALUE (&after) > 0) {
            VALUE (&after)--;
          }
        }
      } else {
        ADDR_T sp1 = A68_SP;
        MP_T *dif = nil_mp (p, digits);
        MP_T *lim = nil_mp (p, digits);
        (void) ten_up_mp (p, lim, -VALUE (&frmt) - 1, digits);
        (void) sub_mp (p, dif, z, lim, digits);
        while (MP_DIGIT (dif, 1) < 0) {
          (void) mul_mp_digit (p, z, z, (MP_T) 10, digits);
          q--;
          if (VALUE (&after) > 0) {
            VALUE (&after)--;
          }
          (void) sub_mp (p, dif, z, lim, digits);
        }
        (void) mul_mp_digit (p, lim, lim, (MP_T) 10, digits);
        (void) sub_mp (p, dif, z, lim, digits);
        while (MP_DIGIT (dif, 1) > 0) {
          (void) div_mp_digit (p, z, z, (MP_T) 10, digits);
          q++;
          if (VALUE (&after) > 0) {
            VALUE (&after)++;
          }
          (void) sub_mp (p, dif, z, lim, digits);
        }
        A68_SP = sp1;
      }
      PUSH_UNION (p, mode);
      MP_DIGIT (z, 1) = (ltz ? -MP_DIGIT (z, 1) : MP_DIGIT (z, 1));
      PUSH (p, z, N_mp);
      INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE_MP (digits)));
      PUSH_VALUE (p, SIGN (VALUE (&width)) * (ABS (VALUE (&width)) - ABS (VALUE (&expo)) - 1), A68_INT);
      PUSH_VALUE (p, VALUE (&after), A68_INT);
      char *t1 = fixed (p);
      PUSH_UNION (p, M_INT);
      PUSH_VALUE (p, q, A68_INT);
      INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_INT)));
      PUSH_VALUE (p, VALUE (&expo), A68_INT);
      char *t2 = whole (p);
      int strwid = 8 + (int) strlen (t1) + 1 + (int) strlen (t2);
      char *s = stack_string (p, strwid);
      a68_bufcpy (s, t1, strwid);
      (void) string_plusab_char (s, EXPONENT_CHAR, strwid);
      a68_bufcat (s, t2, strwid);
      if (VALUE (&expo) == 0 || strchr (s, ERROR_CHAR) != NO_TEXT) {
        A68_SP = arg_sp;
        PUSH_VALUE (p, VALUE (&width), A68_INT);
        PUSH_VALUE (p, VALUE (&after) != 0 ? VALUE (&after) - 1 : 0, A68_INT);
        PUSH_VALUE (p, VALUE (&expo) > 0 ? VALUE (&expo) + 1 : VALUE (&expo) - 1, A68_INT);
        PUSH_VALUE (p, VALUE (&frmt), A68_INT);
        return real (p);
      } else {
        return s;
      }
    } else {
      char *s = stack_string (p, 8 + ABS (VALUE (&width)));
      return error_chars (s, VALUE (&width));
    }
  }
  if (mode == M_INT) {
    int x = VALUE ((A68_INT *) (STACK_OFFSET (A68_UNION_SIZE)));
    PUSH_UNION (p, M_REAL);
    PUSH_VALUE (p, (REAL_T) x, A68_REAL);
    INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER) - (A68_UNION_SIZE + SIZE (M_REAL)));
    PUSH_VALUE (p, VALUE (&width), A68_INT);
    PUSH_VALUE (p, VALUE (&after), A68_INT);
    PUSH_VALUE (p, VALUE (&expo), A68_INT);
    PUSH_VALUE (p, VALUE (&frmt), A68_INT);
    return real (p);
  }
  if (mode == M_LONG_INT || mode == M_LONG_LONG_INT) {
    A68_SP = pop_sp;
    if (mode == M_LONG_INT) {
      VALUE ((A68_UNION *) STACK_TOP) = (void *) M_LONG_REAL;
    } else {
      VALUE ((A68_UNION *) STACK_TOP) = (void *) M_LONG_LONG_REAL;
    } INCREMENT_STACK_POINTER (p, SIZE (M_NUMBER));
    PUSH_VALUE (p, VALUE (&width), A68_INT);
    PUSH_VALUE (p, VALUE (&after), A68_INT);
    PUSH_VALUE (p, VALUE (&expo), A68_INT);
    PUSH_VALUE (p, VALUE (&frmt), A68_INT);
    return real (p);
  }
  return NO_TEXT;
}

//! @brief PROC (NUMBER, INT) STRING whole

void genie_whole (NODE_T * p)
{
  ADDR_T pop_sp = A68_SP;
  char *str = whole (p);
  A68_SP = pop_sp - SIZE (M_INT) - SIZE (M_NUMBER);
  A68_REF ref = tmp_to_a68_string (p, str);
  PUSH_REF (p, ref);
}

//! @brief PROC (NUMBER, INT, INT) STRING bits 

void genie_bits (NODE_T * p)
{
  ADDR_T pop_sp = A68_SP;
  char *str = bits_to_string (p);
  A68_SP = pop_sp - 2 * SIZE (M_INT) - SIZE (M_HEX_NUMBER);
  A68_REF ref = tmp_to_a68_string (p, str);
  PUSH_REF (p, ref);
}

//! @brief PROC (NUMBER, INT, INT) STRING fixed

void genie_fixed (NODE_T * p)
{
  ADDR_T pop_sp = A68_SP;
  char *str = fixed (p);
  A68_SP = pop_sp - 2 * SIZE (M_INT) - SIZE (M_NUMBER);
  A68_REF ref = tmp_to_a68_string (p, str);
  PUSH_REF (p, ref);
}

//! @brief PROC (NUMBER, INT, INT, INT) STRING eng

void genie_real (NODE_T * p)
{
  ADDR_T pop_sp = A68_SP;
  char *str = real (p);
  A68_SP = pop_sp - 4 * SIZE (M_INT) - SIZE (M_NUMBER);
  A68_REF ref = tmp_to_a68_string (p, str);
  PUSH_REF (p, ref);
}

//! @brief PROC (NUMBER, INT, INT, INT) STRING float

void genie_float (NODE_T * p)
{
  PUSH_VALUE (p, 1, A68_INT);
  genie_real (p);
}

// ALGOL68C routines.

//! @def A68C_TRANSPUT
//! @brief Generate Algol68C routines readint, getint, etcetera.

#define A68C_TRANSPUT(n, m)\
 void genie_get_##n (NODE_T * p)\
  {\
    A68_REF ref_file;\
    POP_REF (p, &ref_file);\
    CHECK_REF (p, ref_file, M_REF_FILE);\
    BYTE_T *z = STACK_TOP;\
    INCREMENT_STACK_POINTER (p, SIZE (MODE (m)));\
    ADDR_T pop_sp = A68_SP;\
    open_for_reading (p, ref_file);\
    genie_read_standard (p, MODE (m), z, ref_file);\
    A68_SP = pop_sp;\
  }\
  void genie_put_##n (NODE_T * p)\
  {\
    int size = SIZE (MODE (m)), sizf = SIZE (M_REF_FILE);\
    A68_REF ref_file = * (A68_REF *) STACK_OFFSET (- (size + sizf));\
    CHECK_REF (p, ref_file, M_REF_FILE);\
    reset_transput_buffer (UNFORMATTED_BUFFER);\
    open_for_writing (p, ref_file);\
    genie_write_standard (p, MODE (m), STACK_OFFSET (-size), ref_file);\
    write_purge_buffer (p, ref_file, UNFORMATTED_BUFFER);\
    DECREMENT_STACK_POINTER (p, size + sizf);\
  }\
  void genie_read_##n (NODE_T * p)\
  {\
    BYTE_T *z = STACK_TOP;\
    INCREMENT_STACK_POINTER (p, SIZE (MODE (m)));\
    ADDR_T pop_sp = A68_SP;\
    open_for_reading (p, A68 (stand_in));\
    genie_read_standard (p, MODE (m), z, A68 (stand_in));\
    A68_SP = pop_sp;\
  }\
  void genie_print_##n (NODE_T * p)\
  {\
    int size = SIZE (MODE (m));\
    reset_transput_buffer (UNFORMATTED_BUFFER);\
    open_for_writing (p, A68 (stand_out));\
    genie_write_standard (p, MODE (m), STACK_OFFSET (-size), A68 (stand_out));\
    write_purge_buffer (p, A68 (stand_out), UNFORMATTED_BUFFER);\
    DECREMENT_STACK_POINTER (p, size);\
  }

A68C_TRANSPUT (int, INT);
A68C_TRANSPUT (long_int, LONG_INT);
A68C_TRANSPUT (long_mp_int, LONG_LONG_INT);
A68C_TRANSPUT (real, REAL);
A68C_TRANSPUT (long_real, LONG_REAL);
A68C_TRANSPUT (long_mp_real, LONG_LONG_REAL);
A68C_TRANSPUT (bits, BITS);
A68C_TRANSPUT (long_bits, LONG_BITS);
A68C_TRANSPUT (long_mp_bits, LONG_LONG_BITS);
A68C_TRANSPUT (bool, BOOL);
A68C_TRANSPUT (char, CHAR);
A68C_TRANSPUT (string, STRING);

#undef A68C_TRANSPUT

#define A68C_TRANSPUT(n, s, m)\
 void genie_get_##n (NODE_T * p) {\
    A68_REF ref_file;\
    POP_REF (p, &ref_file);\
    CHECK_REF (p, ref_file, M_REF_FILE);\
    PUSH_REF (p, ref_file);\
    genie_get_##s (p);\
    PUSH_REF (p, ref_file);\
    genie_get_##s (p);\
  }\
  void genie_put_##n (NODE_T * p) {\
    int size = SIZE (MODE (m)), sizf = SIZE (M_REF_FILE);\
    A68_REF ref_file = * (A68_REF *) STACK_OFFSET (- (size + sizf));\
    CHECK_REF (p, ref_file, M_REF_FILE);\
    reset_transput_buffer (UNFORMATTED_BUFFER);\
    open_for_writing (p, ref_file);\
    genie_write_standard (p, MODE (m), STACK_OFFSET (-size), ref_file);\
    write_purge_buffer (p, ref_file, UNFORMATTED_BUFFER);\
    DECREMENT_STACK_POINTER (p, size + sizf);\
  }\
  void genie_read_##n (NODE_T * p) {\
    genie_read_##s (p);\
    genie_read_##s (p);\
  }\
  void genie_print_##n (NODE_T * p) {\
    int size = SIZE (MODE (m));\
    reset_transput_buffer (UNFORMATTED_BUFFER);\
    open_for_writing (p, A68 (stand_out));\
    genie_write_standard (p, MODE (m), STACK_OFFSET (-size), A68 (stand_out));\
    write_purge_buffer (p, A68 (stand_out), UNFORMATTED_BUFFER);\
    DECREMENT_STACK_POINTER (p, size);\
  }

A68C_TRANSPUT (complex, real, COMPLEX);
A68C_TRANSPUT (mp_complex, long_real, LONG_COMPLEX);
A68C_TRANSPUT (long_mp_complex, long_mp_real, LONG_LONG_COMPLEX);

#undef A68C_TRANSPUT

//! @brief PROC STRING read line

void genie_read_line (NODE_T * p)
{
#if defined (HAVE_READLINE)
  char *line = readline ("");
  if (line != NO_TEXT && (int) strlen (line) > 0) {
    add_history (line);
  }
  PUSH_REF (p, c_to_a_string (p, line, DEFAULT_WIDTH));
  a68_free (line);
#else
  genie_read_string (p);
  genie_stand_in (p);
  genie_new_line (p);
#endif
}
