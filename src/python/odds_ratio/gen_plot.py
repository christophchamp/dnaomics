#!/usr/bin/env python
# AUTHOR: P. Christoph Champ <champc@uw.edu>
# DATE: 2012-09-25
# LAST UPDATE: 2012-10-05
# TODO: Add a complete description both here and in the usage() def.
# DESCRIPTION: Reads in two files in TAB format ...
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
#randn = np.random.randn

"""
df['X'] = Series(['A','A','A','A','A','B','B','B','B','B'])
df['Y'] = Series(['A','B','A','B','A','B','A','B','A','B'])
bp = df.boxplot(column=['Col1','Col2'], by=['X','Y'])
"""
def plot_boxplot(df, title, xlabel, ylabel, output_png_name):
    """Generates a boxplot plot of the 'df' DataFrame.
    """
    plt.figure()
    bp = df.boxplot()
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.savefig(output_png_name, papertype='letter', format='png')
    plt.close()

def histogram(dat, title, xlabel, ylabel, output_png_name, bin_size):
    """Generates a histogram plot of the 'dat' DataFrame.
    """
    #print "Generating %s plot ..." % output_png_name
    if len(dat[np.isfinite(dat)]) == 0:
        err  = "No finite values in DataFrame. "
        err += "Skipping '%s' plot." % output_png_name
        return err

    ## Allow the user to set the number of bins in the plot
    bins = int(round(len(dat)/bin_size))
    if bins < 10: bins = len(dat)

    plt.figure()
    plt.hist(dat[np.isfinite(dat)], bins=bins)
    plt.title(title + ' (n=%s)'% len(dat))
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.grid()
    plt.savefig(output_png_name, papertype='letter', format='png')
    plt.close()
    return ''

"""
    ## Plot the log2(odds ratio)
    err = histogram(dA.values, 'Histogram', 'Log2(odds ratio)', 
        'Number of GOs', data_filename + '.odds_ratio_histogram.png', 
        bin_size=BIN_SIZE)
    if err != '':
        print err

    data_df['mean_ratio'] = mean_ratio
    err = plot_hist(mean_ratio.values, 'Mean Ratio', 
        '<Abundances_%s>/<Abundances_%s>' % (control_id, disease_id), 
        'Number of GOs', data_filename + '.mean_ratio.png', bin_size=BIN_SIZE)
    if err != '':
        print err

    ## Plot mean ratio using def
    #mean_ratio_vals = np.array([ re[1] for re in results ])
    #err = plot_hist(mean_ratio_vals, 'Mean Ratio#2', 
    #    '<Abund_lean>/<Abund_obese>', 
    #    'Number of GOs', data_filename + '.mean_ratio2.png', bin_size=BIN_SIZE)
    #if err != '':
    #    print err

    ## Generate p-value plot from t-Test
    #pvals = np.array([ re[0] for re in results ])
    err = plot_hist(results.values, 'P-values from t-Test', 'p-value', 
        'Number of GOs', data_filename + '.t-test_pvals.png', 
        bin_size=BIN_SIZE)
    if err != '':
        print err
"""
