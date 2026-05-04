/*
 * CBLibrary: Check whether a RISC OS sprite has 8 bits per pixel
 * Copyright (C) 2009  Chris Bazley
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

/* History:
  CJB: 22-Aug-09: Created this source file.
  CJB: 26-Jun-10: Updated to use new sprite type.
  CJB: 18-Apr-15: Assertions are now provided by debug.h.
  CJB: 18-Apr-16: Cast pointer parameters to void * to match %p.
                  Used size_t for loop counter to match type of ARRAY_SIZE.
  CJB: 11-Nov-18: Fixed broken #include.
*/

/* ISO library headers */
#include <stddef.h>
#include <stdbool.h>

/* CBOSLib headers */
#include "SprFormats.h"

/* Local headers */
#include "Internal/SF3KMisc.h"
#include "Internal/SprHas8bpp.h"

bool sprite_has_8_bpp(const SpriteHeader *sprite)
{
  unsigned long type;
  bool has_8_bpp = false;

  assert(sprite != NULL);
  DEBUGF("Checking type %u of sprite %p\n", sprite->type, (void *)sprite);

  type = ((long)sprite->type & SPRITE_INFO_TYPE_MASK) >> SPRITE_INFO_TYPE_SHIFT;

  if (type == SPRITE_TYPE_OLD)
  {
    /* Old sprite format: check whether screen mode has 8 bits per pixel */
    static const char mode_nos[] = {10, 13, 15, 21, 24, 28, 32, 36, 40};

    has_8_bpp = false;
    for (size_t i = 0; i < ARRAY_SIZE(mode_nos); i++)
    {
      if (mode_nos[i] == sprite->type)
      {
        has_8_bpp = true;
        break;
      }
    }
  }
  else
  {
    /* New sprite format: check whether it has 8 bits per pixel */
    has_8_bpp = (type == SPRITE_TYPE_8BPP);
  }
  DEBUGF("  Type %s 8 bpp\n", has_8_bpp ? "is" : "isn't");
  return has_8_bpp;
}
