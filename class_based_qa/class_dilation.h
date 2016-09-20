/*****************************************************************************
FILE: class_dilation.h

PURPOSE: Contains functions for performing dilation.

PROJECT:  Land Satellites Data System Science Research and Development (LSRD)
at the USGS EROS

LICENSE TYPE:  NASA Open Source Agreement Version 1.3
*****************************************************************************/


void dilate_class_value
(
    uint8_t *input_data,   /* I: Data to dilate */
    uint8_t search_value,  /* I: Value to dilate */
    int distance,          /* I: Distance to dilate */
    int nrows,             /* I: Number of rows in the data */
    int ncols,             /* I: Number of colums in the data */
    uint8_t *output_data   /* O: Data after dilation */
);
