/*****************************************************************************
FILE: write_pixel_qa.c
  
PURPOSE: Contains functions for creating and writing the Level-2 pixel QA band.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
  1. The XML metadata format written via this library follows the ESPA internal
     metadata format found in ESPA Raw Binary Format v1.0.doc.  The schema for
     the ESPA internal metadata format is available at
     http://espa.cr.usgs.gov/schema/espa_internal_metadata_vx_x.xsd.
*****************************************************************************/
#include "write_pixel_qa.h"

/******************************************************************************
MODULE:  create_pixel_qa

PURPOSE: Creates the pixel QA band and makes it available for writing.

RETURN VALUE:
Type = FILE *
Value           Description
-----           -----------
NULL            Error creating the pixel QA band
not NULL        Successfully created

NOTES:
1. A file pointer to the pixel QA band will be returned. It is expected the
   calling routine will handle closing this file pointer when complete.
******************************************************************************/
FILE *create_pixel_qa
(
    char *l2_qa_file       /* I: pixel QA filename to be created */
)
{
    char FUNC_NAME[] = "create_pixel_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */
    FILE *fp_bqa = NULL;      /* file pointer for the pixel QA file */

    /* Create the pixel QA band for write */
    fp_bqa = open_raw_binary (l2_qa_file, "w+");
    if (fp_bqa == NULL)
    {
        sprintf (errmsg, "Creating the pixel QA file: %s", l2_qa_file);
        error_handler (true, FUNC_NAME, errmsg);
        return (NULL);
    }

    /* Successfully created the pixel QA band */
    return (fp_bqa);
}


/******************************************************************************
MODULE:  write_pixel_qa

PURPOSE: Writes the specified number of lines to the pixel QA band.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error writing the pixel QA
SUCCESS         Successfully written

NOTES:
******************************************************************************/
int write_pixel_qa
(
    FILE *fp_bqa,           /* I: pointer to pixel QA band open for writing */
    int nlines,             /* I: number of lines to write to the QA file */
    int nsamps,             /* I: number of samples to write to the QA file */
    uint16_t *pixel_qa      /* I: pixel QA band values for the specified
                                  number of lines */
)
{
    char FUNC_NAME[] = "write_pixel_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */

    /* Write the current line(s) to the pixel QA band */
    if (write_raw_binary (fp_bqa, nlines, nsamps, sizeof (uint16_t), pixel_qa)
        != SUCCESS)
    {   
        sprintf (errmsg, "Writing %d line(s) to the pixel QA band", nlines);
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Successful write */
    return (SUCCESS);
}


/******************************************************************************
MODULE:  close_pixel_qa

PURPOSE: Closes the pixel QA band.

RETURN VALUE:
Type = None

NOTES:
******************************************************************************/
/* This routine exists in the read_pixel_qa.c file and is interchangable */
