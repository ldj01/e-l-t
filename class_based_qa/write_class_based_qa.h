/*****************************************************************************
FILE: write_class_based_qa.h
  
PURPOSE: Contains function prototypes for the Level-2 class-based QA band
output.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
*****************************************************************************/

#ifndef WRITE_CLASS_BASED_QA_H
#define WRITE_CLASS_BASED_QA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "error_handler.h"
#include "raw_binary_io.h"

/* Defines */
FILE *create_class_based_qa
(
    char *l2_qa_file       /* I: class-based QA filename to be created */
);

int write_class_based_qa
(
    FILE *fp_bqa,           /* I: pointer to the class-based QA band open for
                                  writing */
    int nlines,             /* I: number of lines to write to the QA file */
    int nsamps,             /* I: number of samples to write to the QA file */
    uint8_t *class_based_qa /* I: class-based QA band values for the specified
                                  number of lines */
);

#endif
