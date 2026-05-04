/*
 * SF3KLib: Convert a 'Star Fighter 3000' sky definition to a RISC OS sprite
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
  CJB: 18-Feb-12: Deleted redundant memset to initialize sprite name buffer.
                  Additional assertion to detect sprite name truncation.
                  Fixed debugging output of unterminated sprite names.
  CJB: 18-Apr-15: Assertions are now provided by debug.h.
  CJB: 10-Apr-16: Cast pointer parameters to void * to match %p.
  CJB: 14-Nov-18: More C99-style declarations.
*/

/* ISO library headers */
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

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
                  WORD_ALIGN(SFSky_Width) * SFSky_Height
};

size_t sf_sky_to_lone_spr(SpriteHeader *sprite,
                          size_t        sprite_size,
                          const SFSky  *sky,
                          const char   *sprite_name,
                          bool          new_format)
{
  assert(sky != NULL);

  if (sprite != NULL && sprite_size >= SpriteSize)
  {
    /* Initialise header of new sprite */
    DEBUGF("Initialising header of sprite at %p\n", (void *)sprite);
    sprite->size = SpriteSize;

    assert(sprite_name != NULL);
    assert(strlen(sprite_name) <= sizeof(sprite->name));
    strncpy(sprite->name, sprite_name, sizeof(sprite->name));
    /* Note: sprite names of maximum length needn't be terminated. */
    DEBUGF("Sprite name is %.*s\n", (int)sizeof(sprite->name), sprite->name);

    sprite->width = WORD_ALIGN(SFSky_Width) / 4 - 1;
    sprite->height = SFSky_Height - 1;
    sprite->left_bit = 0; /* lefthand wastage is deprecated */
    sprite->right_bit = SPRITE_RIGHT_BIT(SFSky_Width, 8);
    sprite->image = sizeof(*sprite);
    sprite->mask = sizeof(*sprite);
    sprite->type = new_format ? NewSpriteType : OldSpriteType;

    /* Calculate address of sprite bitmap */
    char *const sprite_bitmap = (char *)sprite + sprite->image;

    /* Append the raw bitmap to the output sprite, one row at a time
       (same pixel format etc) */
    for (int row = 0; row < SFSky_Height; row++)
    {
      /* Note that the bitmap is flipped vertically during copying */
      memcpy(sprite_bitmap + row * WORD_ALIGN(SFSky_Width),
             sky->pixel_data[SFSky_Height - 1 - row],
             SFSky_Width);
    } /* next row */
  }

  DEBUGF("Required space for sprite is %u\n", SpriteSize);
  return SpriteSize;
}
