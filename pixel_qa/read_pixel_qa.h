/*****************************************************************************
FILE: read_pixel_qa.h
  
PURPOSE: Contains function prototypes for reading the Level-2 pixel QA band.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
*****************************************************************************/

#ifndef READ_PIXEL_QA_H
#define READ_PIXEL_QA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "error_handler.h"
#include "espa_metadata.h"
#include "parse_metadata.h"
#include "raw_binary_io.h"
#include "pixel_qa.h"

/* Function prototypes */
FILE *open_pixel_qa
(
    char *espa_xml_file,   /* I: input ESPA XML filename */
    char *l2_qa_file,      /* O: output pixel QA filename */
    int *nlines,           /* O: number of lines in the QA band */
    int *nsamps            /* O: number of samples in the QA band */
);

int read_pixel_qa
(
    FILE *fp_bqa,           /* I: pointer to pixel QA band open for reading */
    int nlines,             /* I: number of lines to read from the QA file */
    int nsamps,             /* I: number of samples to read from the QA file */
    uint16_t *pixel_qa      /* O: pixel QA band values for the specified number
                                  of lines (memory should be allocated for
                                  nlines x nsamps of size uint16 before calling
                                  this routine) */
);

void close_pixel_qa
(
    FILE *fp_bqa           /* I/O: pointer to the open pixel QA band; will be
                                   closed upon return */
);


/* Inline Function Prototypes */

/******************************************************************************
MODULE:  pixel_qa_is_fill

PURPOSE: Determines if the current Level-2 pixel QA pixel is fill

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
static inline bool pixel_qa_is_fill
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    if (((l2_qa_pix >> L2QA_FILL) & L2QA_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  pixel_qa_is_clear

PURPOSE: Determines if the current Level-2 pixel QA pixel is clear

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is clear
false           Pixel is not clear

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool pixel_qa_is_clear
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    if (((l2_qa_pix >> L2QA_CLEAR) & L2QA_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  pixel_qa_is_water

PURPOSE: Determines if the current Level-2 pixel QA pixel is water

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is water
false           Pixel is not water

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool pixel_qa_is_water
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    if (((l2_qa_pix >> L2QA_WATER) & L2QA_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  pixel_qa_is_cloud_shadow

PURPOSE: Determines if the current Level-2 pixel QA pixel is cloud shadow

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is cloud shadow
false           Pixel is not cloud shadow

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool pixel_qa_is_cloud_shadow
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    if (((l2_qa_pix >> L2QA_CLD_SHADOW) & L2QA_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  pixel_qa_is_snow

PURPOSE: Determines if the current Level-2 pixel QA pixel is snow

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is snow
false           Pixel is not snow

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool pixel_qa_is_snow
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    if (((l2_qa_pix >> L2QA_SNOW) & L2QA_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  pixel_qa_is_cloud

PURPOSE: Determines if the current Level-2 pixel QA pixel is cloud

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
static inline bool pixel_qa_is_cloud
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    if (((l2_qa_pix >> L2QA_CLOUD) & L2QA_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  pixel_qa_cloud_confidence

PURPOSE: Returns the cloud confidence value (0-3) for the current Level-2
pixel QA pixel value.

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
static inline uint8_t pixel_qa_cloud_confidence
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    return ((l2_qa_pix >> L2QA_CLOUD_CONF1) & L2QA_DOUBLE_BIT);
}

/******************************************************************************
MODULE:  pixel_qa_cirrus_confidence

PURPOSE: Returns the cirrus confidence value (0-3) for the current Level-2
pixel QA pixel value. These are valid for L8 only.

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
static inline uint8_t pixel_qa_cirrus_confidence
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    return ((l2_qa_pix >> L2QA_CIRRUS_CONF1) & L2QA_DOUBLE_BIT);
}

/******************************************************************************
MODULE:  pixel_qa_is_terrain_occluded

PURPOSE: Determines if the current Level-2 pixel QA pixel is terrain occluded.
This is valid for L8 only.

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
static inline bool pixel_qa_is_terrain_occluded
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    if (((l2_qa_pix >> L2QA_TERRAIN_OCCL) & L2QA_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}


#endif
