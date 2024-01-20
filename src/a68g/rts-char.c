//! @file rts-char.c
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
//! CHAR, STRING and BYTES routines.

#include "a68g.h"
#include "a68g-genie.h"
#include "a68g-prelude.h"
#include "a68g-transput.h"

// OP (CHAR, CHAR) BOOL.

#define A68_CMP_CHAR(n, OP)\
void n (NODE_T * p) {\
  A68_CHAR i, j;\
  POP_OBJECT (p, &j, A68_CHAR);\
  POP_OBJECT (p, &i, A68_CHAR);\
  PUSH_VALUE (p, (BOOL_T) (TO_UCHAR (VALUE (&i)) OP TO_UCHAR (VALUE (&j))), A68_BOOL);\
  }

A68_CMP_CHAR (genie_eq_char, ==);
A68_CMP_CHAR (genie_ne_char, !=);
A68_CMP_CHAR (genie_lt_char, <);
A68_CMP_CHAR (genie_gt_char, >);
A68_CMP_CHAR (genie_le_char, <=);
A68_CMP_CHAR (genie_ge_char, >=);

//! @brief OP ABS = (CHAR) INT

void genie_abs_char (NODE_T * p)
{
  A68_CHAR i;
  POP_OBJECT (p, &i, A68_CHAR);
  PUSH_VALUE (p, TO_UCHAR (VALUE (&i)), A68_INT);
}

//! @brief OP REPR = (INT) CHAR

void genie_repr_char (NODE_T * p)
{
  A68_INT k;
  POP_OBJECT (p, &k, A68_INT);
  PRELUDE_ERROR (VALUE (&k) < 0 || VALUE (&k) > (int) UCHAR_MAX, p, ERROR_OUT_OF_BOUNDS, M_CHAR);
  PUSH_VALUE (p, (char) (VALUE (&k)), A68_CHAR);
}

// OP (CHAR) BOOL.

#define A68_CHAR_BOOL(n, OP)\
void n (NODE_T * p) {\
  A68_CHAR ch;\
  POP_OBJECT (p, &ch, A68_CHAR);\
  PUSH_VALUE (p, (BOOL_T) (OP (VALUE (&ch)) == 0 ? A68_FALSE : A68_TRUE), A68_BOOL);\
  }

A68_CHAR_BOOL (genie_is_alnum, IS_ALNUM);
A68_CHAR_BOOL (genie_is_alpha, IS_ALPHA);
A68_CHAR_BOOL (genie_is_cntrl, IS_CNTRL);
A68_CHAR_BOOL (genie_is_digit, IS_DIGIT);
A68_CHAR_BOOL (genie_is_graph, IS_GRAPH);
A68_CHAR_BOOL (genie_is_lower, IS_LOWER);
A68_CHAR_BOOL (genie_is_print, IS_PRINT);
A68_CHAR_BOOL (genie_is_punct, IS_PUNCT);
A68_CHAR_BOOL (genie_is_space, IS_SPACE);
A68_CHAR_BOOL (genie_is_upper, IS_UPPER);
A68_CHAR_BOOL (genie_is_xdigit, IS_XDIGIT);

#define A68_CHAR_CHAR(n, OP)\
void n (NODE_T * p) {\
  A68_CHAR *ch;\
  POP_OPERAND_ADDRESS (p, ch, A68_CHAR);\
  VALUE (ch) = (char) (OP (TO_UCHAR (VALUE (ch))));\
}

A68_CHAR_CHAR (genie_to_lower, TO_LOWER);
A68_CHAR_CHAR (genie_to_upper, TO_UPPER);

//! @brief OP + = (CHAR, CHAR) STRING

void genie_add_char (NODE_T * p)
{
// Right part.
  A68_CHAR b;
  POP_OBJECT (p, &b, A68_CHAR);
  CHECK_INIT (p, INITIALISED (&b), M_CHAR);
// Left part.
  A68_CHAR a;
  POP_OBJECT (p, &a, A68_CHAR);
  CHECK_INIT (p, INITIALISED (&a), M_CHAR);
// Sum.
  A68_REF c = heap_generator (p, M_STRING, DESCRIPTOR_SIZE (1));
  A68_REF d = heap_generator_2 (p, M_STRING, 2, SIZE (M_CHAR));
  A68_ARRAY *arr_3; A68_TUPLE *tup_3; BYTE_T *str_3;
  GET_DESCRIPTOR (arr_3, tup_3, &c);
  DIM (arr_3) = 1;
  MOID (arr_3) = M_CHAR;
  ELEM_SIZE (arr_3) = SIZE (M_CHAR);
  SLICE_OFFSET (arr_3) = 0;
  FIELD_OFFSET (arr_3) = 0;
  ARRAY (arr_3) = d;
  LWB (tup_3) = 1;
  UPB (tup_3) = 2;
  SHIFT (tup_3) = LWB (tup_3);
  SPAN (tup_3) = 1;
// Add chars.
  str_3 = DEREF (BYTE_T, &ARRAY (arr_3));
  MOVE ((BYTE_T *) & str_3[0], (BYTE_T *) & a, SIZE (M_CHAR));
  MOVE ((BYTE_T *) & str_3[SIZE (M_CHAR)], (BYTE_T *) & b, SIZE (M_CHAR));
  PUSH_REF (p, c);
}

//! @brief OP ELEM = (INT, STRING) CHAR # ALGOL68C #

void genie_elem_string (NODE_T * p)
{
  A68_REF z;
  POP_REF (p, &z);
  CHECK_REF (p, z, M_STRING);
  A68_INT k;
  POP_OBJECT (p, &k, A68_INT);
  A68_ARRAY *arr; A68_TUPLE *tup;
  GET_DESCRIPTOR (arr, tup, &z);
  PRELUDE_ERROR (VALUE (&k) < LWB (tup), p, ERROR_INDEX_OUT_OF_BOUNDS, NO_TEXT);
  PRELUDE_ERROR (VALUE (&k) > UPB (tup), p, ERROR_INDEX_OUT_OF_BOUNDS, NO_TEXT);
  BYTE_T *str = DEREF (BYTE_T, &(ARRAY (arr)));
  A68_CHAR *ch = (A68_CHAR *) & (str[INDEX_1_DIM (arr, tup, VALUE (&k))]);
  PUSH_VALUE (p, VALUE (ch), A68_CHAR);
}

//! @brief OP + = (STRING, STRING) STRING

void genie_add_string (NODE_T * p)
{
// Right part.
  A68_REF b;
  POP_REF (p, &b);
  CHECK_INIT (p, INITIALISED (&b), M_STRING);
  A68_ARRAY *arr_2; A68_TUPLE *tup_2;
  GET_DESCRIPTOR (arr_2, tup_2, &b);
  int l_2 = ROW_SIZE (tup_2);
// Left part.
  A68_REF a;
  POP_REF (p, &a);
  CHECK_REF (p, a, M_STRING);
  A68_ARRAY *arr_1; A68_TUPLE *tup_1;
  GET_DESCRIPTOR (arr_1, tup_1, &a);
  int l_1 = ROW_SIZE (tup_1);
// Sum.
  A68_REF ref_str = heap_generator (p, M_STRING, DESCRIPTOR_SIZE (1));
  A68_REF new_str = heap_generator_2 (p, M_STRING, l_1 + l_2, SIZE (M_CHAR));
// Calculate again since garbage collector might have moved data.
  GET_DESCRIPTOR (arr_1, tup_1, &a);
  GET_DESCRIPTOR (arr_2, tup_2, &b);
  A68_ARRAY *arr_3; A68_TUPLE *tup_3;
  GET_DESCRIPTOR (arr_3, tup_3, &ref_str);
  DIM (arr_3) = 1;
  MOID (arr_3) = M_CHAR;
  ELEM_SIZE (arr_3) = SIZE (M_CHAR);
  SLICE_OFFSET (arr_3) = 0;
  FIELD_OFFSET (arr_3) = 0;
  ARRAY (arr_3) = new_str;
  LWB (tup_3) = 1;
  UPB (tup_3) = l_1 + l_2;
  SHIFT (tup_3) = LWB (tup_3);
  SPAN (tup_3) = 1;
// Add strings.
  BYTE_T *str_3 = DEREF (BYTE_T, &ARRAY (arr_3));
  int m = 0;
  if (ROW_SIZE (tup_1) > 0) {
    BYTE_T *str_1 = DEREF (BYTE_T, &ARRAY (arr_1));
    for (int k = LWB (tup_1); k <= UPB (tup_1); k++) {
      MOVE ((BYTE_T *) & str_3[m], (BYTE_T *) & str_1[INDEX_1_DIM (arr_1, tup_1, k)], SIZE (M_CHAR));
      m += SIZE (M_CHAR);
    }
  }
  if (ROW_SIZE (tup_2) > 0) {
    BYTE_T *str_2 = DEREF (BYTE_T, &ARRAY (arr_2));
    for (int k = LWB (tup_2); k <= UPB (tup_2); k++) {
      MOVE ((BYTE_T *) & str_3[m], (BYTE_T *) & str_2[INDEX_1_DIM (arr_2, tup_2, k)], SIZE (M_CHAR));
      m += SIZE (M_CHAR);
    }
  }
  PUSH_REF (p, ref_str);
}

//! @brief OP * = (INT, STRING) STRING

void genie_times_int_string (NODE_T * p)
{
  A68_REF a;
  POP_REF (p, &a);
  A68_INT k;
  POP_OBJECT (p, &k, A68_INT);
  PRELUDE_ERROR (VALUE (&k) < 0, p, ERROR_INVALID_ARGUMENT, M_INT);
  CHECK_INT_SHORTEN (p, VALUE (&k));
  PUSH_REF (p, empty_string (p));
  while (VALUE (&k)--) {
    PUSH_REF (p, a);
    genie_add_string (p);
  }
}

//! @brief OP * = (STRING, INT) STRING

void genie_times_string_int (NODE_T * p)
{
  A68_INT k;
  POP_OBJECT (p, &k, A68_INT);
  A68_REF a;
  POP_REF (p, &a);
  PUSH_VALUE (p, VALUE (&k), A68_INT);
  PUSH_REF (p, a);
  genie_times_int_string (p);
}

//! @brief OP * = (INT, CHAR) STRING

void genie_times_int_char (NODE_T * p)
{
// Pop operands.
  A68_CHAR a;
  POP_OBJECT (p, &a, A68_CHAR);
  A68_INT str_size;
  POP_OBJECT (p, &str_size, A68_INT);
  PRELUDE_ERROR (VALUE (&str_size) < 0, p, ERROR_INVALID_ARGUMENT, M_INT);
  CHECK_INT_SHORTEN (p, VALUE (&str_size));
// Make new string.
  A68_REF z, row; A68_ARRAY arr; A68_TUPLE tup;
  NEW_ROW_1D (z, row, arr, tup, M_ROW_CHAR, M_CHAR, (int) (VALUE (&str_size)));
  BYTE_T *str = ADDRESS (&row);
  for (int k = 0; k < VALUE (&str_size); k++) {
    A68_CHAR ch;
    STATUS (&ch) = INIT_MASK;
    VALUE (&ch) = VALUE (&a);
    *(A68_CHAR *) & str[k * SIZE (M_CHAR)] = ch;
  }
  PUSH_REF (p, z);
}

//! @brief OP * = (CHAR, INT) STRING

void genie_times_char_int (NODE_T * p)
{
  A68_INT k;
  POP_OBJECT (p, &k, A68_INT);
  A68_CHAR a;
  POP_OBJECT (p, &a, A68_CHAR);
  PUSH_VALUE (p, VALUE (&k), A68_INT);
  PUSH_VALUE (p, VALUE (&a), A68_CHAR);
  genie_times_int_char (p);
}

//! @brief OP +:= = (REF STRING, STRING) REF STRING

void genie_plusab_string (NODE_T * p)
{
  genie_f_and_becomes (p, M_REF_STRING, genie_add_string);
}

//! @brief OP +=: = (STRING, REF STRING) REF STRING

void genie_plusto_string (NODE_T * p)
{
  A68_REF refa;
  POP_REF (p, &refa);
  CHECK_REF (p, refa, M_REF_STRING);
  A68_REF a = *DEREF (A68_REF, &refa);
  CHECK_INIT (p, INITIALISED (&a), M_STRING);
  A68_REF refb;
  POP_REF (p, &refb);
  PUSH_REF (p, refb);
  PUSH_REF (p, a);
  genie_add_string (p);
  POP_REF (p, DEREF (A68_REF, &refa));
  PUSH_REF (p, refa);
}

//! @brief OP *:= = (REF STRING, INT) REF STRING

void genie_timesab_string (NODE_T * p)
{
  A68_INT k;
  POP_OBJECT (p, &k, A68_INT);
  PRELUDE_ERROR (VALUE (&k) < 0, p, ERROR_INVALID_ARGUMENT, M_INT);
  A68_REF ref;
  POP_REF (p, &ref);
  CHECK_REF (p, ref, M_REF_STRING);
  A68_REF a = *DEREF (A68_REF, &ref);
  CHECK_INIT (p, INITIALISED (&a), M_STRING);
// Multiplication as repeated addition.
  PUSH_REF (p, empty_string (p));
  for (int i = 1; i <= VALUE (&k); i++) {
    PUSH_REF (p, a);
    genie_add_string (p);
  }
// The stack contains a STRING, promote to REF STRING.
  POP_REF (p, DEREF (A68_REF, &ref));
  PUSH_REF (p, ref);
}

//! @brief Difference between two STRINGs in the stack.

int string_difference (NODE_T * p)
{
// Pop operands.
  A68_REF row2;
  POP_REF (p, &row2);
  CHECK_INIT (p, INITIALISED (&row2), M_STRING);
  A68_ARRAY *arr_2; A68_TUPLE *tup_2;
  GET_DESCRIPTOR (arr_2, tup_2, &row2);
  int len_2 = ROW_SIZE (tup_2);
  A68_REF row1;
  POP_REF (p, &row1);
  CHECK_INIT (p, INITIALISED (&row1), M_STRING);
  A68_ARRAY *arr_1; A68_TUPLE *tup_1;
  GET_DESCRIPTOR (arr_1, tup_1, &row1);
  int len_1 = ROW_SIZE (tup_1);
// Compute string difference.
  int size = (len_1 > len_2 ? len_1 : len_2), diff = 0;
  BYTE_T *str_1 = (len_1 > 0 ? DEREF (BYTE_T, &ARRAY (arr_1)) : NO_BYTE);
  BYTE_T *str_2 = (len_2 > 0 ? DEREF (BYTE_T, &ARRAY (arr_2)) : NO_BYTE);
  for (int k = 0; k < size && diff == 0; k++) {
    int a = 0, b = 0;
    if (len_1 > 0 && k < len_1) {
      A68_CHAR *ch = (A68_CHAR *) & str_1[INDEX_1_DIM (arr_1, tup_1, LWB (tup_1) + k)];
      a = (int) VALUE (ch);
    } else {
      a = 0;
    }
    if (len_2 > 0 && k < len_2) {
      A68_CHAR *ch = (A68_CHAR *) & str_2[INDEX_1_DIM (arr_2, tup_2, LWB (tup_2) + k)];
      b = (int) VALUE (ch);
    } else {
      b = 0;
    }
    diff += (TO_UCHAR (a) - TO_UCHAR (b));
  }
  return diff;
}

// OP (STRING, STRING) BOOL.

#define A68_CMP_STRING(n, OP)\
void n (NODE_T * p) {\
  int k = string_difference (p);\
  PUSH_VALUE (p, (BOOL_T) (k OP 0), A68_BOOL);\
}

A68_CMP_STRING (genie_eq_string, ==);
A68_CMP_STRING (genie_ne_string, !=);
A68_CMP_STRING (genie_lt_string, <);
A68_CMP_STRING (genie_gt_string, >);
A68_CMP_STRING (genie_le_string, <=);
A68_CMP_STRING (genie_ge_string, >=);

// BYTES operations.

//! @brief OP ELEM = (INT, BYTES) CHAR

void genie_elem_bytes (NODE_T * p)
{
  A68_BYTES j;
  POP_OBJECT (p, &j, A68_BYTES);
  A68_INT i;
  POP_OBJECT (p, &i, A68_INT);
  PRELUDE_ERROR (VALUE (&i) < 1 || VALUE (&i) > A68_BYTES_WIDTH, p, ERROR_OUT_OF_BOUNDS, M_INT);
  if (VALUE (&i) > (int) strlen (VALUE (&j))) {
    genie_null_char (p);
  } else {
    PUSH_VALUE (p, VALUE (&j)[VALUE (&i) - 1], A68_CHAR);
  }
}

//! @brief PROC bytes pack = (STRING) BYTES

void genie_bytespack (NODE_T * p)
{
  A68_REF z;
  POP_REF (p, &z);
  CHECK_REF (p, z, M_STRING);
  PRELUDE_ERROR (a68_string_size (p, z) > A68_BYTES_WIDTH, p, ERROR_OUT_OF_BOUNDS, M_STRING);
  A68_BYTES b;
  STATUS (&b) = INIT_MASK;
  ASSERT (a_to_c_string (p, VALUE (&b), z) != NO_TEXT);
  PUSH_BYTES (p, VALUE (&b));
}

//! @brief PROC bytes pack = (STRING) BYTES

void genie_add_bytes (NODE_T * p)
{
  A68_BYTES *i, *j;
  POP_OPERAND_ADDRESSES (p, i, j, A68_BYTES);
  PRELUDE_ERROR (((int) strlen (VALUE (i)) + (int) strlen (VALUE (j))) > A68_BYTES_WIDTH, p, ERROR_OUT_OF_BOUNDS, M_BYTES);
  a68_bufcat (VALUE (i), VALUE (j), A68_BYTES_WIDTH);
}

//! @brief OP +:= = (REF BYTES, BYTES) REF BYTES

void genie_plusab_bytes (NODE_T * p)
{
  genie_f_and_becomes (p, M_REF_BYTES, genie_add_bytes);
}

//! @brief OP +=: = (BYTES, REF BYTES) REF BYTES

void genie_plusto_bytes (NODE_T * p)
{
  A68_REF z;
  POP_REF (p, &z);
  CHECK_REF (p, z, M_REF_BYTES);
  A68_BYTES *address = DEREF (A68_BYTES, &z);
  CHECK_INIT (p, INITIALISED (address), M_BYTES);
  A68_BYTES i;
  POP_OBJECT (p, &i, A68_BYTES);
  PRELUDE_ERROR (((int) strlen (VALUE (address)) + (int) strlen (VALUE (&i))) > A68_BYTES_WIDTH, p, ERROR_OUT_OF_BOUNDS, M_BYTES);
  A68_BYTES j;
  a68_bufcpy (VALUE (&j), VALUE (&i), A68_BYTES_WIDTH);
  a68_bufcat (VALUE (&j), VALUE (address), A68_BYTES_WIDTH);
  a68_bufcpy (VALUE (address), VALUE (&j), A68_BYTES_WIDTH);
  PUSH_REF (p, z);
}

//! @brief Difference between BYTE strings.

int compare_bytes (NODE_T * p)
{
  A68_BYTES y;
  POP_OBJECT (p, &y, A68_BYTES);
  A68_BYTES x;
  POP_OBJECT (p, &x, A68_BYTES);
  return strcmp (VALUE (&x), VALUE (&y));
}

// OP (BYTES, BYTES) BOOL.

#define A68_CMP_BYTES(n, OP)\
void n (NODE_T * p) {\
  int k = compare_bytes (p);\
  PUSH_VALUE (p, (BOOL_T) (k OP 0), A68_BOOL);\
}

A68_CMP_BYTES (genie_eq_bytes, ==);
A68_CMP_BYTES (genie_ne_bytes, !=);
A68_CMP_BYTES (genie_lt_bytes, <);
A68_CMP_BYTES (genie_gt_bytes, >);
A68_CMP_BYTES (genie_le_bytes, <=);
A68_CMP_BYTES (genie_ge_bytes, >=);

//! @brief OP LENG = (BYTES) LONG BYTES

void genie_leng_bytes (NODE_T * p)
{
  A68_LONG_BYTES a;
  a68_bufset (VALUE (&a), 0, sizeof (VALUE (&a)));
  POP_OBJECT (p, (A68_BYTES *) &a, A68_BYTES);
  PUSH_LONG_BYTES (p, VALUE (&a));
}

//! @brief OP SHORTEN = (LONG BYTES) BYTES

void genie_shorten_bytes (NODE_T * p)
{
  A68_LONG_BYTES a;
  POP_OBJECT (p, &a, A68_LONG_BYTES);
  PRELUDE_ERROR (strlen (VALUE (&a)) >= A68_BYTES_WIDTH, p, ERROR_OUT_OF_BOUNDS, M_BYTES);
  PUSH_BYTES (p, VALUE (&a));
}

//! @brief OP ELEM = (INT, LONG BYTES) CHAR

void genie_elem_long_bytes (NODE_T * p)
{
  A68_LONG_BYTES j;
  POP_OBJECT (p, &j, A68_LONG_BYTES);
  A68_INT i;
  POP_OBJECT (p, &i, A68_INT);
  PRELUDE_ERROR (VALUE (&i) < 1 || VALUE (&i) > A68_LONG_BYTES_WIDTH, p, ERROR_OUT_OF_BOUNDS, M_INT);
  if (VALUE (&i) > (int) strlen (VALUE (&j))) {
    genie_null_char (p);
  } else {
    PUSH_VALUE (p, VALUE (&j)[VALUE (&i) - 1], A68_CHAR);
  }
}

//! @brief PROC long bytes pack = (STRING) LONG BYTES

void genie_long_bytespack (NODE_T * p)
{
  A68_REF z;
  POP_REF (p, &z);
  CHECK_REF (p, z, M_STRING);
  PRELUDE_ERROR (a68_string_size (p, z) > A68_LONG_BYTES_WIDTH, p, ERROR_OUT_OF_BOUNDS, M_STRING);
  A68_LONG_BYTES b;
  STATUS (&b) = INIT_MASK;
  ASSERT (a_to_c_string (p, VALUE (&b), z) != NO_TEXT);
  PUSH_LONG_BYTES (p, VALUE (&b));
}

//! @brief OP + = (LONG BYTES, LONG BYTES) LONG BYTES

void genie_add_long_bytes (NODE_T * p)
{
  A68_LONG_BYTES *i, *j;
  POP_OPERAND_ADDRESSES (p, i, j, A68_LONG_BYTES);
  PRELUDE_ERROR (((int) strlen (VALUE (i)) + (int) strlen (VALUE (j))) > A68_LONG_BYTES_WIDTH, p, ERROR_OUT_OF_BOUNDS, M_LONG_BYTES);
  a68_bufcat (VALUE (i), VALUE (j), A68_LONG_BYTES_WIDTH);
}

//! @brief OP +:= = (REF LONG BYTES, LONG BYTES) REF LONG BYTES

void genie_plusab_long_bytes (NODE_T * p)
{
  genie_f_and_becomes (p, M_REF_LONG_BYTES, genie_add_long_bytes);
}

//! @brief OP +=: = (LONG BYTES, REF LONG BYTES) REF LONG BYTES

void genie_plusto_long_bytes (NODE_T * p)
{
  A68_REF z;
  POP_REF (p, &z);
  CHECK_REF (p, z, M_REF_LONG_BYTES);
  A68_LONG_BYTES *address = DEREF (A68_LONG_BYTES, &z);
  CHECK_INIT (p, INITIALISED (address), M_LONG_BYTES);
  A68_LONG_BYTES i;
  POP_OBJECT (p, &i, A68_LONG_BYTES);
  PRELUDE_ERROR (((int) strlen (VALUE (address)) + (int) strlen (VALUE (&i))) > A68_LONG_BYTES_WIDTH, p, ERROR_OUT_OF_BOUNDS, M_LONG_BYTES);
  A68_LONG_BYTES j;
  a68_bufcpy (VALUE (&j), VALUE (&i), A68_LONG_BYTES_WIDTH);
  a68_bufcat (VALUE (&j), VALUE (address), A68_LONG_BYTES_WIDTH);
  a68_bufcpy (VALUE (address), VALUE (&j), A68_LONG_BYTES_WIDTH);
  PUSH_REF (p, z);
}

//! @brief Difference between LONG BYTE strings.

int compare_long_bytes (NODE_T * p)
{
  A68_LONG_BYTES y;
  POP_OBJECT (p, &y, A68_LONG_BYTES);
  A68_LONG_BYTES x;
  POP_OBJECT (p, &x, A68_LONG_BYTES);
  return strcmp (VALUE (&x), VALUE (&y));
}

// OP (LONG BYTES, LONG BYTES) BOOL.

#define A68_CMP_LONG_BYTES(n, OP)\
  void n (NODE_T * p) {\
    int k = compare_long_bytes (p);\
    PUSH_VALUE (p, (BOOL_T) (k OP 0), A68_BOOL);\
  }

A68_CMP_LONG_BYTES (genie_eq_long_bytes, ==);
A68_CMP_LONG_BYTES (genie_ne_long_bytes, !=);
A68_CMP_LONG_BYTES (genie_lt_long_bytes, <);
A68_CMP_LONG_BYTES (genie_gt_long_bytes, >);
A68_CMP_LONG_BYTES (genie_le_long_bytes, <=);
A68_CMP_LONG_BYTES (genie_ge_long_bytes, >=);

//! @brief PROC char in string = (CHAR, REF INT, STRING) BOOL

void genie_char_in_string (NODE_T * p)
{
  A68_REF ref_str;
  POP_REF (p, &ref_str);
  A68_ROW row = *(A68_REF *) &ref_str;
  CHECK_INIT (p, INITIALISED (&row), M_ROWS);
  A68_ARRAY *arr; A68_TUPLE *tup;
  GET_DESCRIPTOR (arr, tup, &row);
  A68_REF ref_pos;
  POP_REF (p, &ref_pos);
  A68_CHAR c;
  POP_OBJECT (p, &c, A68_CHAR);
  reset_transput_buffer (PATTERN_BUFFER);
  add_a_string_transput_buffer (p, PATTERN_BUFFER, (BYTE_T *) & ref_str);
  int len = get_transput_buffer_index (PATTERN_BUFFER);
  char *q = get_transput_buffer (PATTERN_BUFFER);
  char ch = (char) VALUE (&c);
  for (int k = 0; k < len; k++) {
    if (q[k] == ch) {
      A68_INT pos;
      STATUS (&pos) = INIT_MASK;
      VALUE (&pos) = k + LOWER_BOUND (tup);
      *DEREF (A68_INT, &ref_pos) = pos;
      PUSH_VALUE (p, A68_TRUE, A68_BOOL);
      return;
    }
  }
  PUSH_VALUE (p, A68_FALSE, A68_BOOL);
}

//! @brief PROC last char in string = (CHAR, REF INT, STRING) BOOL

void genie_last_char_in_string (NODE_T * p)
{
  A68_REF ref_str;
  POP_REF (p, &ref_str);
  A68_ROW row = *(A68_REF *) &ref_str;
  CHECK_INIT (p, INITIALISED (&row), M_ROWS);
  A68_ARRAY *arr; A68_TUPLE *tup;
  GET_DESCRIPTOR (arr, tup, &row);
  A68_REF ref_pos;
  POP_REF (p, &ref_pos);
  A68_CHAR c;
  POP_OBJECT (p, &c, A68_CHAR);
  reset_transput_buffer (PATTERN_BUFFER);
  add_a_string_transput_buffer (p, PATTERN_BUFFER, (BYTE_T *) & ref_str);
  int len = get_transput_buffer_index (PATTERN_BUFFER);
  char *q = get_transput_buffer (PATTERN_BUFFER);
  char ch = (char) VALUE (&c);
  for (int k = len - 1; k >= 0; k--) {
    if (q[k] == ch) {
      A68_INT pos;
      STATUS (&pos) = INIT_MASK;
      VALUE (&pos) = k + LOWER_BOUND (tup);
      *DEREF (A68_INT, &ref_pos) = pos;
      PUSH_VALUE (p, A68_TRUE, A68_BOOL);
      return;
    }
  }
  PUSH_VALUE (p, A68_FALSE, A68_BOOL);
}

//! @brief PROC string in string = (STRING, REF INT, STRING) BOOL

void genie_string_in_string (NODE_T * p)
{
  A68_REF ref_str;
  POP_REF (p, &ref_str);
  A68_ROW row = *(A68_REF *) &ref_str;
  CHECK_INIT (p, INITIALISED (&row), M_ROWS);
  A68_ARRAY *arr; A68_TUPLE *tup;
  GET_DESCRIPTOR (arr, tup, &row);
  A68_REF ref_pos; 
  POP_REF (p, &ref_pos);
  A68_REF ref_pat; 
  POP_REF (p, &ref_pat);
  reset_transput_buffer (PATTERN_BUFFER);
  reset_transput_buffer (STRING_BUFFER);
  add_a_string_transput_buffer (p, PATTERN_BUFFER, (BYTE_T *) & ref_pat);
  add_a_string_transput_buffer (p, STRING_BUFFER, (BYTE_T *) & ref_str);
  char *q = strstr (get_transput_buffer (STRING_BUFFER), get_transput_buffer (PATTERN_BUFFER));
  if (q != NO_TEXT) {
    if (!IS_NIL (ref_pos)) {
      A68_INT pos;
      STATUS (&pos) = INIT_MASK;
// ANSI standard leaves pointer difference undefined.
      VALUE (&pos) = LOWER_BOUND (tup) + (int) get_transput_buffer_index (STRING_BUFFER) - (int) strlen (q);
      *DEREF (A68_INT, &ref_pos) = pos;
    }
    PUSH_VALUE (p, A68_TRUE, A68_BOOL);
  } else {
    PUSH_VALUE (p, A68_FALSE, A68_BOOL);
  }
}
