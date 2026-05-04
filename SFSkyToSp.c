/*
 * SF3KLib: Convert a 'Star Fighter 3000' sky definition to a RISC OS sprite
 * Copyright (C) 2009 Christopher Bazley
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
  CJB: 21-Aug-09: Created this source file.
  CJB: 04-Oct-09: Replaced macro values with enumerated constants. Updated
                  to use new SpriteAreaHeader and SpriteHeader type names.
  CJB: 26-Jun-10: Updated to use new SF3000 type and constant names.
  CJB: 17-Jan-11: Moved sprite creation code to sf_sky_to_lone_spr.
  CJB: 18-Apr-15: Assertions are now provided by debug.h.
  CJB: 03-Apr-16: Explicitly ignored the value returned by
                  sf_sky_to_lone_spr to avoid GNU C compiler warnings.
  CJB: 28-Apr-16: Now calls sf_sky_to_lone_spr to get the expected sprite
                  size instead of using an independent numeric constant.
*/

/* ISO library headers */
#include <stddef.h>
#include <stdbool.h>

/* CBOSLib headers */
#include "SprFormats.h"

/* Local headers */
#include "Internal/SF3KMisc.h"
#include "SFFormats.h"
#include "SpriteArea.h"
#include "SFSprConv.h"

size_t sf_sky_to_spr(SpriteAreaHeader *sprite_area,
                     const SFSky      *sky,
                     const char       *sprite_name,
                     bool              new_format)
{
  const size_t sprite_size = sf_sky_to_lone_spr(
    NULL, 0, sky, sprite_name, new_format);

  assert(sky != NULL);

  if (sprite_area != NULL)
  {
    /* Allocate space for a new sprite in the output sprite area */
    SpriteHeader * const sph = spritearea_alloc_spr(sprite_area, sprite_size);
    if (sph == NULL)
    {
      DEBUGF("Ran out of space during conversion\n");
    }
    else
    {
      /* Convert sky definition to sprite */
      size_t req_size = sf_sky_to_lone_spr(
        sph, sprite_size, sky, sprite_name, new_format);

      assert(req_size == sprite_size);
      NOT_USED(req_size);
    }
  }

  DEBUGF("Required free space is %zu\n", sprite_size);
  return sprite_size;
}
