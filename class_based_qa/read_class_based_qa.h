/*****************************************************************************
FILE: read_class_based_qa.h
  
PURPOSE: Contains function prototypes for reading the Level-2 class-based QA
band.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
*****************************************************************************/

#ifndef READ_CLASS_BASED_QA_H
#define READ_CLASS_BASED_QA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "error_handler.h"
#include "espa_metadata.h"
#include "parse_metadata.h"
#include "raw_binary_io.h"

/* Function prototypes */
FILE *open_class_based_qa
(
    char *espa_xml_file,   /* I: input ESPA XML filename */
    char *l2_qa_file,      /* O: output class-based QA filename */
    int *nlines,           /* O: number of lines in the QA band */
    int *nsamps            /* O: number of samples in the QA band */
);

int read_class_based_qa
(
    FILE *fp_bqa,           /* I: pointer to the class-based QA band open for
                                  reading */
    int nlines,             /* I: number of lines to read from the QA file */
    int nsamps,             /* I: number of samples to read from the QA file */
    uint8_t *class_based_qa /* O: class-based QA band values for the specified
                                  number of lines (memory should be allocated
                                  for nlines x nsamps of size uint8 before
                                  calling this routine) */
);

void close_class_based_qa
(
    FILE *fp_bqa           /* I/O: pointer to the open class-based QA band;
                                   will be closed upon return */
);

#endif
