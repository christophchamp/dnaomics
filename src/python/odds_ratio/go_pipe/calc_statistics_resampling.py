#!/usr/bin/env python
# AUTHOR: P. Christoph Champ <champc@uw.edu>
# DATE: 2012-09-25
# LAST UPDATE: 2012-10-08
# TODO: Add a complete description of this module
# DESCRIPTION: Reads in two files in TAB format ...
import numpy as np
import random
from pandas import DataFrame, Series
from collections import Counter

## Local modules
import csv_io
import statistics
from utils import min_gt

def vet_data(data_frame):
    """Performs various checks on the input files returning any errors found.
    """
    ## TODO: Add a _lot_ more sanity/vetting checks!
    err = ''
    ## Make sure there are only 2 classes in the metafile.
    num_uniq_classes = len(
        list(set([ c[0] for c in data_frame.values.tolist() ])))
    if num_uniq_classes != 2:
        err  = "ERROR: There are %s unique classes in the metafile. " % (
            num_uniq_classes)
        err += "Expecting 2 unique classes."
    return err

def calc_statistics_resampling(abund_file, meta_file, stats_file,
                               control_id, which_statistics):
    """Runs the main analysis on the two input files.
    """
    ## Initialize go_list as empty, which will stay empty if there are any
    ## errors with the input files and/or the statistics
    go_list = ''

    ## Read data files into DataFrames.
    raw_abund_df = csv_io.csv_to_data_frame(abund_file)
    meta_df = csv_io.csv_to_data_frame(meta_file)

    ## Normalize data values (i.e., each row sums to 1.0)
    ## Note: For rows that sum to 0.0, force the values for each cell to be
    ## 0.0 instead of NaN
    #abund_df = DataFrame(np.nan_to_num(
    #                     [ tmp_abund_df.ix[i] / tmp_abund_df.ix[i].sum() 
    #                       for i in tmp_abund_df.index ]),
    #                     columns=tmp_abund_df.columns,
    #                     index=tmp_abund_df.index)
    #abund_df = abund_df.apply(lambda x: x + abund_df.min(1))
    min_vals = list(raw_abund_df.apply(
                   lambda x: min_gt(x, 0), axis=0).values.tolist())
    #abund_df = abund_df.add(min_vals, axis=1)
    raw_abund_df += min_vals
    sum_columns_list = raw_abund_df.sum(0).values.tolist()
    #abund_df = abund_df.apply(lambda x: x / abund_df.sum(1))
    abund_df = raw_abund_df / sum_columns_list

    ## Run various checks on the input files. Exit printing errors.
    err = vet_data(meta_df)
    if err != '':
        return go_list, err

    ## Create a list of "row names" from the various DataFrames to use as
    ## indices or column names
    index_ids = list([i for i in abund_df.index])
    sample_labels = list([i for i in meta_df.index])
    class_ids = list(set([ c[0] for c in meta_df.values.tolist() ]))

    ## Check to see if the user supplied control_id exists in the metafile
    control_id = str(control_id)
    if control_id not in class_ids:
        err  = "control_id = '%s' not found in metafile " % control_id
        err += "from available class_ids: %s" % (','.join(class_ids))
        return go_list, err

    ## Assign the disease_id to the opposite of the control_id (there should
    ## only be 2 unique class ids listed in the metafile!)
    disease_id = [ c for c in class_ids if not c == control_id ][0]
    ## Get the number of samples from each class_id
    sample_set = Counter(meta_df[meta_df.columns[0]].values)
    control_size = sample_set[control_id]
    disease_size = sample_set[[ i for i in sample_set if not i == control_id ][0]]
    num_sample_sizes = int(control_size + disease_size)

    ## Create a dicionary containing the class_ids as the keys and the 
    ## sample_labels as the values.
    ## E.g., {'Control': ['S1', 'S2'], 'Disease': ['S3', 'S4']}
    sample_dict = {}
    for c in class_ids:
        sample_dict[c] = [ s for s in sample_labels if meta_df.ix[s] == c ]
    print sample_dict[disease_id]

    sample_size = 5
    #random_labels = random.sample(sample_labels, sample_size)
    #random_df = DataFrame(abund_df.values.tolist(), index=abund_df.index, 
    #                      columns=random_labels)
    #print random_df[random_labels[0:control_size]]

    for iter_sample_size in range(40, 110, 10):
        sample_size = int(np.round(num_sample_sizes * (iter_sample_size / 100.0)))
        #print iter_sample_size, sample_size
        for rand in range(10):
            ## Get a random selection of n columns, where n is sample_size
            rand_df = abund_df[random.sample(abund_df.columns, sample_size)]

            ## Force there to always be at least 2 samples from the Control set and
            ## 2 samples from the Disease set
            while (len([ c for c in rand_df.columns.tolist() if\
                         c in sample_dict[control_id] ]) < 2) or\
                  (len([ c for c in rand_df.columns.tolist() if\
                         c in sample_dict[disease_id] ]) < 2):
                rand_df = abund_df[random.sample(abund_df.columns, sample_size)]

            control_df = rand_df[[ s for s in sample_dict[control_id] 
                                   if s in rand_df.columns]]
            disease_df = rand_df[[ s for s in sample_dict[disease_id] 
                                   if s in rand_df.columns]]
    
            odds_ratio = statistics.calc_odds_ratio(control_df, disease_df)
            print sample_size, rand, odds_ratio.ix['GO1']
    return '', ''

    ## Create a dicionary containing the class_ids as the keys and the 
    ## sample_labels as the values.
    ## E.g., {'Control': ['S1', 'S2'], 'Disease': ['S3', 'S4']}
    #sample_dict = {}
    #for c in class_ids:
    #    sample_dict[c] = [ s for s in sample_labels if meta_df.ix[s] == c ]

    ## Create separate DataFrame for the "control" and "disease" samples
    #control_df = abund_df[[ s for s in sample_dict[control_id] ]]
    #disease_df = abund_df[[ s for s in sample_dict[disease_id] ]]

    ## Calculate the odds ratio, log2(odds ratio), and update the DataFrame
    #odds_ratio = statistics.calc_odds_ratio(control_df, disease_df)
    odds_ratio = statistics.calc_odds_ratio(
                     random_df[random_labels[0:control_size]],
                     random_df[random_labels[control_size:disease_size]])
    log2_OR = np.log2(odds_ratio)
    ## FIXME: Figure out how to round values in a Series
    #odds_ratio = Series([ round(i, 6) for i in odds_ratio.tolist() ])
    #log2_OR = Series([ round(i, 6) for i in log2_OR.tolist() ])
    abund_df['odds_ratio'] = odds_ratio
    abund_df['log2_OR'] = log2_OR
    #print "OR>=1.0: ",sum(log2_OR.dropna() >= 1.0) ## DEBUG

    ## Perform whichever tests/statistics are defined in 'which_statistics'
    ## (e.g., t-Test, Wilcoxon, mean ratios, etc.) and update the DataFrame
    ## FIXME: This is a _REALLY_ bad way to do this!
    if 'ttest' in which_statistics:
        results = abund_df.apply(statistics.calc_ttest,
                                control=list(control_df.columns), 
                                disease=list(disease_df.columns), 
                                axis=1)
        abund_df['ttest'] = results['ttest']

    if 'wilcoxon' in which_statistics:
        results = abund_df.apply(statistics.calc_wilcoxon,
                                control=list(control_df.columns), 
                                disease=list(disease_df.columns), 
                                axis=1)
        abund_df['wilcoxon'] = results['wilcoxon']

    if 'ranksums' in which_statistics:
        results = abund_df.apply(statistics.calc_ranksums,
                                control=list(control_df.columns), 
                                disease=list(disease_df.columns), 
                                axis=1)
        abund_df['ranksums'] = results['ranksums']

    if 'mean_ratio' in which_statistics:
        results = abund_df.apply(statistics.calc_mean_ratio,
                                control=list(control_df.columns), 
                                disease=list(disease_df.columns), 
                                axis=1)
        abund_df['mean_ratio'] = results['mean_ratio']

    ## DataFrame containing all of the statistics run on the abund_filename
    stats_df = abund_df[which_statistics]

    ## Save statistics DataFrame to file
    csv_io.data_frame_to_csv(stats_df, stats_file)

    ## Save list of GOs to file (one GO per line)
    #f = open(out_go_list_filename, 'w')
    #f.write('\n'.join(index_ids))
    #f.close()

    ## Return go_list and, hopefully, an empty err string
    return list(index_ids), err
