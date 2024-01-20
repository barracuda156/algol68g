//! @file a68g-transput.h
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
//! Transput related definitions.

#if !defined (__A68G_TRANSPUT_H__)
#define __A68G_TRANSPUT_H__

#if (A68_LEVEL >= 3)
extern char *long_sub_whole_double (NODE_T *, DOUBLE_NUM_T, int);
#endif

BOOL_T convert_radix_mp (NODE_T *, MP_T *, int, int, MOID_T *, MP_T *, MP_T *);
BOOL_T convert_radix (NODE_T *, UNSIGNED_T, int, int);
BOOL_T genie_string_to_value_internal (NODE_T *, MOID_T *, char *, BYTE_T *);
char digchar (int);
char *error_chars (char *, int);
char *fixed (NODE_T * p);
char *get_transput_buffer (int);
char *long_sub_fixed (NODE_T *, MP_T *, int, int, int);
char *long_sub_whole (NODE_T *, MP_T *, int, int);
char pop_char_transput_buffer (int);
char *real (NODE_T *);
char *stack_string (NODE_T *, int);
char *sub_fixed_double (NODE_T *, DOUBLE_T, int, int, int);
char *sub_fixed (NODE_T *, REAL_T, int, int);
char *sub_whole (NODE_T *, INT_T, int);
char *whole (NODE_T * p);
FILE *a68_fopen (char *, char *, char *);
FILE_T open_physical_file (NODE_T *, A68_REF, int, mode_t);
GPROC genie_fixed;
GPROC genie_float;
GPROC genie_real;
GPROC genie_whole;
int char_scanner (A68_FILE *);
int end_of_format (NODE_T *, A68_REF);
int get_replicator_value (NODE_T *, BOOL_T);
int get_transput_buffer_index (int);
int get_transput_buffer_size (int);
int get_unblocked_transput_buffer (NODE_T *);
int store_file_entry (NODE_T *, FILE_T, char *, BOOL_T);
void add_a_string_transput_buffer (NODE_T *, int, BYTE_T *);
void add_chars_transput_buffer (NODE_T *, int, int, char *);
void add_string_from_stack_transput_buffer (NODE_T *, int);
void add_string_transput_buffer (NODE_T *, int, char *);
void end_of_file_error (NODE_T * p, A68_REF ref_file);
void enlarge_transput_buffer (NODE_T *, int, int);
void format_error (NODE_T *, A68_REF, char *);
void long_standardise (NODE_T *, MP_T *, int, int, int, int *);
void on_event_handler (NODE_T *, A68_PROCEDURE, A68_REF);
void open_error (NODE_T *, A68_REF, char *);
void open_for_reading (NODE_T *, A68_REF);
void open_for_writing (NODE_T *, A68_REF);
void pattern_error (NODE_T *, MOID_T *, int);
void plusab_transput_buffer (NODE_T *, int, char);
void plusto_transput_buffer (NODE_T *, char, int);
void read_insertion (NODE_T *, A68_REF);
void read_sound (NODE_T *, A68_REF, A68_SOUND *);
void reset_transput_buffer (int);
void set_default_event_procedure (A68_PROCEDURE *);
void set_default_event_procedures (A68_FILE *);
void set_transput_buffer_index (int, int);
void set_transput_buffer_size (int, int);
void standardise (REAL_T *, int, int, int *);
void transput_error (NODE_T *, A68_REF, MOID_T *);
void unblock_transput_buffer (int);
void unchar_scanner (NODE_T *, A68_FILE *, char);
void value_error (NODE_T *, MOID_T *, A68_REF);
void write_insertion (NODE_T *, A68_REF, MOOD_T);
void write_purge_buffer (NODE_T *, A68_REF, int);
void write_sound (NODE_T *, A68_REF, A68_SOUND *);

#endif
