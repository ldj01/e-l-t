/*****************************************************************************
FILE: generate_class_based_qa.h
  
PURPOSE: Contains function prototypes for generating the class-based QA band
from the Level-1 QA band.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
*****************************************************************************/

#ifndef GENERATE_CLASS_BASED_QA_H
#define GENERATE_CLASS_BASED_QA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "read_level1_qa.h"
#include "read_class_based_qa.h"
#include "write_class_based_qa.h"
#include "l2qa_common.h"
#include "write_metadata.h"
#include "envi_header.h"

/* Defines */
#define L2QA_CLEAR 0
#define L2QA_WATER 1
#define L2QA_CLD_SHADOW 2
#define L2QA_SNOW  3
#define L2QA_CLOUD 4
#define L2QA_FILL  255

#define MAX_DATE_LEN 28

/* Function prototypes */
int generate_class_based_qa
(
    char *espa_xml_file    /* I: input ESPA XML filename */
);

#endif