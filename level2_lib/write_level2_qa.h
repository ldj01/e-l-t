/*****************************************************************************
FILE: write_level2_qa.h
  
PURPOSE: Contains function prototypes for the Level-2 QA band output.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
*****************************************************************************/

#ifndef WRITE_LEVEL2_QA_H
#define WRITE_LEVEL2_QA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "error_handler.h"
#include "espa_metadata.h"
#include "parse_metadata.h"
#include "raw_binary_io.h"

/* Defines */
FILE *create_level2_qa
(
    char *l2_qa_file       /* I: Level-2 QA filename to be created */
);

int write_level2_qa
(
    FILE *fp_bqa,          /* I: pointer to the Level-2 QA band open for
                                 writing */
    int nlines,            /* I: number of lines to write to the QA file */
    int nsamps,            /* I: number of samples to write to the QA file */
    uint8_t *level2_qa     /* I: Level-2 QA band values for the specified
                                 number of lines */
);

#endif
