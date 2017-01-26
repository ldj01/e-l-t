/*****************************************************************************
FILE: test_read_pixel_qa.c
  
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
#include "read_pixel_qa.h"

/******************************************************************************
MODULE: usage

PURPOSE: Prints the usage information for this application.

RETURN VALUE:
Type = None

NOTES:
******************************************************************************/
void usage ()
{
    printf ("test_read_pixel_qa is a simple test program that opens, reads, "
            "and closes the Level-2 pixel QA band. It prints out a few pixel "
            "values to allow for verification of the read.  It also uses the "
            "Level-2 pixel QA interrogation functions to determine cloud, "
            "shadow, etc. and other QA bits for testing those functions.\n\n");
    printf ("usage: test_read_pixel_qa --xml=input_xml_filename\n");

    printf ("\nwhere the following parameters are required:\n");
    printf ("    -xml: name of the input XML metadata file which follows "
            "the ESPA internal raw binary schema\n");
    printf ("\nExample: test_read_pixel_qa --xml=LE70230282011250EDC00.xml\n");
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

PURPOSE:  Reads the Level-2 pixel QA band and then spits out a few pixel values
to make sure the read is working correctly.

RETURN VALUE:
Type = int
Value           Description
-----           -----------
ERROR           Error with the Level-2 pixel QA test
SUCCESS         No errors with the Level-2 pixel QA test

NOTES:
******************************************************************************/
int main (int argc, char** argv)
{
    char FUNC_NAME[] = "test_read_pixel_qa";   /* function name */
    char errmsg[STR_SIZE];       /* error message */
    char *xml_infile = NULL;     /* input XML filename */
    char l2_qa_file[STR_SIZE];   /* Level-2 pixel QA filename from XML file */
    int nlines;                  /* number of lines in the QA band */
    int nsamps;                  /* number of samples in the QA band */
    uint16_t *pixel_qa = NULL;   /* Level-1 QA band */
    FILE *fp_pqa = NULL;         /* file pointer for the pixel QA band */

    /* Read the command-line arguments */
    printf ("DEBUG: get_args\n"); fflush(stdout);
    if (get_args (argc, argv, &xml_infile) != SUCCESS)
    {   /* get_args already printed the error message */
        exit (EXIT_FAILURE);
    }

    /* Open the Level-2 pixel QA band */
    printf ("DEBUG: open_pixel_qa\n"); fflush(stdout);
    fp_pqa = open_pixel_qa (xml_infile, l2_qa_file, &nlines, &nsamps);
    if (fp_pqa == NULL)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }
    printf ("Level-2 pixel QA information:\n");
    printf ("  Filename: %s\n", l2_qa_file);
    printf ("  Filesize: %d lines x %d samples\n", nlines, nsamps);

    /* Allocate memory for the entire Level-2 pixel QA band */
    printf ("DEBUG: alloc for pixel_qa\n"); fflush(stdout);
    pixel_qa = calloc (nlines*nsamps, sizeof (uint16_t));
    if (pixel_qa == NULL)
    {
        sprintf (errmsg, "Allocating memory for the Level-1 pixel QA band");
        error_handler (true, FUNC_NAME, errmsg);
        return (ERROR);
    }

    /* Read the entire band of Level-2 pixel QA data */
    printf ("DEBUG: read_pixel_qa\n"); fflush(stdout);
    if (read_pixel_qa (fp_pqa, nlines, nsamps, pixel_qa) != SUCCESS)
    {  /* Error messages already written */
        exit (EXIT_FAILURE);
    }

    /* Close the Level-2 pixel QA band */
    printf ("DEBUG: close_pixel_qa\n"); fflush(stdout);
    close_pixel_qa (fp_pqa);

    /* Print out the desired pixel values for testing */
    printf ("Pixel line 0, sample 0: %d\n", pixel_qa[0]);
    printf ("Pixel line 2747, sample 4207: %d\n", pixel_qa[2747*nsamps+4207]);
    printf ("Pixel line 2723, sample 4347: %d\n", pixel_qa[2723*nsamps+4347]);
    printf ("Pixel line 736, sample 1971: %d\n", pixel_qa[736*nsamps+1971]);
    printf ("Pixel line 808, sample 2468: %d\n", pixel_qa[808*nsamps+2468]);
    printf ("Pixel line 6393, sample 5692: %d\n", pixel_qa[6393*nsamps+5692]);

    /* Test the pixel interrogation */
    printf ("Value 1: ");   /* Fill */
    if (pixel_qa_is_fill (1))
        printf ("Fill\n");
    else
        printf ("Not fill\n");

    printf ("Value 0: ");   /* Not fill */
    if (pixel_qa_is_fill (0))
        printf ("Fill\n");
    else
        printf ("Not fill\n");


    printf ("Value 66: ");   /* Clear (and low cloud confidence) */
    if (pixel_qa_is_clear (66))
        printf ("Clear\n");
    else
        printf ("Not clear\n");

    printf ("Value 224: ");   /* Not clear */
    if (pixel_qa_is_clear (224))
        printf ("Clear\n");
    else
        printf ("Not clear\n");


    printf ("Value 4: ");   /* Water */
    if (pixel_qa_is_water (4))
        printf ("Water\n");
    else
        printf ("Not water\n");

    printf ("Value 130: ");   /* Not water */
    if (pixel_qa_is_water (130))
        printf ("Water\n");
    else
        printf ("Not water\n");


    printf ("Value 136: ");   /* Cloud shadow (and mod cloud conf) */
    if (pixel_qa_is_cloud_shadow (136))
        printf ("Cloud shadow\n");
    else
        printf ("Not cloud shadow\n");

    printf ("Value 66: ");   /* Not cloud shadow */
    if (pixel_qa_is_cloud_shadow (66))
        printf ("cloud shadow\n");
    else
        printf ("Not cloud shadow\n");


    printf ("Value 80: ");   /* Snow (and low cloud conf) */
    if (pixel_qa_is_snow (80))
        printf ("Snow\n");
    else
        printf ("Not snow\n");

    printf ("Value 136: ");   /* Not snow */
    if (pixel_qa_is_snow (136))
        printf ("snow\n");
    else
        printf ("Not snow\n");


    printf ("Value 224: ");   /* Cloud (and high conf cloud) */
    if (pixel_qa_is_cloud (224))
        printf ("Cloud\n");
    else
        printf ("Not cloud\n");

    printf ("Value 136: ");   /* Not cloud */
    if (pixel_qa_is_cloud (136))
        printf ("cloud\n");
    else
        printf ("Not cloud\n");


    printf ("Cloud confidence (12): %d\n", pixel_qa_cloud_confidence (12));
    printf ("Cloud confidence (64): %d\n", pixel_qa_cloud_confidence (64));
    printf ("Cloud confidence (128): %d\n", pixel_qa_cloud_confidence (128));
    printf ("Cloud confidence (128+64): %d\n", pixel_qa_cloud_confidence (128+64));
    printf ("Cloud confidence (224): %d\n", pixel_qa_cloud_confidence (224));

    /* Free the pointers */
    free (xml_infile);
    free (pixel_qa);

    /* Successful completion */
    printf ("Successful read and processing!\n");
    exit (EXIT_SUCCESS);
}
