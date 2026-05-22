/*
 * SF3KLib: Convert 'Star Fighter 3000' map tile to RISC OS sprite
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
  CJB: 09-Apr-16: Added a cast and explicitly ignored the return value of
                  sprintf to avoid GNU C compiler warnings.
  CJB: 21-Apr-16: Substituted format specifier %zu for %lu to avoid the need
                  to cast the matching parameter.
  CJB: 11-Nov-18: Removed reliance on snprintf (not in older C library) by
                  generating the decimal part of the sprite name separately.
  CJB: 14-Nov-18: Allow the sprite name pointer to be null if no output
                  buffer is supplied (restoring original behaviour).
                  More C99-style declarations.
  CJB: 14-Mar-26: Use type int instead of unsigned int for bitmap indices.
  CJB: 26-May-26: Try to avoid warning about assignment of output_size to int.
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
                  WORD_ALIGN(SFMapTile_Width) * SFMapTile_Height
};

size_t sf_tiles_to_lone_spr(SpriteHeader       *sprite,
                            size_t              sprite_size,
                            const SFMapTileSet *tiles,
                            int                 n,
                            const char         *sprite_name,
                            bool                new_format)
{
  size_t output_size;

  assert(tiles != NULL);

  if (n < 0 ||
      n > tiles->last_tile_num)
  {
    output_size = SIZE_MAX; /* Bad tile number */
  }
  else
  {
    output_size = SpriteSize;
    if (sprite != NULL && sprite_size >= output_size)
    {
      assert(sprite_name != NULL);

      /* Initialise header of new sprite */
      DEBUGF("Initialising header of sprite %d at %p\n", n, (void *)sprite);
      sprite->size = SpriteSize;
      memset(sprite->name, 0, sizeof(sprite->name));

      char numstr[16];
      int nout = sprintf(numstr, "%d", n);
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

      sprite->width = WORD_ALIGN(SFMapTile_Width) / 4 - 1;
      sprite->height = SFMapTile_Height - 1;
      sprite->left_bit = 0;
      sprite->right_bit = SPRITE_RIGHT_BIT(SFMapTile_Width, 8);
      sprite->image = sizeof(*sprite);
      sprite->mask = sizeof(*sprite);
      sprite->type = new_format ? NewSpriteType : OldSpriteType;

      /* Calculate address of sprite bitmap */
      char *const sprite_bitmap = (char *)sprite + sprite->image;

      /* Calculate address of end of the tile bitmap to be read */
      const char *const end_of_tile = (char *)tiles + sizeof(*tiles) +
                                      (n + 1) * WORD_ALIGN(SFMapTile_Width) *
                                      SFMapTile_Height;

      /* Append the raw bitmap to the output sprite, one row at a time
         (same pixel format etc) */
      for (int row = 0; row < SFMapTile_Height; row++)
      {
        /* Note that the bitmap is flipped vertically during copying */
        memcpy(sprite_bitmap + row * WORD_ALIGN(SFMapTile_Width),
               end_of_tile - (row + 1) * WORD_ALIGN(SFMapTile_Width),
               SFMapTile_Width);

      } /* next row of image */
    }
  }

  DEBUGF("Required space for sprite is %zu\n", output_size);
  return output_size;
}
