/*****************************************************************************
FILE: write_level2_qa.c
  
PURPOSE: Contains functions for creating and writing the Level-2 QA
band.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
  1. The XML metadata format written via this library follows the ESPA internal
     metadata format found in ESPA Raw Binary Format v1.0.doc.  The schema for
     the ESPA internal metadata format is available at
     http://espa.cr.usgs.gov/schema/espa_internal_metadata_vx_x.xsd.
*****************************************************************************/
#include "write_level2_qa.h"

/******************************************************************************
MODULE:  create_level2_qa

PURPOSE: Creates the Level-2 QA band and makes it available for writing.

RETURN VALUE:
Type = FILE *
Value           Description
-----           -----------
NULL            Error creating the Level-2 QA band
not NULL        Successfully created

NOTES:
1. A file pointer to the Level-2 QA band will be returned. It is expected the
   calling routine will handle closing this file pointer when complete.
******************************************************************************/
FILE *create_level2_qa
(
    char *l2_qa_file       /* I: Level-2 QA filename to be created */
)
{
    char FUNC_NAME[] = "create_level2_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */
    FILE *fp_bqa = NULL;      /* file pointer for the Level-2 QA file */

    /* Create the Level-2 QA band for write */
    fp_bqa = open_raw_binary (l2_qa_file, "w+");
    if (fp_bqa == NULL)
    {
        sprintf (errmsg, "Creating the Level-2 QA file: %s", l2_qa_file);
        error_handler (true, FUNC_NAME, errmsg);
        return (NULL);
    }

    /* Successfully created the Level-2 QA band */
    return (fp_bqa);
}


/******************************************************************************
MODULE:  write_level2_qa

PURPOSE: Writes the specified number of lines to the Level-2 QA band.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error writing the Level-2 QA
SUCCESS         Successfully written

NOTES:
******************************************************************************/
int write_level2_qa
(
    FILE *fp_bqa,          /* I: pointer to the Level-2 QA band open for
                                 writing */
    int nlines,            /* I: number of lines to write to the QA file */
    int nsamps,            /* I: number of samples to write to the QA file */
    uint8_t *level2_qa     /* I: Level-2 QA band values for the specified
                                 number of lines */
)
{
    char FUNC_NAME[] = "write_level2_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */

    /* Write the current line(s) to the Level-2 QA band */
    if (write_raw_binary (fp_bqa, nlines, nsamps, sizeof (uint8_t), level2_qa)
        != SUCCESS)
    {   
        sprintf (errmsg, "Writing %d line(s) to the Level-2 QA band", nlines);
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Successful write */
    return (SUCCESS);
}


/******************************************************************************
MODULE:  close_level2_qa

PURPOSE: Closes the Level-2 QA band.

RETURN VALUE:
Type = None

NOTES:
******************************************************************************/
/* This routine exists in the read_level2_qa.c file and is interchangable */
