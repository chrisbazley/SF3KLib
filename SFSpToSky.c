/*
 * SF3KLib: Convert a RISC OS sprite to a 'Star Fighter 3000' sky definition
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
  CJB: 23-Aug-09: Created this source file.
  CJB: 30-Sep-09: Updated to use new SpriteAreaHeader and SpriteHeader type
                  names.
  CJB: 26-Jun-10: Updated to use new SF3000 type and constant names.
  CJB: 18-Apr-15: Assertions are now provided by debug.h.
  CJB: 09-Apr-16: Added a check for the count of sprites being -ve and if
                  not then cast it to unsigned to avoid GNU C compiler
                  warnings.
  CJB: 21-Apr-16: Substituted format specifier %zu for %u where necessary
                  to match the parameter type.
  CJB: 21-Nov-20: Pass the actual sprite name to the query callback instead
                  of the expected name.
*/

/* ISO library headers */
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* CBOSLib headers */
#include "SprFormats.h"

/* Local headers */
#include "Internal/SF3KMisc.h"
#include "Internal/SprHas8bpp.h"
#include "SFFormats.h"
#include "SFSprConv.h"

size_t sf_spr_to_sky(SFSky                  *sky,
                     size_t                  sky_size,
                     const SpriteAreaHeader *sprite_area,
                     unsigned int            start,
                     unsigned int            end,
                     const char             *sprite_name,
                     SFBitmapsProgressFn    *prog_cb,
                     SFBitmapsQueryFn       *query_cb,
                     const void             *cb_arg)
{
  size_t output_size = 0;

  assert(sprite_area != NULL);
  assert(start <= end);

  if (sky == NULL)
    sky_size = 0;

  /* Ensure we do not read from beyond the end of the sprite area */
  if (sprite_area->sprite_count < 0)
    end = 0;
  if (end > (unsigned int)sprite_area->sprite_count)
    end = sprite_area->sprite_count;

  assert(sprite_name != NULL);

  /* Calculate address of first sprite */
  const SpriteHeader *sph = (SpriteHeader *)((char *)sprite_area + sprite_area->first);

  /* We always have to iterate through the sprites from the beginning of the
     area until we find the first one to be converted. */
  for (unsigned int sp = 0; sp < end; sp++)
  {
    if (sp >= start)
    {
      char name[sizeof(sph->name) + 1];

      if (prog_cb != NULL)
      {
        if (!prog_cb(cb_arg, sp))
        {
          DEBUGF("Conversion aborted by progress callback\n");
          break;
        }
      }

      /* The sprite name embedded in the header may be unterminated,
         so copy it to a separate buffer and append a nul terminator */
      STRCPY_SAFE(name, sph->name);
      DEBUGF("Validating sprite %u:%p ('%s')\n", sp, (void *)sph, name);

      /* Check sprite header (ignore DPI, mask, palette) */
      if (strcmp(name, sprite_name) == 0 &&
          sph->width == WORD_ALIGN(SFSky_Width) / 4 - 1 &&
          sph->height == SFSky_Height - 1 &&
          sph->left_bit == 0 &&
          sph->right_bit == SPRITE_RIGHT_BIT(SFSky_Width, 8) &&
          sprite_has_8_bpp(sph))
      {
        DEBUGF("Valid sky\n");
        output_size = sizeof(*sky);

        /* Guard against overrunning the end of the output buffer */
        if (output_size <= sky_size)
        {
          /* Calculate address of start of sprite bitmap */
          const const char * const sprite_bitmap = (char *)sph + sph->image;
          DEBUGF("Source bitmap is at %p\n", sprite_bitmap);

          /* Copy sprite's bitmap as the sky pixel data, one row at a time
             (same pixel format etc) */
          for (int row = 0; row < SFSky_Height; row++)
          {
            /* Note that the bitmap is flipped vertically during copying */
            /* eg row 0-row 3,row 1-row 2,row 2-row 1, row 3-row 0 */
            memcpy(sky->pixel_data[SFSky_Height - 1 - row],
                   sprite_bitmap + row * WORD_ALIGN(SFSky_Width),
                   SFSky_Width);
          } /* next row of tile */
        }
      }
      else
      {
        DEBUGF("Not a valid sky\n");
        if (query_cb != NULL)
        {
          if (!query_cb(cb_arg, name))
          {
            DEBUGF("Conversion aborted by query callback\n");
            break;
          }
        }
      }
    }
    else
    {
      DEBUGF("Skipping sprite %u:%p\n", sp, (void *)sph);
    }

    /* Calculate address of next sprite */
    sph = (SpriteHeader *)((char *)sph + sph->size);
  } /* loop back (next image) */

  DEBUGF("Required buffer size is %zu\n", output_size);
  return output_size;
}
