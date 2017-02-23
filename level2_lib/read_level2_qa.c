/*****************************************************************************
FILE: read_level2_qa.c
  
PURPOSE: Contains functions for opening, reading, and manipulating the Level-2
QA bands (LEDAPS and LaSRC) for Collection products. QA bands for pre-collection
scenes is not fully supported.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
1. The XML metadata format written via this library follows the ESPA internal
   metadata format found in ESPA Raw Binary Format v1.0.doc.  The schema for
   the ESPA internal metadata format is available at
   http://espa.cr.usgs.gov/schema/espa_internal_metadata_vx_x.xsd.
2. Refer to http://landsat.usgs.gov/collectionqualityband.php for the Level-2
   QA band information.
*****************************************************************************/
#include "read_level2_qa.h"

/******************************************************************************
MODULE:  open_level2_qa

PURPOSE: Reads the ESPA XML file and opens the user-specified Level-2 QA band.

RETURN VALUE:
Type = FILE *
Value           Description
-----           -----------
NULL            Error parsing the XML file or opening the Level-2 QA band
not NULL        Successfully read

NOTES:
1. It is expected that this QA band will be an unsigned 8-bit integer. If the
   data type does not match that expectation, then an error will be flagged
   when obtaining information about the QA band from the XML file.
2. A file pointer to the Level-2 QA band will be returned. It is expected the
   calling routine will handle closing this file pointer when complete.
******************************************************************************/
FILE *open_level2_qa
(
    char *espa_xml_file,   /* I: input ESPA XML filename */
    Espa_level2_qa_type qa_category, /* I: type of Level-2 QA data to be opened
                                (LEDAPS radsat, LEDAPS cloud, LaSRC aerosol) */
    char *l2_qa_file,      /* O: output Level-2 QA filename (memory must be
                                 allocated ahead of time) */
    int *nlines,           /* O: number of lines in the QA band */
    int *nsamps            /* O: number of samples in the QA band */
)
{
    char FUNC_NAME[] = "open_level2_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */
    int i;                    /* looping variable */
    bool found;               /* was the Level-2 QA band found? */
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure to be
                                 populated by reading the XML metadata file */
    Espa_global_meta_t *gmeta;/* pointer to the global metadata structure */
    Espa_band_meta_t *bmeta;  /* pointer to the array of bands metadata */
    FILE *fp_l2qa = NULL;     /* file pointer for the Level-2 QA band */

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

    /* Loop through the bands and look for the desired Level-2 QA band */
    found = false;
    for (i = 0; i < xml_metadata.nbands; i++)
    {
        /* Is this the desired Level-2 quality band */
        if ((qa_category == LEDAPS_RADSAT &&
               (!strcmp (bmeta[i].name, "radsat_qa") &&
                !strcmp (bmeta[i].category, "qa"))) ||
            (qa_category == LEDAPS_CLOUD &&
               (!strcmp (bmeta[i].name, "sr_cloud_qa") &&
                !strcmp (bmeta[i].category, "qa"))) ||
            (qa_category == LASRC_AEROSOL &&
               (!strcmp (bmeta[i].name, "sr_aerosol") &&
                !strcmp (bmeta[i].category, "qa"))))
        {
            strcpy (l2_qa_file, bmeta[i].file_name);
            *nlines = bmeta[i].nlines;
            *nsamps = bmeta[i].nsamps;
            if (bmeta[i].data_type != ESPA_UINT8)
            {
                sprintf (errmsg, "Expecting UINT8 data type for Level-2 QA "
                    "band, however the data type was something other than "
                    "UINT8.  Please check the input XML file.");
                error_handler (true, FUNC_NAME, errmsg);
                return (NULL);
            }
            found = true;
            break;
        }
    }

    /* Make sure the desired Level-2 QA band was found */
    if (!found)
    {
        sprintf (errmsg, "Unable to find the specified Level-2 QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (NULL);
    }

    /* Open the Level-2 QA band for read only */
    fp_l2qa = open_raw_binary (l2_qa_file, "r");
    if (fp_l2qa == NULL)
    {
        sprintf (errmsg, "Opening the Level-2 QA band file: %s", l2_qa_file);
        error_handler (true, FUNC_NAME, errmsg);
        return (NULL);
    }

    /* Free the metadata structure */
    free_metadata (&xml_metadata);

    /* Successfully opened the Level-2 QA band */
    return (fp_l2qa);
}


/******************************************************************************
MODULE:  read_level2_qa

PURPOSE: Reads the specified number of lines from the Level-2 QA band.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error reading the Level-2 QA
SUCCESS         Successfully read

NOTES:
1. open_level2_qa is available for opening the Level-2 QA band.
******************************************************************************/
int read_level2_qa
(
    FILE *fp_l2qa,         /* I: pointer to the Level-2 QA band open for
                                 reading */
    int nlines,            /* I: number of lines to read from the QA file */
    int nsamps,            /* I: number of samples to read from the QA file */
    uint8_t *level2_qa     /* O: Level-2 QA band values for the specified
                                 number of lines (memory should be allocated
                                 for nlines x nsamps of size uint8 before
                                 calling this routine) */
)
{
    char FUNC_NAME[] = "read_level2_qa";  /* function name */
    char errmsg[STR_SIZE];    /* error message */

    /* Read the current line from the band quality band */
    if (read_raw_binary (fp_l2qa, nlines, nsamps, sizeof (uint8_t), level2_qa)
        != SUCCESS)
    {   
        sprintf (errmsg, "Reading %d lines from Level-2 QA band", nlines);
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Successful read */
    return (SUCCESS);
}


/******************************************************************************
MODULE:  close_level2_qa

PURPOSE: Closes the Level-2 QA band.

RETURN VALUE:
Type = None

NOTES:
******************************************************************************/
void close_level2_qa
(
    FILE *fp_l2qa          /* I/O: pointer to the open Level-2 QA band; will
                                   be closed upon return */
)
{
    close_raw_binary (fp_l2qa);
}
