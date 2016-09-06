/*****************************************************************************
FILE: write_class_based_qa.c
  
PURPOSE: Contains functions for creating and writing the Level-2 class-based QA
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
#include "write_class_based_qa.h"

/******************************************************************************
MODULE:  create_class_based_qa

PURPOSE: Creates the class-based QA band and makes it available for writing.

RETURN VALUE:
Type = FILE *
Value           Description
-----           -----------
NULL            Error creating the class-based QA band
not NULL        Successfully created

NOTES:
1. A file pointer to the class-based QA band will be returned. It is expected
   the calling routine will handle closing this file pointer when complete.
******************************************************************************/
FILE *create_class_based_qa
(
    char *l2_qa_file       /* I: class-based QA filename to be created */
)
{
    char FUNC_NAME[] = "create_class_based_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */
    FILE *fp_bqa = NULL;      /* file pointer for the class-based QA file */

    /* Create the class-based QA band for write */
    fp_bqa = open_raw_binary (l2_qa_file, "w+");
    if (fp_bqa == NULL)
    {
        sprintf (errmsg, "Creating the class-based QA file: %s", l2_qa_file);
        error_handler (true, FUNC_NAME, errmsg);
        return (NULL);
    }

    /* Successfully created the class-based QA band */
    return (fp_bqa);
}


/******************************************************************************
MODULE:  write_class_based_qa

PURPOSE: Writes the specified number of lines to the class-based QA band.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error writing the class-based QA
SUCCESS         Successfully written

NOTES:
******************************************************************************/
int write_class_based_qa
(
    FILE *fp_bqa,           /* I: pointer to the class-based QA band open for
                                  writing */
    int nlines,             /* I: number of lines to write to the QA file */
    int nsamps,             /* I: number of samples to write to the QA file */
    uint8_t *class_based_qa /* I: class-based QA band values for the specified
                                  number of lines */
)
{
    char FUNC_NAME[] = "write_class_based_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */

    /* Write the current line(s) to the class-based QA band */
    if (write_raw_binary (fp_bqa, nlines, nsamps, sizeof (uint8_t),
        class_based_qa) != SUCCESS)
    {   
        sprintf (errmsg, "Writing %d line(s) to the class-based QA band",
            nlines);
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Successful write */
    return (SUCCESS);
}


/******************************************************************************
MODULE:  close_class_based_qa

PURPOSE: Closes the class-based QA band.

RETURN VALUE:
Type = None

NOTES:
******************************************************************************/
/* This routine exists in the read_class_based_qa.c file and is
   interchangable */
