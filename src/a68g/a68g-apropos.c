//! @file a68g-apropos.c
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
//! Command line help.

#include "a68g.h"
#include "a68g-prelude.h"
#include "a68g-mp.h"
#include "a68g-genie.h"

// Interactive help.

typedef struct A68_INFO A68_INFO;

struct A68_INFO
{
  char *cat;
  char *term;
  char *def;
};

static A68_INFO info_text[] = {
  {"monitor", "breakpoint clear [all]", "clear breakpoints and watchpoint expression"},
  {"monitor", "breakpoint clear breakpoints", "clear breakpoints"},
  {"monitor", "breakpoint clear watchpoint", "clear watchpoint expression"},
  {"monitor", "breakpoint [list]", "list breakpoints"},
  {"monitor", "breakpoint \"n\" clear", "clear breakpoints in line \"n\""},
  {"monitor", "breakpoint \"n\" if \"expression\"", "break in line \"n\" when expression evaluates to true"},
  {"monitor", "breakpoint \"n\"", "set breakpoints in line \"n\""},
  {"monitor", "breakpoint watch \"expression\"", "break on watchpoint expression when it evaluates to true"},
  {"monitor", "calls [n]", "print \"n\" frames in the call stack (default n=3)"},
  {"monitor", "continue, resume", "continue execution"},
  {"monitor", "do \"command\", exec \"command\"", "pass \"command\" to the shell and print return code"},
  {"monitor", "elems [n]", "print first \"n\" elements of rows (default n=24)"},
  {"monitor", "evaluate \"expression\", x \"expression\"", "print result of \"expression\""},
  {"monitor", "examine \"n\"", "print value of symbols named \"n\" in the call stack"},
  {"monitor", "exit, hx, quit", "terminates the program"},
  {"monitor", "finish, out", "continue execution until current procedure incarnation is finished"},
  {"monitor", "frame 0", "set current stack frame to top of frame stack"},
  {"monitor", "frame \"n\"", "set current stack frame to \"n\""},
  {"monitor", "frame", "print contents of the current stack frame"},
  {"monitor", "heap \"n\"", "print contents of the heap with address not greater than \"n\""},
  {"monitor", "help [expression]", "print brief help text"},
  {"monitor", "ht", "halts typing to standard output"},
  {"monitor", "list [n]", "show \"n\" lines around the interrupted line (default n=10)"},
  {"monitor", "next", "continue execution to next interruptable unit (do not enter routine-texts)"},
  {"monitor", "prompt \"s\"", "set prompt to \"s\""},
  {"monitor", "rerun, restart", "restarts a program without resetting breakpoints"},
  {"monitor", "reset", "restarts a program and resets breakpoints"},
  {"monitor", "rt", "resumes typing to standard output"},
  {"monitor", "sizes", "print size of memory segments"},
  {"monitor", "stack [n]", "print \"n\" frames in the stack (default n=3)"},
  {"monitor", "step", "continue execution to next interruptable unit"},
  {"monitor", "until \"n\"", "continue execution until line number \"n\" is reached"},
  {"monitor", "where", "print the interrupted line"},
  {"monitor", "xref \"n\"", "give detailed information on source line \"n\""},
  {"options", "--assertions, --noassertions", "switch elaboration of assertions on or off"},
  {"options", "--backtrace, --nobacktrace", "switch stack backtracing in case of a runtime error"},
  {"options", "--boldstropping", "set stropping mode to bold stropping"},
  {"options", "--brackets", "consider [ .. ] and { .. } as equivalent to ( .. )"},
  {"options", "--check, --norun", "check syntax only, interpreter does not start"},
  {"options", "--clock", "report execution time excluding compilation time"},
  {"options", "--compile", "compile source file"},
  {"options", "--debug, --monitor", "start execution in the debugger and debug in case of runtime error"},
  {"options", "--echo string", "echo \"string\" to standard output"},
  {"options", "--execute unit", "execute algol 68 unit \"unit\""},
  {"options", "--exit, --", "ignore next options"},
  {"options", "--extensive", "make extensive listing"},
  {"options", "--file string", "accept string as generic filename"},
  {"options", "--frame \"number\"", "set frame stack size to \"number\""},
  {"options", "--handles \"number\"", "set handle space size to \"number\""},
  {"options", "--heap \"number\"", "set heap size to \"number\""},
  {"options", "--keep, --nokeep", "switch object file deletion off or on"},
  {"options", "--listing", "make concise listing"},
  {"options", "--moids", "make overview of moids in listing file"},
  {"options", "-O0, -O1, -O2, -O3", "switch compilation on and pass option to back-end C compiler"},
  {"options", "--optimise, --nooptimise", "switch compilation on or off"},
  {"options", "--pedantic", "equivalent to --warnings --portcheck"},
  {"options", "--portcheck, --noportcheck", "switch portability warnings on or off"},
  {"options", "--pragmats, --nopragmats", "switch elaboration of pragmat items on or off"},
  {"options", "--precision \"number\"", "set precision for long long modes to \"number\" significant digits"},
  {"options", "--preludelisting", "make a listing of preludes"},
  {"options", "--pretty-print", "pretty-print the source file"},
  {"options", "--print unit", "print value yielded by algol 68 unit \"unit\""},
  {"options", "--quiet", "suppresses all warning diagnostics"},
  {"options", "--quotestropping", "set stropping mode to quote stropping"},
  {"options", "--reductions", "print parser reductions"},
  {"options", "--run", "override --check/--norun options"},
  {"options", "--rerun", "run using already compiled code"},
  {"options", "--script", "set next option as source file name; pass further options to algol 68 program"},
  {"options", "--source, --nosource", "switch listing of source lines in listing file on or off"},
  {"options", "--stack \"number\"", "set expression stack size to \"number\""},
  {"options", "--statistics", "print statistics in listing file"},
  {"options", "--strict", "disable most extensions to Algol 68 syntax"},
  {"options", "--timelimit \"number\"", "interrupt the interpreter after \"number\" seconds"},
  {"options", "--trace, --notrace", "switch tracing of a running program on or off"},
  {"options", "--tree, --notree", "switch syntax tree listing in listing file on or off"},
  {"options", "--unused", "make an overview of unused tags in the listing file"},
  {"options", "--verbose", "inform on program actions"},
  {"options", "--version", "state version of the running copy"},
  {"options", "--warnings, --nowarnings", "switch warning diagnostics on or off"},
  {"options", "--xref, --noxref", "switch cross reference in the listing file on or off"},
  {NO_TEXT, NO_TEXT, NO_TEXT}
};

//! @brief Print_info.

void print_info (FILE_T f, char *prompt, int k)
{
  if (prompt != NO_TEXT) {
    ASSERT (a68_bufprt (A68 (output_line), SNPRINTF_SIZE, "%s %s: %s.", prompt, TERM (&info_text[k]), DEF (&info_text[k])) >= 0);
  } else {
    ASSERT (a68_bufprt (A68 (output_line), SNPRINTF_SIZE, "%s: %s.", TERM (&info_text[k]), DEF (&info_text[k])) >= 0);
  }
  WRITE (f, A68 (output_line));
  WRITELN (f, "");
}

//! @brief Apropos.

void apropos (FILE_T f, char *prompt, char *item)
{
  io_close_tty_line ();
  if (item == NO_TEXT) {
    for (int k = 0; CAT (&info_text[k]) != NO_TEXT; k++) {
      print_info (f, prompt, k);
    }
    return;
  }
  int n = 0;
  for (int k = 0; CAT (&info_text[k]) != NO_TEXT; k++) {
    if (grep_in_string (item, CAT (&info_text[k]), NO_INT, NO_INT) == 0) {
      print_info (f, prompt, k);
      n++;
    }
  }
  if (n > 0) {
    return;
  }
  for (int k = 0; CAT (&info_text[k]) != NO_TEXT; k++) {
    if (grep_in_string (item, TERM (&info_text[k]), NO_INT, NO_INT) == 0 || grep_in_string (item, DEF (&info_text[k]), NO_INT, NO_INT) == 0) {
      print_info (f, prompt, k);
    }
  }
}
