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
    printf ("    -qa_type: LEDAPS CLOUD=%d, LASRC AEROSOL=%d\n",
            LEDAPS_CLOUD, LASRC_AEROSOL);
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
    if (*qa_type < LEDAPS_CLOUD || *qa_type > LASRC_AEROSOL)
    {
        sprintf (errmsg, "QA type must be between %d and %d\n", LEDAPS_CLOUD,
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
    int exit_status = EXIT_SUCCESS; /* test exit status */
    int value;                   /* test qa value */
    int nlines;                  /* number of lines in the QA band */
    int nsamps;                  /* number of samples in the QA band */
    uint8_t *level2_qa = NULL;   /* Level-2 QA band */
    Espa_level2_qa_type qa_type; /* type of Level-2 QA data to be read */
    FILE *fp_l2qa = NULL;        /* file pointer for the band quality band */

    /* Read the command-line arguments */
    if (get_args (argc, argv, &xml_infile, &qa_type) != SUCCESS)
    {   /* get_args already printed the error message */
        exit (EXIT_FAILURE);
    }

    /* Open the Level-2 QA band */
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
    if (qa_type == LEDAPS_CLOUD)
        printf ("LEDAPS CLOUD\n");
    else if (qa_type == LASRC_AEROSOL)
        printf ("LaSRC AEROSOL\n");
    else
        printf ("UNKNOWN\n");

    /* Handle the 8-bit QA values */
    if (qa_type == LEDAPS_CLOUD ||
        qa_type == LASRC_AEROSOL)
    {
        /* Allocate memory for the entire Level-2 QA band */
        level2_qa = calloc (nlines*nsamps, sizeof (uint8_t));
        if (level2_qa == NULL)
        {
            sprintf (errmsg, "Allocating memory for the Level-2 QA band");
            error_handler (true, FUNC_NAME, errmsg);
            return (ERROR);
        }

        /* Read the entire band of Level-2 QA data */
        if (read_level2_qa (fp_l2qa, nlines, nsamps, level2_qa)
            != SUCCESS)
        {  /* Error messages already written */
            exit (EXIT_FAILURE);
        }

        /* Close the Level-2 QA band */
        close_level2_qa (fp_l2qa);

        /* Print out the desired pixel values for testing */
        printf ("Pixel line 0, sample 0: %d\n", level2_qa[0]);
        printf ("Pixel line 0, sample 1000: %d\n", level2_qa[1000]);
        printf ("Pixel line 4557, sample 4432: %d\n",
                level2_qa[4557*nsamps+4432]);
        printf ("Pixel line 1560, sample 6305: %d\n",
                level2_qa[1560*nsamps+6305]);
        printf ("Pixel line 3589, sample 6898: %d\n",
                level2_qa[3589*nsamps+6898]);
        printf ("Pixel line 775, sample 3468: %d\n",
                level2_qa[775*nsamps+3468]);
        printf ("Pixel line 2335, sample 4403: %d\n",
                level2_qa[2335*nsamps+4403]);

        /* Test the pixel interrogation for LEDAPS CLOUD */
        if (qa_type == LEDAPS_CLOUD)
        {
            value = LEDAPS_DDV_BIT;
            printf ("DDV Value %d: ", value);
            if (ledaps_qa_is_ddv (value))
                printf ("**DDV\n");
            else
            {
                printf ("Not DDV\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not DDV Value %d: ", value);
            if (ledaps_qa_is_ddv (value))
            {
                printf ("DDV\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not DDV\n");

            value = LEDAPS_CLOUD_BIT;
            printf ("Cloud Value %d: ", value);
            if (ledaps_qa_is_cloud (value))
                printf ("**Cloud\n");
            else
            {
                printf ("Not cloud\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Cloud Value %d: ", value);
            if (ledaps_qa_is_cloud (value))
            {
                printf ("Cloud\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not cloud\n");

            value = LEDAPS_CLOUD_SHADOW_BIT;
            printf ("Cloud shadow Value %d: ", value);
            if (ledaps_qa_is_cloud_shadow (value))
                printf ("**Cloud shadow\n");
            else
            {
                printf ("Not cloud shadow\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Cloud shadow Value %d: ", value);
            if (ledaps_qa_is_cloud_shadow (value))
            {
                printf ("Cloud shadow\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not cloud shadow\n");

            value = LEDAPS_ADJ_CLOUD_BIT;
            printf ("Adjacent cloud Value %d: ", value);
            if (ledaps_qa_is_adj_cloud (value))
                printf ("**Adjacent cloud\n");
            else
            {
                printf ("Not adjacent cloud\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Adjacent cloud Value %d: ", value);
            if (ledaps_qa_is_adj_cloud (value))
            {
                printf ("Adjacent cloud\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not adjacent cloud\n");

            value = LEDAPS_SNOW_BIT;
            printf ("Snow Value %d: ", value);
            if (ledaps_qa_is_snow (value))
                printf ("**Snow\n");
            else
            {
                printf ("Not snow\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Snow Value %d: ", value);
            if (ledaps_qa_is_snow (value))
            {
                printf ("Snow\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not snow\n");

            value = LEDAPS_LAND_WATER_BIT;
            printf ("Land Water Value %d: ", value);
            if (ledaps_qa_is_land_water (value))
                printf ("**Land\n");
            else
            {
                printf ("Water\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Land Water Value %d: ", value);
            if (ledaps_qa_is_land_water (value))
            {
                printf ("Land\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Water\n");
        }

        /* Test the pixel interrogation for LaSRC AEROSOL */
        else if (qa_type == LASRC_AEROSOL)
        {
            value = LASRC_FILL_BIT;
            printf ("Fill Value %d: ", value);
            if (lasrc_qa_is_fill (value))
                printf ("**Fill\n");
            else
            {
                printf ("Not fill\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Fill Value %d: ", value);
            if (lasrc_qa_is_fill (value))
            {
                printf ("Fill\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not fill\n");

            value = LASRC_VALID_AEROSOL_RET_BIT;
            printf ("Valid aerosol retrieval Value %d: ", value);
            if (lasrc_qa_is_valid_aerosol_retrieval (value))
                printf ("**Valid aerosol retrieval\n");
            else
            {
                printf ("Not valid aerosol retrieval\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Valid aerosol retrieval Value %d: ", value);
            if (lasrc_qa_is_valid_aerosol_retrieval (value))
            {
                printf ("Valid aerosol retrieval\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not valid aerosol retrieval\n");

            value = LASRC_WATER_BIT;
            printf ("Water Value %d: ", value);
            if (lasrc_qa_is_water (value))
                printf ("**Water\n");
            else
            {
                printf ("Not water\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Water Value %d: ", value);
            if (lasrc_qa_is_water (value))
            {
                printf ("Water\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not water\n");

            value = LASRC_CLOUD_CIRRUS_BIT;
            printf ("Cloud cirrus Value %d: ", value);
            if (lasrc_qa_is_cloud_cirrus (value))
                printf ("**Cloud cirrus\n");
            else
            {
                printf ("Not Cloud cirrus\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Cloud cirrus Value %d: ", value);
            if (lasrc_qa_is_cloud_cirrus (value))
            {
                printf ("Cloud cirrus\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not Cloud cirrus\n");

            value = LASRC_CLOUD_SHADOW_BIT;
            printf ("Cloud shadow Value %d: ", value);
            if (lasrc_qa_is_cloud_shadow (value))
                printf ("**Cloud shadow\n");
            else
            {
                printf ("Not Cloud shadow\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Cloud shadow Value %d: ", value);
            if (lasrc_qa_is_cloud_shadow (value))
            {
                printf ("Cloud shadow\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Not Cloud shadow\n");

            value = LASRC_AEROSOL_INTERP_BIT;
            printf ("Aerosol interpolated Value %d: ", value);
            if (lasrc_qa_is_aerosol_interp (value))
                printf ("**Aerosol was interpolated\n");
            else
            {
                printf ("Aerosol was not interpolated\n");
                exit_status = EXIT_FAILURE;
            }

            value = 0;
            printf ("Not Aerosol interpolated Value %d: ", value);
            if (lasrc_qa_is_aerosol_interp (value))
            {
                printf ("Aerosol was interpolated\n");
                exit_status = EXIT_FAILURE;
            }
            else
                printf ("**Aerosol was not interpolated\n");

            value = 0x0 << LASRC_AEROSOL_LEVEL_BIT_NUMBER;
            printf ("Aerosol level (%d): %d\n",
                value, lasrc_qa_aerosol_level (value));
            if (lasrc_qa_aerosol_level(value) != 0)
                exit_status = EXIT_FAILURE;
            value = 0x1 << LASRC_AEROSOL_LEVEL_BIT_NUMBER;
            printf ("Aerosol level (%d): %d\n",
                value, lasrc_qa_aerosol_level (value));
            if (lasrc_qa_aerosol_level(value) != 1)
                exit_status = EXIT_FAILURE;
            value = 0x2 << LASRC_AEROSOL_LEVEL_BIT_NUMBER;
            printf ("Aerosol level (%d): %d\n",
                value, lasrc_qa_aerosol_level (value));
            if (lasrc_qa_aerosol_level(value) != 2)
                exit_status = EXIT_FAILURE;
            value = 0x3 << LASRC_AEROSOL_LEVEL_BIT_NUMBER;
            printf ("Aerosol level (%d): %d\n",
                value, lasrc_qa_aerosol_level (value));
            if (lasrc_qa_aerosol_level(value) != 3)
                exit_status = EXIT_FAILURE;

        }

        /* Free the data pointer */
        free (level2_qa);
    }

    /* Free the pointers */
    free (xml_infile);

    /* Successful completion */
    if (exit_status == EXIT_SUCCESS)
        printf ("Successful read and processing!\n");
    else
        printf ("ERROR! Please examine test output.\n");
    exit (exit_status);
}
