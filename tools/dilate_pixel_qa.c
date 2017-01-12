/*****************************************************************************
File: dilate_pixel_qa.c

Purpose: Modify in-place a bit from each value in the bit-packed QA band.

Project:  Land Satellites Data System Science Research and Development (LSRD)
          at the USGS EROS

License Type:  NASA Open Source Agreement Version 1.3
*****************************************************************************/


#include <getopt.h>

#include "l2qa_common.h"
#include "pixel_qa.h"
#include "read_pixel_qa.h"
#include "write_pixel_qa.h"
#include "pixel_qa_dilation.h"


#define PROG_NAME "dilate_pixel_qa"


/*****************************************************************************
Method: usage

Description: Prints the usage information for this application.

Return Value: None
*****************************************************************************/
void usage()
{
    printf("%s is a program that dilates the specified bit of the bit-packed"
           " QA band with the specified distance.\n\n", PROG_NAME);

    printf("usage: %s --xml=<xml_filename>"
           " --bit=<bit> --distance=<distance>\n\n", PROG_NAME);

    printf("where the following parameters are required:\n");
    printf("    -xml: name of the input XML metadata file which follows"
           " the ESPA internal raw binary schema\n");
    printf("    -bit: bit value to dilate (0=fill, 1=clear, 2=water, 3=cloud"
           " shadow, 4=snow, 5=cloud, 6=cloud confidence 1, 7=cloud"
           " confidence 2)\n");
    printf("    -distance: search distance from current pixel\n");
    printf("\nExample: %s --xml=LE70230282011250EDC00.xml --bit=5"
           " --distance=3\n", PROG_NAME);
}


/*****************************************************************************
Method: version

Description: Prints the version information and exits the application.

Return Value: None
*****************************************************************************/
void version()
{
    printf("%s: %s\n", PROG_NAME, L2QA_COMMON_VERSION);
    exit(EXIT_SUCCESS);
}


/*****************************************************************************
Method:  get_args

Description:  Gets the command-line arguments and validates that the required
              arguments were specified.

Return Value: Type = int
Value    Description
-----    -----------
ERROR    Error getting the command-line arguments or a command-line
         argument and associated value were not specified
SUCCESS  No errors encountered

NOTES:
  1. Memory is allocated for the input filename.  The caller is responsible
     for freeing the allocated memory upon successful return.
*****************************************************************************/
short get_args
(
    int argc,              /* I: number of cmd-line args */
    char *argv[],          /* I: string of cmd-line args */
    char **xml_infile,     /* O: address of input XML filename */
    uint8_t *bit_value,    /* O: address of bit value variable */
    uint8_t *distance      /* O: address of distance value variable */
)
{
    char FUNC_NAME[] = "get_args";

    int c;              /* current argument index */
    int option_index;   /* index for the command-line option */
    char msg[STR_SIZE]; /* error message */
    static struct option long_options[] =
    {
        {"xml", required_argument, 0, 'i'},
        {"bit", required_argument, 0, 'b'},
        {"distance", required_argument, 0, 'd'},
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    /* Loop through all the cmd-line options */
    opterr = 0;   /* turn off getopt_long error msgs as we'll print our own */
    while (1)
    {
        /* optstring in call to getopt_long is empty since we will only
           support the long options */
        c = getopt_long(argc, argv, "", long_options, &option_index);
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
                usage();
                return ERROR;
                break;

            case 'i':  /* XML infile */
                *xml_infile = strdup(optarg);
                break;

            case 'b':  /* bit value */
                *bit_value = (uint8_t) atoi(optarg);
                break;

            case 'd':  /* distance value */
                *distance = (uint8_t) atoi(optarg);
                break;

            case 'v':  /* version */
                version();
                break;

            case '?':
            default:
                snprintf(msg, sizeof(msg), "Unknown option %s",
                         argv[optind-1]);
                error_handler(true, FUNC_NAME, msg);
                usage();
                return ERROR;
                break;
        }
    }

    /* Make sure the infiles were specified */
    if (*xml_infile == NULL)
    {
        snprintf(msg, sizeof(msg), "--xml is a required argument");
        error_handler(true, FUNC_NAME, msg);
        usage();
        return ERROR;
    }

    if (*bit_value == 255)
    {
        snprintf(msg, sizeof(msg), "--bit is a required argument");
        error_handler(true, FUNC_NAME, msg);
        usage();
        return ERROR;
    }

    if (*distance == 255)
    {
        snprintf(msg, sizeof(msg), "--distance is a required argument");
        error_handler(true, FUNC_NAME, msg);
        usage();
        return ERROR;
    }

    return SUCCESS;
}


/*****************************************************************************
Method:  main

Description:  Reads the bit-packed QA band and updates it with a dilation
              of the specified bit.

RETURN VALUE: Type = int
Value           Description
-----           -----------
ERROR           Error with the dilation generation
SUCCESS         No errors with the dilation generation
*****************************************************************************/
int main(int argc, char *argv[])
{
    char FUNC_NAME[] = "main";

    uint8_t bit_value = 255;   /* bit to dilate */
    uint8_t distance = 255;    /* search distance from the current pixel */
    int nlines = -1;           /* number of lines in the data */
    int nsamps = -1;           /* number of samples in the data */

    char *xml_infile = NULL; /* XML input filename */
    char msg[STR_SIZE];      /* error message */
    char input_qa_filename[PATH_MAX];

    FILE *input_qa_fd = NULL;

    uint16_t *idata = NULL; /* Holds the bit-packed input data */
    uint16_t *ddata = NULL; /* Holds the dilated bit-packed output data */

    /* Read the command line arguments */
    if (get_args(argc, argv, &xml_infile, &bit_value, &distance)
        != SUCCESS)
    {
        return EXIT_FAILURE;
    }

    /* Open the input band */
    input_qa_fd = open_pixel_qa(xml_infile, input_qa_filename, &nlines,
         &nsamps);
                                         
    if (input_qa_fd == NULL)
    {
        snprintf(msg, sizeof(msg), "opening input band data for reading");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }

    printf("%s, %d, %d\n", xml_infile, bit_value, distance);
    printf("%s, %d, %d\n", input_qa_filename, nlines, nsamps);

    /* Allocate memory for the input and output */
    idata = calloc(nlines * nsamps, sizeof(uint16_t));
    if (idata == NULL)
    {
        fclose(input_qa_fd);
        snprintf(msg, sizeof(msg), "allocating memory for input band data");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }

    ddata = calloc(nlines * nsamps, sizeof(uint16_t));
    if (ddata == NULL)
    {
        free(idata);
        fclose(input_qa_fd);
        snprintf(msg, sizeof(msg), "allocating memory for output band data");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }

    /* Read the band into memory here */
    if (read_pixel_qa(input_qa_fd, nlines, nsamps, idata)
        != SUCCESS)
    {
        free(idata);
        free(ddata);
        fclose(input_qa_fd);
        snprintf(msg, sizeof(msg), "reading input band data");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }
    /* Close the READ input band file descriptor */
    fclose(input_qa_fd);
    input_qa_fd = NULL;

    /* Process dilation here */
    dilate_pixel_qa(idata, bit_value, distance, nlines, nsamps, ddata);

    /* Free the input memory */
    free(idata);
    idata = NULL;

    /* Open the WRITE output band file descriptor */
    input_qa_fd = create_pixel_qa(input_qa_filename);
    if (input_qa_fd == NULL)
    {
        free(ddata);
        snprintf(msg, sizeof(msg), "opening output band data for reading");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }

    /* Write the dilated memory out to the file */
    if (write_pixel_qa(input_qa_fd, nlines, nsamps, ddata)
        != SUCCESS)
    {
        free(ddata);
        sprintf (msg, "unable to write the entire bit-packed QA band");
        error_handler (true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }
    /* Close the WRITE output band file descriptor */
    fclose(input_qa_fd);
    input_qa_fd = NULL;

    /* Free the output memory */
    free(ddata);
    ddata = NULL;

    return EXIT_SUCCESS;
}
