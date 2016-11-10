/*****************************************************************************
FILE: read_level1_qa.c
  
PURPOSE: Contains functions for opening, reading, and manipulating the Level-1
QA band.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
1. The XML metadata format written via this library follows the ESPA internal
   metadata format found in ESPA Raw Binary Format v1.0.doc.  The schema for
   the ESPA internal metadata format is available at
   http://espa.cr.usgs.gov/schema/espa_internal_metadata_vx_x.xsd.
2. Refer to http://landsat.usgs.gov/collectionqualityband.php for the Level-1
   QA band information.
*****************************************************************************/
#include "read_level1_qa.h"

/******************************************************************************
MODULE:  open_level1_qa

PURPOSE: Reads the ESPA XML file and opens the Level-1 QA band.

RETURN VALUE:
Type = FILE *
Value           Description
-----           -----------
NULL            Error parsing the XML file or opening the Level-1 QA band
not NULL        Successfully read

NOTES:
1. It is expected that this QA band will be an unsigned 16-bit integer. If the
   data type does not match that expectation, then an error will be flagged
   when obtaining information about the QA band from the XML file.
2. A file pointer to the Level-1 QA band will be returned. It is expected the
   calling routine will handle closing this file pointer when complete.
******************************************************************************/
FILE *open_level1_qa
(
    char *espa_xml_file,   /* I: input ESPA XML filename */
    char *l1_qa_file,      /* O: output Level-1 QA filename (memory must be
                                 allocated ahead of time) */
    int *nlines,           /* O: number of lines in the QA band */
    int *nsamps,           /* O: number of samples in the QA band */
    Espa_level1_qa_type *qa_category /* O: type of Level-1 QA data (L4-7, L8) */
)
{
    char FUNC_NAME[] = "open_level1_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */
    int i;                    /* looping variable */
    bool found;               /* was the Level-1 QA band found? */
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure to be
                                 populated by reading the XML metadata file */
    Espa_global_meta_t *gmeta;/* pointer to the global metadata structure */
    Espa_band_meta_t *bmeta;  /* pointer to the array of bands metadata */
    FILE *fp_bqa = NULL;      /* file pointer for the band quality band */

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

    /* Loop through the bands and look for the Level-1 QA band */
    found = false;
    for (i = 0; i < xml_metadata.nbands; i++)
    {
        /* Is this the Level-1 quality band */
        if (!strcmp (bmeta[i].name, "bqa") && !strcmp (bmeta[i].category, "qa"))
        {
            strcpy (l1_qa_file, bmeta[i].file_name);
            *nlines = bmeta[i].nlines;
            *nsamps = bmeta[i].nsamps;
            if (bmeta[i].data_type != ESPA_UINT16)
            {
                sprintf (errmsg, "Expecting UINT16 data type for Level-1 QA "
                    "band, however the data type was something other than "
                    "UINT16.  Please check the input XML file.");
                error_handler (true, FUNC_NAME, errmsg);
                return (NULL);
            }
            found = true;
        }
    }

    /* Make sure the Level-1 QA band was found */
    if (!found)
    {
        sprintf (errmsg, "Unable to find the Level-1 QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (NULL);
    }

    /* Open the Level-1 QA band for read only */
    fp_bqa = open_raw_binary (l1_qa_file, "r");
    if (fp_bqa == NULL)
    {
        sprintf (errmsg, "Opening the quality band file: %s", l1_qa_file);
        error_handler (true, FUNC_NAME, errmsg);
        return (NULL);
    }

    /* Determine the instrument type for actual QA bit identification */
    if (!strcmp (gmeta->instrument, "TM") || !strcmp (gmeta->instrument, "ETM"))
        *qa_category = LEVEL1_L457;
    else
        *qa_category = LEVEL1_L8;

    /* Free the metadata structure */
    free_metadata (&xml_metadata);

    /* Successfully opened the Level-1 QA band */
    return (fp_bqa);
}


/******************************************************************************
MODULE:  read_level1_qa

PURPOSE: Reads the specified number of lines from the Level-1 QA band.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error reading the Level-1 QA
SUCCESS         Successfully read

NOTES:
1. open_level1_qa is available for opening the Level-1 QA band.
******************************************************************************/
int read_level1_qa
(
    FILE *fp_bqa,          /* I: pointer to the Level-1 QA band open for
                                 reading */
    int nlines,            /* I: number of lines to read from the QA file */
    int nsamps,            /* I: number of samples to read from the QA file */
    uint16_t *level1_qa    /* O: Level-1 QA band values for the specified
                                 number of lines (memory should be allocated
                                 for nlines x nsamps of size uint16 before
                                 calling this routine) */
)
{
    char FUNC_NAME[] = "read_level1_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */

    /* Read the current line from the band quality band */
    if (read_raw_binary (fp_bqa, nlines, nsamps, sizeof (uint16_t), level1_qa)
        != SUCCESS)
    {   
        sprintf (errmsg, "Reading %d lines from Level-1 QA band", nlines);
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Successful read */
    return (SUCCESS);
}


/******************************************************************************
MODULE:  close_level1_qa

PURPOSE: Closes the Level-1 QA band.

RETURN VALUE:
Type = None

NOTES:
******************************************************************************/
void close_level1_qa
(
    FILE *fp_bqa           /* I/O: pointer to the open Level-1 QA band;will
                                   be closed upon return */
)
{
    close_raw_binary (fp_bqa);
}
