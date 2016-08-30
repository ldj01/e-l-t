/*****************************************************************************
FILE: generate_level2_qa.c
  
PURPOSE: Contains functions for generating the Level-2 QA band from the input
Level-1 QA band.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
  1. The XML metadata format written via this library follows the ESPA internal
     metadata format found in ESPA Raw Binary Format v1.0.doc.  The schema for
     the ESPA internal metadata format is available at
     http://espa.cr.usgs.gov/schema/espa_internal_metadata_vx_x.xsd.
*****************************************************************************/
#include <time.h>
#include "generate_level2_qa.h"


/******************************************************************************
MODULE:  generate_level2_qa

PURPOSE: Generates the Level-2 QA band, using input from the input Level-1
quality band, and adds the band to the XML file.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error generating the Level-2 QA
SUCCESS         Successfully generated

NOTES:
1. This QA band will be an unsigned 8-bit integer containing classes as
   identified in the generate_level2_qa.h include file.
2. Refer to http://landsat.usgs.gov/collectionqualityband.php for the Level-1
   QA band information.
******************************************************************************/
int generate_level2_qa
(
    char *espa_xml_file    /* I: input ESPA XML filename */
)
{
    char FUNC_NAME[] = "generate_level2_qa";  /* function name */
    char errmsg[STR_SIZE];     /* error message */
    char l1_qa_file[STR_SIZE]; /* output Level-2 QA filename */
    char l2_qa_file[STR_SIZE]; /* output Level-2 QA filename */
    char tmpstr[STR_SIZE];     /* tempoary string for filenames */
    char envi_file[STR_SIZE];  /* name of the output ENVI header file */
    char production_date[MAX_DATE_LEN+1]; /* current date/time for production */
    char *cptr = NULL;         /* character pointer for the '.' in XML name
                                  and image name */
    int i;                     /* looping variable */
    int nlines;                /* number of lines in the QA band */
    int nsamps;                /* number of samples in the QA band */
    int refl_indx = -99;       /* index of band1 or first band */
    uint8_t *l2_qa = NULL;     /* Level-2 QA band values */
    uint16_t *l1_qa = NULL;    /* Level-1 QA band values */
    FILE *l1_fp_bqa = NULL;    /* file pointer for the Level-1 QA band */
    FILE *l2_fp_bqa = NULL;    /* file pointer for the Level-2 QA band */
    time_t tp;                 /* time structure */
    struct tm *tm = NULL;      /* time structure for UTC time */
    Espa_level1_qa_type qa_category;    /* type of Level-1 QA data (L4-7, L8) */
    Espa_internal_meta_t l2qa_metadata; /* metadata container to hold the band
                                  metadata for the L2 QA band; global metadata
                                  won't be valid */
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure to be
                                  populated by reading the XML metadata file */
    Espa_band_meta_t *l2qa_bmeta; /* pointer to the array of bands in the
                                     Level-2 QA metadata */
    Espa_band_meta_t *bmeta;    /* pointer to the array of bands metadata */
    Envi_header_t envi_hdr;     /* output ENVI header information */

    /* Open the Level-1 QA file */
    l1_fp_bqa = open_level1_qa (espa_xml_file, l1_qa_file, &nlines, &nsamps,
        &qa_category);
    if (l1_fp_bqa == NULL)
    {
        sprintf (errmsg, "Unable to open the Level-1 QA file");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Allocate memory for the Level-1 QA band */
    l1_qa = calloc (nlines * nsamps, sizeof (uint16_t));
    if (l1_qa == NULL)
    {
        sprintf (errmsg, "Allocating memory for Level-1 QA data");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Read the Level-1 QA band */
    if (read_level1_qa (l1_fp_bqa, nlines, nsamps, l1_qa) != SUCCESS)
    {
        sprintf (errmsg, "Unable to read the entire Level-1 QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Close the Level-1 QA file */
    close_level1_qa (l1_fp_bqa);

    /* Allocate memory for the Level-2 QA band; also allocates these values
       to L2QA_CLEAR (0). */
    l2_qa = calloc (nlines * nsamps, sizeof (uint8_t));
    if (l2_qa == NULL)
    {
        sprintf (errmsg, "Allocating memory for Level-2 QA data");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Determine the name of the Level-2 QA file */
    strcpy (l2_qa_file, espa_xml_file);
    cptr = strrchr (l2_qa_file, '.');
    if (!cptr)
    {
        sprintf (errmsg, "Unable to find the file extension in the XML file. "
            "Error creating the Level-2 QA filename.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }
    sprintf (cptr, "_level2_qa.img");

    /* Create the Level-2 QA file */
    l2_fp_bqa = create_level2_qa (l2_qa_file);
    if (l2_fp_bqa == NULL)
    {
        sprintf (errmsg, "Unable to create the Level-2 QA file");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Loop through the pixels in the Level-1 QA band and create the Level-2 QA
       band.  Buffers were already initialied to 0 (L2QA_CLEAR). */
    /* Water is not available in the Level-1 QA.  The snow class is based on
       the snow/ice confidence, and the cloud shadow class is based on the 
       cloud shadow confidence.  The Level-2 QA will be turned on for snow
       and cloud shadow if the confidence is high (i.e. both bits turned on
       is a value of 3). */
    for (i = 0; i < nlines * nsamps; i++)
    {
        if (level1_qa_is_fill (l1_qa[i]))
            l2_qa[i] = L2QA_FILL;
        else if (level1_qa_is_cloud (l1_qa[i]))
            l2_qa[i] = L2QA_CLOUD;
        else if (level1_qa_snow_ice_confidence (l1_qa[i]) == 3)
            l2_qa[i] = L2QA_SNOW;
        else if (level1_qa_cloud_shadow_confidence (l1_qa[i]) == 3)
            l2_qa[i] = L2QA_CLD_SHADOW;
    }

    /* Write the Level-2 QA band */
    if (write_level2_qa (l2_fp_bqa, nlines, nsamps, l2_qa) != SUCCESS)
    {
        sprintf (errmsg, "Unable to write the entire Level-2 QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Close the Level-2 QA file */
    close_level2_qa (l2_fp_bqa);

    /* Free the Level-1 and Level-2 QA buffers */
    free (l1_qa);
    free (l2_qa);

    /* Initialize the metadata structure */
    init_metadata_struct (&xml_metadata);

    /* Parse the metadata file into our internal metadata structure; also
       allocates space as needed for various pointers in the global and band
       metadata */
    if (parse_metadata (espa_xml_file, &xml_metadata) != SUCCESS)
    {  /* Error messages already written */
        return (ERROR);
    }

    /* Use band 1 as the representative band in the XML */
    for (i = 0; i < xml_metadata.nbands; i++)
    {
        if (!strcmp (xml_metadata.band[i].name, "band1"))
        {
            /* this is the index we'll use for reflectance band info */
            refl_indx = i;
            break;
        }
    }

    /* Make sure the representative band was found in the XML file */
    if (refl_indx == -99)
    {
        sprintf (errmsg, "Band 1 (band1) was not found in the XML file");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Make sure the band 1 number of lines and samples matches what was used
       for creating the land/water mask, otherwise we will have a mismatch
       in the resolution and output XML information. */
    bmeta = &xml_metadata.band[refl_indx];
    if (nlines != bmeta->nlines || nsamps != bmeta->nsamps)
    {
        sprintf (errmsg, "Size of band 1 from this XML file does not match the "
            "Level-1 quality band.  Band 1 nlines/nsamps: %d, %d   Level-1 "
            "quality band nlines/nsamps: %d, %d", bmeta->nlines, bmeta->nsamps,
            nlines, nsamps);
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Initialize the internal metadata for the output product. The global
       metadata won't be updated, however the band metadata will be updated
       and used for appending to the original XML file. */
    init_metadata_struct (&l2qa_metadata);

    /* Allocate memory for the total bands of 1 QA band */
    if (allocate_band_metadata (&l2qa_metadata, 1) != SUCCESS)
    {
        sprintf (errmsg, "Allocating band metadata for Level-2 QA.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }
    l2qa_bmeta = &l2qa_metadata.band[0];

    /* Set up the band metadata for the Level-2 QA band */
    strcpy (l2qa_bmeta->product, "level2_qa");
    strcpy (l2qa_bmeta->source, "level1");
    strcpy (l2qa_bmeta->name, "level2_qa");
    strcpy (l2qa_bmeta->category, "qa");
    l2qa_bmeta->data_type = ESPA_UINT8;
    l2qa_bmeta->nlines = nlines;
    l2qa_bmeta->nsamps = nsamps;
    strncpy (tmpstr, bmeta->short_name, 3);
    sprintf (l2qa_bmeta->short_name, "%sL2QA", tmpstr);
    strcpy (l2qa_bmeta->long_name, "level-2 quality band");
    l2qa_bmeta->pixel_size[0] = bmeta->pixel_size[0];
    l2qa_bmeta->pixel_size[1] = bmeta->pixel_size[1];
    strcpy (l2qa_bmeta->pixel_units, bmeta->pixel_units);
    strcpy (l2qa_bmeta->data_units, "quality/feature classification");
    l2qa_bmeta->valid_range[0] = 0.0;
    l2qa_bmeta->valid_range[1] = 255.0;
    sprintf (l2qa_bmeta->app_version, "generate_level2_qa_%s",
        L2QA_COMMON_VERSION);
    strcpy (l2qa_bmeta->file_name, l2_qa_file);

    /* Set up the classes for the Level-2 QA band */
    l2qa_bmeta->nclass = 6;
    if (allocate_class_metadata (l2qa_bmeta, 6) != SUCCESS)
    {
        sprintf (errmsg, "Cannot allocate memory for the Level-2 QA classes");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }
    l2qa_bmeta->class_values[0].class = 0;
    l2qa_bmeta->class_values[1].class = 1;
    l2qa_bmeta->class_values[2].class = 2;
    l2qa_bmeta->class_values[3].class = 3;
    l2qa_bmeta->class_values[4].class = 4;
    l2qa_bmeta->class_values[5].class = 255;
    strcpy (l2qa_bmeta->class_values[0].description, "clear");
    strcpy (l2qa_bmeta->class_values[1].description, "water");
    strcpy (l2qa_bmeta->class_values[2].description, "cloud_shadow");
    strcpy (l2qa_bmeta->class_values[3].description, "snow");
    strcpy (l2qa_bmeta->class_values[4].description, "cloud");
    strcpy (l2qa_bmeta->class_values[5].description, "fill");

    /* Get the current date/time (UTC) for the production date of each band */
    if (time (&tp) == -1)
    {
        sprintf (errmsg, "Unable to obtain the current time.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    tm = gmtime (&tp);
    if (tm == NULL)
    {
        sprintf (errmsg, "Converting time to UTC.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    if (strftime (production_date, MAX_DATE_LEN, "%Y-%m-%dT%H:%M:%SZ", tm) == 0)
    {
        sprintf (errmsg, "Formatting the production date/time.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }
    strcpy (l2qa_bmeta->production_date, production_date);

    /* Create the ENVI header file this band */
    if (create_envi_struct (l2qa_bmeta, &xml_metadata.global, &envi_hdr) !=
        SUCCESS)
    {
        sprintf (errmsg, "Creating ENVI header structure.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Write the ENVI header */
    strcpy (envi_file, l2qa_bmeta->file_name);
    cptr = strchr (envi_file, '.');
    strcpy (cptr, ".hdr");
    if (write_envi_hdr (envi_file, &envi_hdr) != SUCCESS)
    {
        sprintf (errmsg, "Writing ENVI header file.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Add the Level-2 QA band to the XML file */
    if (append_metadata (1, l2qa_bmeta, espa_xml_file) != SUCCESS)
    {
        sprintf (errmsg, "Appending Level-2 QA band to XML file.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Free the input and output XML metadata */
    free_metadata (&xml_metadata);
    free_metadata (&l2qa_metadata);

    /* Successfully generated the Level-2 QA band */
    return (SUCCESS);
}
