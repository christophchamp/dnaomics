/*
NAME: nstats
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates N statistics for a Fasta file
    - argv 1  Fasta file to read from
*/

#include <stdio.h>
#include <string.h>

int main(int argc,char *argv[])
{
	FILE *file;
	char line[1020];
	int i,bp=0,n=0;
	
	if(argc!=2)
		return fprintf(stderr,"usage: %s FastaFile\n",argv[0]);

	if(!(file=fopen(argv[1],"r")))
		return fprintf(stderr,"Invalid file: '%s'\n",argv[1]);

	for(fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
	{
		for(i=0;i<strlen(line);i++)
		{
			bp++;
			if(line[i]=='N'&&!n)
				n=bp;
			if(line[i]!='N'&&n)
			{
				printf("%10d %10d %10d\n",n,bp,bp-n);
				n=0;
			}
		}
	}

	printf("%10d %10d %10d\n",n,bp,bp-n);

	fclose(file);
	return 0;
}
