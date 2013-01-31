/*
NAME: randomsequence
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates an entirely random sequence with ATGC percentage distributions
    - argv 1  Number of base pairs (sequence length) 
    - argv 2  Number of base pairs per line
    - argv 3/4/5/6  A/T/G/C percentage, N% = 100 - A% - T% - G% - C%, e.g. 22 22 22 22 for true random with 8% N's
    - argv 7  File to store resulting sequence

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char **argv)
{
	FILE *file;
	int i,bp,bpl;
	double A,T,G,C;
	double random;

	if(argc!=8)
		return fprintf(stderr,"usage: %s num_base_pairs base_pairs_per_line A%% C%% G%% T%% outfile\n",argv[0]);

	srand(time(NULL));

	bp=atoi(argv[1]);
	bpl=atoi(argv[2]);

	A=atof(argv[3]);
	C=atof(argv[4])+A;
	G=atof(argv[5])+C;
	T=atof(argv[6])+G;

	file=fopen(argv[7],"w");
	if(!file)
		fprintf(stderr,"Invalid file: '%s'",argv[7]);

	for(i=0;i<bp;i++)
	{
		if(i&&i%bpl==0)
			fprintf(file,"\n");
		random=(100.0*rand())/RAND_MAX;
		if(random<A)
			fprintf(file,"A");
		else if(random<C)
			fprintf(file,"C");
		else if(random<G)
			fprintf(file,"G");
		else if(random<T)
			fprintf(file,"T");
		else
			fprintf(file,"N");
	}
	fprintf(file,"\n");
	fclose(file);
	return 0;
}
