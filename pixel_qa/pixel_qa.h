/*****************************************************************************
FILE: pixel_qa.h

PURPOSE: Contains the currently defined bits for each of the QA values.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3
*****************************************************************************/

#ifndef PIXEL_QA_H
#define PIXEL_QA_H


/* Defines for the bit in the pixel QA band
   ----------------------------------------
0: fill (pulled from level-1 QA)
1: clear (pulled from level-1 QA)
2: water (generated at level-2, using cfmask-type application)
3: cloud shadow (pulled from level-1 QA)
4: snow (pulled from level-1 QA)
5: cloud (pulled from level-1 QA and dilated at level-2, using cfmask-type
   application which dilates the clouds)
6: cloud confidence (pulled from level-1 QA)
7: cloud confidence (pulled from level-1 QA)
8: cirrus confidence (pulled from level-1 QA, L8 only)
9: cirrus confidence (pulled from level-1 QA, L8 only)
10: terrain occlusion (pulled from level-1 QA, L8 only)
8-15: reserved for later use
*/

#define L2QA_FILL 0
#define L2QA_CLEAR 1
#define L2QA_WATER 2
#define L2QA_CLD_SHADOW 3
#define L2QA_SNOW  4
#define L2QA_CLOUD 5
#define L2QA_CLOUD_CONF1 6
#define L2QA_CLOUD_CONF2 7
#define L2QA_CIRRUS_CONF1 8
#define L2QA_CIRRUS_CONF2 9
#define L2QA_TERRAIN_OCCL 10

#define L2QA_SINGLE_BIT 0x01             /* 00000001 */
#define L2QA_DOUBLE_BIT 0x03             /* 00000011 */

#define L2QA_LOW_CONF 1           /* low confidence (01) */
#define L2QA_MODERATE_CONF 2      /* moderate confidence (10) */
#define L2QA_HIGH_CONF 3          /* high confidence (11) */

#endif /* PIXEL_QA_H */
