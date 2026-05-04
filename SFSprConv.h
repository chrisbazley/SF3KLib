/*
 * SF3KLib: Convert between RISC OS sprites and 'Star Fighter 3000' formats
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

/* SFSprConv.h declares functions to convert bitmap graphics formats used
   internally by the game 'Star Fighter 3000' to RISC OS's standard sprite
   format, and vice-versa.

Dependencies: ANSI C library.
Message tokens: None.
History:
  CJB: 19-Aug-09: Created this header file.
  CJB: 30-Sep-09: Updated to use new SpriteAreaHeader and SpriteHeader type
                  names.
  CJB: 26-Jun-10: Updated to use new SF3000 type and constant names. Removed
                  unnecessary brackets from function type definitions.
  CJB: 16-Jan-10: Added functions that output a lone sprite instead of
                  creating sprite(s) in a proffered sprite area.
  CJB: 19-Sep-15: Documented which parameters are ignored if no output buffer
                  is specified.
*/

#ifndef SFBitmaps_h
#define SFBitmaps_h

/* ISO library headers */
#include <stdbool.h>
#include <stddef.h>

/* Local headers */
#include "SFFormats.h"
#include "SprFormats.h"

typedef bool SFBitmapsProgressFn(const void *arg, unsigned int n);
   /*
    * Type of function called back before processing each bitmap in the source
    * data, to allow the client to display the percentage done and check for
    * user input (if required). If it returns false then the conversion
    * operation will be aborted; otherwise it will continue.
    */

typedef bool SFBitmapsQueryFn(const void *arg, const char *name);
   /*
    * Type of function called back upon encountering a sprite unsuitable for
    * conversion to a 'Star Fighter 3000' bitmap, to allow the client to warn
    * the user and/or abort the conversion operation. If it returns false then
    * the operation will be aborted; otherwise it will continue.
    */

extern size_t sf_tiles_to_lone_spr(SpriteHeader       */*sprite*/,
                                   size_t              /*sprite_size*/,
                                   const SFMapTileSet */*tiles*/,
                                   unsigned int        /*n*/,
                                   const char         */*sprite_name*/,
                                   bool                /*new_format*/);
   /*
    * Converts a 'Star Fighter 3000' map tile to a sprite. May be called with
    * sprite == NULL to find the output sprite size, in which case sprite_name
    * and new_format are ignored. The sprite will be named by appending the
    * tile index n to a given prefix. If new_format is true then the sprite
    * will be given a new-style type specifier instead of a mode number.
    * If the returned size exceeds the output buffer size then the tile was
    * not converted.
    * Returns: the buffer size that would have been required to convert the
    *          tile had an output buffer been provided, or SIZE_MAX if the
    *          tile index was invalid.
    */


extern size_t sf_tiles_to_spr(SpriteAreaHeader    */*sprite_area*/,
                              const SFMapTileSet  */*tiles*/,
                              unsigned int         /*start*/,
                              unsigned int         /*end*/,
                              const char          */*sprite_name*/,
                              bool                 /*new_format*/,
                              SFBitmapsProgressFn */*prog_cb*/,
                              const void          */*cb_arg*/);
   /*
    * Converts the specified subset of 'Star Fighter 3000' map tiles to sprites
    * and writes them at the end of any existing data in a given sprite area
    * (which must already have been initialised). May be called with
    * sprite_area == NULL to find the amount of free space required, in which
    * case sprite_name and new_format are ignored. May be called with
    * start == 0 and end == UINT_MAX to convert all tiles in the set.
    * Each sprite will be named by appending the tile number to a given
    * prefix; it is the caller's responsibility to ensure that no sprites with
    * conflicting names already exist. If new_format is true then sprites will
    * be created with a new-style type specifier instead of a mode number. If a
    * callback function is specified then it will be called before converting
    * each tile. If the returned size exceeds the amount of space previously
    * free in the sprite area then some of the tiles were not converted.
    * Returns: the free space that would have been required to convert the
    *          specified tiles, had a sprite area been provided.
    */

extern size_t sf_spr_to_tiles(SFMapTileSet           */*tiles*/,
                              size_t                  /*tiles_size*/,
                              const SpriteAreaHeader */*sprite_area*/,
                              unsigned int            /*start*/,
                              unsigned int            /*end*/,
                              const char             */*sprite_name*/,
                              SFBitmapsProgressFn    */*prog_cb*/,
                              SFBitmapsQueryFn       */*query_cb*/,
                              const void             */*cb_arg*/);
   /*
    * Converts the contents of a sprite area to a 'Star Fighter 3000' map tiles
    * set (which must already have been initialised). May be called with
    * tiles == NULL to find the required size of the output buffer. May be
    * called with start == 0 and end == UINT_MAX to process all sprites. If a
    * progress callback function is specified then it will be called before
    * processing each sprite. Only correctly-named sprites with appropriate
    * dimensions and colour depth etc. will be converted; others will be
    * flagged up via the second callback function (if specified). If the
    * returned size exceeds the output buffer size then some suitable sprites
    * were not converted.
    * Returns: the buffer size that would have been required to convert all
    *          suitable sprites in the specified range, had an output buffer
    *          been provided.
    */

extern size_t sf_planets_to_lone_spr(SpriteHeader          */*sprite*/,
                                     size_t                 /*sprite_size*/,
                                     const SFPlanetsHeader */*planets*/,
                                     unsigned int           /*n*/,
                                     const char            */*sprite_name*/,
                                     bool                   /*new_format*/);
   /*
    * Converts a 'Star Fighter 3000' planet image to a sprite. May be called
    * with sprite == NULL to find the output sprite size, in which case
    * sprite_name and new_format are ignored. The sprite will be named by
    * appending the image index n to a given prefix. If new_format is
    * true then the sprite will be given a new-style type specifier instead of
    * a mode number. If the returned size exceeds the output buffer size then
    * the planet image was not converted.
    * Returns: the buffer size that would have been required to convert the
    *          planet image had an output buffer been provided, or SIZE_MAX if
    *          the image index was invalid.
    */

extern size_t sf_planets_to_spr(SpriteAreaHeader       */*sprite_area*/,
                                const SFPlanetsHeader  */*planets*/,
                                unsigned int            /*start*/,
                                unsigned int            /*end*/,
                                const char             */*sprite_name*/,
                                bool                    /*new_format*/,
                                SFBitmapsProgressFn    */*prog_cb*/,
                                const void             */*cb_arg*/);
   /*
    * Converts the specified subset of 'Star Fighter 3000' planet images to
    * sprites and writes them at the end of any existing data in a given sprite
    * area (which must already have been initialised). May be called with
    * sprite_area == NULL to find the amount of free space required, in which
    * case sprite_name and new_format are ignored. May be called with
    * start == 0 and end == UINT_MAX to convert all images in the set. Each
    * sprite will be named by appending the image number to a given prefix; it
    * is the caller's responsibility to ensure that no sprites with
    * conflicting names already exist. If new_format is true then sprites will
    * be created with a new-style type specifier instead of a mode number. If
    * a callback function is specified then it will be called before converting
    * each image. If the returned size exceeds the amount of space previously
    * free in the sprite area then some of the images were not converted.
    * Returns: the free space that would have been required to convert the
    *          specified images, had a sprite area been provided.
    */

extern size_t sf_spr_to_planets(SFPlanetsHeader        */*planets*/,
                                size_t                  /*planets_size*/,
                                const SpriteAreaHeader */*sprite_area*/,
                                unsigned int            /*start*/,
                                unsigned int            /*end*/,
                                const char             */*sprite_name*/,
                                SFBitmapsProgressFn    */*prog_cb*/,
                                SFBitmapsQueryFn       */*query_cb*/,
                                const void             */*cb_arg*/);
   /*
    * Converts the contents of a sprite area to a 'Star Fighter 3000' planet
    * bitmaps and writes them at the end of any existing data for a given
    * planets set (which must already have been initialised). May be called
    * with planets == NULL to find the required size of the output buffer.
    * Buffer sizes do not include the planets set header because there is no
    * fixed relationship between a planet number and the position of its
    * bitmaps within the output buffer (it depends on the order of the source
    * sprites). May be called with start == 0 and end == UINT_MAX to process
    * all sprites. If a progress callback function is specified then it will be
    * called before processing each sprite. Only correctly-named sprites with
    * appropriate dimensions and colour depth etc. will be converted; others
    * will be flagged up via the second callback function (if specified). If
    * the returned size exceeds the output buffer size then some suitable
    * sprites were not converted.
    * Returns: the buffer size (excluding header) that would have been required
    *          to convert all suitable sprites in the specified range, had an
    *          output buffer been provided.
    */

extern size_t sf_sky_to_lone_spr(SpriteHeader */*sprite*/,
                                 size_t        /*sprite_size*/,
                                 const SFSky  */*sky*/,
                                 const char   */*sprite_name*/,
                                 bool          /*new_format*/);
   /*
    * Converts a 'Star Fighter 3000' sky definition to a sprite. May be called
    * with sprite == NULL to find the output sprite size, in which case
    * sprite_name and new_format are ignored. If new_format is true
    * then the sprite will be given a new-style type specifier instead of a
    * mode number. If the returned size exceeds the output buffer size then the
    * sky definition was not converted.
    * Returns: the buffer size that would have been required to convert the
    *          sky definition, had an output buffer been provided.
    */

extern size_t sf_sky_to_spr(SpriteAreaHeader */*sprite_area*/,
                            const SFSky      */*sky*/,
                            const char       */*sprite_name*/,
                            bool              /*new_format*/);
   /*
    * Converts 'Star Fighter 3000' sky colours to a sprite and writes it at the
    * end of any existing data in a given sprite area (which must already have
    * been initialised). May be called with sprite_area == NULL to find the
    * amount of free space required, in which case sprite_name and new_format
    * are ignored. It is the caller's responsibility to ensure that no sprite
    * of the same name already exists. If new_format is true then the sprite
    * will be given a new-style type specifier instead of a mode number.
    * If the returned size exceeds the amount of space previously
    * free in the sprite area then the sky was not converted.
    * Returns: the free space that would have been required to convert the sky
    *          definition, had a sprite area been provided.
    */

extern size_t sf_spr_to_sky(SFSky                  */*sky*/,
                            size_t                  /*sky_size*/,
                            const SpriteAreaHeader */*sprite_area*/,
                            unsigned int            /*start*/,
                            unsigned int            /*end*/,
                            const char             */*sprite_name*/,
                            SFBitmapsProgressFn    */*prog_cb*/,
                            SFBitmapsQueryFn       */*query_cb*/,
                            const void             */*cb_arg*/);
   /*
    * Converts the contents of a sprite area to a 'Star Fighter 3000' sky
    * definition. May be called with sky == NULL to find the required size of
    * the output buffer. May be called with start == 0 and end == UINT_MAX to
    * process all sprites. If a progress callback function is specified then it
    * will be called before processing each sprite. Only a correctly-named
    * sprite with appropriate dimensions and colour depth etc. will be
    * converted; others will be flagged up via the second callback function (if
    * specified). If the returned size exceeds the output buffer size then a
    * suitable sprite was found but not converted.
    * Returns: the buffer size that would have been required to convert any
    *          suitable sprite found, had an output buffer been provided.
    */

#endif
