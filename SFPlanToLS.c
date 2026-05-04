/*
 * SF3KLib: Convert 'Star Fighter 3000' planet image to RISC OS sprite
 * Copyright (C) 2011 Christopher Bazley
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
  CJB: 17-Jan-11: Created this source file.
  CJB: 18-Feb-12: Allow sprite names of maximum length (no trailing nuls)
                  to be generated when compiling for C99 library.
                  Additional assertions to detect string formatting errors
                  and buffer overflow/truncation. Fixed debugging output
                  of unterminated sprite names.
  CJB: 18-Apr-15: Assertions are now provided by debug.h.
  CJB: 19-Sep-15: Deleted a bad assertion (since 2011 if not earlier) that
                  sprite_name was not NULL even if not used.
  CJB: 09-Apr-16: Added casts and explicitly ignored the result of sprintf
                  to avoid GNU C compiler warnings.
  CJB: 21-Apr-16: Substituted format specifier %zu for %lu to avoid the need
                  to cast the matching parameter.
  CJB: 11-Nov-18: Use the no. of elements in the data_offsets array as an
                  upper bound on the number of images to convert.
                  Removed reliance on snprintf (not in older C library) by
                  generating the decimal part of the sprite name separately.
  CJB: 14-Nov-18: Allow the sprite name pointer to be null if no output
                  buffer is supplied (restoring original behaviour).
                  More C99-style declarations.
*/

/* ISO library headers */
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* CBOSLib headers */
#include "SprFormats.h"

/* Local headers */
#include "Internal/SF3KMisc.h"
#include "SFFormats.h"
#include "SFSprConv.h"

enum
{
  OldSpriteType = 13, /* Mode number */
  NewSpriteType = (SPRITE_INFO_NOT_MODE_SEL |
                   45 << SPRITE_INFO_HOZ_DPI_SHIFT |
                   45 << SPRITE_INFO_VER_DPI_SHIFT |
                   SPRITE_TYPE_8BPP << SPRITE_INFO_TYPE_SHIFT),
  SpriteSize    = sizeof(SpriteHeader) +
                  WORD_ALIGN(SFPlanet_Width - 2) * SFPlanet_Height
};

size_t sf_planets_to_lone_spr(SpriteHeader          *sprite,
                              size_t                 sprite_size,
                              const SFPlanetsHeader *planets,
                              unsigned int           n,
                              const char            *sprite_name,
                              bool                   new_format)
{
  size_t output_size;

  assert(planets != NULL);

  if (planets->last_image_num < 0 ||
      n > (unsigned int)planets->last_image_num ||
      n >= ARRAY_SIZE(planets->data_offsets))
  {
    output_size = SIZE_MAX; /* Bad planet number */
  }
  else
  {
    output_size = SpriteSize;
    if (sprite != NULL && sprite_size >= output_size)
    {
      assert(sprite_name != NULL);

      /* Initialise header of new sprite */
      DEBUGF("Initialising header of sprite %u at %p\n", n, (void *)sprite);
      sprite->size = output_size;
      memset(sprite->name, 0, sizeof(sprite->name));

      char numstr[16];
      int nout = sprintf(numstr, "%u", n);
      assert(nout >= 0); /* no formatting error */

      const int avail = (int)sizeof(sprite->name) - 1; /* -1 for _ */
      if (nout <= avail)
      {
        /* Note: may overwrite first byte of 'width' member with '\0'
           (sprite names of maximum length needn't be terminated). */
        nout = sprintf(sprite->name, "%.*s_%s",
               avail - nout, sprite_name, numstr);
        assert(nout >= 0); /* no formatting error */
      }
      DEBUGF("Sprite name is %.*s\n", (int)sizeof(sprite->name), sprite->name);

      /* We will chop 2 pixel columns off the left or right edge
         (depending on alignment) */
      sprite->width = WORD_ALIGN(SFPlanet_Width - 2) / 4 - 1;
      sprite->height = SFPlanet_Height - 1;
      sprite->left_bit = 0; /* lefthand wastage is deprecated */
      sprite->right_bit = SPRITE_RIGHT_BIT(SFPlanet_Width - 2, 8);
      sprite->image = sizeof(*sprite);
      sprite->mask = sizeof(*sprite);
      sprite->type = new_format ? NewSpriteType : OldSpriteType;

      /* Calculate address of sprite bitmap */
      char *const sprite_bitmap = (char *)sprite + sprite->image;

      /* Calculate address of left-aligned planet image bitmap */
      const char *const image_bitmap = (char *)planets +
                                       planets->data_offsets[n].image_A;

      /* Copy raw bitmap image to sprite area, one row at a time
         (same pixel format etc) */
      for (int row = 0; row < SFPlanet_Height; row++)
      {
        /* The first copy of the image is left-aligned, so we just
           chop the last two pixels off each row */
        memcpy(sprite_bitmap + row * WORD_ALIGN(SFPlanet_Width - 2),
               image_bitmap + row * WORD_ALIGN(SFPlanet_Width),
               SFPlanet_Width - 2);
      } /* next row of tile */
    }
  }

  DEBUGF("Required space for sprite is %zu\n", output_size);
  return output_size;
}
