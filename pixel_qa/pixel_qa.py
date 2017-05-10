#! /usr/bin/env python
import sys
import os

# Define the Pixel QA bit values
PQA_FILL = 0                      # 1
PQA_CLEAR = 1                     # 2
PQA_WATER = 2                     # 4
PQA_CLD_SHADOW = 3                # 8
PQA_SNOW = 4                      # 16
PQA_CLOUD = 5                     # 32
PQA_CLOUD_CONF1 = 6               # 64
PQA_CLOUD_CONF2 = 7               # 128
PQA_CIRRUS_CONF1 = 8              # 256
PQA_CIRRUS_CONF2 = 9              # 512
PQA_TERRAIN_OCCL = 10             # 1024

PQA_SINGLE_BIT = 0x01             # 00000001
PQA_DOUBLE_BIT = 0x03             # 00000011

PQA_LOW_CONF = 1                  # low confidence (01)
PQA_MODERATE_CONF = 2             # moderate confidence (10)
PQA_HIGH_CONF = 3                 # high confidence (11)


class PixelQA():
    '''Provides methods for interrogating the Level-2 pixel QA values
    '''

    def __init__(self):
        pass


    '''Determines if the current Level-2 pixel QA pixel is fill
    
    Returns:
        True if pixel is fill
        False otherwise
    '''
    @staticmethod
    def pixel_qa_is_fill(l2_qa_pix):
        if (((l2_qa_pix >> PQA_FILL) & PQA_SINGLE_BIT) == 1):
            return True
        else:
            return False


    '''Determines if the current Level-2 pixel QA pixel is clear
    
    Returns:
        True if pixel is clear
        False otherwise
    '''
    @staticmethod
    def pixel_qa_is_clear(l2_qa_pix):
        if (((l2_qa_pix >> PQA_CLEAR) & PQA_SINGLE_BIT) == 1):
            return True
        else:
            return False


    '''Determines if the current Level-2 pixel QA pixel is water
    
    Returns:
        True if pixel is water
        False otherwise
    '''
    @staticmethod
    def pixel_qa_is_water(l2_qa_pix):
        if (((l2_qa_pix >> PQA_WATER) & PQA_SINGLE_BIT) == 1):
            return True
        else:
            return False


    '''Determines if the current Level-2 pixel QA pixel is cloud shadow
    
    Returns:
        True if pixel is cloud shadow
        False otherwise
    '''
    @staticmethod
    def pixel_qa_is_cloud_shadow(l2_qa_pix):
        if (((l2_qa_pix >> PQA_CLD_SHADOW) & PQA_SINGLE_BIT) == 1):
            return True
        else:
            return False


    '''Determines if the current Level-2 pixel QA pixel is snow
    
    Returns:
        True if pixel is snow
        False otherwise
    '''
    @staticmethod
    def pixel_qa_is_snow(l2_qa_pix):
        if (((l2_qa_pix >> PQA_SNOW) & PQA_SINGLE_BIT) == 1):
            return True
        else:
            return False


    '''Determines if the current Level-2 pixel QA pixel is cloud
    
    Returns:
        True if pixel is cloud
        False otherwise
    '''
    @staticmethod
    def pixel_qa_is_cloud(l2_qa_pix):
        if (((l2_qa_pix >> PQA_CLOUD) & PQA_SINGLE_BIT) == 1):
            return True
        else:
            return False


    '''Returns the cloud confidence value (0-3) for the current Level-2
       pixel QA pixel value.
    
    Returns:
        0 if cloud confidence bits are 00
        1 if cloud confidence bits are 01
        2 if cloud confidence bits are 10
        3 if cloud confidence bits are 11
    '''
    @staticmethod
    def pixel_qa_cloud_confidence(l2_qa_pix):
        return ((l2_qa_pix >> PQA_CLOUD_CONF1) & PQA_DOUBLE_BIT);


    '''Returns the cirrus confidence value (0-3) for the current Level-2
       pixel QA pixel value. These are valid for L8 only.
    
    Returns:
        0 if cirrus confidence bits are 00
        1 if cirrus confidence bits are 01
        2 if cirrus confidence bits are 10
        3 if cirrus confidence bits are 11
    '''
    @staticmethod
    def pixel_qa_cirrus_confidence(l2_qa_pix):
        return ((l2_qa_pix >> PQA_CIRRUS_CONF1) & PQA_DOUBLE_BIT);


    '''Determines if the current Level-2 pixel QA pixel is terrain occluded
    
    Returns:
        True if pixel is terrain occluded
        False otherwise
    '''
    @staticmethod
    def pixel_qa_is_terrain_occluded(l2_qa_pix):
        if (((l2_qa_pix >> PQA_TERRAIN_OCCL) & PQA_SINGLE_BIT) == 1):
            return True
        else:
            return False

# ##### end of PixelQA class #####
