/*
NAME: randomsequencefasta
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates sequence with ATGC percentage distribution derived from bins of the input sequence
    - argv 1  Fasta file to read from
    - argv 2  Bin size to derive percentage distributions from
    - argv 3  Number of base pairs per line
    - argv 4  File to store resulting sequence
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char **argv)
{
	unsigned i,A=0,T=0,G=0,C=0,binsize,bp,bpl,r=0,w=0;
	FILE *infile,*outfile;

	if(argc!=5)
			return fprintf(stderr,"usage: %s infile bin_size base_pairs_per_line outfile\n",argv[0]);

	srand(time(NULL));

	infile=fopen(argv[1],"r");
	outfile=fopen(argv[4],"w");
	if(!infile)
		return fprintf(stderr,"Could not open file :  '%s'\n",argv[1]);
	if(!outfile)
		return fprintf(stderr,"Could not write to file :  '%s'\n",argv[4]);

	binsize=atoi(argv[2]);
	bpl=atoi(argv[3]);
	while(!feof(infile))
	{
		while(bp=fgetc(infile),bp=='\n');
		switch(toupper(bp))
		{
			case 'A':	A++;
			case 'T':	T++;
			case 'G':	G++;
			case 'C':	C++;
		}	// No break statements, this is intentional
		r++;
		if(r>=binsize)
		{
			unsigned random;
			for(i=0;i<binsize;i++)
			{
				random=rand()%r;
				if(random<A)
					fputc('A',outfile);
				else if(random<T)
					fputc('T',outfile);
				else if(random<G)
					fputc('G',outfile);
				else if(random<C)
					fputc('C',outfile);
				else
					fputc('N',outfile);
				w++;
				if(w%bpl==0)
					fputc('\n',outfile);
			}
			A=T=G=C=0;
			r=0;
		}
	}
	unsigned random;
	for(i=0;i<r-1;i++)
	{
		random=rand()%r;
		if(random<A)
			fputc('A',outfile);
		else if(random<T)
			fputc('T',outfile);
		else if(random<G)
			fputc('G',outfile);
		else if(random<C)
			fputc('C',outfile);
		else
			fputc('N',outfile);
		w++;
		if(w%bpl==0)
			fputc('\n',outfile);
	}
	fputc('\n',outfile);
	return 0;
}
