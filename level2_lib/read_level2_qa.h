/*****************************************************************************
FILE: read_level2_qa.h

PURPOSE: Contains function prototypes for the LEDAPS and LaSRC Level-2 QA band
manipulation for collection products.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
*****************************************************************************/

#ifndef READ_LEVEL2_QA_H
#define READ_LEVEL2_QA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "error_handler.h"
#include "espa_metadata.h"
#include "parse_metadata.h"
#include "raw_binary_io.h"

/* Defines */

/* LEDAPS QA bits - cloud */
#define LEDAPS_DDV_BIT              0x01   /* Bit 0 */
#define LEDAPS_CLOUD_BIT            0x02   /* Bit 1 */
#define LEDAPS_CLOUD_SHADOW_BIT     0x04   /* Bit 2 */
#define LEDAPS_ADJ_CLOUD_BIT        0x08   /* Bit 3 */
#define LEDAPS_SNOW_BIT             0x10   /* Bit 4 */
#define LEDAPS_LAND_WATER_BIT       0x20   /* Bit 5 (1=land, 0=water) */

/* LaSRC QA bits - aerosol */
#define LASRC_FILL_BIT              0x01   /* Bit 0 */
#define LASRC_VALID_AEROSOL_RET_BIT 0x02   /* Bit 1 */
#define LASRC_WATER_BIT             0x04   /* Bit 2 */
#define LASRC_CLOUD_CIRRUS_BIT      0x08   /* Bit 3 */
#define LASRC_CLOUD_SHADOW_BIT      0x10   /* Bit 4 */
#define LASRC_AEROSOL_INTERP_BIT    0x20   /* Bit 5 */
#define LASRC_AEROSOL_LEVEL_BIT     0xC0   /* Bits 6-7 */
#define LASRC_AEROSOL_LEVEL_BIT_NUMBER  6  /* Bit number of the first aerosol 
                                              level bit */

/* Level-2 QA types */
typedef enum
{
    LEDAPS_CLOUD,
    LASRC_AEROSOL
} Espa_level2_qa_type;

/* Function Prototypes */
FILE *open_level2_qa
(
    char *espa_xml_file,   /* I: input ESPA XML filename */
    Espa_level2_qa_type qa_category, /* I: type of Level-2 QA data to be opened
                                (LEDAPS cloud, LaSRC aerosol) */
    char *l2_qa_file,      /* O: output Level-2 QA filename (memory must be
                                 allocated ahead of time) */
    int *nlines,           /* O: number of lines in the QA band */
    int *nsamps            /* O: number of samples in the QA band */
);

int read_level2_qa
(
    FILE *fp_l2qa,         /* I: pointer to the Level-2 QA band open for
                                 reading */
    int nlines,            /* I: number of lines to read from the QA file */
    int nsamps,            /* I: number of samples to read from the QA file */
    void *level2_qa        /* O: Level-2 QA band values for the specified
                                 number of lines (memory should be allocated
                                 for nlines x nsamps of size uint8 before
                                 calling this routine) */
);

void close_level2_qa
(
    FILE *fp_l2qa          /* I/O: pointer to the open Level-2 QA band; will
                                   be closed upon return */
);


/* Inline Function Prototypes */

/*** LEDAPS CLOUD QA ***/
/******************************************************************************
MODULE:  ledaps_qa_is_ddv

PURPOSE: Determines if the LEDAPS cloud QA pixel is DDV

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is DDV
false           Pixel is not DDV

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool ledaps_qa_is_ddv
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LEDAPS_DDV_BIT)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  ledaps_qa_is_cloud

PURPOSE: Determines if the LEDAPS cloud QA pixel is a cloud

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
static inline bool ledaps_qa_is_cloud
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LEDAPS_CLOUD_BIT)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  ledaps_qa_is_cloud_shadow

PURPOSE: Determines if the LEDAPS cloud QA pixel is a cloud shadow

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
static inline bool ledaps_qa_is_cloud_shadow
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LEDAPS_CLOUD_SHADOW_BIT)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  ledaps_qa_is_adj_cloud

PURPOSE: Determines if the LEDAPS cloud QA pixel is a adjacent cloud

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is adjacent cloud
false           Pixel is not adjacent cloud

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool ledaps_qa_is_adj_cloud
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LEDAPS_ADJ_CLOUD_BIT)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  ledaps_qa_is_snow

PURPOSE: Determines if the LEDAPS cloud QA pixel is snow

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
static inline bool ledaps_qa_is_snow
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LEDAPS_SNOW_BIT)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  ledaps_qa_is_land_water

PURPOSE: Determines if the LEDAPS cloud QA pixel is land (otherwise it's water)

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is land
false           Pixel is water

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool ledaps_qa_is_land_water
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LEDAPS_LAND_WATER_BIT)
        return true;
    else
        return false;
}


/*** LaSRC AEROSOL ***/
/******************************************************************************
MODULE:  lasrc_qa_is_fill

PURPOSE: Determines if the LaSRC aerosol QA pixel is fill

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
static inline bool lasrc_qa_is_fill
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LASRC_FILL_BIT)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  lasrc_qa_is_valid_aerosol_retrieval

PURPOSE: Determines if the aerosol retrievel for the LaSRC QA pixel value is
valid

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is valid aerosol retrieval
false           Pixel is not valid aerosol retrieval

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool lasrc_qa_is_valid_aerosol_retrieval
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LASRC_VALID_AEROSOL_RET_BIT)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  lasrc_qa_is_water

PURPOSE: Determines if the LaSRC aerosol QA pixel is water

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
static inline bool lasrc_qa_is_water
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LASRC_WATER_BIT)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  lasrc_qa_is_cloud_cirrus

PURPOSE: Determines if the LaSRC aerosol QA pixel is cloud or cirrus

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is cloud or cirrus
false           Pixel is not cloud or cirrus

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool lasrc_qa_is_cloud_cirrus
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LASRC_CLOUD_CIRRUS_BIT)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  lasrc_qa_is_cloud_shadow

PURPOSE: Determines if the LaSRC aerosol QA pixel is cloud shadow

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
static inline bool lasrc_qa_is_cloud_shadow
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LASRC_CLOUD_SHADOW_BIT)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  lasrc_qa_is_aerosol_interp

PURPOSE: Determines if aerosol value for the LaSRC aerosol QA pixel is
interpolated

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is aerosol interpolation
false           Pixel is not aerosol interpolation

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline bool lasrc_qa_is_aerosol_interp
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    if (l2_qa_pix & LASRC_AEROSOL_INTERP_BIT)
        return true;
    else
        return false;
}


/******************************************************************************
MODULE:  lasrc_qa_aerosol_level

PURPOSE: Returns the aerosol level (0-3) for the current LaSRC aerosol QA
pixel.

RETURN VALUE:
Type = uint8_t
Value           Description
-----           -----------
0               Aerosol level bits are 00 (none)
1               Aerosol level bits are 01 (low)
2               Aerosol level bits are 10 (moderate)
3               Aerosol level bits are 11 (high)

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
******************************************************************************/
static inline uint8_t lasrc_qa_aerosol_level
(
    uint8_t l2_qa_pix      /* I: Level-2 QA value for current pixel */
)
{
    return ((l2_qa_pix & LASRC_AEROSOL_LEVEL_BIT) 
        >> LASRC_AEROSOL_LEVEL_BIT_NUMBER);
}

#endif
