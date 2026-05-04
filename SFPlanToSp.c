/*
 * SF3KLib: Convert 'Star Fighter 3000' planet images to RISC OS sprites
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
  CJB: 17-Jan-11: Moved sprite creation code to sf_planets_to_lone_spr.
  CJB: 18-Apr-15: Assertions are now provided by debug.h.
  CJB: 19-Sep-15: Deleted a bad assertion (since 2011 if not earlier) that
                  sprite_name was not NULL even if not used.
  CJB: 09-Apr-16: Added casts and explicitly ignored the value returned by
                  sf_planets_to_lone_spr to avoid GNU C compiler warnings.
  CJB: 21-Apr-16: Substituted format specifier %zu for %lu to avoid the need
                  to cast the matching parameter.
  CJB: 28-Apr-16: Now calls sf_planets_to_lone_spr to get the expected
                  sprite size instead of using an independent numeric
                  constant.
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

size_t sf_planets_to_spr(SpriteAreaHeader       *sprite_area,
                         const SFPlanetsHeader  *planets,
                         unsigned int            start,
                         unsigned int            end,
                         const char             *sprite_name,
                         bool                    new_format,
                         SFBitmapsProgressFn    *prog_cb,
                         const void             *prog_arg)
{
  size_t output_size = 0;
  const size_t sprite_size = sf_planets_to_lone_spr(
    NULL, 0, planets, 0, sprite_name, new_format);

  assert(planets != NULL);
  assert(start <= end);

  /* Ensure we do not read from beyond the end of the planets set */
  if (planets->last_image_num < 0)
    end = 0;
  else if (end > (unsigned int)planets->last_image_num + 1)
    end = planets->last_image_num + 1;

  if (sprite_area != NULL)
  {
    for (unsigned int image = start; image < end; image++)
    {
      /* Call the progress function before processing every bitmap,
         if supplied */
      if (prog_cb != NULL)
      {
        if (!prog_cb(prog_arg, image))
        {
          DEBUGF("Conversion aborted by progress callback\n");
          break;
        }
      }

      /* Allocate space for a new sprite in the output sprite area */
      SpriteHeader * const sph = spritearea_alloc_spr(sprite_area, sprite_size);
      if (sph == NULL)
      {
        DEBUGF("Ran out of space during conversion\n");
        break; /* not enough free space */
      }

      /* Convert planet image bitmap to sprite */
      size_t req_size = sf_planets_to_lone_spr(
        sph, sprite_size, planets, image, sprite_name, new_format);

      assert(req_size == sprite_size);
      NOT_USED(req_size);
    } /* loop back (next image) */
  }

  output_size = (end - start) * sprite_size;
  DEBUGF("Required free space is %zu\n", output_size);
  return output_size;
}
