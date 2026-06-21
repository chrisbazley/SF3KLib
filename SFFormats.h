/*
 * SF3KLib: Definitions of data formats for the game 'Star Fighter 3000'
 * Copyright (C) 2003 Christopher Bazley
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

/* SFFormats.h declares many types and macros for use in programs that deal
   with file types and data formats for the game 'Star Fighter 3000'.

Dependencies: None
Message tokens: None
History:
  CJB: 13-Sep-03: Changed the names of various of the types and structures.
  CJB: 31-Oct-04: Added a tag field to the _SF_GroundMapStampHdr structure.
  CJB: 04-Nov-04: Added summary and dependency information.
  CJB: 28-Apr-05: Added macro definitions of map array dimensions.
  CJB: 03-Jul-05: Made some members of _SFGroundMapAnimation unsigned and substituted
                  array for free_A...C in _SFGroundMapAnimationsHdr.
  CJB: 03-Jul-05: To prevent ambiguity updated all declarations to use types
                  of guaranteed width as defined in <stdint.h>. Renamed fourth
                  field of SF_FlightPathPoint.
  CJB: 22-Jul-05: Changed type of _SF_GroundMapStampHdr member 'tag' to uint32_t
                  to allow easier comparison with STAMPS_TAG.
  CJB: 24-Jul-05: Renamed SF_GroundMapStampHdr type as SF_GroundMapTransferHdr
                  (and similar for associated macro definitions).
  CJB: 24-Aug-05: Substituted an array for _SFMission members 'cloud_colour_1'
                  and 'cloud_colour_2'.
  CJB: 16-Sep-05: Incremented TRANSFERS_FORMAT_VERSION for changed usage of
                  width & height members of SF_GroundMapTransferHdr.
  CJB: 23-Oct-06: Changed type of SF_HillCols into a one dimensional array of
                  36 elements instead of a two dimensional array of 3 by 12.
  CJB: 01-Nov-06: Added export file formats used by SFColours and SFSkyEdit.
                  Renamed some structure members and symbolic constants for
                  clarity. Removed the 'planets' member of the _SF_PlanetsSetHdr
                  structure because the image data needn't follow immediately.
  CJB: 10-Nov-06: Removed export file format used by SFSkyEdit (unnecessary).
  CJB: 18-Nov-06: Corrected the definitions of types SF_MapTile and
                  SF_PlanetBitmap to be [rows][columns] rather than vice-versa.
                  Added symbolic definitions of the width and height of the
                  pixel data for a sky, and modified SF_SkyColours to represent
                  it as a two dimensional array of bytes (instead of 126 words).
  CJB: 23-Jan-07: Added SF_PolyObj... structure and macro value definitions to
                  describe the file format for polygonal graphics. Also new
                  structure SF_ObjectsGridTransferHdr for the map editor.
  CJB: 09-Sep-09: Stop using reserved identifiers (starting with underscores
                  followed by a capital letter) as structure tags.
  CJB: 05-Oct-09: Renamed a huge number of types and macro values (for which
                  enumerated constants have been substituted where possible)
                  to follow Toolbox library conventions. A subset of the old
                  names are supported via macro aliases.
  CJB: 26-Jun-10: Made definition of deprecated type and constant names
                  conditional upon definition of CBLIB_OBSOLETE.
  CJB: 21-Jun-26: Make some integer types signed in the SFColours export
                  structure definitions.
*/
#ifndef SFFormats_h
#define SFFormats_h

/* ISO library headers */
#include <stdint.h>

/* Unofficial game file types */
enum
{
  FileType_Fednet   = 0x154,
  FileType_SFObjGfx = 0x300,
  FileType_SFBasMap = 0x400,
  FileType_SFOvrMap = 0x401,
  FileType_SFBasObj = 0x402,
  FileType_SFOvrObj = 0x403,
  FileType_SFSkyCol = 0x404,
  FileType_SFMissn  = 0x405,
  FileType_SFSkyPic = 0x406,
  FileType_SFMapGfx = 0x407,
  FileType_SFMapAni = 0x408
};

/*
  Level animations
  Purpose: Holds a list of ground map animations for a given mission.
*/

typedef struct
{
  int32_t  map_offset;
  int32_t  timer_counter;
  uint16_t reset_timer; /* b1,b0 = Timer reset ctr */
  uint16_t current_tile; /* b3,b2 = Current Spr# */
  uint32_t tiles[4]; /* Sprite # byte, Expanded on load to sprite location */
}
SFGroundMapAnimation;

typedef struct
{
  int32_t              num_animations; /* Number of animations held */
  int32_t              free[3]; /* FREE Header data */
  /*-- animations follow here in memory --*/
  SFGroundMapAnimation array[];
}
SFGroundMapAnimationSet;

/*
  Polygonal graphics set
  Purpose: Defines a complete set of polygonal objects - including all
           fighters, spaceships, ground installations, bonus coins etc.
*/

/* At the start of the file are command sequences which control plotting of
   complex objects (i.e. those for which (plot_type_and_last_group &
   SFObject_PlotTypeMask) != 0. Each command sequence is terminated by
   SFPlotCommands_EndOfType. A command may be one or two bytes long. Bits
   5-7 of the first byte specify a condition to control whether a group of
   facets should be plotted and, if so, whether facets should then be culled
   individually. Bits 0-4 encode an operand which is usually the vector test
   upon which plotting is predicated. The following byte usually gives the
   group number to be plotted if the vector test passes. Commands with action
   SFPlotAction_FacingAlways are instead encoded as a single byte: bits 0-4
   give the group to be plotted. The end of the commands data is marked by
   SFPlotCommands_EndOfData. */

#define SFPlotCommands_OperandMask 0x1fu
#define SFPlotCommands_ActionMask  0xe0u

enum
{
  SFPlotCommands_OperandShift = 0,
  SFPlotCommands_ActionShift  = 5,
  SFPlotCommands_EndOfType    = 255, /* Followed by data for next plot type,
                                        or SFPlotCommands_EndOfData */
  SFPlotCommands_EndOfData    = 254 /* Indicates no more plot types */
};

typedef enum
{
  SFPlotAction_FacingAlways, /* Always plot facing facets in the group specified
                                by bits 0-4. */
  SFPlotAction_FacingIf,     /* Plot facing facets in the group specified by the
                                next byte if the vector test specified by bits
                                0-4 passes. */
  SFPlotAction_FacingIfNot,  /* Plot facing facets in the group specified by the
                                next byte if the vector test specified by bits
                                0-4 fails. */
  SFPlotAction_AllIf,        /* Plot all facets in the group specified by the
                                next byte if the vector test specified by bits
                                0-4 passes. */
  SFPlotAction_AllIfNot      /* Plot all facets in the group specified by the
                                next byte if the vector test specified by bits
                                0-4 fails. */
}
SFPlotAction;

/* The explosions data for the first polygonal object follows the vectors data
   at the first word aligned address that is at least 4 bytes ahead of the '254'
   terminator. Silly calculation is (n + 7) AND NOT 3; should have been
   (n + 4) AND NOT 3. */

typedef struct
{
  int32_t x0; /* sign inverted and multiplied by 128 on loading */
  int32_t y0; /* sign inverted and multiplied by 128 on loading */
  int32_t z0; /* multiplied by 128 on loading */
  int32_t x1; /* sign inverted and multiplied by 128 on loading */
  int32_t y1; /* sign inverted and multiplied by 128 on loading */
  int32_t z1; /* multiplied by 128 on loading */
  int32_t unknown[2];
  int32_t num_groups;
}
SFObjectExplosion;

typedef struct
{
  int32_t           last_explosion_num;
  SFObjectExplosion explosions[];
}
SFObjectExplosionSet;

/* The graphics data follows immediately after the explosions data
   (address should already be word aligned) */

typedef enum
{
  SFObjectType_Ground, /* Ground objects (trees, buildings) */
  SFObjectType_Bit,    /* (Broken?) bits */
  SFObjectType_Aerial  /* Aerial things (fighters, coins, missiles)*/
}
SFObjectType;

#define SFObjectCollisionSize_YMask 0x0fu
#define SFObjectCollisionSize_XMask 0xf0u

enum
{
  SFObjectCollisionSize_YShift = 0,
  SFObjectCollisionSize_XShift = 4,
};

#define SFObject_PlotTypeMask 0x0fu
#define SFObject_LastGroupMask 0xf0u

enum
{
  SFObject_PlotTypeShift = 0,
  SFObject_LastGroupShift = 4
};

typedef enum
{
  SFVertexCoord_SubMul32 = 85,
  SFVertexCoord_SubMul16,
  SFVertexCoord_SubMul8,
  SFVertexCoord_SubMul4,
  SFVertexCoord_SubMul2,
  SFVertexCoord_SubUnit, /* subtract unit vector from previous coordinate */
  SFVertexCoord_SubDiv2 = 96,
  SFVertexCoord_SubDiv4,
  SFVertexCoord_SubDiv8,
  SFVertexCoord_SubDiv16,
  SFVertexCoord_Zero,    /* no change from previous coordinate */
  SFVertexCoord_AddDiv16,
  SFVertexCoord_AddDiv8,
  SFVertexCoord_AddDiv4,
  SFVertexCoord_AddDiv2,
  SFVertexCoord_AddUnit = 110, /* add unit vector to previous coordinate */
  SFVertexCoord_AddMul2,
  SFVertexCoord_AddMul4,
  SFVertexCoord_AddMul8,
  SFVertexCoord_AddMul16,
  SFVertexCoord_AddMul32,
}
SFVertexCoord;

typedef enum
{
  SFCoordinateScale_Small,
  SFCoordinateScale_Medium,
  SFCoordinateScale_Large
}
SFCoordinateScale;

typedef struct
{
  uint8_t type; /* 0:ground, 1:bit or 2:aerial (see above) */
  uint8_t coords_scale; /* 0, 1 or 2 */
  uint8_t radar_rotator;
  uint8_t gr_obj_coll_size; /* For ground objects, the packed collision area in
                               map tiles (x size in bits 4-7, y in bits 0-3) */
  uint16_t x_clip_size; /* For non-bits, multiplied by 2 when loaded */
  uint16_t y_clip_size; /* For non-bits, multiplied by 2 when loaded */
  uint8_t score; /* For non-bits, multiplied by 25 when loaded */
  uint8_t hits_or_min_z; /* No. of hits a ground object can take, or minimum
                            height for a big ship (aerial objects 13-15) */
  /* (Can't use a union because the start isn't word-aligned) */
  uint8_t explosion_style; /* For non-bits */
  uint8_t plot_type_and_last_group; /* Highest facets group no. in bits 4-7,
                                       sequence of plot commands to use in
                                       bits 0-3 */
  uint8_t num_vertices;
  uint8_t coords[]; /* Each vertex is three bytes x,y,z (can't use a structure
                       because the start of each vertex is not word-aligned).
                       Interpretation of coordinate values is arcane:
                       85...90   : -2 to the power of 5...0
                       96...99   : -1 / (2 to the power of 1...4)
                       100       : 0
                       101...104 : 1 / (2 to the power of 4...1)
                       110...115 : 2 to the power of 0...5 */
}
SFObjectData1;

/* The clip distance and facets data follows at the first word-aligned address
   after the array of coordinates for the same polygonal object. */

typedef struct
{
  int32_t clip_distance;
  uint8_t num_facets;
  uint8_t packed_facets_data[]; /* at offset 5 */
}
SFObjectData2;

#define SFObjectFacet_NumSidesMask  0x0fu
#define SFObjectFacet_GroupMask     0x70u
#define SFObjectFacet_SpecialColour 0x80u

enum
{
  SFObjectFacet_NumSidesShift = 0,
  SFObjectFacet_GroupShift    = 4,
  SFObjectFacet_VectorsGroup  = 7
};

/* The following structure represents the data for a single facet (except its
   colour). Beware that the start of the data for each facet is not aligned. */

typedef struct
{
  uint8_t num_sides_and_group; /* Number of sides for facet is in bits 0-3,
                                  group number (for clipping) is in bits 4-6,
                                  if set, bit 7 adds 256 to the colour number */
  uint8_t vertex_refs[];
}
SFObjectFacet;

/* Immediately after the last vertex reference is a byte to give the colour
   number for this facet (0-255, or +256 if flag bit 7 set). */

/* The collision data follows at the first word-aligned address after the
   packed facets data for the same polygonal object. */

typedef struct
{
  int32_t type;
  int32_t x0; /* coordinates are divided by 128 when loaded */
  int32_t y0;
  int32_t z0;
  int32_t x1;
  int32_t y1;
  int32_t z1;
}
SFObjectCollisionBox;

typedef struct
{
  int32_t              last_collision_num;
  int32_t              unknown[2];
  SFObjectCollisionBox collisions[];
}
SFObjectCollisionBoxSet;

/* The graphics data for the next polygonal object follows 4 bytes after the
   last collision box for the preceding object (no obvious reason for this gap).
   The address should already be word aligned. If there are no more objects
   then the next word (i.e. at +8) will be 99. */

enum
{
  SFObjects_EndOfData = 99
};

/*
  Ground map/overlay file format
  Purpose: Holds a base ground map or mission-specific overlay.
*/

enum
{
  SFGroundMap_Width  = 256,
  SFGroundMap_Height = 256
};

typedef uint8_t SFGroundMap[SFGroundMap_Height][SFGroundMap_Width];

/*
  Map tiles transfer file format
  Purpose: Used by the level editor to hold smaller map areas
*/

#define SFGroundMapTransfer_Tag *(uint32_t *)"STMP"
#define SFGroundMapTransfer_HasAnimations (1u << 0)

enum
{
  SFGroundMapTransfer_Version = 2,
};

typedef struct
{
  uint32_t tag;
  uint8_t version; /* for future expansion */
  uint8_t width; /* inclusive (+1) prior to version 2 */
  uint8_t height; /* inclusive (+1) prior to version 2 */
  uint8_t flags; /* meaningless in version 0 */
  /*-- tiles (same format as map overlay) follow here in memory --*/
  uint8_t tiles[][1]; /* dummy width to force 2 dimensional array */
}
SFGroundMapTransfer;

typedef struct
{
  uint32_t tag;
  uint8_t version; /* for future expansion */
  uint8_t width;
  uint8_t height;
  uint8_t flags; /* meaningless in version 0 */
  /*-- object refs (same format as grid overlay) follow here in memory --*/
  uint8_t objects[][1]; /* dummy width to force 2 dimensional array */
}
SFObjectsMapTransfer;

/*
   Colour numbers with positional information
   Purpose: Used by SFcolours when a selection is exported
*/

#define ExportColoursFile_Tag *(uint32_t *)"COLS"

enum
{
  ExportColoursFile_Version = 0
};

typedef struct
{
  int32_t x_offset, y_offset, colour;
}
ExportColoursFileRecord;

typedef struct
{
  uint32_t                tag;
  uint32_t                version;
  int32_t                 num_cols;
  ExportColoursFileRecord records[];
}
ExportColoursFile;

/*
  Object map/overlay file format
  Purpose: Holds a base ground objects map or mission-specific overlay.
*/

enum
{
  SFObjectsMap_Width  = 128,
  SFObjectsMap_Height = 128
};

typedef uint8_t SFObjectsMap[SFObjectsMap_Height][SFObjectsMap_Width];

/*
  Sky colour definition file
  Purpose: Stores the colours used to paint a planetary atmosphere.
*/

enum
{
  SFSky_Width  = 4,
  SFSky_Height = 126
};

typedef struct
{
  int32_t render_offset;
  int32_t min_stars_height;
  uint8_t pixel_data[SFSky_Height][SFSky_Width];
}
SFSky;

/*
  Mission file format
  Purpose: Stores all details about a mission, including the text
           briefing, location briefings, details of flying ships,
           flightpaths, ship performance data, map and graphics to use.
*/

/* Ground checks - triggered when object destroyed */
typedef enum
{
  SFGroundCheckAction_MissionTarget,
  SFGroundCheckAction_BonusMultiATA,
  SFGroundCheckAction_BonusMegaLaser,
  SFGroundCheckAction_BonusBombs,
  SFGroundCheckAction_BonusMines,
  SFGroundCheckAction_DefenceTimer,
  SFGroundCheckAction_ChainReaction,
  SFGroundCheckAction_Dummy,        /* may be used as base of chain reaction */
  SFGroundCheckAction_CrippleShipType,
  SFGroundCheckAction_CashBonus,
  SFGroundCheckAction_MissionTimer,
  SFGroundCheckAction_FriendlyDead,
  /* investigate this gap */
  SFGroundCheckAction_FixScanners = 13
}
SFGroundCheckAction;

typedef struct
{
  uint8_t x_pos;
  uint8_t y_pos;
  uint8_t action; /* see definition of SFGroundCheckAction */
  uint8_t parameter;
}
SFGroundCheck;

typedef int32_t SFLocationBrief;

typedef struct
{
  int32_t laser_freq;
  int32_t laser_type;
  int32_t engine;           /* Fighters only */
  int32_t control;          /* Fighters only */
  int32_t shields;
  int32_t ATA_freq;         /* Fighters only */
  int32_t pleb_launch_freq; /* Big ships only */
  int32_t num_plebs;        /* Big ships only */
}
SFShipPerformance;

/* Ship type controls performance and which polygonal object is used.
   Appearance also varies according to the selected polygonal objects set
   (names below give a rough idea). */
typedef enum
{
  SFShipType_Fighter1 = 1, /* Cobra / Blackbird / FIGHTER1 / FIGHTER3 */
  SFShipType_Fighter2,     /* Avenger / FIGHTER1 / FIGHTER4 / TIE_F */
  SFShipType_Fighter3,     /* Avenger / Phantom / Sharke W2 / TIE_F2 */
  SFShipType_Fighter4,     /* Predator Mk IV / Sharke W2 / SkyHawk-A / Sabre /
                              NEW_ONE7 */
  SFShipType_Big1 = 13,    /* Transporter / SkyTrain / TANKER / STAR_TREK /
                              MEGA_SHIP / WARRIOR */
  SFShipType_Big2,         /* TriWing (Mothership) */
  SFShipType_Big3,         /* Polar V / StarBase / Excalibur / DEATH_SHIP /
                              KLINGON2 */
  SFShipType_Satellite = 21 /* Sentinel (Defence/Comms) */
}
SFShipType;

/* Constants for first flags byte in SFShip structure */
#define SFShipFlag_ModeMask 0x3u      /* see definition of SFShipMode */
#define SFShipFlag_Friendly (1u << 2) /* otherwise enemy */
#define SFShipFlag_Cloaked  (1u << 3)

typedef enum
{
  SFShipMode_Attack,
  SFShipMode_FlightPath,
  SFShipMode_FollowShip
}
SFShipMode;

/* Constants for second flags byte in SFShip structure */
#define SFShipFlag2_IgnoreAttacks   (1u << 0)
#define SFShipFlag2_ProximityAction (1u << 1) /* join formation or attack */

/* Significance of a ship to the player's mission */
typedef enum
{
  SFShipMission_NotImportant,
  SFShipMission_Target,
  SFShipMission_Protect,
  SFShipMission_ProtectUntilArrival,
  SFShipMission_NoSpecialData,
  SFShipMission_PreventLanding
}
SFShipMission;

/* Behaviour patterns of big ships */
typedef enum
{
  SFShipBehaviour_Moving,
  SFShipBehaviour_RotateHorizontally, /* for space stations */
  SFShipBehaviour_RotateVertically,   /* for space stations */
  SFShipBehaviour_Stationary
}
SFShipBehaviour;

typedef struct
{
  int32_t x_pos;
  int32_t y_pos;
  int32_t height;
  uint8_t type; /* see definition of SFShipType */
  uint8_t flags_1; /* see definitions of SFShipFlag_... */
  uint8_t follow_path_or_ship; /* flight path number (0-7) or ship number
                                  (0-15, 255=follow players ship) */
  /* (Can't use a union because the start isn't word-aligned) */
  int32_t x_follow_offset;
  int32_t y_follow_offset;
  int32_t height_follow_offset;
  uint8_t mission_significance; /* see definition of SFShipMission */
  uint8_t flags_2; /* see definitions of SFShipFlag2_... */
  uint8_t big_ship_movement; /* see definition of SFShipBehaviour */
  uint8_t direction_and_pilot;
  /*  [b0-b3]	Pilot's name (0-15)
      [b4-b7]	Starting direction (0-15) */
}
SFShip;

enum
{
  SFMission_PlayerNotDocked = 255,
  SFMission_NumCloudColours = 2,
  SFMission_NumGroundChecks = 64,
};

typedef struct
{
  uint8_t x_coord;
  uint8_t y_coord;
  uint8_t z_coord;
  uint8_t free;
}
SFFlightPathPoint;

typedef struct
{
  int32_t           number_of_points;
  SFFlightPathPoint points[64];
}
SFFlightPath;

typedef char SFFilename[12];

typedef struct
{
  /* Player starting position */
  uint8_t player_x_pos;
  uint8_t player_y_pos;
  uint8_t player_height;
  uint8_t player_direction;

  /* Special ship details */
  uint8_t player_has_special_ship; /* (0=No, 1=Yes) */
  uint8_t special_ship_laser_type;
  uint8_t special_ship_engine;
  uint8_t special_ship_control;
  uint8_t special_ship_shields;
  uint8_t special_ship_ATA;
  uint8_t special_ship_ATG;
  uint8_t special_ship_mines;
  uint8_t special_ship_bombs;
  uint8_t special_ship_mega_laser;
  uint8_t special_ship_multi_ATA;
  uint8_t special_ship_type;

  /* Misc stuff */
  uint8_t num_hits_against_allowed;
  uint8_t plebadder_type1_2;
  uint8_t plebadder_type3_4;
  uint8_t space_mission; /* (0=No, 1=Yes) */
  uint8_t cloud_colours[SFMission_NumCloudColours];
  uint8_t dock_to_finish; /* (0=No, 1=Yes) */
  uint8_t start_docked; /* (SFMission_PlayerNotDocked if not docked) */

  /* Ground targets/check objects */
  int32_t   num_ground_checks;
  SFGroundCheck ground_checks[SFMission_NumGroundChecks];

  /* Location briefings */
  int32_t         num_location_briefings;
  SFLocationBrief location_briefings[16];

  /* Timers */
  int32_t mission_timer;
  int32_t defence_timer;

  /* General mission difficulty */
  uint8_t laser_fire_freq;
  uint8_t laser_fire_type;
  uint8_t launch_type_and_number;
  uint8_t hangar_launch_freq;

  /* Mission number */
  int32_t level_number;
  int32_t pyramid_number;

  /* Special mission variables */
  uint8_t scanner_broken; /* static displayed instead of map if value is 1 */
  uint8_t impervious_ground; /* prevents ground damage if non-zero */
  uint8_t free[30]; /* reserved for other variables */

  /* Ship performance data */
  SFShipPerformance ships_performance[7]; /* for types 1-4, 13-15 */

  /* Flying ships list */
  int32_t num_ships_flying;
  SFShip  ships_flying[32];

  /* Flightpaths */
  int32_t      num_flight_paths;
  SFFlightPath flight_paths[8];

  /* Associated / component files */
  SFFilename map_tiles_basemap;
  SFFilename map_tiles_levmap;
  SFFilename ground_objects_basemap;
  SFFilename ground_objects_levmap;
  SFFilename animations;
  SFFilename map_tiles_set;
  SFFilename polygonal_objects_set;
  SFFilename sky_colour_bands;
  SFFilename planet_pictures_set;
  SFFilename palette;
  SFFilename hill_colours;

  /* Mission and location briefing text */
  int32_t text_briefs;
  int32_t text_visits;
  int32_t text_string_offsets[19];
  uint8_t title[32];
  uint8_t text_strings[3*1024];
}
SFMission;

typedef struct
{
   int32_t header[12];
   /*-- string follows here in memory, terminated by 255 --*/
   uint8_t string[];
}
SFTextString;


/*
  Planets file format
  Purpose: Contains the pictures of distant planets painted onto the sky.
*/

enum
{
  SFPlanet_Width  = 36,
  SFPlanet_Height = 36
};

typedef uint8_t SFPlanetBitmap[SFPlanet_Height][SFPlanet_Width];

typedef struct
{
   int32_t x_offset;
   int32_t y_offset;
}
SFPlanetPaintOffset;

typedef struct
{
   uint32_t image_A;
   uint32_t image_B;
}
SFPlanetBitmapOffset;

typedef struct
{
   int32_t              last_image_num;
   SFPlanetPaintOffset  paint_offsets[2];
   SFPlanetBitmapOffset data_offsets[2];
}
SFPlanetsHeader;

/*
  Map tile graphics set format
  Purpose: Contains the the full range of available map tiles in a given set.
*/

enum
{
  SFMapTile_Width  = 16,
  SFMapTile_Height = 16
};

typedef uint8_t SFMapTile[SFMapTile_Height][SFMapTile_Width];

typedef struct
{
   int32_t last_tile_num;
   uint8_t splash_anim_1[4];
   uint8_t splash_anim_2[4];
   uint8_t splash_2_triggers[4];
   /*-- tiles follow here in memory --*/
   SFMapTile tiles[];
}
SFMapTileSet;

/*
  Hill colours format
  Purpose: Gives the colour palette used for the polygonal mountains.
*/

typedef uint8_t SFHillColours[36];

/*
  Object colours format
  Purpose: Maps the 320 logical colours used on polygonal objects in the
           game to the standard static 256 colours available in mode 13.
*/

typedef union
{
  /* Plain and simple */
  uint8_t colour_mappings[320];

  /* Area allocations */
  struct
  {
    uint8_t static_colours[256];
    struct
    {
      uint8_t player_engine[4];
      uint8_t fighter_engine[4];
      uint8_t cruiser_engine[4];
      uint8_t super_engine[4];
    }
    engine_colours;
    struct
    {
      uint8_t enemy_ships[4];
      uint8_t friendly_ships[4];
      uint8_t player_ship[4];
    }
    fast_flashing;
    struct
    {
      uint8_t ground_obj_1[4];
      uint8_t ground_obj_2[4];
      uint8_t misc_1[4];
      uint8_t misc_2[4];
    }
    med_flashing;
    uint8_t player_livery[20];
  }
  areas;
}
SFObjectColours;

/*
  Music tracks format
  Purpose: Used by SFX_Handler module music player.
*/

typedef struct
{
  uint8_t pitch;
  uint8_t octave_and_volume;
  uint8_t sample_and_command;
  uint8_t repeats;
}
SFMusicCommand;

/* Encoding of note data:
  bits... 28 24       20      16      12       8  4      0
   {repeats}{ }{command}{sample}{volume}{octave}{ }{pitch} */

typedef struct
{
  SFMusicCommand commands[4];
}
SFMusicLine;
/* (each line is 16 bytes) */

typedef struct
{
  SFMusicLine lines[64];
}
SFMusicSheet;
/* (each sheet is 1024 bytes) */

typedef struct
{
  uint8_t tempo; /* Metronome value */
  uint8_t free[15]; /* FREE Header data */
  uint8_t note_trans[16]; /* Logical to actual voice mapping */
  int32_t number_sheets; /* Number of sheets */
  int32_t number_play; /* obsolete */
  uint8_t sheets_order[64]; /* Sheets play order (255 Term) */

  /*-- sheets follow here in memory --*/
  SFMusicSheet sheets[];
}
SFMusic;

/* Deprecated file type names */
#define FILETYPE_FEDNET    FileType_Fednet
#define FILETYPE_POLYOBJS  FileType_SFObjGfx
#define FILETYPE_BASEMAP   FileType_SFBasMap
#define FILETYPE_LEVELMAP  FileType_SFOvrMap
#define FILETYPE_BASEOBJS  FileType_SFBasObj
#define FILETYPE_LEVELOBJS FileType_SFOvrObj
#define FILETYPE_SKYCOLS   FileType_SFSkyCol
#define FILETYPE_MISSION   FileType_SFMissn
#define FILETYPE_PLANETS   FileType_SFSkyPic
#define FILETYPE_MAPTILES  FileType_SFMapGfx
#define FILETYPE_ANIMS     FileType_SFMapAni

/* Deprecated type names */
#define SF_SkyColours        SFSky
#define SF_PlanetPaintOffset SFPlanetPaintOffset
#define SF_PlanetDataOffset  SFPlanetBitmapOffset
#define SF_PlanetsSetHdr     SFPlanetsHeader
#define SF_PlanetBitmap      SFPlanetBitmap
#define SF_MapTile           SFMapTile
#define SF_MapTilesSetHdr    SFMapTileSet
#define SF_ColourMap         SFObjectColours
#define SF_HillCols          SFHillColours
#define SF_MusicCommand      SFMusicCommand
#define SF_MusicLine         SFMusicLine
#define SF_MusicSheet        SFMusicSheet
#define SF_MusicHdr          SFMusic
#define ExportColsFileHdr    ExportColoursFile
#define ExportColsFileRec    ExportColoursFileRecord

/* Deprecated enumeration constant names */
#define SF_SKY_WIDTH           SFSky_Width
#define SF_SKY_HEIGHT          SFSky_Height
#define SF_PLANET_WIDTH        SFPlanet_Width
#define SF_PLANET_HEIGHT       SFPlanet_Height
#define SF_MAPTILE_WIDTH       SFMapTile_Width
#define SF_MAPTILE_HEIGHT      SFMapTile_Height
#define EXPORTCOLSFILE_VERSION ExportColoursFile_Version
#define EXPORTCOLSFILE_TAG     ExportColoursFile_Tag

#endif /* SFFormats_h */
