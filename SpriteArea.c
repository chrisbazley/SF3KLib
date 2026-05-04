/*
 * SF3KLib: Initialise a RISC OS sprite area and allocate space within it
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
  CJB: 20-Aug-09: Created this source file.
  CJB: 30-Sep-09: Updated to use new SpriteAreaHeader and SpriteHeader type
                  names.
  CJB: 18-Apr-15: Assertions are now provided by debug.h.
  CJB: 27-Aug-15: Updated to allow for the fact that address offsets in a
                  sprite area can be negative.
  CJB: 09-Apr-16: Added casts to avoid GNU C compiler warnings about
                  unsigned comparisons with signed integers.
  CJB: 21-Apr-16: Cast pointer parameters to void * to match %p and
                  substituted %zu for %u to avoid casting parameters of
                  type size_t to unsigned int.
*/

/* ISO library headers */
#include <stddef.h>
#include <string.h>

/* CBOSLib headers */
#include "SprFormats.h"

/* Local headers */
#include "Internal/SF3KMisc.h"
#include "SpriteArea.h"

void spritearea_init(SpriteAreaHeader *sprite_area, size_t size)
{
  DEBUGF("Initialising header of sprite area at %p\n", (void *)sprite_area);

  assert(sprite_area != NULL);
  sprite_area->size = size;
  sprite_area->sprite_count = 0;
  sprite_area->first = sizeof(*sprite_area);
  sprite_area->used = sizeof(*sprite_area);
}

void *spritearea_alloc_ext(SpriteAreaHeader *sprite_area, size_t size)
{
  char *ext_data = NULL;

  DEBUGF("Allocating %zu bytes of extension data in sprite area %p\n",
        size, (void *)sprite_area);

  assert(sprite_area != NULL);

  const int area_used = sprite_area->used;
  const int area_size = sprite_area->size;
  const int area_first = sprite_area->first;

  assert(area_size >= 0);
  assert(area_used >= area_first);
  assert((size_t)area_size >= sizeof(*sprite_area) + (area_used - area_first));

  /* Unless the offset to the first free word is positive, we can't tell
     the amount of free space in the area. */
  if (area_used >= 0)
  {
    DEBUGF("%d bytes are free in the sprite area\n", area_size - area_used);
    assert(area_first >= (int)sizeof(*sprite_area));

    /* Guard against overrunning the end of the sprite area. */
    if (area_used + size <= (size_t)area_size)
    {
      ext_data = (char *)sprite_area + area_first;

      /* If there are any sprites in the sprite area then shift them upward
         to make room for (extra) extension data. */
      if (area_used > area_first)
      {
        DEBUGF("Moving %d bytes of sprite data from %p to %p\n",
              area_used - area_first, ext_data, ext_data + size);

        memmove(ext_data + size, ext_data, area_used - area_first);
      }

      sprite_area->used = area_used + size;
      sprite_area->first = area_first + size;

      assert(sprite_area->size >= sprite_area->used);
      assert(sprite_area->used >= sprite_area->first);

      DEBUGF("Remaining free space is %d\n", sprite_area->size - sprite_area->used);
    }
    else
    {
      DEBUGF("Not enough free space in sprite area\n");
    }
  }
  else
  {
    DEBUGF("Unable to determine free space in sprite area (offset %d)\n",
          area_used);
  }

  DEBUGF("Returning extension data pointer %p\n", ext_data);
  return ext_data;
}

SpriteHeader *spritearea_alloc_spr(SpriteAreaHeader *sprite_area, size_t size)
{
  SpriteHeader *sph = NULL;

  DEBUGF("Allocating %zu bytes for a new sprite in area %p\n",
        size, (void *)sprite_area);

  assert(sprite_area != NULL);

  const int area_used = sprite_area->used;
  const int area_size = sprite_area->size;

  assert(area_size >= 0);
  assert(area_used >= sprite_area->first);
  assert((size_t)area_size >= sizeof(*sprite_area) + (area_used - sprite_area->first));

  /* Unless the offset to the first free word is positive, we can't tell
     the amount of free space in the area. */
  if (area_used >= 0)
  {
    DEBUGF("%d bytes are free in the sprite area\n", area_size - area_used);

    /* Guard against overrunning the end of the sprite area. */
    if (area_used + size <= (size_t)area_size)
    {
      /* Calculate address of the free space within the sprite area */
      sph = (SpriteHeader *)((char *)sprite_area + area_used);

      /* Only initialise the offset to the next sprite */
      sph->size = size;

      sprite_area->sprite_count++;
      sprite_area->used = area_used + size;

      assert(sprite_area->size >= sprite_area->used);
      assert(sprite_area->used >= sprite_area->first);

      DEBUGF("No. of sprites is now %d, remaining free space is %d\n",
            sprite_area->sprite_count, sprite_area->size - sprite_area->used);
    }
    else
    {
      DEBUGF("Not enough free space in sprite area\n");
    }
  }
  else
  {
    DEBUGF("Unable to determine free space in sprite area (offset %d)\n",
          area_used);
  }

  DEBUGF("Returning sprite pointer %p\n", (void *)sph);
  return sph;
}
