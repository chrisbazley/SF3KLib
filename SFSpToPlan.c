/*
 * SF3KLib: Convert RISC OS sprites to 'Star Fighter 3000' planet images
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
  CJB: 22-Aug-09: Created this source file.
  CJB: 30-Sep-09: Updated to use new SpriteAreaHeader and SpriteHeader type
                  names.
  CJB: 26-Jun-10: Updated to use new SF3000 type and constant names.
  CJB: 18-Apr-15: Assertions are now provided by debug.h.
  CJB: 09-Apr-16: Added checks for the count of sprites or images being -ve
                  and if not then cast them to unsigned to avoid GNU C
                  compiler warnings.
  CJB: 21-Apr-16: Substituted format specifier %zu for %u where necessary
                  to match the parameter type.
  CJB: 11-Nov-18: Use the no. of elements in the data_offsets array as the
                  upper bound on the number of images to convert.
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

size_t sf_spr_to_planets(SFPlanetsHeader        *planets,
                         size_t                  planets_size,
                         const SpriteAreaHeader *sprite_area,
                         unsigned int            start,
                         unsigned int            end,
                         const char             *sprite_name,
                         SFBitmapsProgressFn    *prog_cb,
                         SFBitmapsQueryFn       *query_cb,
                         const void             *cb_arg)
{
  size_t output_size = 0, mem_used = sizeof(*planets);

  assert(sprite_area != NULL);
  assert(start <= end);

  if (planets == NULL)
  {
    planets_size = 0;
  }
  else
  {
    /* Calculate the end of memory already used within the output buffer */
    for (int pl = 0; pl <= planets->last_image_num; pl++)
    {
      size_t end = planets->data_offsets[pl].image_A + sizeof(SFPlanetBitmap);
      if (end > mem_used)
        mem_used = end;

      end = planets->data_offsets[pl].image_B + sizeof(SFPlanetBitmap);
      if (end > mem_used)
        mem_used = end;
    }
  }
  DEBUGF("Amount of buffer space already used is %zu\n", mem_used);

  /* Ensure we do not read from beyond the end of the sprite area */
  if (sprite_area->sprite_count < 0)
    end = 0;
  if (end > (unsigned int)sprite_area->sprite_count)
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
      unsigned long image_num;
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
          (image_num = strtoul(name + name_len + 1, &endp, 10)) <
          ARRAY_SIZE(planets->data_offsets) &&
          *endp == '\0' &&
          sph->width == WORD_ALIGN(SFPlanet_Width - 2) / 4 - 1 &&
          sph->height == SFPlanet_Height - 1 &&
          sph->left_bit == 0 &&
          sph->right_bit == SPRITE_RIGHT_BIT(SFPlanet_Width - 2, 8) &&
          sprite_has_8_bpp(sph))
      {
        DEBUGF("Valid planet image (no. %lu)\n", image_num);

        output_size += sizeof(SFPlanetBitmap) * 2;

        /* Guard against overrunning the end of the output buffer */
        if (output_size <= planets_size)
        {
          const char *sprite_bitmap;
          char *image_A, *image_B;

          /* Increase the record of the number of images if necessary */
          if (planets->last_image_num < 0 ||
              image_num > (unsigned long)planets->last_image_num)
          {
            planets->last_image_num = (int)image_num;
          }

          /* Initialise the offsets to each copy of the bitmap image. We always
             write at the end of the existing data instead of overwriting any
             existing bitmaps for this image because it would be weird for the
             returned size requirement to vary depending upon whether an output
             buffer was specified. */
          planets->data_offsets[image_num].image_A = mem_used;
          mem_used += sizeof(SFPlanetBitmap);
          image_A = (char *)planets + planets->data_offsets[image_num].image_A;
          DEBUGF("Aligned bitmap will be written at %p\n", image_A);

          planets->data_offsets[image_num].image_B = mem_used;
          mem_used += sizeof(SFPlanetBitmap);
          image_B = (char *)planets + planets->data_offsets[image_num].image_B;
          DEBUGF("Non-aligned bitmap will be written at %p\n", (void *)image_B);

          /* Calculate address of start of sprite bitmap */
          sprite_bitmap = (char *)sph + sph->image;
          DEBUGF("Source bitmap is at %p\n", (void *)sprite_bitmap);

          /* We make two copies of the input sprite; one word-aligned and the
             other half-word aligned. This requires copying one row at a time.
           */
          for (int row = 0; row < SFPlanet_Height; row++)
          {
            /* The first copy of the image is word-aligned */
            memcpy(image_A + row * WORD_ALIGN(SFPlanet_Width),
                   sprite_bitmap + row * WORD_ALIGN(SFPlanet_Width - 2),
                   SFPlanet_Width - 2);

            /* The second copy of the image is half-word aligned */
            memcpy(image_B + row * WORD_ALIGN(SFPlanet_Width) + 2,
                   sprite_bitmap + row * WORD_ALIGN(SFPlanet_Width - 2),
                   SFPlanet_Width - 2);
          } /* next row of tile */
        }
      }
      else
      {
        DEBUGF("Not a valid planet image\n");
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
