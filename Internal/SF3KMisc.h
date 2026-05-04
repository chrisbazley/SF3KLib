/*
 * SF3KLib: Miscellaneous macro definitions
 * Copyright (C) 2018 Christopher Bazley
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SF3KMisc_h
#define SF3KMisc_h

#ifdef USE_CBDEBUG

#include "Debug.h"

#else /* USE_CBDEBUG */

#include <stdio.h>
#include <assert.h>

#ifdef DEBUG_OUTPUT
#define DEBUGF if (1) printf
#else
#define DEBUGF if (0) printf
#endif /* DEBUG_OUTPUT */

#endif /* USE_CBDEBUG */

#define NOT_USED(x) ((void)(x))

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/* Return the nearest word aligned value greater than or equal to a given
 * expression (useful for sprite widths, which must include right hand wastage).
 */
#define WORD_ALIGN(value) (((value) + 3) & ~3)

/* Copy a string into a character array of known size, truncating it to fit if
 * necessary. Unlike strncpy(), this macro ensures that the copied string is NUL
 * terminated if it has to be truncated.
 */
#define STRCPY_SAFE(string_1, string_2) do { \
  strncpy((string_1), (string_2), sizeof(string_1) - 1); \
  string_1[sizeof(string_1) - 1]='\0'; \
} while (0)

#endif /* SF3KMisc_h */
