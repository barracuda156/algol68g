//! @file a68g-environ.h
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
//! Standard prelude and postlude.

#if !defined (__A68G_ENVIRON_H__)
#define __A68G_ENVIRON_H__

// Standard environ.

static char *bold_prelude_start[] = {
  "BEGIN MODE ZAHL = LONG INT,",
  "           DOUBLE = LONG REAL,",
  "           QUAD = LONG LONG REAL;",
  "      start: commence:",
  "      BEGIN",
  NO_TEXT
};

static char *bold_postlude[] = {
  "      END;",
  "      stop: abort: halt: SKIP",
  "END",
  NO_TEXT
};

static char *quote_prelude_start[] = {
  "'BEGIN' 'MODE' 'ZAHL' = 'LONG' 'INT'",
  "               'DOUBLE' = 'LONG' 'REAL',",
  "               'QUAD' = 'LONG' 'LONG' 'REAL';",
  "        START: COMMENCE:",
  "        'BEGIN'",
  NO_TEXT
};

static char *quote_postlude[] = {
  "     'END';",
  "     STOP: ABORT: HALT: 'SKIP'",
  "'END'",
  NO_TEXT
};

#endif
