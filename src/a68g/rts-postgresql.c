//! @file rts-postgresql.c
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
//! PostgreSQL libpq interface. 

// PostgreSQL libpq interface based on initial work by Jaap Boender. 
// Wraps "connection" and "result" objects in a FILE variable to support 
// multiple connections.
// 
// Error codes:
// 0    Success
// -1   No connection
// -2   No result
// -3   Other error

#include "a68g.h"
#include "a68g-prelude.h"
#include "a68g-genie.h"
#include "a68g-transput.h"

#if defined (HAVE_POSTGRESQL)

#define LIBPQ_STRING "PostgreSQL libq"
#define ERROR_NOT_CONNECTED "not connected to a database"
#define ERROR_NO_QUERY_RESULT "no query result available"

#define NO_PGCONN ((PGconn *) NULL)
#define NO_PGRESULT ((PGresult *) NULL)

//! @brief PROC pg connect db (REF FILE, STRING, REF STRING) INT

void genie_pq_connectdb (NODE_T * p)
{
  A68_REF ref_string, ref_file, conninfo;
  POP_REF (p, &ref_string);
  CHECK_REF (p, ref_string, M_REF_STRING);
  POP_REF (p, &conninfo);
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  if (IS_IN_HEAP (&ref_file) && !IS_IN_HEAP (&ref_string)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_SCOPE_DYNAMIC_1, M_REF_STRING);
    exit_genie (p, A68_RUNTIME_ERROR);
  } else if (IS_IN_FRAME (&ref_file) && IS_IN_FRAME (&ref_string)) {
    if (REF_SCOPE (&ref_string) > REF_SCOPE (&ref_file)) {
      diagnostic (A68_RUNTIME_ERROR, p, ERROR_SCOPE_DYNAMIC_1, M_REF_STRING);
      exit_genie (p, A68_RUNTIME_ERROR);
    }
  }
// Initialise the file.
  A68_FILE *file = FILE_DEREF (&ref_file);
  if (OPENED (file)) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_FILE_ALREADY_OPEN);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  STATUS (file) = INIT_MASK;
  CHANNEL (file) = A68 (associate_channel);
  OPENED (file) = A68_TRUE;
  OPEN_EXCLUSIVE (file) = A68_FALSE;
  READ_MOOD (file) = A68_FALSE;
  WRITE_MOOD (file) = A68_FALSE;
  CHAR_MOOD (file) = A68_FALSE;
  DRAW_MOOD (file) = A68_FALSE;
  TMP_FILE (file) = A68_FALSE;
  if (INITIALISED (&(IDENTIFICATION (file))) && !IS_NIL (IDENTIFICATION (file))) {
    UNBLOCK_GC_HANDLE (&(IDENTIFICATION (file)));
  }
  IDENTIFICATION (file) = nil_ref;
  TERMINATOR (file) = nil_ref;
  FORMAT (file) = nil_format;
  FD (file) = -1;
  if (INITIALISED (&(STRING (file))) && !IS_NIL (STRING (file))) {
    UNBLOCK_GC_HANDLE (&(STRING (file)));
  }
  STRING (file) = ref_string;
  BLOCK_GC_HANDLE (&(STRING (file)));
  STRPOS (file) = 0;
  STREAM (&DEVICE (file)) = NULL;
  set_default_event_procedures (file);
// Establish a connection.
  A68_REF ref_z = heap_generator (p, M_C_STRING, 1 + a68_string_size (p, conninfo));
  CONNECTION (file) = PQconnectdb (a_to_c_string (p, DEREF (char, &ref_z), conninfo));
  RESULT (file) = NO_PGRESULT;
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -3, INT);
  }
  (void) PQsetErrorVerbosity (CONNECTION (file), PQERRORS_DEFAULT);
  if (PQstatus (CONNECTION (file)) != CONNECTION_OK) {
    PUSH_PRIMAL (p, -1, INT);
  } else {
    PUSH_PRIMAL (p, 0, INT);
  }
}

//! @brief PROC pq finish (REF FILE) VOID

void genie_pq_finish (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) != NO_PGRESULT) {
    PQclear (RESULT (file));
  }
  PQfinish (CONNECTION (file));
  CONNECTION (file) = NO_PGCONN;
  RESULT (file) = NO_PGRESULT;
  PUSH_PRIMAL (p, 0, INT);
}

//! @brief PROC pq reset (REF FILE) VOID

void genie_pq_reset (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) != NO_PGRESULT) {
    PQclear (RESULT (file));
  }
  PQreset (CONNECTION (file));
  PUSH_PRIMAL (p, 0, INT);
}

//! @brief PROC pq exec = (REF FILE, STRING) INT

void genie_pq_exec (NODE_T * p)
{
  A68_REF query, ref_file;
  POP_REF (p, &query);
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) != NO_PGRESULT) {
    PQclear (RESULT (file));
  }
  A68_REF ref_z = heap_generator (p, M_C_STRING, 1 + a68_string_size (p, query));
  RESULT (file) = PQexec (CONNECTION (file), a_to_c_string (p, DEREF (char, &ref_z), query));
  if ((PQresultStatus (RESULT (file)) != PGRES_TUPLES_OK)
      && (PQresultStatus (RESULT (file)) != PGRES_COMMAND_OK)) {
    PUSH_PRIMAL (p, -3, INT);
  } else {
    PUSH_PRIMAL (p, 0, INT);
  }
}

//! @brief PROC pq parameterstatus (REF FILE) INT

void genie_pq_parameterstatus (NODE_T * p)
{
  A68_REF parameter, ref_file;
  POP_REF (p, &parameter);
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  A68_REF ref_z = heap_generator (p, M_C_STRING, 1 + a68_string_size (p, parameter));
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, (char *) PQparameterStatus (CONNECTION (file), a_to_c_string (p, DEREF (char, &ref_z), parameter)), DEFAULT_WIDTH);
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq cmdstatus (REF FILE) INT

void genie_pq_cmdstatus (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQcmdStatus (RESULT (file)), DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq cmdtuples (REF FILE) INT

void genie_pq_cmdtuples (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQcmdTuples (RESULT (file)), DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq ntuples (REF FILE) INT

void genie_pq_ntuples (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -2, INT);
    return;
  }
  PUSH_PRIMAL (p, (PQresultStatus (RESULT (file))) == PGRES_TUPLES_OK ? PQntuples (RESULT (file)) : -3, INT);
}

//! @brief PROC pq nfields (REF FILE) INT

void genie_pq_nfields (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -2, INT);
    return;
  }
  PUSH_PRIMAL (p, (PQresultStatus (RESULT (file))) == PGRES_TUPLES_OK ? PQnfields (RESULT (file)) : -3, INT);
}

//! @brief PROC pq fname (REF FILE, INT) INT

void genie_pq_fname (NODE_T * p)
{
  A68_INT a68_index; A68_REF ref_file;
  POP_OBJECT (p, &a68_index, A68_INT);
  CHECK_INIT (p, INITIALISED (&a68_index), M_INT);
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -2, INT);
    return;
  }
  int upb = (PQresultStatus (RESULT (file)) == PGRES_TUPLES_OK ? PQnfields (RESULT (file)) : 0);
  if (VALUE (&a68_index) < 1 || VALUE (&a68_index) > upb) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_INDEX_OUT_OF_BOUNDS);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQfname (RESULT (file), VALUE (&a68_index) - 1), DEFAULT_WIDTH);
    STRPOS (file) = 0;
  }
  PUSH_PRIMAL (p, 0, INT);
}

//! @brief PROC pq fnumber = (REF FILE, STRING) INT

void genie_pq_fnumber (NODE_T * p)
{
  A68_REF name, ref_file;
  POP_REF (p, &name);
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -2, INT);
    return;
  }
  A68_REF ref_z = heap_generator (p, M_C_STRING, 1 + a68_string_size (p, name));
  int k = PQfnumber (RESULT (file), a_to_c_string (p, DEREF (char, &ref_z), name));
  if (k == -1) {
    PUSH_PRIMAL (p, -3, INT);
  } else {
    PUSH_PRIMAL (p, k + 1, INT);
  }
}

//! @brief PROC pq fformat (REF FILE, INT) INT

void genie_pq_fformat (NODE_T * p)
{
  A68_INT a68_index; A68_REF ref_file;
  POP_OBJECT (p, &a68_index, A68_INT);
  CHECK_INIT (p, INITIALISED (&a68_index), M_INT);
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -2, INT);
    return;
  }
  int upb = (PQresultStatus (RESULT (file)) == PGRES_TUPLES_OK ? PQnfields (RESULT (file)) : 0);
  if (VALUE (&a68_index) < 1 || VALUE (&a68_index) > upb) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_INDEX_OUT_OF_BOUNDS);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  PUSH_PRIMAL (p, PQfformat (RESULT (file), VALUE (&a68_index) - 1), INT);
}

//! @brief PROC pq getvalue (REF FILE, INT, INT) INT

void genie_pq_getvalue (NODE_T * p)
{
  A68_INT row, column; A68_REF ref_file;
  POP_OBJECT (p, &column, A68_INT);
  CHECK_INIT (p, INITIALISED (&column), M_INT);
  POP_OBJECT (p, &row, A68_INT);
  CHECK_INIT (p, INITIALISED (&row), M_INT);
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -2, INT);
    return;
  }
  int upb = (PQresultStatus (RESULT (file)) == PGRES_TUPLES_OK ? PQnfields (RESULT (file)) : 0);
  if (VALUE (&column) < 1 || VALUE (&column) > upb) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_INDEX_OUT_OF_BOUNDS);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  upb = (PQresultStatus (RESULT (file)) == PGRES_TUPLES_OK ? PQntuples (RESULT (file)) : 0);
  if (VALUE (&row) < 1 || VALUE (&row) > upb) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_INDEX_OUT_OF_BOUNDS);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  char *str = PQgetvalue (RESULT (file), VALUE (&row) - 1, VALUE (&column) - 1);
  if (str == NULL) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_NO_QUERY_RESULT);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, str, DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq getisnull (REF FILE, INT, INT) INT

void genie_pq_getisnull (NODE_T * p)
{
  A68_INT row, column; A68_REF ref_file;
  POP_OBJECT (p, &column, A68_INT);
  CHECK_INIT (p, INITIALISED (&column), M_INT);
  POP_OBJECT (p, &row, A68_INT);
  CHECK_INIT (p, INITIALISED (&row), M_INT);
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -2, INT);
    return;
  }
  int upb = (PQresultStatus (RESULT (file)) == PGRES_TUPLES_OK ? PQnfields (RESULT (file)) : 0);
  if (VALUE (&column) < 1 || VALUE (&column) > upb) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_INDEX_OUT_OF_BOUNDS);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  upb = (PQresultStatus (RESULT (file)) == PGRES_TUPLES_OK ? PQntuples (RESULT (file)) : 0);
  if (VALUE (&row) < 1 || VALUE (&row) > upb) {
    diagnostic (A68_RUNTIME_ERROR, p, ERROR_INDEX_OUT_OF_BOUNDS);
    exit_genie (p, A68_RUNTIME_ERROR);
  }
  PUSH_PRIMAL (p, PQgetisnull (RESULT (file), VALUE (&row) - 1, VALUE (&column) - 1), INT);
}

//! @brief Edit error message string from libpq.

char *pq_edit (char *str)
{
  if (str == NULL) {
    return "";
  } else {
    static BUFFER edt;
    char *q = edt;
    int newlines = 0, len = (int) strlen (str);
    BOOL_T suppress_blank = A68_FALSE;
    while (len > 0 && str[len - 1] == NEWLINE_CHAR) {
      str[len - 1] = NULL_CHAR;
      len = (int) strlen (str);
    }
    while (str[0] != NULL_CHAR) {
      if (str[0] == CR_CHAR) {
        str++;
      } else if (str[0] == NEWLINE_CHAR) {
        if (newlines++ == 0) {
          *(q++) = POINT_CHAR;
          *(q++) = BLANK_CHAR;
          *(q++) = '(';
        } else {
          *(q++) = BLANK_CHAR;
        }
        suppress_blank = A68_TRUE;
        str++;
      } else if (IS_SPACE (str[0])) {
        if (suppress_blank) {
          str++;
        } else {
          if (str[1] != NEWLINE_CHAR) {
            *(q++) = BLANK_CHAR;
          }
          str++;
          suppress_blank = A68_TRUE;
        }
      } else {
        *(q++) = *(str++);
        suppress_blank = A68_FALSE;
      }
    }
    if (newlines > 0) {
      *(q++) = ')';
    }
    q[0] = NULL_CHAR;
    return edt;
  }
}

//! @brief PROC pq errormessage (REF FILE) INT 

void genie_pq_errormessage (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    BUFFER str;
    if (PQerrorMessage (CONNECTION (file)) != NULL) {
      a68_bufcpy (str, pq_edit (PQerrorMessage (CONNECTION (file))), BUFFER_SIZE);
      int upb = (int) strlen (str);
      if (upb > 0 && str[upb - 1] == NEWLINE_CHAR) {
        str[upb - 1] = NULL_CHAR;
      }
    } else {
      a68_bufcpy (str, "no error message available", BUFFER_SIZE);
    }
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, str, DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq resulterrormessage (REF FILE) INT 

void genie_pq_resulterrormessage (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (RESULT (file) == NO_PGRESULT) {
    PUSH_PRIMAL (p, -2, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    BUFFER str;
    if (PQresultErrorMessage (RESULT (file)) != NULL) {
      a68_bufcpy (str, pq_edit (PQresultErrorMessage (RESULT (file))), BUFFER_SIZE);
      int upb = (int) strlen (str);
      if (upb > 0 && str[upb - 1] == NEWLINE_CHAR) {
        str[upb - 1] = NULL_CHAR;
      }
    } else {
      a68_bufcpy (str, "no error message available", BUFFER_SIZE);
    }
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, str, DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq db (REF FILE) INT 

void genie_pq_db (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQdb (CONNECTION (file)), DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq user (REF FILE) INT 

void genie_pq_user (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQuser (CONNECTION (file)), DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq pass (REF FILE) INT 

void genie_pq_pass (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQpass (CONNECTION (file)), DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq host (REF FILE) INT 

void genie_pq_host (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQhost (CONNECTION (file)), DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq port (REF FILE) INT 

void genie_pq_port (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQport (CONNECTION (file)), DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq tty (REF FILE) INT 

void genie_pq_tty (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQtty (CONNECTION (file)), DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq options (REF FILE) INT 

void genie_pq_options (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    *DEREF (A68_REF, &STRING (file)) = c_to_a_string (p, PQoptions (CONNECTION (file)), DEFAULT_WIDTH);
    STRPOS (file) = 0;
    PUSH_PRIMAL (p, 0, INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq protocol version (REF FILE) INT 

void genie_pq_protocolversion (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    PUSH_PRIMAL (p, PQprotocolVersion (CONNECTION (file)), INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq server version (REF FILE) INT 

void genie_pq_serverversion (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    PUSH_PRIMAL (p, PQserverVersion (CONNECTION (file)), INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq socket (REF FILE) INT 

void genie_pq_socket (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    PUSH_PRIMAL (p, PQsocket (CONNECTION (file)), INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

//! @brief PROC pq backend pid (REF FILE) INT 

void genie_pq_backendpid (NODE_T * p)
{
  A68_REF ref_file;
  POP_REF (p, &ref_file);
  CHECK_REF (p, ref_file, M_REF_FILE);
  A68_FILE *file = FILE_DEREF (&ref_file);
  CHECK_INIT (p, INITIALISED (file), M_FILE);
  if (CONNECTION (file) == NO_PGCONN) {
    PUSH_PRIMAL (p, -1, INT);
    return;
  }
  if (!IS_NIL (STRING (file))) {
    PUSH_PRIMAL (p, PQbackendPID (CONNECTION (file)), INT);
  } else {
    PUSH_PRIMAL (p, -3, INT);
  }
}

#endif
