

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "pixel_qa.h"
#include "read_pixel_qa.h"
#include "pixel_qa_dilation.h"

/*****************************************************************************
METHOD: dilate_pixel_qa

PURPOSE: Dilate the input data with an (N x N + 1) 2D window

3x3 would be: where T = the target
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, T, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
   1, 1, 1, 1, 1, 1, 1
*****************************************************************************/
void dilate_pixel_qa
(
    uint16_t *input_data,  /* I: Data to dilate */
    uint8_t search_bit,    /* I: Bit to dilate */
    int distance,          /* I: Distance to dilate */
    int nrows,             /* I: Number of rows in the data */
    int ncols,             /* I: Number of colums in the data */
    uint16_t *output_data  /* O: Data after dilation */
)
{
    bool found;       /* flag to add the bit to the output mask */
    /* loop indices */
    int row, col;
    int window_row;
    int window_col;
    int start_row;      /* start row */
    int start_col;      /* start column */
    int end_row;        /* end row */
    int end_col;        /* end column */
    /* locations */
    int row_index;
    int window_row_index;  /* window */
    int output_index;
    int input_index;
    uint16_t input_val; /* value for specified bit from input QA band */
    int user_bit_mask;  /* mask based on the search bit */
    int cleaning_bit_mask; /* mask based on bits to clean because otherwise 
                           they would contradict the search bit */

    /* Set the mask to dilate the user-selected bit. */
    user_bit_mask = pow(2, search_bit); 

    /* Initialize all bits in the cleaning mask to 1 */ 
    cleaning_bit_mask = ~(cleaning_bit_mask & 0);

    /* If cloud is being dilated, then turn off clear and cloud shadow.  Leave 
       on snow and water.  This tool is normally only used for cloud dilation,
       so we don't have a policy for other bits.  Therefore, if another bit is 
       being dilated, just do a simple dilation */
    if (search_bit == L2QA_CLOUD)
    {
        cleaning_bit_mask &= ~(1 << L2QA_CLEAR);
        cleaning_bit_mask &= ~(1 << L2QA_CLD_SHADOW);
    }

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
            if (pixel_qa_is_fill(input_data[output_index]))
            {
                output_data[output_index] = input_data[output_index];
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

                    /* Only check specified bit from the input QA */
                    input_val = input_data[input_index] >> search_bit;
                    if (input_val & L2QA_SINGLE_BIT)
                    {
                        found = true;
                        break;
                    }
                }
            }

            if (found)
            {
                /* Dilate by turning the requested bit on */
                output_data[output_index] = input_data[output_index] 
                    | user_bit_mask;

                /* Turn off some bits that contradict the dilation */
                output_data[output_index] &= cleaning_bit_mask;
            }
            else
            {
                output_data[output_index] = input_data[output_index];
            }
        }
    }
}
