import csv_io
dat = csv_io.csv_to_data_frame('chk.tab')
lines = open('chk.tab','r').readlines()
dat = list([ float(l.strip()) for l in lines ])
dat[0]
len(dat)
plt.boxplot(dat)
plt.errorbar(dat)
