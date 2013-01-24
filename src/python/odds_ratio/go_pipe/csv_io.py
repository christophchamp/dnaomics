#!/usr/bin/env python
# AUTHOR: P. Christoph Champ <champc@uw.edu>
# DATE: 2012-10-02
# LAST UPDATE: 2012-10-03
# DESCRIPTION: Reads/writes file in CSV/TAB format and returns a DataFrame
#import pandas as pd
from pandas import read_csv, set_printoptions
import project_info

__version__ = project_info.VERSION

NAME = project_info.NAME
URL = project_info.URL

def csv_to_data_frame(infile, delimiter='\t'):
    """Returns a DataFrame (a structured named data matrix) from a given 
    CSV/TAB file.

    Parameters
    ----------
    infile : input filepath
        A standard CSV/TAB file
    delimiter : field separator, default '\\t'

    Returns
    -------
    data_frame : DataFrame of CSV/TAB file contents.
    """
    # Read in TAB file as a DataFrame (a structured named data matrix)
    data_frame = read_csv(infile, index_col=0, delimiter=delimiter)
    return data_frame

def data_frame_to_csv(data_frame, outfile, delimiter='\t'):
    """
    Write DataFrame to a comma-separated values (csv) file

    Parameters
    ----------
    data_frame : input DataFrame to be written out
    outfile : string of output filepath
        File path
    delimiter : field separator, default '\\t'
    """
    #set_printoptions(precision=6)
    data_frame.to_csv(outfile, sep=delimiter, na_rep='NaN')
