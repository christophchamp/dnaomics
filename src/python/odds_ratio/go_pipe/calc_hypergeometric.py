#!/usr/bin/env python
# AUTHOR: P. Christoph Champ <champc@uw.edu>
# DATE: 2012-09-25
# LAST UPDATE: 2012-10-11
# TODO: Add a complete description of this module
# DESCRIPTION: Performers various statistics on given DataFrame(s) and/or 
#   filenames

## CLI modules
import sys
## Function modules
import pandas as pd
import numpy as np
from scipy.stats import hypergeom
## Local modules
import csv_io

def calc_hypergeometric(pathway_file, stats_file, go_list, threshold_go_list,
                        pathway_ids_list):
    """
    Calculates the hypergeometric probability mass function evaluated at k

    N = number of GOs in study
    n = number of GOs in given pathway
    m = number of disease-associated GOs
    k = number of disease-associated GOs in given pathway
    """
    ## Initialize an empty pvalues DataFrame (really just an empty string) to
    ## return when there are errors
    pvalues_df = ''
    err = ''

    ## Convert TAB/CSV files into DataFrames
    pathway_df = csv_io.csv_to_data_frame(pathway_file, delimiter=',')
    stats_df = csv_io.csv_to_data_frame(stats_file)

    ## DEBUG
    #combined_full_df = stats_df.combineAdd(pathway_df.ix[list(go_list)])
    #print combined_full_df.to_string()

    ## Check that user supplied pathway_ids exist in the pathway_df
    s = set(pathway_df.columns.tolist())
    diff = [x for x in pathway_ids_list if x not in s]
    if len(diff) != 0:
        err = "ERROR: %s does not contain pathway_ids: %s" % (
               pathway_file, diff)
        return pvalues_df, err

    ## Number of GOs in study
    N = len(go_list)

    ## Number of disease-associated GOs (this is a sub-set DataFrame of only
    ## those values in the stats_file whose chosen statistic was above the 
    ## threshold)
    m = len(stats_df.ix[threshold_go_list])

    ## Initialize empty dictionary
    hyper_dict = {'N': [], 'n': [], 'm': [], 'k': [], 
                  'p_upper': [], 'p_lower': [], 'pvalue': []}

    ## Loop over pathway ids in the DataFrame
    for pw_id in pathway_ids_list:
        ## Number of disease-associated GOs in pathway pw_id
        #print pathway_df.ix[threshold_go_list][pw_id]
        k = int(pathway_df.ix[threshold_go_list][pw_id].sum())

        ## Number of GOs in pathway pw_id
        n = int(pathway_df.ix[go_list][pw_id].sum())

        ## Now calculate the p-values
        p_upper = float(sum(hypergeom.pmf(range(k,min(m,n)+1), N, m, n)))
        p_lower = float(1 - p_upper + hypergeom.pmf(k, N, m, n))
        pvalue = min(p_upper, p_lower)

        ## Save p-values to dictionary
        hyper_dict['N'].append(N)
        hyper_dict['n'].append(n)
        hyper_dict['m'].append(m)
        hyper_dict['k'].append(k)
        hyper_dict['p_upper'].append(p_upper)
        hyper_dict['p_lower'].append(p_lower)
        hyper_dict['pvalue'].append(pvalue)

        ## DEBUG
        #test = sum(hypergeom.pmf(range(0,k+1), N, m, n))
        #print "[%s] f(%s; %s, %s, %s) = %s vs. %s (%s)" % (
        #    pw_id, k, N, m, n, p_upper, p_lower, test)

    ## Format dictionary as a DataFrame
    pvalues_df = pd.DataFrame(hyper_dict, index=pathway_ids_list)

    ## Save DataFrame to output filename
    #pvalues_df.to_csv('path_pvals.dat', na_rep='NaN', sep='\t')

    return pvalues_df, err
