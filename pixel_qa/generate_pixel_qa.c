/*****************************************************************************
FILE: generate_pixel_qa.c
  
PURPOSE: Contains functions for generating the pixel QA band from the input
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
#include "pixel_qa.h"
#include "generate_pixel_qa.h"
#include "read_level1_qa.h"

/******************************************************************************
MODULE:  generate_pixel_qa

PURPOSE: Generates the pixel QA band, using input from the input Level-1
quality band, and adds the band to the XML file.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error generating the pixel QA
SUCCESS         Successfully generated

NOTES:
1. This QA band will be an unsigned 16-bit integer containing some of the
   pixel-level QA information from the Level-1 QA band.  The bits represented
   are identified in the pixel_qa.h include file.
2. Refer to http://landsat.usgs.gov/collectionqualityband.php for the Level-1
   QA band information.
******************************************************************************/
int generate_pixel_qa
(
    char *espa_xml_file    /* I: input ESPA XML filename */
)
{
    char FUNC_NAME[] = "generate_pixel_qa";  /* function name */
    char errmsg[STR_SIZE];     /* error message */
    char l1_qa_file[STR_SIZE]; /* input Level-1 QA filename */
    char l2_qa_file[STR_SIZE]; /* output pixel QA filename */
    char tmpstr[STR_SIZE];     /* tempoary string for filenames */
    char envi_file[STR_SIZE];  /* name of the output ENVI header file */
    char production_date[MAX_DATE_LEN+1]; /* current date/time for production */
    char *cptr = NULL;         /* character pointer for the '.' in XML name
                                  and image name */
    int i;                     /* looping variable */
    int nlines;                /* number of lines in the QA band */
    int nsamps;                /* number of samples in the QA band */
    int refl_indx = -99;       /* index of band1 or first band */
    uint16_t *l2_qa = NULL;    /* pixel QA band values */
    uint16_t *l1_qa = NULL;    /* Level-1 QA band values */
    FILE *l1_fp_bqa = NULL;    /* file pointer for the Level-1 QA band */
    FILE *l2_fp_bqa = NULL;    /* file pointer for the pixel QA band */
    time_t tp;                 /* time structure */
    struct tm *tm = NULL;      /* time structure for UTC time */
    Espa_level1_qa_type qa_category;    /* type of Level-1 QA data (L4-7, L8) */
    Espa_internal_meta_t l2qa_metadata; /* metadata container to hold the band
                                  metadata for the L2 QA band; global metadata
                                  won't be valid */
    Espa_internal_meta_t xml_metadata;  /* XML metadata structure to be
                                  populated by reading the XML metadata file */
    Espa_band_meta_t *l2qa_bmeta; /* pointer to the array of bands in the
                                     pixel QA metadata */
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

    /* Allocate memory for the pixel QA band; also initializes these values
       to L2QA_CLEAR. */
    l2_qa = calloc (nlines * nsamps, sizeof (uint16_t));
    if (l2_qa == NULL)
    {
        sprintf (errmsg, "Allocating memory for pixel QA data");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    for (i = 0; i < nlines * nsamps; i++)
        l2_qa[i] = (1 << L2QA_CLEAR);

    /* Determine the name of the pixel QA file */
    strcpy (l2_qa_file, espa_xml_file);
    cptr = strrchr (l2_qa_file, '.');
    if (!cptr)
    {
        sprintf (errmsg, "Unable to find the file extension in the XML file. "
            "Error creating the pixel QA filename.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }
    sprintf (cptr, "_pixel_qa.img");

    /* Create the pixel QA file */
    l2_fp_bqa = create_pixel_qa (l2_qa_file);
    if (l2_fp_bqa == NULL)
    {
        sprintf (errmsg, "Unable to create the pixel QA file");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Loop through the pixels in the Level-1 QA band and create the pixel QA
       band.  Buffers were already initialized with the L2QA_CLEAR bit set.  It
       needs to be turned off if any condition is turned on, except the low or
       moderate cloud or cirrus confidence. */
    /* Water is not available in the Level-1 QA.  The snow class is based on
       the snow/ice confidence, and the cloud shadow class is based on the 
       cloud shadow confidence.  The pixel QA will be turned on for snow and
       cloud shadow if the confidence is high (i.e. both bits turned on is a
       value of 3). */
    for (i = 0; i < nlines * nsamps; i++)
    {
        if (level1_qa_is_fill (l1_qa[i]))
        { /* if it's fill then we are done */
            l2_qa[i] &= ~(1 << L2QA_CLEAR);
            l2_qa[i] |= (1 << L2QA_FILL);
        }
        else
        { /* if it's not fill, then multiple options can apply */
            if (level1_qa_cloud_shadow_confidence (l1_qa[i]) == L2QA_HIGH_CONF)
            {
                l2_qa[i] &= ~(1 << L2QA_CLEAR);
                l2_qa[i] |= (1 << L2QA_CLD_SHADOW);
            }

            if (level1_qa_snow_ice_confidence (l1_qa[i]) == L2QA_HIGH_CONF)
            {
                l2_qa[i] &= ~(1 << L2QA_CLEAR);
                l2_qa[i] |= (1 << L2QA_SNOW);
            }

            if (level1_qa_is_cloud (l1_qa[i]))
            {
                l2_qa[i] &= ~(1 << L2QA_CLEAR);
                l2_qa[i] |= (1 << L2QA_CLOUD);
            }

            if (level1_qa_cloud_confidence (l1_qa[i]) == L2QA_LOW_CONF)
                l2_qa[i] |= (1 << L2QA_CLOUD_CONF1);

            if (level1_qa_cloud_confidence (l1_qa[i]) == L2QA_MODERATE_CONF)
                l2_qa[i] |= (1 << L2QA_CLOUD_CONF2);

            if (level1_qa_cloud_confidence (l1_qa[i]) == L2QA_HIGH_CONF)
            {
                l2_qa[i] &= ~(1 << L2QA_CLEAR);
                l2_qa[i] |= (1 << L2QA_CLOUD_CONF1);
                l2_qa[i] |= (1 << L2QA_CLOUD_CONF2);
            }

            /* Cirrus confidence and terrain occlusion only apply for L8, and
               neither will affect the clear bit. */
            if (qa_category == LEVEL1_L8)
            {
                if (level1_qa_cirrus_confidence (l1_qa[i]) == L2QA_LOW_CONF)
                    l2_qa[i] |= (1 << L2QA_CIRRUS_CONF1);

                if (level1_qa_cirrus_confidence (l1_qa[i]) ==
                    L2QA_MODERATE_CONF)
                    l2_qa[i] |= (1 << L2QA_CIRRUS_CONF2);

                if (level1_qa_cirrus_confidence (l1_qa[i]) == L2QA_HIGH_CONF)
                {
                    l2_qa[i] |= (1 << L2QA_CIRRUS_CONF1);
                    l2_qa[i] |= (1 << L2QA_CIRRUS_CONF2);
                }

                if (level1_qa_is_terrain_occluded (l1_qa[i]))
                    l2_qa[i] |= (1 << L2QA_TERRAIN_OCCL);
            }
        }
    }

    /* Write the pixel QA band */
    if (write_pixel_qa (l2_fp_bqa, nlines, nsamps, l2_qa) != SUCCESS)
    {
        sprintf (errmsg, "Unable to write the entire pixel QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Close the pixel QA file */
    close_pixel_qa (l2_fp_bqa);

    /* Free the Level-1 and pixel QA buffers */
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
        if (!strcmp (xml_metadata.band[i].name, "b1"))
        {
            /* this is the index we'll use for reflectance band info */
            refl_indx = i;
            break;
        }
    }

    /* Make sure the representative band was found in the XML file */
    if (refl_indx == -99)
    {
        sprintf (errmsg, "Band 1 (b1) was not found in the XML file");
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
        sprintf (errmsg, "Allocating band metadata for pixel QA.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }
    l2qa_bmeta = &l2qa_metadata.band[0];

    /* Set up the band metadata for the pixel QA band */
    strcpy (l2qa_bmeta->product, "level2_qa");
    strcpy (l2qa_bmeta->source, "level1");
    strcpy (l2qa_bmeta->name, "pixel_qa");
    strcpy (l2qa_bmeta->category, "qa");
    l2qa_bmeta->data_type = ESPA_UINT16;
    l2qa_bmeta->fill_value = (1 << L2QA_FILL);
    l2qa_bmeta->nlines = nlines;
    l2qa_bmeta->nsamps = nsamps;
    strncpy (tmpstr, bmeta->short_name, 4);
    sprintf (l2qa_bmeta->short_name, "%sPQA", tmpstr);
    strcpy (l2qa_bmeta->long_name, "level-2 pixel quality band");
    l2qa_bmeta->pixel_size[0] = bmeta->pixel_size[0];
    l2qa_bmeta->pixel_size[1] = bmeta->pixel_size[1];
    strcpy (l2qa_bmeta->pixel_units, bmeta->pixel_units);
    strcpy (l2qa_bmeta->data_units, "quality/feature classification");
    sprintf (l2qa_bmeta->app_version, "generate_pixel_qa_%s",
        L2QA_COMMON_VERSION);
    strcpy (l2qa_bmeta->file_name, l2_qa_file);

    /* Identify the bitmap values for the pixel QA */
    if (allocate_bitmap_metadata (l2qa_bmeta, 16) != SUCCESS)
    {
        sprintf (errmsg, "Cannot allocate memory for the pixel QA bitmap");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Identify the bitmap values for the mask */
    strcpy (l2qa_bmeta->bitmap_description[0], "fill");
    strcpy (l2qa_bmeta->bitmap_description[1], "clear");
    strcpy (l2qa_bmeta->bitmap_description[2], "water");
    strcpy (l2qa_bmeta->bitmap_description[3], "cloud shadow");
    strcpy (l2qa_bmeta->bitmap_description[4], "snow");
    strcpy (l2qa_bmeta->bitmap_description[5], "cloud");
    strcpy (l2qa_bmeta->bitmap_description[6], "cloud confidence");
    strcpy (l2qa_bmeta->bitmap_description[7], "cloud confidence");
    strcpy (l2qa_bmeta->bitmap_description[8], "unused");
    strcpy (l2qa_bmeta->bitmap_description[9], "unused");
    strcpy (l2qa_bmeta->bitmap_description[10], "unused");
    strcpy (l2qa_bmeta->bitmap_description[11], "unused");
    strcpy (l2qa_bmeta->bitmap_description[12], "unused");
    strcpy (l2qa_bmeta->bitmap_description[13], "unused");
    strcpy (l2qa_bmeta->bitmap_description[14], "unused");
    strcpy (l2qa_bmeta->bitmap_description[15], "unused");

    /* If processing L8, then we need to support the cirrus confidence and
       terrain occlusion */
    if (qa_category == LEVEL1_L8)
    {
        strcpy (l2qa_bmeta->bitmap_description[8], "cirrus confidence");
        strcpy (l2qa_bmeta->bitmap_description[9], "cirrus confidence");
        strcpy (l2qa_bmeta->bitmap_description[10], "terrain occlusion");
    }

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

    /* Add the pixel QA band to the XML file */
    if (append_metadata (1, l2qa_bmeta, espa_xml_file) != SUCCESS)
    {
        sprintf (errmsg, "Appending pixel QA band to XML file.");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Free the input and output XML metadata */
    free_metadata (&xml_metadata);
    free_metadata (&l2qa_metadata);

    /* Successfully generated the pixel QA band */
    return (SUCCESS);
}
