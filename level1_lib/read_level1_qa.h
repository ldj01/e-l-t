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

#include "rxx_QualityBand.h" /* L1-7 Quality bit definitions */
#include "ias_const.h"       /* L8-9 Quality bit definitions */

#define L1QA_NO_CONF   0x0   /* no confidence level (not checked) */
#define L1QA_LOW_CONF  0x1   /* low confidence level */
#define L1QA_MED_CONF  0x2   /* medium confidence level */
#define L1QA_HIGH_CONF 0x3   /* high confidence level */

/* Data types */
typedef enum
{
    LEVEL1_BQA_PIXEL,
    LEVEL1_BQA_RADSAT,
    LEVEL1_BQA_PIXEL_L457,
    LEVEL1_BQA_RADSAT_L457,
    LEVEL1_BQA_PIXEL_L89,
    LEVEL1_BQA_RADSAT_L89
} Espa_level1_qa_type;

/* Function Prototypes */
FILE *open_level1_qa
(
    char *espa_xml_file,   /* I: input ESPA XML filename */
    char *l1_qa_file,      /* O: output Level-1 QA filename */
    int *nlines,           /* O: number of lines in the QA band */
    int *nsamps,           /* O: number of samples in the QA band */
    Espa_level1_qa_type *qa_category /* I/O: type of Level-1 QA data (L4-7,L8)*/
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
    uint16_t l1_qa_pix      /* I: Level-1 pixel QA value for current pixel */
)
{
    if (l1_qa_pix & IAS_QUALITY_BIT_FILL)
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
    uint16_t l1_qa_pix      /* I: Level-1 radsat QA value for current pixel */
)
{
    if (l1_qa_pix & IAS_QUALITY_BIT_TERRAIN_OCCLUSION)
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
    uint16_t l1_qa_pix      /* I: Level-1 radsat QA value for current pixel */
)
{
    if (l1_qa_pix & CC_MASK_DROPLINE)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  level1_qa_is_saturated

PURPOSE: Determines if the current Level-1 QA pixel is saturated for a given
         band.

RETURN VALUE:
Type = bool
true            Saturated pixel
false           Not a saturated pixel

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline uint8_t level1_qa_is_saturated
(
    uint16_t l1_qa_pix,      /* I: Level-1 radsat QA value for current pixel */
    int      band            /* I: Band number */
)
{
    static unsigned short band_saturation_bits[] =
    {
        IAS_QUALITY_BIT_RAD_SAT_B1,
        IAS_QUALITY_BIT_RAD_SAT_B2,
        IAS_QUALITY_BIT_RAD_SAT_B3,
        IAS_QUALITY_BIT_RAD_SAT_B4,
        IAS_QUALITY_BIT_RAD_SAT_B5,
        IAS_QUALITY_BIT_RAD_SAT_B6,
        IAS_QUALITY_BIT_RAD_SAT_B7,
        IAS_QUALITY_BIT_RAD_SAT_B8,
        IAS_QUALITY_BIT_RAD_SAT_B9,
        IAS_QUALITY_BIT_RAD_SAT_B10,
        IAS_QUALITY_BIT_RAD_SAT_B11
    };

    /* L7 band number translation */
    if (band == 61) band = 6;
    if (band == 62) band = 9;

    if (l1_qa_pix & band_saturation_bits[band - 1])
        return true;
    else
        return false;
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
    uint16_t l1_qa_pix      /* I: Level-1 pixel QA value for current pixel */
)
{
    if (l1_qa_pix & IAS_QUALITY_BIT_CLOUD)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  level1_qa_bit_number

PURPOSE: Returns the 0-relative bit number of the least-significant set bit for
a given QA field.  This can also be used to shift the QA value to get just
the desired QA field bits.

RETURN VALUE:
Type = int
Value = 0-15
If the given QA value has no bits set (0), 0 is returned.  Since this function
is used to do bit-shifting, it resuilts in a no-op.

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline int level1_qa_bit_number
(
    uint16_t l1_qa_pix      /* I: Level-1 pixel QA value */
)
{
    /* Determine the number of bits to shift for these confidence bits */
    int num_bits = sizeof(l1_qa_pix) * CHAR_BIT;
    int i;
    for (i = 0; i < num_bits; i++)
    {
        if (l1_qa_pix & 0x01 << i) 
            return i;
    }
    return 0;
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
    uint16_t l1_qa_pix      /* I: Level-1 pixel QA value for current pixel */
)
{
    return ((l1_qa_pix & IAS_QUALITY_BIT_CLOUD_11)
        >> level1_qa_bit_number(IAS_QUALITY_BIT_CLOUD_11));
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
    uint16_t l1_qa_pix      /* I: Level-1 pixel QA value for current pixel */
)
{
    return ((l1_qa_pix & IAS_QUALITY_BIT_CLOUD_SHADOW_11)
        >> level1_qa_bit_number(IAS_QUALITY_BIT_CLOUD_SHADOW_11));
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
    uint16_t l1_qa_pix      /* I: Level-1 pixel QA value for current pixel */
)
{
    return ((l1_qa_pix & IAS_QUALITY_BIT_SNOW_ICE_11)
        >> level1_qa_bit_number(IAS_QUALITY_BIT_SNOW_ICE_11));
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
    uint16_t l1_qa_pix      /* I: Level-1 pixel QA value for current pixel */
)
{
    return ((l1_qa_pix & IAS_QUALITY_BIT_CIRRUS_11)
        >> level1_qa_bit_number(IAS_QUALITY_BIT_CIRRUS_11));
}

#endif
