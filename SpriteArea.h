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

/* SpriteArea.h declares functions useful for creating sprite areas and
   allocating space within them.

Dependencies: ANSI C library.
Message tokens: None.
History:
  CJB: 20-Aug-09: Created this header file.
  CJB: 30-Sep-09: Updated to use new SpriteAreaHeader and SpriteHeader type
                  names.
*/

#ifndef SpriteArea_h
#define SpriteArea_h

/* ISO library headers */
#include <stddef.h>

/* Local headers */
#include "SprFormats.h"

extern void spritearea_init(SpriteAreaHeader *sprite_area, size_t size);
   /*
    * Initialises the header of a sprite area of a given size, for future use.
    */

extern void *spritearea_alloc_ext(SpriteAreaHeader *sprite_area, size_t size);
   /*
    * Allocates space within a sprite area for the specified amount of
    * extension data, the initial value of which is indeterminate. Any sprites
    * in the area will be moved upwards in memory to make room for the new
    * extension data. Existing extension data will not be moved or modified.
    * Returns: a null pointer if there was insufficient free space in the
    *          sprite area, otherwise a pointer to the allocated space.
    */

extern SpriteHeader *spritearea_alloc_spr(SpriteAreaHeader *sprite_area,
                                          size_t            size);
   /*
    * Allocates space within a sprite area for a sprite of the specified size.
    * The initial value of the sprite data is indeterminate except for the
    * offset to the next sprite, which will be initialised.
    * Returns: a null pointer if there was insufficient free space in the
    *          sprite area, otherwise a pointer to the allocated space.
    */

#endif
