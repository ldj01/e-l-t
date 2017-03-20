/*****************************************************************************
FILE: test_read_level2_qa.c
  
PURPOSE: Contains test functions for testing the read and manipulation of the
Level-2 QA band.

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
#include "read_level2_qa.h"

/******************************************************************************
MODULE: usage

PURPOSE: Prints the usage information for this application.

RETURN VALUE:
Type = None

NOTES:
******************************************************************************/
void usage ()
{
    printf ("test_read_level2_qa is a simple test program that opens, reads, "
            "and closes the Level-2 QA band. It prints out a few pixel values "
            "to allow for verification of the read.  It also uses the "
            "Level-2 QA interrogation functions to determine cloud, shadow, "
            "saturation, aerosol levels, and other QA bits for testing those "
            "functions.\n\n");
    printf ("usage: test_read_level2_qa --xml=input_xml_filename "
            "--qa_type=type_of_qa_band_to_read\n");

    printf ("\nwhere the following parameters are required:\n");
    printf ("    -xml: name of the input XML metadata file which follows "
            "the ESPA internal raw binary schema (for Collections only)\n");
    printf ("    -qa_type: LEDAPS RADSAT=%d, LEDAPS CLOUD=%d, "
            "LASRC AEROSOL=%d\n", LEDAPS_RADSAT, LEDAPS_CLOUD, LASRC_AEROSOL);
    printf ("\nExample: test_read_level2_qa "
            "--xml=LC08_L1TP_168081_20160429_20170117_01_T1.xml --qa_type=2\n");
    printf ("\nExample: test_read_level2_qa "
            "--xml=LE07_L1TP_022033_20140228_20161028_01_T1.xml --qa_type=0\n");
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
  1. Memory is allocated for the input file.  This should be a character
     pointers set to NULL on input.  The caller is responsible for freeing the
     allocated memory upon successful return.
******************************************************************************/
short get_args
(
    int argc,             /* I: number of cmd-line args */
    char *argv[],         /* I: string of cmd-line args */
    char **xml_infile,    /* O: address of input XML filename */
    Espa_level2_qa_type *qa_type   /* O: which QA is being read */
)
{
    int c;                           /* current argument index */
    int option_index;                /* index for the command-line option */
    char errmsg[STR_SIZE];           /* error message */
    char FUNC_NAME[] = "get_args";   /* function name */
    static struct option long_options[] =
    {
        {"xml", required_argument, 0, 'i'},
        {"qa_type", required_argument, 0, 'q'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    /* Loop through all the cmd-line options */
    opterr = 0;   /* turn off getopt_long error msgs as we'll print our own */
    *qa_type = -99;  /* initialize the QA type */
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
     
            case 'q':  /* QA type */
                *qa_type = atoi (optarg);
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

    /* Validate the QA type */
    if (*qa_type < LEDAPS_RADSAT || *qa_type > LASRC_AEROSOL)
    {
        sprintf (errmsg, "QA type must be between %d and %d\n", LEDAPS_RADSAT,
            LASRC_AEROSOL);
        error_handler (true, FUNC_NAME, errmsg);
        usage ();
        return (ERROR);
    }

    return (SUCCESS);
}


/******************************************************************************
MODULE:  main

PURPOSE:  Reads the Level-2 QA band and then spits out a few pixel values
to make sure the read is working correctly.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error with the Level-2 QA test
SUCCESS         No errors with the Level-2 QA test

NOTES:
******************************************************************************/
int main (int argc, char** argv)
{
    char FUNC_NAME[] = "test_read_level2_qa";   /* function name */
    char errmsg[STR_SIZE];       /* error message */
    char *xml_infile = NULL;     /* input XML filename */
    char l2_qa_file[STR_SIZE];   /* Level-2 QA filename from XML file */
    int nlines;                  /* number of lines in the QA band */
    int nsamps;                  /* number of samples in the QA band */
    uint8_t *level2_qa = NULL;   /* Level-2 QA band */
    Espa_level2_qa_type qa_type; /* type of Level-2 QA data to be read */
    FILE *fp_l2qa = NULL;        /* file pointer for the band quality band */

    /* Read the command-line arguments */
    printf ("DEBUG: get_args\n"); fflush(stdout);
    if (get_args (argc, argv, &xml_infile, &qa_type) != SUCCESS)
    {   /* get_args already printed the error message */
        exit (EXIT_FAILURE);
    }

    /* Open the Level-2 QA band */
    printf ("DEBUG: open_level2_qa file\n"); fflush(stdout);
    fp_l2qa = open_level2_qa (xml_infile, qa_type, l2_qa_file, &nlines,
        &nsamps);
    if (fp_l2qa == NULL)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }
    printf ("Level-2 QA information:\n");
    printf ("  Filename: %s\n", l2_qa_file);
    printf ("  Filesize: %d lines x %d samples\n", nlines, nsamps);
    printf ("  QA Category: ");
    if (qa_type == LEDAPS_RADSAT)
        printf ("LEDAPS RADSAT\n");
    else if (qa_type == LEDAPS_CLOUD)
        printf ("LEDAPS CLOUD\n");
    else if (qa_type == LASRC_AEROSOL)
        printf ("LaSRC AEROSOL\n");
    else
        printf ("UNKNOWN\n");

    /* Allocate memory for the entire Level-2 QA band */
    printf ("DEBUG: alloc for level2_qa\n"); fflush(stdout);
    level2_qa = calloc (nlines*nsamps, sizeof (uint8_t));
    if (level2_qa == NULL)
    {
        sprintf (errmsg, "Allocating memory for the Level-2 QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Read the entire band of Level-2 QA data */
    printf ("DEBUG: read_level2_qa\n"); fflush(stdout);
    if (read_level2_qa (fp_l2qa, nlines, nsamps, level2_qa) != SUCCESS)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }

    /* Close the Level-2 QA band */
    printf ("DEBUG: close_level2_qa\n"); fflush(stdout);
    close_level2_qa (fp_l2qa);

    /* Print out the desired pixel values for testing */
    printf ("Pixel line 0, sample 0: %d\n", level2_qa[0]);
    printf ("Pixel line 0, sample 1000: %d\n", level2_qa[1000]);
    printf ("Pixel line 4557, sample 4432: %d\n", level2_qa[4557*nsamps+4432]);
    printf ("Pixel line 1560, sample 6305: %d\n", level2_qa[1560*nsamps+6305]);
    printf ("Pixel line 3589, sample 6898: %d\n", level2_qa[3589*nsamps+6898]);
    printf ("Pixel line 775, sample 3468: %d\n", level2_qa[775*nsamps+3468]);
    printf ("Pixel line 2335, sample 4403: %d\n", level2_qa[2335*nsamps+4403]);

    /* Test the pixel interrogation for LEDAPS RADSAT */
    if (qa_type == LEDAPS_RADSAT)
    {
        printf ("Value 1: ");   /* LEDAPS fill */
        if (ledaps_qa_is_fill (1))
            printf ("**Fill\n");
        else
            printf ("Not fill\n");

        printf ("Value 0: ");   /* LEDAPS not fill */
        if (ledaps_qa_is_fill (0))
            printf ("Fill\n");
        else
            printf ("**Not fill\n");

        printf ("Value 2: ");   /* LEDAPS band 1 saturated */
        if (ledaps_qa_is_saturated (2, LEDAPS_B1_SAT_BIT))
            printf ("**Band 1 Saturated\n");
        else
            printf ("Band 1 Not saturated\n");

        printf ("Value 4: ");   /* LEDAPS band 2 saturated */
        if (ledaps_qa_is_saturated (4, LEDAPS_B2_SAT_BIT))
            printf ("**Band 2 Saturated\n");
        else
            printf ("Band 2 Not saturated\n");

        printf ("Value 8: ");   /* LEDAPS band 3 saturated */
        if (ledaps_qa_is_saturated (8, LEDAPS_B3_SAT_BIT))
            printf ("**Band 3 Saturated\n");
        else
            printf ("Band 3 Not saturated\n");

        printf ("Value 16: ");   /* LEDAPS band 4 saturated */
        if (ledaps_qa_is_saturated (16, LEDAPS_B4_SAT_BIT))
            printf ("**Band 4 Saturated\n");
        else
            printf ("Band 4 Not saturated\n");

        printf ("Value 32: ");   /* LEDAPS band 5 saturated */
        if (ledaps_qa_is_saturated (32, LEDAPS_B5_SAT_BIT))
            printf ("**Band 5 Saturated\n");
        else
            printf ("Band 5 Not saturated\n");

        printf ("Value 64: ");   /* LEDAPS band 6 saturated */
        if (ledaps_qa_is_saturated (64, LEDAPS_B6_SAT_BIT))
            printf ("**Band 6 Saturated\n");
        else
            printf ("Band 6 Not saturated\n");

        printf ("Value 128: ");   /* LEDAPS band 7 saturated */
        if (ledaps_qa_is_saturated (128, LEDAPS_B7_SAT_BIT))
            printf ("**Band 7 Saturated\n");
        else
            printf ("Band 7 Not saturated\n");

        printf ("Value 128: ");   /* LEDAPS band 1 not saturated */
        if (ledaps_qa_is_saturated (128, LEDAPS_B1_SAT_BIT))
            printf ("Band 1 Saturated\n");
        else
            printf ("**Band 1 Not saturated\n");
    }

    /* Test the pixel interrogation for LEDAPS CLOUD */
    if (qa_type == LEDAPS_CLOUD)
    {
        printf ("Value 1: ");   /* LEDAPS DDV */
        if (ledaps_qa_is_ddv (1))
            printf ("**DDV\n");
        else
            printf ("Not DDV\n");

        printf ("Value 0: ");
        if (ledaps_qa_is_ddv (0))
            printf ("DDV\n");
        else
            printf ("**Not DDV\n");

        printf ("Value 2: ");   /* LEDAPS cloud */
        if (ledaps_qa_is_cloud (2))
            printf ("**Cloud\n");
        else
            printf ("Not cloud\n");

        printf ("Value 4: ");
        if (ledaps_qa_is_cloud (4))
            printf ("Cloud\n");
        else
            printf ("**Not cloud\n");

        printf ("Value 4: ");   /* LEDAPS cloud shadow */
        if (ledaps_qa_is_cloud_shadow (4))
            printf ("**Cloud shadow\n");
        else
            printf ("Not cloud shadow\n");

        printf ("Value 0: ");
        if (ledaps_qa_is_cloud_shadow (0))
            printf ("Cloud shadow\n");
        else
            printf ("**Not cloud shadow\n");

        printf ("Value 8: ");   /* LEDAPS adjacent cloud */
        if (ledaps_qa_is_adj_cloud (8))
            printf ("**Adjacent cloud\n");
        else
            printf ("Not adjacent cloud\n");

        printf ("Value 3: ");
        if (ledaps_qa_is_adj_cloud (3))
            printf ("Adjacent cloud\n");
        else
            printf ("**Not adjacent cloud\n");

        printf ("Value 16: ");   /* LEDAPS snow */
        if (ledaps_qa_is_snow (16))
            printf ("**Snow\n");
        else
            printf ("Not snow\n");

        printf ("Value 15: ");
        if (ledaps_qa_is_snow (15))
            printf ("Snow\n");
        else
            printf ("**Not snow\n");

        printf ("Value 32: ");   /* LEDAPS land/water */
        if (ledaps_qa_is_land_water (32))
            printf ("**Land\n");
        else
            printf ("Water\n");

        printf ("Value 15: ");
        if (ledaps_qa_is_land_water (15))
            printf ("Land\n");
        else
            printf ("**Water\n");
    }

    /* Test the pixel interrogation for LaSRC AEROSOL */
    if (qa_type == LASRC_AEROSOL)
    {
        printf ("Value 1: ");   /* LaSRC fill */
        if (lasrc_qa_is_fill (1))
            printf ("**Fill\n");
        else
            printf ("Not fill\n");

        printf ("Value 0: ");
        if (lasrc_qa_is_fill (0))
            printf ("Fill\n");
        else
            printf ("**Not fill\n");

        printf ("Value 2: ");   /* LaSRC valid aerosol retrieval */
        if (lasrc_qa_is_valid_aerosol_retrieval (2))
            printf ("**Valid aerosol retrieval\n");
        else
            printf ("Not valid aerosol retrieval\n");

        printf ("Value 8: ");
        if (lasrc_qa_is_valid_aerosol_retrieval (8))
            printf ("Valid aerosol retrieval\n");
        else
            printf ("**Not valid aerosol retrieval\n");

        printf ("Value 4: ");   /* LaSRC aerosol interpolation */
        if (lasrc_qa_is_aerosol_interp (4))
            printf ("**Aerosol was interpolated\n");
        else
            printf ("Aerosol was not interpolated\n");

        printf ("Value 3: ");
        if (lasrc_qa_is_aerosol_interp (3))
            printf ("Aerosol was interpolated\n");
        else
            printf ("**Aerosol was not interpolated\n");

        printf ("Value 8: ");   /* LaSRC water */
        if (lasrc_qa_is_water (8))
            printf ("**Water\n");
        else
            printf ("Not water\n");

        printf ("Value 7: ");
        if (lasrc_qa_is_water (7))
            printf ("Water\n");
        else
            printf ("**Not water\n");

        printf ("Aerosol level (12): %d\n", lasrc_qa_aerosol_level (12));
        printf ("Aerosol level (64): %d\n", lasrc_qa_aerosol_level (64));
        printf ("Aerosol level (128): %d\n", lasrc_qa_aerosol_level (128));
        printf ("Aerosol level (64_128): %d\n", lasrc_qa_aerosol_level (64+128));
    }

    /* Free the pointers */
    free (xml_infile);
    free (level2_qa);

    /* Successful completion */
    printf ("Successful read and processing!\n");
    exit (EXIT_SUCCESS);
}
