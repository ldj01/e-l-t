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
    int nlines;                  /* number of lines in the QA band */
    int nsamps;                  /* number of samples in the QA band */
    uint16_t *level1_qa = NULL;  /* Level-1 QA band */
    Espa_level1_qa_type qa_cat;  /* type of Level-1 QA data (L4-7, L8) */
    FILE *fp_bqa = NULL;         /* file pointer for the band quality band */

    /* Read the command-line arguments */
    if (get_args (argc, argv, &xml_infile) != SUCCESS)
    {   /* get_args already printed the error message */
        exit (EXIT_FAILURE);
    }

    /* Open the Level-1 QA band */
    fp_bqa = open_level1_qa (xml_infile, l1_qa_file, &nlines, &nsamps, &qa_cat);
    if (fp_bqa == NULL)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }
    printf ("Level-1 QA information:\n");
    printf ("  Filename: %s\n", l1_qa_file);
    printf ("  Filesize: %d lines x %d samples\n", nlines, nsamps);
    printf ("  QA Category: ");
    if (qa_cat == LEVEL1_L457)
        printf ("Landsat 4-7\n");
    else if (qa_cat == LEVEL1_L8)
        printf ("Landsat 8\n");
    else
        printf ("UNKNOWN\n");

    /* Allocate memory for the entire Level-1 QA band */
    level1_qa = calloc (nlines*nsamps, sizeof (uint16_t));
    if (level1_qa == NULL)
    {
        sprintf (errmsg, "Allocating memory for the Level-1 QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Read the entire band of Level-1 QA data */
    if (read_level1_qa (fp_bqa, nlines, nsamps, level1_qa) != SUCCESS)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }

    /* Close the Level-1 QA band */
    close_level1_qa (fp_bqa);

    /* Print out the desired pixel values for testing */
    printf ("Pixel line 0, sample 0: %d\n", level1_qa[0]);
    printf ("Pixel line 0, sample 1000: %d\n", level1_qa[1000]);
    printf ("Pixel line 4557, sample 4432: %d\n", level1_qa[4557*nsamps+4432]);
    printf ("Pixel line 1560, sample 6305: %d\n", level1_qa[1560*nsamps+6305]);
    printf ("Pixel line 3589, sample 6898: %d\n", level1_qa[3589*nsamps+6898]);
    printf ("Pixel line 775, sample 3468: %d\n", level1_qa[775*nsamps+3468]);

    /* Test the pixel interrogation */
    printf ("Value 1: ");   /* Fill */
    if (level1_qa_is_fill (1))
        printf ("Fill\n");
    else
        printf ("Not fill\n");

    printf ("Value 0: ");   /* Not fill */
    if (level1_qa_is_fill (0))
        printf ("Fill\n");
    else
        printf ("Not fill\n");

    printf ("Value 2: ");   /* Terrain occluded */
    if (level1_qa_is_terrain_occluded (2))
        printf ("Terrain occluded\n");
    else
        printf ("Not terrain occluded\n");

    printf ("Value 5: ");   /* Not terrain occluded */
    if (level1_qa_is_terrain_occluded (4))
        printf ("Terrain occluded\n");
    else
        printf ("Not terrain occluded\n");

    printf ("Value 2: ");   /* Dropped pixel */
    if (level1_qa_is_dropped_pixel (2))
        printf ("Dropped pixel\n");
    else
        printf ("Not dropped pixel\n");

    printf ("Value 5: ");   /* Not dropped pixel */
    if (level1_qa_is_dropped_pixel (5))
        printf ("Dropped pixel\n");
    else
        printf ("Not dropped pixel\n");

    printf ("Radiometric saturation level (0): %d\n",
        level1_qa_radiometric_saturation (0));
    printf ("Radiometric saturation level (4): %d\n",
        level1_qa_radiometric_saturation (4));
    printf ("Radiometric saturation level (8): %d\n",
        level1_qa_radiometric_saturation (8));
    printf ("Radiometric saturation level (12): %d\n",
        level1_qa_radiometric_saturation (12));

    printf ("Value 16: ");   /* Cloud */
    if (level1_qa_is_cloud (16))
        printf ("Cloud\n");
    else
        printf ("Not cloud\n");

    printf ("Value 15: ");   /* Not cloud */
    if (level1_qa_is_cloud (15))
        printf ("cloud\n");
    else
        printf ("Not cloud\n");

    printf ("Cloud confidence (12): %d\n", level1_qa_cloud_confidence (12));
    printf ("Cloud confidence (32): %d\n", level1_qa_cloud_confidence (32));
    printf ("Cloud confidence (64): %d\n", level1_qa_cloud_confidence (64));
    printf ("Cloud confidence (32+64): %d\n", level1_qa_cloud_confidence (32+64));

    printf ("Cloud shadow conf (12): %d\n", level1_qa_cloud_shadow_confidence (12));
    printf ("Cloud shadow conf (128): %d\n", level1_qa_cloud_shadow_confidence (128));
    printf ("Cloud shadow conf (256): %d\n", level1_qa_cloud_shadow_confidence (256));
    printf ("Cloud shadow conf (128+256): %d\n", level1_qa_cloud_shadow_confidence (128+256));


    printf ("Snow/ice conf (12): %d\n", level1_qa_snow_ice_confidence (12));
    printf ("Snow/ice conf (512): %d\n", level1_qa_snow_ice_confidence (512));
    printf ("Snow/ice conf (1024): %d\n", level1_qa_snow_ice_confidence (1024));
    printf ("Snow/ice conf (512+1024): %d\n", level1_qa_snow_ice_confidence (512+1024));

    printf ("Cirrus conf (12): %d\n", level1_qa_cirrus_confidence (12));
    printf ("Cirrus conf (2048): %d\n", level1_qa_cirrus_confidence (2048));
    printf ("Cirrus conf (4096): %d\n", level1_qa_cirrus_confidence (4096));
    printf ("Cirrus conf (2048+4096): %d\n", level1_qa_cirrus_confidence (2048+4096));
    printf ("Cirrus conf (8192+2048+4096): %d\n", level1_qa_cirrus_confidence (8192+2048+4096));


    /* Free the pointers */
    free (xml_infile);
    free (level1_qa);

    /* Successful completion */
    printf ("Successful read and processing!\n");
    exit (EXIT_SUCCESS);
}
