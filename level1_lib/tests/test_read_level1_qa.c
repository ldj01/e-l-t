/*****************************************************************************
FILE: test_read_level1_qa.c

PURPOSE: Contains test functions for testing the read and manipulation of the
Level-1 QA band.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3

NOTES:
  1. The XML metadata format parsed or written via this library follows the
     ESPA internal metadata format found in ESPA Raw Binary Format v1.0.doc.
     The schema for the ESPA internal metadata format is available at
     http://espa.cr.usgs.gov/schema/espa_internal_metadata_v1_0.xsd.
*****************************************************************************/
#include <getopt.h>
#include "read_level1_qa.h"

/******************************************************************************
MODULE: usage

PURPOSE: Prints the usage information for this application.

RETURN VALUE:
Type = None

NOTES:
******************************************************************************/
void usage ()
{
    printf ("test_read_level1_qa is a simple test program that opens, reads, "
            "and closed the Level-1 QA band. It prints out a few pixel values "
            "to allow for verification of the read.  It also uses the "
            "Level-1 QA interrogation functions to determine cloud, shadow, "
            "etc. and other QA bits for testing those functions.\n\n");
    printf ("usage: test_read_level1_qa --xml=input_xml_filename\n");

    printf ("\nwhere the following parameters are required:\n");
    printf ("    -xml: name of the input XML metadata file which follows "
            "the ESPA internal raw binary schema\n");
    printf ("\nExample: test_read_level1_qa "
            "--xml=LE07_L1TP_022033_20140228_20160905_01_T1.xml\n");
}


/******************************************************************************
MODULE:  get_args

PURPOSE:  Gets the command-line arguments and validates that the required
arguments were specified.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error getting the command-line arguments or a command-line
                argument and associated value were not specified
SUCCESS         No errors encountered

NOTES:
  1. Memory is allocated for the input and output files.  All of these should
     be character pointers set to NULL on input.  The caller is responsible
     for freeing the allocated memory upon successful return.
******************************************************************************/
short get_args
(
    int argc,             /* I: number of cmd-line args */
    char *argv[],         /* I: string of cmd-line args */
    char **xml_infile     /* O: address of input XML filename */
)
{
    int c;                           /* current argument index */
    int option_index;                /* index for the command-line option */
    char errmsg[STR_SIZE];           /* error message */
    char FUNC_NAME[] = "get_args";   /* function name */
    static struct option long_options[] =
    {
        {"xml", required_argument, 0, 'i'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    /* Loop through all the cmd-line options */
    opterr = 0;   /* turn off getopt_long error msgs as we'll print our own */
    while (1)
    {
        /* optstring in call to getopt_long is empty since we will only
           support the long options */
        c = getopt_long (argc, argv, "", long_options, &option_index);
        if (c == -1)
        {   /* Out of cmd-line options */
            break;
        }

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;

            case 'h':  /* help */
                usage ();
                return (ERROR);
                break;

            case 'i':  /* XML infile */
                *xml_infile = strdup (optarg);
                break;

            case '?':
            default:
                sprintf (errmsg, "Unknown option %s", argv[optind-1]);
                error_handler (true, FUNC_NAME, errmsg);
                usage ();
                return (ERROR);
                break;
        }
    }

    /* Make sure the infiles were specified */
    if (*xml_infile == NULL)
    {
        sprintf (errmsg, "XML input file is a required argument");
        error_handler (true, FUNC_NAME, errmsg);
        usage ();
        return (ERROR);
    }

    return (SUCCESS);
}


/******************************************************************************
MODULE:  main

PURPOSE:  Reads the Level-1 QA band and then spits out a few pixel values
to make sure the read is working correctly.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error with the Level-1 QA test
SUCCESS         No errors with the Level-1 QA test

NOTES:
******************************************************************************/
int main (int argc, char** argv)
{
    char FUNC_NAME[] = "test_read_level1_qa";   /* function name */
    char errmsg[STR_SIZE];       /* error message */
    char *xml_infile = NULL;     /* input XML filename */
    char l1_qa_file[STR_SIZE];   /* Level-1 QA filename from XML file */
    int exit_status = EXIT_SUCCESS; /* test exit status */
    int nlines;                  /* number of lines in the QA band */
    int nsamps;                  /* number of samples in the QA band */
    uint16_t *level1_qa_pixel = NULL;   /* Level-1 pixel QA band */
    uint16_t *level1_qa_radsat = NULL;  /* Level-1 radsat QA band */
    Espa_level1_qa_type qa_cat;  /* type of Level-1 QA data (L4-7/L89;
                                    pixel/radsat) */
    FILE *fp_bqa_pixel = NULL;   /* file pointer for the pixel quality band */
    FILE *fp_bqa_radsat = NULL;  /* file pointer for the radsat quality band */

    /* Read the command-line arguments */
    if (get_args (argc, argv, &xml_infile) != SUCCESS)
    {   /* get_args already printed the error message */
        exit (EXIT_FAILURE);
    }

    /* Open the Level-1 Pixel QA band */
    qa_cat = LEVEL1_BQA_PIXEL;
    fp_bqa_pixel = open_level1_qa (xml_infile, l1_qa_file, &nlines, &nsamps,
        &qa_cat);
    if (fp_bqa_pixel == NULL)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }
    printf ("Level-1 QA information:\n");
    printf ("  Filename: %s\n", l1_qa_file);
    printf ("  Filesize: %d lines x %d samples\n", nlines, nsamps);
    printf ("  QA Category: ");
    if (qa_cat == LEVEL1_BQA_PIXEL_L457)
        printf ("Landsat 4-7 Pixel QA\n");
    else if (qa_cat == LEVEL1_BQA_PIXEL_L89)
        printf ("Landsat 8 Pixel QA\n");
    else
        printf ("UNKNOWN\n");

    /* Allocate memory for the entire Level-1 QA band */
    level1_qa_pixel = calloc (nlines*nsamps, sizeof (uint16_t));
    if (level1_qa_pixel == NULL)
    {
        sprintf (errmsg, "Allocating memory for the Level-1 QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Read the entire band of Level-1 QA data */
    if (read_level1_qa (fp_bqa_pixel, nlines, nsamps, level1_qa_pixel)
        != SUCCESS)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }

    /* Close the Level-1 QA band */
    close_level1_qa (fp_bqa_pixel);

    /* Open the Level-1 Radsat QA band */
    qa_cat = LEVEL1_BQA_RADSAT;
    fp_bqa_radsat = open_level1_qa (xml_infile, l1_qa_file, &nlines, &nsamps,
        &qa_cat);
    if (fp_bqa_radsat == NULL)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }
    printf ("Level-1 QA information:\n");
    printf ("  Filename: %s\n", l1_qa_file);
    printf ("  Filesize: %d lines x %d samples\n", nlines, nsamps);
    printf ("  QA Category: ");
    if (qa_cat == LEVEL1_BQA_RADSAT_L457)
        printf ("Landsat 4-7 Radsat QA\n");
    else if (qa_cat == LEVEL1_BQA_RADSAT_L89)
        printf ("Landsat 8 Radsat QA\n");
    else
        printf ("UNKNOWN\n");

    /* Allocate memory for the entire Level-1 QA band */
    level1_qa_radsat = calloc (nlines*nsamps, sizeof (uint16_t));
    if (level1_qa_radsat == NULL)
    {
        sprintf (errmsg, "Allocating memory for the Level-1 QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Read the entire band of Level-1 QA data */
    if (read_level1_qa (fp_bqa_radsat, nlines, nsamps, level1_qa_radsat)
        != SUCCESS)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }

    /* Close the Level-1 QA band */
    close_level1_qa (fp_bqa_radsat);

    /* Print out the desired pixel values for testing */
    printf ("Pixel line 0, sample 0: Pixel %u Radsat %u\n",
        level1_qa_pixel[0], level1_qa_radsat[0]);
    printf ("Pixel line 0, sample 1000: Pixel %u Radsat %u\n",
        level1_qa_pixel[1000], level1_qa_radsat[1000]);
    printf ("Pixel line 4557, sample 4432: Pixel %u Radsat %u\n",
        level1_qa_pixel[4557*nsamps+4432], level1_qa_radsat[4557*nsamps+4432]);
    printf ("Pixel line 1560, sample 6305: Pixel %u Radsat %u\n",
        level1_qa_pixel[1560*nsamps+6305], level1_qa_radsat[1560*nsamps+6305]);
    printf ("Pixel line 3589, sample 6898: Pixel %u Radsat %u\n",
        level1_qa_pixel[1560*nsamps+6305], level1_qa_radsat[3589*nsamps+6898]);
    printf ("Pixel line 775, sample 3468: Pixel %u Radsat %u\n",
        level1_qa_pixel[1560*nsamps+6305], level1_qa_radsat[775*nsamps+3468]);

    /* Test the pixel interrogation */
    printf ("Fill Value %X: ", IAS_QUALITY_BIT_FILL);
    if (level1_qa_is_fill (1))
        printf ("Fill\n");
    else
    {
        printf ("Not Fill\n");
        exit_status = EXIT_FAILURE;
    }

    printf ("Not Fill Value %X: ", IAS_QUALITY_BIT_FILL-1);
    if (level1_qa_is_fill (0))
    {
        printf ("Fill\n");
        exit_status = EXIT_FAILURE;
    }
    else
        printf ("Not Fill\n");

    printf ("Terrain occluded Value %X: ",
        IAS_QUALITY_BIT_TERRAIN_OCCLUSION);
    if (level1_qa_is_terrain_occluded (IAS_QUALITY_BIT_TERRAIN_OCCLUSION))
        printf ("Terrain occluded\n");
    else
    {
        printf ("Not Terrain occluded\n");
        exit_status = EXIT_FAILURE;
    }

    printf ("Not Terrain occluded Value %X: ",
        IAS_QUALITY_BIT_TERRAIN_OCCLUSION-1);
    if (level1_qa_is_terrain_occluded (IAS_QUALITY_BIT_TERRAIN_OCCLUSION-1))
    {
        printf ("Terrain occluded\n");
        exit_status = EXIT_FAILURE;
    }
    else
        printf ("Not Terrain occluded\n");

    printf ("Dropped pixel Value %X: ", CC_MASK_DROPLINE);
    if (level1_qa_is_dropped_pixel (CC_MASK_DROPLINE))
        printf ("Dropped pixel\n");
    else
    {
        printf ("Not Dropped pixel\n");
        exit_status = EXIT_FAILURE;
    }

    printf ("Not Dropped pixel Value %X: ", CC_MASK_DROPLINE-1);
    if (level1_qa_is_dropped_pixel (CC_MASK_DROPLINE-1))
    {
        printf ("Dropped pixel\n");
        exit_status = EXIT_FAILURE;
    }
    else
        printf ("Not Dropped pixel\n");

    int saturation = 0xAAAA; /* test pixel, every other band has saturation */
    int band;
    for (band = 1; band <= 11; band++)
    {
        printf ("Radiometric saturation level (%X) band %d: %d\n",
            saturation, band,
            level1_qa_is_saturated (saturation, band));
        if (band % 2 == 0 && !level1_qa_is_saturated(saturation, band))
            exit_status = EXIT_FAILURE;
        if (band % 2 == 1 && level1_qa_is_saturated(saturation, band))
            exit_status = EXIT_FAILURE;
    }

    printf ("Cloud Value %X: ", IAS_QUALITY_BIT_CLOUD);
    if (level1_qa_is_cloud (IAS_QUALITY_BIT_CLOUD))
        printf ("Cloud\n");
    else
    {
        printf ("Not Cloud\n");
        exit_status = EXIT_FAILURE;
    }

    printf ("Not Cloud Value %X: ", IAS_QUALITY_BIT_CLOUD-1);
    if (level1_qa_is_cloud (IAS_QUALITY_BIT_CLOUD-1))
    {
        printf ("Cloud\n");
        exit_status = EXIT_FAILURE;
    }
    else
        printf ("Not Cloud\n");

    printf ("Water Value %X: ", IAS_QUALITY_BIT_WATER);
    if (level1_qa_is_water (IAS_QUALITY_BIT_WATER))
        printf ("Water\n");
    else
    {
        printf ("Not Water\n");
        exit_status = EXIT_FAILURE;
    }

    printf ("Not Water Value %X: ", IAS_QUALITY_BIT_WATER-1);
    if (level1_qa_is_water (IAS_QUALITY_BIT_WATER-1))
    {
        printf ("Water\n");
        exit_status = EXIT_FAILURE;
    }
    else
        printf ("Not Water\n");

    printf ("Cloud confidence (0): %d\n", level1_qa_cloud_confidence (0));
    if (level1_qa_cloud_confidence (0) != L1QA_NO_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Cloud confidence (IAS_QUALITY_BIT_CLOUD_01): %d\n",
        level1_qa_cloud_confidence (IAS_QUALITY_BIT_CLOUD_01));
    if (level1_qa_cloud_confidence (IAS_QUALITY_BIT_CLOUD_01) != L1QA_LOW_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Cloud confidence (IAS_QUALITY_BIT_CLOUD_10): %d\n",
        level1_qa_cloud_confidence (IAS_QUALITY_BIT_CLOUD_10));
    if (level1_qa_cloud_confidence (IAS_QUALITY_BIT_CLOUD_10) != L1QA_MED_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Cloud confidence (IAS_QUALITY_BIT_CLOUD_11): %d\n",
        level1_qa_cloud_confidence (IAS_QUALITY_BIT_CLOUD_11));
    if (level1_qa_cloud_confidence (IAS_QUALITY_BIT_CLOUD_11) != L1QA_HIGH_CONF)
        exit_status = EXIT_FAILURE;

    printf ("Cloud shadow conf (0): %d\n",
        level1_qa_cloud_shadow_confidence (0));
    if (level1_qa_cloud_shadow_confidence (0) != L1QA_NO_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Cloud shadow conf (IAS_QUALITY_BIT_CLOUD_SHADOW_01): %d\n",
        level1_qa_cloud_shadow_confidence (IAS_QUALITY_BIT_CLOUD_SHADOW_01));
    if (level1_qa_cloud_shadow_confidence (IAS_QUALITY_BIT_CLOUD_SHADOW_01)
        != L1QA_LOW_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Cloud shadow conf (IAS_QUALITY_BIT_CLOUD_SHADOW_10): %d\n",
        level1_qa_cloud_shadow_confidence (IAS_QUALITY_BIT_CLOUD_SHADOW_10));
    if (level1_qa_cloud_shadow_confidence (IAS_QUALITY_BIT_CLOUD_SHADOW_10)
        != L1QA_MED_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Cloud shadow conf (IAS_QUALITY_BIT_CLOUD_SHADOW_11): %d\n",
        level1_qa_cloud_shadow_confidence (IAS_QUALITY_BIT_CLOUD_SHADOW_11));
    if (level1_qa_cloud_shadow_confidence (IAS_QUALITY_BIT_CLOUD_SHADOW_11)
        != L1QA_HIGH_CONF)
        exit_status = EXIT_FAILURE;


    printf ("Snow/ice conf (0): %d\n",
        level1_qa_snow_ice_confidence (0));
    if (level1_qa_snow_ice_confidence (0) != L1QA_NO_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Snow/ice conf (IAS_QUALITY_BIT_SNOW_ICE_01): %d\n",
        level1_qa_snow_ice_confidence (IAS_QUALITY_BIT_SNOW_ICE_01));
    if (level1_qa_snow_ice_confidence (IAS_QUALITY_BIT_SNOW_ICE_01)
        != L1QA_LOW_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Snow/ice conf (IAS_QUALITY_BIT_SNOW_ICE_10): %d\n",
        level1_qa_snow_ice_confidence (IAS_QUALITY_BIT_SNOW_ICE_10));
    if (level1_qa_snow_ice_confidence (IAS_QUALITY_BIT_SNOW_ICE_10)
        != L1QA_MED_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Snow/ice conf (IAS_QUALITY_BIT_SNOW_ICE_11): %d\n",
        level1_qa_snow_ice_confidence (IAS_QUALITY_BIT_SNOW_ICE_11));
    if (level1_qa_snow_ice_confidence (IAS_QUALITY_BIT_SNOW_ICE_11)
        != L1QA_HIGH_CONF)
        exit_status = EXIT_FAILURE;

    printf ("Cirrus conf (0): %d\n",
        level1_qa_cirrus_confidence (0));
    if (level1_qa_cirrus_confidence (0) != L1QA_NO_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Cirrus conf (IAS_QUALITY_BIT_CIRRUS_01): %d\n",
        level1_qa_cirrus_confidence (IAS_QUALITY_BIT_CIRRUS_01));
    if (level1_qa_cirrus_confidence (IAS_QUALITY_BIT_CIRRUS_01)
        != L1QA_LOW_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Cirrus conf (IAS_QUALITY_BIT_CIRRUS_10): %d\n",
        level1_qa_cirrus_confidence (IAS_QUALITY_BIT_CIRRUS_10));
    if (level1_qa_cirrus_confidence (IAS_QUALITY_BIT_CIRRUS_10)
        != L1QA_MED_CONF)
        exit_status = EXIT_FAILURE;
    printf ("Cirrus conf (IAS_QUALITY_BIT_CIRRUS_11): %d\n",
        level1_qa_cirrus_confidence (IAS_QUALITY_BIT_CIRRUS_11));
    if (level1_qa_cirrus_confidence (IAS_QUALITY_BIT_CIRRUS_11)
        != L1QA_HIGH_CONF)
        exit_status = EXIT_FAILURE;


    /* Free the pointers */
    free (xml_infile);
    free (level1_qa_pixel);
    free (level1_qa_radsat);

    /* Successful completion */
    if (exit_status == EXIT_SUCCESS)
        printf ("Successful read and processing!\n");
    else
        printf ("ERROR! Please examine test output.\n");
    exit (exit_status);
}
