/*****************************************************************************
FILE: read_level1_qa.h
  
PURPOSE: Contains function prototypes for the Level-1 QA band manipulation.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
*****************************************************************************/

#ifndef READ_LEVEL1_QA_H
#define READ_LEVEL1_QA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "error_handler.h"
#include "espa_metadata.h"
#include "parse_metadata.h"
#include "raw_binary_io.h"

/* Defines */
/* Define the constants used for shifting bits and ANDing with the bits to
   get to the desire quality bits */
#define ESPA_L1_SINGLE_BIT 0x01             /* 00000001 */
#define ESPA_L1_DOUBLE_BIT 0x03             /* 00000011 */
#define ESPA_L1_DESIGNATED_FILL_BIT 0       /* one bit */
#define ESPA_L1_TERRAIN_OCCLUSION_BIT 1     /* one bit (L8/OLI) */
#define ESPA_L1_DROPPED_PIXEL_BIT 1         /* one bit (L4-7 TM/ETM+) */
#define ESPA_L1_RAD_SATURATION_BIT 2        /* two bits */
#define ESPA_L1_CLOUD_BIT 4                 /* one bit */
#define ESPA_L1_CLOUD_CONF_BIT 5            /* two bits */
#define ESPA_L1_CLOUD_SHADOW_CONF_BIT 7     /* two bits */
#define ESPA_L1_SNOW_ICE_CONF_BIT 9         /* two bits */
#define ESPA_L1_CIRRUS_CONF_BIT 11          /* two bits (L8/OLI) */

/* Data types */
typedef enum
{
    LEVEL1_L457, LEVEL1_L8
} Espa_level1_qa_type;

/* Function Prototypes */
FILE *open_level1_qa
(
    char *espa_xml_file,   /* I: input ESPA XML filename */
    char *l1_qa_file,      /* O: output Level-1 QA filename */
    int *nlines,           /* O: number of lines in the QA band */
    int *nsamps,           /* O: number of samples in the QA band */
    Espa_level1_qa_type *qa_category /* O: type of Level-1 QA data (L4-7, L8) */
);

int read_level1_qa
(
    FILE *fp_bqa,          /* I: pointer to the Level-1 QA band open for
                                 reading */
    int nlines,            /* I: number of lines to read from the QA file */
    int nsamps,            /* I: number of samples to read from the QA file */
    uint16_t *level1_qa    /* O: Level-1 QA band values for the specified
                                 number of lines (memory should be allocated
                                 for nlines x nsamps of size uint16 before
                                 calling this routine) */
);

void close_level1_qa
(
    FILE *fp_bqa           /* I/O: pointer to the open Level-1 QA band;will
                                   be closed upon return */
);

/* Inline Function Prototypes */

/******************************************************************************
MODULE:  level1_qa_is_fill

PURPOSE: Determines if the current Level-1 QA pixel is fill

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is fill
false           Pixel is not fill

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool level1_qa_is_fill
(
    uint16_t l1_qa_pix      /* I: Level-1 QA value for current pixel */
)
{
    if (((l1_qa_pix >> ESPA_L1_DESIGNATED_FILL_BIT) & ESPA_L1_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  level1_qa_is_terrain_occluded

PURPOSE: Determines if the current Level-1 QA pixel is terrain occluded

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is terrain occluded
false           Pixel is not terrain occluded

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool level1_qa_is_terrain_occluded
(
    uint16_t l1_qa_pix      /* I: Level-1 QA value for current pixel */
)
{
    if (((l1_qa_pix >> ESPA_L1_TERRAIN_OCCLUSION_BIT) & ESPA_L1_SINGLE_BIT)
          == 1)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  level1_qa_is_dropped_pixel

PURPOSE: Determines if the current Level-1 QA pixel is a dropped pixel

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Dropped pixel
false           Not a dropped pixel

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool level1_qa_is_dropped_pixel
(
    uint16_t l1_qa_pix      /* I: Level-1 QA value for current pixel */
)
{
    if (((l1_qa_pix >> ESPA_L1_DROPPED_PIXEL_BIT) & ESPA_L1_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  level1_qa_radiometric_saturation

PURPOSE: Returns the radiometric saturation value (0-3) for the current
Level-1 QA pixel.

RETURN VALUE:
Type = uint8_t
Value           Description
-----           -----------
0               Saturation bits are 00
1               Saturation bits are 01
2               Saturation bits are 10
3               Saturation bits are 11

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline uint8_t level1_qa_radiometric_saturation
(
    uint16_t l1_qa_pix      /* I: Level-1 QA value for current pixel */
)
{
    return ((l1_qa_pix >> ESPA_L1_RAD_SATURATION_BIT) & ESPA_L1_DOUBLE_BIT);
}


/******************************************************************************
MODULE:  level1_qa_is_cloud

PURPOSE: Determines if the current Level-1 QA pixel is a cloud

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is cloud
false           Pixel is not cloud

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool level1_qa_is_cloud
(
    uint16_t l1_qa_pix      /* I: Level-1 QA value for current pixel */
)
{
    if (((l1_qa_pix >> ESPA_L1_CLOUD_BIT) & ESPA_L1_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  level1_qa_cloud_confidence

PURPOSE: Returns the cloud confidence value (0-3) for the current Level-1 QA
pixel.

RETURN VALUE:
Type = uint8_t
Value           Description
-----           -----------
0               Cloud confidence bits are 00
1               Cloud confidence bits are 01
2               Cloud confidence bits are 10
3               Cloud confidence bits are 11

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline uint8_t level1_qa_cloud_confidence
(
    uint16_t l1_qa_pix      /* I: Level-1 QA value for current pixel */
)
{
    return ((l1_qa_pix >> ESPA_L1_CLOUD_CONF_BIT) & ESPA_L1_DOUBLE_BIT);
}


/******************************************************************************
MODULE:  level1_qa_cloud_shadow_confidence

PURPOSE: Returns the cloud shadow value (0-3) for the current Level-1 QA
pixel.

RETURN VALUE:
Type = uint8_t
Value           Description
-----           -----------
0               Cloud shadow bits are 00
1               Cloud shadow bits are 01
2               Cloud shadow bits are 10
3               Cloud shadow bits are 11

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline uint8_t level1_qa_cloud_shadow_confidence
(
    uint16_t l1_qa_pix      /* I: Level-1 QA value for current pixel */
)
{
    return ((l1_qa_pix >> ESPA_L1_CLOUD_SHADOW_CONF_BIT) & ESPA_L1_DOUBLE_BIT);
}


/******************************************************************************
MODULE:  level1_qa_snow_ice_confidence

PURPOSE: Returns the snow/ice value (0-3) for the current Level-1 QA
pixel.

RETURN VALUE:
Type = uint8_t
Value           Description
-----           -----------
0               Snow/ice bits are 00
1               Snow/ice bits are 01
2               Snow/ice bits are 10
3               Snow/ice bits are 11

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline uint8_t level1_qa_snow_ice_confidence
(
    uint16_t l1_qa_pix      /* I: Level-1 QA value for current pixel */
)
{
    return ((l1_qa_pix >> ESPA_L1_SNOW_ICE_CONF_BIT) & ESPA_L1_DOUBLE_BIT);
}


/******************************************************************************
MODULE:  level1_qa_cirrus_confidence

PURPOSE: Returns the cirrus confidence value (0-3) for the current Level-1 QA
pixel.

RETURN VALUE:
Type = uint8_t
Value           Description
-----           -----------
0               Cirrus confidence bits are 00
1               Cirrus confidence bits are 01
2               Cirrus confidence bits are 10
3               Cirrus confidence bits are 11

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline uint8_t level1_qa_cirrus_confidence
(
    uint16_t l1_qa_pix      /* I: Level-1 QA value for current pixel */
)
{
    return ((l1_qa_pix >> ESPA_L1_CIRRUS_CONF_BIT) & ESPA_L1_DOUBLE_BIT);
}

#endif
