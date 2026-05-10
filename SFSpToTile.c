/*
 * SF3KLib: Convert RISC OS sprites to 'Star Fighter 3000' map tiles
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
  CJB: 30-Sep-09: Updated to use new SpriteAreaHeader and SpriteHeader type
                  names.
  CJB: 26-Jun-10: Updated to use new SF3000 type and constant names.
  CJB: 18-Apr-15: Assertions are now provided by debug.h.
  CJB: 09-Apr-16: Added checks for the count of sprites or tiles being -ve
                  and if not then cast them to unsigned to avoid GNU C
                  compiler warnings.
  CJB: 21-Apr-16: Substituted format specifier %zu for %u where necessary
                  to match the parameter type.
  CJB: 21-Nov-20: Pass the actual sprite name to the query callback instead
                  of the expected name prefix.
*/

/* ISO library headers */
#include <stdlib.h>
#include <ctype.h>
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

size_t sf_spr_to_tiles(SFMapTileSet           *tiles,
                       size_t                  tiles_size,
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

  if (tiles == NULL)
    tiles_size = 0;

  /* Ensure we do not read from beyond the end of the sprite area */
  if (sprite_area->sprite_count < 0)
    end = 0;
  else if (end > (unsigned int)sprite_area->sprite_count)
    end = sprite_area->sprite_count;

  assert(sprite_name != NULL);
  size_t const name_len = strlen(sprite_name);

  /* Calculate address of first sprite */
  const SpriteHeader *sph = (SpriteHeader *)((char *)sprite_area + sprite_area->first);

  /* We always have to iterate through the sprites from the beginning of the
     area until we find the first one to be converted. */
  for (unsigned int sp = 0; sp < end; sp++)
  {
    if (sp >= start)
    {
      unsigned long tile_num;
      char name[sizeof(sph->name) + 1];
      char *endp;

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
      if (strncmp(name, sprite_name, name_len) == 0 &&
          name[name_len] == '_' &&
          isdigit(name[name_len + 1]) &&
          (tile_num = strtoul(name + name_len + 1, &endp, 10)) <= 254 &&
          *endp == '\0' &&
          sph->width == WORD_ALIGN(SFMapTile_Width) / 4 - 1 &&
          sph->height == SFMapTile_Height - 1 &&
          sph->left_bit == 0 &&
          sph->right_bit == SPRITE_RIGHT_BIT(SFMapTile_Width, 8) &&
          sprite_has_8_bpp(sph))
      {
        DEBUGF("Valid map tile (no. %lu)\n", tile_num);
        size_t const req_size = sizeof(*tiles) +
                                ((size_t)tile_num + 1) * sizeof(tiles->tiles[0]);

        if (req_size > output_size)
          output_size = req_size;

        /* Guard against overrunning the end of the output buffer */
        if (output_size <= tiles_size)
        {
          /* Increase the record of the number of tiles, if necessary */
          if (tiles->last_tile_num < 0 ||
              tile_num > (unsigned long)tiles->last_tile_num)
          {
            tiles->last_tile_num = (int)tile_num;
          }

          /* Calculate address of start of sprite bitmap */
          const char * const sprite_bitmap = (char *)sph + sph->image;
          DEBUGF("Source bitmap is at %p\n", (void *)sprite_bitmap);

          /* Copy sprite's bitmap as the new map tile, one row at a time
             (same pixel format etc) */
          for (int row = 0; row < SFMapTile_Height; row++)
          {
            /* Note that the bitmap is flipped vertically during copying */
            memcpy(tiles->tiles[tile_num][SFMapTile_Height - row - 1],
                   sprite_bitmap + row * WORD_ALIGN(SFMapTile_Width),
                   SFMapTile_Width);

          } /* next row of tile */
        }
      }
      else
      {
        DEBUGF("Not a valid map tile\n");
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
