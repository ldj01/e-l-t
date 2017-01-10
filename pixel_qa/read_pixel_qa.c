/*****************************************************************************
FILE: read_pixel_qa.c
  
PURPOSE: Contains functions for opening and reading the Level-2 pixel QA band.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
  1. The XML metadata format written via this library follows the ESPA internal
     metadata format found in ESPA Raw Binary Format v1.0.doc.  The schema for
     the ESPA internal metadata format is available at
     http://espa.cr.usgs.gov/schema/espa_internal_metadata_vx_x.xsd.
*****************************************************************************/
#include "read_pixel_qa.h"


/******************************************************************************
MODULE:  open_pixel_qa

PURPOSE: Opens the ESPA XML file and opens the pixel QA band for read and
update.  The XML and pixel QA files must exist.

RETURN VALUE:
Type = FILE *
Value           Description
-----           -----------
NULL            Error parsing the XML file or opening the pixel QA band
not NULL        Successfully read

NOTES:
1. It is expected that this QA band will be an unsigned 8-bit integer. If the
   data type does not match that expectation, then an error will be flagged
   when obtaining information about the QA band from the XML file.
2. A file pointer to the pixel QA band will be returned. It is expected the
   calling routine will handle closing this file pointer when complete.
******************************************************************************/
FILE *open_pixel_qa
(
    char *espa_xml_file,   /* I: input ESPA XML filename */
    char *l2_qa_file,      /* O: output pixel filename (memory must be
                                 allocated ahead of time) */
    int *nlines,           /* O: number of lines in the QA band */
    int *nsamps            /* O: number of samples in the QA band */
)
{
    char FUNC_NAME[] = "open_pixel_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */
    char curr_band[STR_SIZE]; /* current band to process */
    int i;                    /* looping variable */
    bool found;               /* was the pixel QA band found? */
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure to be
                                 populated by reading the XML metadata file */
    Espa_global_meta_t *gmeta;/* pointer to the global metadata structure */
    Espa_band_meta_t *bmeta;  /* pointer to the array of bands metadata */
    FILE *fp_bqa = NULL;      /* file pointer for the QA band */

    /* Validate the input metadata file */
    if (validate_xml_file (espa_xml_file) != SUCCESS)
    {  /* Error messages already written */
        return (NULL);
    }

    /* Initialize the metadata structure */
    init_metadata_struct (&xml_metadata);

    /* Parse the metadata file into our internal metadata structure; also
       allocates space as needed for various pointers in the global and band
       metadata */
    if (parse_metadata (espa_xml_file, &xml_metadata) != SUCCESS)
    {  /* Error messages already written */
        return (NULL);
    }
    gmeta = &xml_metadata.global;
    bmeta = xml_metadata.band;

    /* Loop through the bands and look for the pixel QA band */
    found = false;
    for (i = 0; i < xml_metadata.nbands; i++)
    {
        /* Is this the pixel quality band */
        sprintf (curr_band, "qa");
        if (!strcmp (bmeta[i].name, "pixel_qa") &&
            !strcmp (bmeta[i].category, "qa"))
        {
            strcpy (l2_qa_file, bmeta[i].file_name);
            *nlines = bmeta[i].nlines;
            *nsamps = bmeta[i].nsamps;
            if (bmeta[i].data_type != ESPA_UINT16)
            {
                sprintf (errmsg, "Expecting UINT16 data type for pixel QA "
                    "band, however the data type was something other than "
                    "UINT16. Please check the input XML file.");
                error_handler (true, FUNC_NAME, errmsg);
                return (NULL);
            }
            found = true;
        }
    }

    /* Make sure the pixel QA band was found */
    if (!found)
    {
        sprintf (errmsg, "Unable to find the pixel QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (NULL);
    }

    /* Open the pixel QA band for read and update */
    fp_bqa = open_raw_binary (l2_qa_file, "r+");
    if (fp_bqa == NULL)
    {
        sprintf (errmsg, "Opening the pixel QA file: %s", l2_qa_file);
        error_handler (true, FUNC_NAME, errmsg);
        return (NULL);
    }

    /* Free the metadata structure */
    free_metadata (&xml_metadata);

    /* Successfully opened the pixel QA band */
    return (fp_bqa);
}


/******************************************************************************
MODULE:  read_pixel_qa

PURPOSE: Reads the specified number of lines from the pixel QA band.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error reading the pixel QA
SUCCESS         Successfully read

NOTES:
1. open_pixel_qa is available for opening the pixel QA band.
******************************************************************************/
int read_pixel_qa
(
    FILE *fp_bqa,           /* I: pointer to pixel QA band open for reading */
    int nlines,             /* I: number of lines to read from the QA file */
    int nsamps,             /* I: number of samples to read from the QA file */
    uint16_t *pixel_qa      /* O: pixel QA band values for the specified
                                  number of lines (memory should be allocated
                                  for nlines x nsamps of size uint16 before
                                  calling this routine) */
)
{
    char FUNC_NAME[] = "read_pixel_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */

    /* Read the current line from the pixel QA band */
    if (read_raw_binary (fp_bqa, nlines, nsamps, sizeof (uint16_t), pixel_qa)
        != SUCCESS)
    {   
        sprintf (errmsg, "Reading %d lines from pixel QA band", nlines);
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Successful read */
    return (SUCCESS);
}


/******************************************************************************
MODULE:  close_pixel_qa

PURPOSE: Closes the pixel QA band.

RETURN VALUE:
Type = None

NOTES:
******************************************************************************/
void close_pixel_qa
(
    FILE *fp_bqa           /* I/O: pointer to the open pixel QA band; will be
                                   closed upon return */
)
{
    close_raw_binary (fp_bqa);
}
