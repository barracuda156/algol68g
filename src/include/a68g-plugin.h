//! @file a68g-plugin.h
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
//! Definitions for the plugin compiler.

#if ! defined (__A68G_PLUGIN_H__)
#define __A68G_PLUGIN_H__

typedef union UFU UFU;

union UFU
{
  UNSIGNED_T u;
  REAL_T f;
};

#define BASIC(p, n) (basic_unit (stems_from ((p), (n))))

#define CON "const"
#define ELM "elem"
#define TMP "tmp"
#define ARG "arg"
#define ARR "array"
#define DEC "declarer"
#define DRF "deref"
#define DSP "display"
#define FUN "function"
#define PUP "pop"
#define REF "ref"
#define SEL "field"
#define TUP "tuple"

#define A68_MAKE_NOTHING 0
#define A68_MAKE_OTHERS 1
#define A68_MAKE_FUNCTION 2

#define OFFSET_OFF(s) (OFFSET (NODE_PACK (SUB (s))))
#define WIDEN_TO(p, a, b) (MOID (p) == MODE (b) && MOID (SUB (p)) == MODE (a))

#define NEEDS_DNS(m) (m != NO_MOID && (IS (m, REF_SYMBOL) || IS (m, PROC_SYMBOL) || IS (m, UNION_SYMBOL) || IS (m, FORMAT_SYMBOL)))

#define CODE_EXECUTE(p) {\
  indentf (out, a68_bufprt (A68 (edit_line), SNPRINTF_SIZE, "GENIE_UNIT_TRACE (_NODE_ (%d));", NUMBER (p)));\
  }

#define NAME_SIZE 200

BOOK_T *signed_in (int, int, const char *);
BOOL_T basic_argument (NODE_T *);
BOOL_T basic_call (NODE_T *);
BOOL_T basic_collateral (NODE_T *);
BOOL_T basic_conditional (NODE_T *);
BOOL_T basic_formula (NODE_T *);
BOOL_T basic_indexer (NODE_T *);
BOOL_T basic_mode (MOID_T *);
BOOL_T basic_mode_non_row (MOID_T *);
BOOL_T basic_monadic_formula (NODE_T *);
BOOL_T basic_serial (NODE_T *, int);
BOOL_T basic_slice (NODE_T *);
BOOL_T basic_unit (NODE_T *);
BOOL_T basic_unit (NODE_T *);
BOOL_T need_initialise_frame (NODE_T *);
BOOL_T primitive_mode (const MOID_T *);
BOOL_T same_tree (NODE_T *, NODE_T *);
char *compile_call (NODE_T *, FILE_T);
char *compile_cast (NODE_T *, FILE_T);
char *compile_denotation (NODE_T *, FILE_T);
char *compile_dereference_identifier (NODE_T *, FILE_T);
char *compile_formula (NODE_T *, FILE_T);
char *compile_identifier (NODE_T *, FILE_T);
char *gen_basic_conditional (NODE_T *, FILE_T, int);
char *gen_basic (NODE_T *, FILE_T);
char *gen_call (NODE_T *, FILE_T, int);
char *gen_cast (NODE_T *, FILE_T, int);
char *gen_closed_clause (NODE_T *, FILE_T, int);
char *gen_code_clause (NODE_T *, FILE_T, int);
char *gen_collateral_clause (NODE_T *, FILE_T, int);
char *gen_conditional_clause (NODE_T *, FILE_T, int);
char *gen_denotation (NODE_T *, FILE_T, int);
char *gen_deproceduring (NODE_T *, FILE_T, int);
char *gen_dereference_identifier (NODE_T *, FILE_T, int);
char *gen_dereference_selection (NODE_T *, FILE_T, int);
char *gen_dereference_slice (NODE_T *, FILE_T, int);
char *gen_formula (NODE_T *, FILE_T, int);
char *gen_identifier (NODE_T *, FILE_T, int);
char *gen_identity_relation (NODE_T *, FILE_T, int);
char *gen_int_case_clause (NODE_T *, FILE_T, int);
char *gen_loop_clause (NODE_T *, FILE_T, int);
char *gen_selection (NODE_T *, FILE_T, int);
char *gen_slice (NODE_T *, FILE_T, int);
char *gen_uniting (NODE_T *, FILE_T, int);
char *gen_unit (NODE_T *, FILE_T, int);
char *gen_voiding_assignation_identifier (NODE_T *, FILE_T, int);
char *gen_voiding_assignation_selection (NODE_T *, FILE_T, int);
char *gen_voiding_assignation_slice (NODE_T *, FILE_T, int);
char *gen_voiding_call (NODE_T *, FILE_T, int);
char *gen_voiding_deproceduring (NODE_T *, FILE_T, int);
char *gen_voiding_formula (NODE_T *, FILE_T, int);
char *inline_mode (MOID_T *);
char *internal_mode (const MOID_T *);
char *make_name (char *, char *, char *, int);
char *make_unic_name (char *, char *, char *, char *);
char *moid_with_name (char *, MOID_T *, char *);
DEC_T *add_declaration (DEC_T **, char *, int, char *);
DEC_T *add_identifier (DEC_T **, int, char *);
NODE_T *stems_from (NODE_T *, int);
void comment_source (NODE_T *, FILE_T);
void constant_folder (NODE_T *, FILE_T, int);
void gen_assign (NODE_T *, FILE_T, char *);
void gen_basics (NODE_T *, FILE_T);
void gen_check_init (NODE_T *, FILE_T, char *);
void gen_declaration_list (NODE_T *, FILE_T, int *, char *);
void gen_push (NODE_T *, FILE_T);
void gen_serial_clause (NODE_T *, FILE_T, NODE_T **, int *, int *, char *, int);
void gen_units (NODE_T *, FILE_T);
void get_stack (NODE_T *, FILE_T, char *, char *);
void indentf (FILE_T, int);
void indentf (FILE_T, int);
void indent (FILE_T, char *);
void indent (FILE_T, char *);
void init_static_frame (FILE_T, NODE_T *);
void inline_arguments (NODE_T *, FILE_T, int, int *);
void inline_call (NODE_T *, FILE_T, int);
void inline_closed (NODE_T *, FILE_T, int);
void inline_collateral (NODE_T *, FILE_T, int);
void inline_collateral_units (NODE_T *, FILE_T, int);
void inline_comment_source (NODE_T *, FILE_T);
void inline_conditional (NODE_T *, FILE_T, int);
void inline_denotation (NODE_T *, FILE_T, int);
void inline_dereference_identifier (NODE_T *, FILE_T, int);
void inline_dereference_selection (NODE_T *, FILE_T, int);
void inline_dereference_slice (NODE_T *, FILE_T, int);
void inline_formula (NODE_T *, FILE_T, int);
void inline_identifier (NODE_T *, FILE_T, int);
void inline_identity_relation (NODE_T *, FILE_T, int);
void inline_indexer (NODE_T *, FILE_T, int, INT_T *, char *);
void inline_monadic_formula (NODE_T *, FILE_T, int);
void inline_ref_identifier (NODE_T *, FILE_T, int);
void inline_selection (NODE_T *, FILE_T, int);
void inline_selection_ref_to_ref (NODE_T *, FILE_T, int);
void inline_single_argument (NODE_T *, FILE_T, int);
void inline_slice (NODE_T *, FILE_T, int);
void inline_slice_ref_to_ref (NODE_T *, FILE_T, int);
void inline_unit (NODE_T *, FILE_T, int);
void inline_unit (NODE_T *, FILE_T, int);
void inline_widening (NODE_T *, FILE_T, int);
void print_declarations (FILE_T, DEC_T *);
void sign_in (int, int, char *, void *, int);
void sign_in_name (char *, int *);
void undentf (FILE_T, int);
void undent (FILE_T, char *);
void write_fun_postlude (NODE_T *, FILE_T, char *);
void write_fun_prelude (NODE_T *, FILE_T, char *);
void write_prelude (FILE_T);

// The phases we go through.

enum
{ L_NONE = 0, L_DECLARE = 1, L_INITIALISE, L_EXECUTE, L_EXECUTE_2, L_YIELD, L_PUSH };

#define UNIC_NAME(k) (A68_OPT (unic_functions)[k].fun)

enum
{ UNIC_EXISTS, UNIC_MAKE_NEW, UNIC_MAKE_ALT };

// TRANSLATION tabulates translations for genie actions.
// This tells what to call for an A68 action.

typedef int LEVEL_T;

typedef struct
{
  GPROC *procedure;
  char *code;
} TRANSLATION;

extern TRANSLATION *monadics, *dyadics, *functions;

extern TRANSLATION monadics_nocheck[];
extern TRANSLATION monadics_check[];
extern TRANSLATION dyadics_nocheck[];
extern TRANSLATION dyadics_check[];
extern TRANSLATION functions_nocheck[];
extern TRANSLATION functions_check[];
extern TRANSLATION constants[];

#endif
