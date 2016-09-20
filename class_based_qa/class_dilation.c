

#include <stdbool.h>
#include <stdint.h>

#include "qa_class_values.h"
#include "class_dilation.h"


/*****************************************************************************
METHOD: dilate_class_value

PURPOSE: Dilate the input data with an (N x N + 1) 2D window

3x3 would be: where t = the target
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, T, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
*****************************************************************************/
void dilate_class_value
(
    uint8_t *input_data,   /* I: Data to dilate */
    uint8_t search_value,  /* I: Value to dilate */
    int distance,          /* I: Distance to dilate */
    int nrows,             /* I: Number of rows in the data */
    int ncols,             /* I: Number of colums in the data */
    uint8_t *output_data   /* O: Data after dilation */
)
{
    bool found;       /* flag to add the bit to the output mask */
    /* loop indices */
    int row, col;
    int window_row;
    int window_col;
    int start_row;      /* start */
    int start_col;      /* start */
    int end_row;        /* end */
    int end_col;        /* end */
    /* locations */
    int row_index;
    int window_row_index;  /* window */
    int output_index;
    int input_index;

#ifdef _OPENMP
    #pragma omp parallel for private(start_row, end_row, row_index, col, output_index, start_col, end_col, found, window_row, window_row_index, window_col, input_index)
#endif
    for (row = 0; row < nrows; row++)
    {
        start_row = row - distance;
        end_row = row + distance;

        row_index = row * ncols;

        for (col = 0; col < ncols; col++)
        {
            output_index = row_index + col;

            /* Skip processing input that is a fill pixel */
            if (input_data[output_index] == L2QA_FILL)
            {
                output_data[output_index] = L2QA_FILL;
                continue;
            }

            /* Quick check the current pixel */
            if (input_data[output_index] == search_value)
            {
                output_data[output_index] = search_value;
                continue;
            }

            start_col = col - distance;
            end_col = col + distance;

            found = false;
            /* For each row in the window */
            for (window_row = start_row;
                 window_row < end_row + 1 && !found;
                 window_row++)
            {
                /* Skip out of bounds locations */
                if (window_row < 0 || window_row > (nrows - 1))
                    continue;

                window_row_index = window_row * ncols;

                /* For each column in the window */
                for (window_col = start_col;
                     window_col < end_col + 1;
                     window_col++)
                {
                    /* Skip out of bounds locations */
                    if (window_col < 0 || window_col > (ncols - 1))
                        continue;

                    input_index = window_row_index + window_col;

                    if (input_data[input_index] == search_value)
                    {
                        found = true;
                        break;
                    }
                }
            }

            if (found)
                output_data[output_index] = search_value;
            else
                output_data[output_index] = input_data[output_index];
        }
    }
}
