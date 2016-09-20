/*****************************************************************************
File: dilate_class_value.c

Purpose: Modify in-place a class value within the class band.

Project:  Land Satellites Data System Science Research and Development (LSRD)
          at the USGS EROS

License Type:  NASA Open Source Agreement Version 1.3
*****************************************************************************/


#include <getopt.h>

#include "l2qa_common.h"
#include "qa_class_values.h"
#include "read_class_based_qa.h"
#include "write_class_based_qa.h"
#include "class_dilation.h"


#define PROG_NAME "dilate_class_value"


/*****************************************************************************
Method: usage

Description: Prints the usage information for this application.

Return Value: None
*****************************************************************************/
void usage()
{
    printf("%s is a program that opens the Class Based QA"
           " band and dilates the specified class value with the specified"
           " distance.\n\n", PROG_NAME);

    printf("usage: %s --xml=<xml_filename>"
           " --class=<class> --distance=<distance>\n\n", PROG_NAME);

    printf("where the following parameters are required:\n");
    printf("    -xml: name of the input XML metadata file which follows"
           " the ESPA internal raw binary schema\n");
    printf("    -class: class value to dilate\n");
    printf("    -distance: search distance from current pixel\n");
    printf("\nExample: %s --xml=LE70230282011250EDC00.xml --class=4"
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
    int argc,          /* I: number of cmd-line args */
    char *argv[],      /* I: string of cmd-line args */
    char **xml_infile, /* O: address of input XML filename */
    int *class_value,  /* O: address of class value variable*/
    int *distance      /* O: address of distance value variable */
)
{
    char FUNC_NAME[] = "get_args";

    int c;              /* current argument index */
    int option_index;   /* index for the command-line option */
    char msg[STR_SIZE]; /* error message */
    static struct option long_options[] =
    {
        {"xml", required_argument, 0, 'i'},
        {"class", required_argument, 0, 'c'},
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

            case 'c':  /* class value */
                *class_value = atoi(optarg);
                break;

            case 'd':  /* distance value */
                *distance = atoi(optarg);
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

    if (*class_value == 255)
    {
        snprintf(msg, sizeof(msg), "--class is a required argument");
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

Description:  Reads the class based qa band and updates it with a dilation
              of the specified class.

RETURN VALUE: Type = int
Value           Description
-----           -----------
ERROR           Error with the dilation generation
SUCCESS         No errors with the dilation generation
*****************************************************************************/
int main(int argc, char *argv[])
{
    char FUNC_NAME[] = "main";

    int status;                /* status returned from function calls */
    uint8_t class_value = 255; /* class value to dilate */
    uint8_t distance = 255;    /* search distance from the current pixel */
    int nlines = -1;           /* number of lines in the data */
    int nsamps = -1;           /* number of samples in the data */

    char *xml_infile = NULL; /* XML input filename */
    char msg[STR_SIZE];      /* error message */
    char class_based_filename[PATH_MAX];

    FILE *class_based_fd = NULL;

    uint8_t *idata = NULL; /* Holds the class based input data */
    uint8_t *ddata = NULL; /* Holds the dilated class based output data */

    /* Read the command line arguments */
    if (get_args(argc, argv, &xml_infile, &class_value, &distance)
        != SUCCESS)
    {
        return EXIT_FAILURE;
    }

    /* Open the input band */
    class_based_fd = open_class_based_qa(xml_infile, class_based_filename,
                                         &nlines, &nsamps);
    if (class_based_fd == NULL)
    {
        snprintf(msg, sizeof(msg), "opening input band data for reading");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }

    printf("%s, %d, %d\n", xml_infile, class_value, distance);
    printf("%s, %d, %d\n", class_based_filename, nlines, nsamps);

    /* Allocate memory for the input and output */
    idata = calloc(nlines * nsamps, sizeof(uint8_t));
    if (idata == NULL)
    {
        fclose(class_based_fd);
        snprintf(msg, sizeof(msg), "allocating memory for input band data");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }

    ddata = calloc(nlines * nsamps, sizeof(uint8_t));
    if (ddata == NULL)
    {
        free(idata);
        fclose(class_based_fd);
        snprintf(msg, sizeof(msg), "allocating memory for output band data");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }

    /* Read the band into memory here */
    if (read_class_based_qa(class_based_fd, nlines, nsamps, idata)
        != SUCCESS)
    {
        free(idata);
        free(ddata);
        fclose(class_based_fd);
        snprintf(msg, sizeof(msg), "reading input band data");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }
    /* Close the READ input band file descriptor */
    fclose(class_based_fd);
    class_based_fd = NULL;

    /* Process dilation here */
    dilate_class_value(idata, class_value, distance, nlines, nsamps, ddata);

    /* Free the input memory */
    free(idata);
    idata = NULL;

    /* Open the WRITE output band file descriptor */
    class_based_fd = create_class_based_qa(class_based_filename);
    if (class_based_fd == NULL)
    {
        free(ddata);
        snprintf(msg, sizeof(msg), "opening output band data for reading");
        error_handler(true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }

    /* Write the dilated memory out to the file */
    if (write_class_based_qa(class_based_fd, nlines, nsamps, ddata)
        != SUCCESS)
    {
        free(ddata);
        sprintf (msg, "unable to write the entire class-based QA band");
        error_handler (true, FUNC_NAME, msg);
        return EXIT_FAILURE;
    }
    /* Close the WRITE output band file descriptor */
    fclose(class_based_fd);
    class_based_fd = NULL;

    /* Free the output memory */
    free(ddata);
    ddata = NULL;

    return EXIT_SUCCESS;
}
