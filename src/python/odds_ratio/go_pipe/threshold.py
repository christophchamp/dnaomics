import numpy as np

## Local modules
import csv_io

def get_threshold_go_list(go_list, stats_file, which_statistic, threshold):
    """
    Returns a list of GOs above a given threshold for a given statistic from
    an input list of GOs and their previously generated statistics.

    Examples
    --------
    Return a list of all input GOs 'GO0001,GO0002' whose 'log2_OR' is above a
    threshold of 2.0.

    Note: Only one statistic can be passed at a time!
    """
    ## Create a DataFrame from the input TAB/CSV stats file
    full_stats_df = csv_io.csv_to_data_frame(stats_file)

    ## Select a subset of entries as specified from the go_list
    stats_df = full_stats_df.ix[go_list]

    ## Return a list of those GOs whose statistics was above the threshold
    return list( stats_df[(stats_df[which_statistic] > threshold) &\
                          (np.isfinite(stats_df[which_statistic]))].index )
