/*****************************************************************************
FILE: write_pixel_qa.h
  
PURPOSE: Contains function prototypes for the Level-2 pixel QA band output.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
*****************************************************************************/

#ifndef WRITE_PIXEL_QA_H
#define WRITE_PIXEL_QA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "error_handler.h"
#include "raw_binary_io.h"

/* Defines */
FILE *create_pixel_qa
(
    char *l2_qa_file       /* I: pixel QA filename to be created */
);

int write_pixel_qa
(
    FILE *fp_bqa,           /* I: pointer to pixel QA band open for writing */
    int nlines,             /* I: number of lines to write to the QA file */
    int nsamps,             /* I: number of samples to write to the QA file */
    uint16_t *pixel_qa      /* I: pixel QA band values for the specified
                                  number of lines */
);

#endif
