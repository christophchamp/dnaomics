#!/usr/bin/env python
# AUTHOR: P. Christoph Champ <champc@uw.edu>
# DATE: 2012-09-25
# LAST UPDATE: 2012-10-17
# DESCRIPTION: Performers various statistics on given DataFrame(s) and/or 
#   filenames
# TODO:
#   - Add a complete description of this module
#   - Figure out how to round the values returned for each statistic
import pandas as pd

def calc_single_ttest(control_df, disease_df):
    from scipy.stats import ttest_ind
    zstat, ttest = ttest_ind(control_df.values.flatten(), 
                         disease_df.values.flatten())
    return ttest

def calc_ttest(series, control, disease):
    """
    Calculates the T-test for the means of TWO INDEPENDENT samples of scores.
    
    This is a two-sided test for the null hypothesis that 2 independent samples
    have identical average (expected) values.

    Examples
    --------
    
    >>> from scipy import stats
    >>> import numpy as np
    
    >>> #fix seed to get the same result
    >>> np.random.seed(12345678)
    
    test with sample with identical means
    
    >>> rvs1 = stats.norm.rvs(loc=5,scale=10,size=500)
    >>> rvs2 = stats.norm.rvs(loc=5,scale=10,size=500)
    >>> stats.ttest_ind(rvs1,rvs2)
    (0.26833823296239279, 0.78849443369564765)
    """
    from scipy.stats import ttest_ind
    control_group = series[control]
    disease_group = series[disease]

    ## Only keep the two-tailed p-value and label it as 'ttest'
    zstat, pvalue = ttest_ind(control_group, disease_group)

    return pd.Series({'ttest': pvalue})

def calc_wilcoxon(series, control, disease):
    """
    The Wilcoxon signed-rank test tests the null hypothesis that two related
    paired samples come from the same distribution. In particular, it tests
    whether the distribution of the differences x - y is symmetric about zero.
    It is a non-parametric version of the paired T-test.
    """
    from scipy.stats import wilcoxon

    control_group = series[control]
    disease_group = series[disease]

    ## Only keep the two-sided p-value and label it as 'wilcoxon'
    zstat, pvalue = wilcoxon(control_group, disease_group)

    return pd.Series({'wilcoxon': pvalue})

def calc_ranksums(series, control, disease):
    """
    The Wilcoxon rank-sum test tests the null hypothesis that two sets
    of measurements are drawn from the same distribution.  The alternative
    hypothesis is that values in one sample are more likely to be
    larger than the values in the other sample.
    
    This test should be used to compare two samples from continuous
    distributions.  It does not handle ties between measurements
    in x and y.  For tie-handling and an optional continuity correction
    see `stats.mannwhitneyu`_
    """
    from scipy.stats import ranksums

    control_group = series[control]
    disease_group = series[disease]

    ## Only keep the two-sided p-value of the test and label it as 'ranksums'
    zstat, pvalue = ranksums(control_group, disease_group)

    return pd.Series({'ranksums': pvalue})

def calc_mean_ratio(series, control, disease):
    """
    Calculates the mean ratio of the control vs. disease abundances
    """
    control_group = series[control]
    disease_group = series[disease]

    ## FIXME: This works, but only with the correct DataFrame
    mean_ratio = control_group.mean() / disease_group.mean()

    return pd.Series({'mean_ratio': mean_ratio})

def calc_odds_ratio(control_df, disease_df):
    """
    Calculates the odds ratio of the control vs. disease abundances
    """
    ## Calculate the sum ratios (sum of GOs in class / sum of GOs not in class)
    sum_control_ratio = control_df.sum(1) / \
        (control_df.sum(0).sum() - control_df.sum(1))
    sum_disease_ratio = disease_df.sum(1) / \
        (disease_df.sum(0).sum() - disease_df.sum(1))

    return sum_disease_ratio / sum_control_ratio
