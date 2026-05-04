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

/* SprHas8bpp.h declares a function to check whether a sprite's colour depth
   is 8 bits per pixel

Dependencies: None
Message tokens: None
History:
  CJB: 22-Aug-09: Created this header file.
  CJB: 26-Jun-10: Updated to use new sprite type name.
*/

#ifndef SprHas8bpp_h
#define SprHas8bpp_h

/* ISO library headers */
#include <stdbool.h>

/* Local headers */
#include "SprFormats.h"

extern bool sprite_has_8_bpp(const SpriteHeader *sprite);

#endif
