/*
NAME: fastarevcomp
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates the reverse compliment of a fasta file
    - argv 1  infile, Fasta file to read data from
    - argv 2  outfile, Fasta file in which to store result
*/

#include <stdio.h>

char Compliment(char bp)
{
	switch(bp)
	{
		case 'A':	return 'T';
		case 'T':	return 'A';
		case 'G':	return 'C';
		case 'C':	return 'G';
		default:	return bp;
	}
}

char    *LoadFastaFile(const char *filename,const unsigned npad);

int main(int argc,char **argv)
{
	char *sequence,line[1024];
	int i,seqlength;
	FILE *outfile;

	if(argc!=3)
		return fprintf(stderr,"usage: %s infile outfile\n",argv[0]);

	if(!(sequence=LoadFastaFile(argv[1],0)))
		return fprintf(stderr,"Could not open infile: '%s'\n",argv[1]);
	seqlength=strlen(sequence);

	if(!(outfile=fopen(argv[2],"w")))
		return fprintf(stderr,"Could not open outfile: '%s'\n",argv[2]);

	for(i=seqlength-1;i>=0;i--)
	{
		fprintf(outfile,"%c",Compliment(sequence[i]));
		if((seqlength-i)%60==0)
			fprintf(outfile,"%c",'\n');
	}
	fprintf(outfile,"%c",'\n');

	fclose(outfile);
}

char *LoadFastaFile(const char *filename,const unsigned npad)
{
	char *sequence;
	unsigned i,j,seqlength;
	FILE *file=fopen(filename,"r");
	if(!file)
		return fprintf(stderr,"Could not open file :  '%s'\n",filename);

	char line[1024];
	for(i=0,seqlength=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		seqlength+=strlen(line);

	sequence=(char*)malloc(seqlength+npad+1);
	rewind(file);

	for(i=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		for(j=0;j<strlen(line);j++)
			sequence[i++]=toupper(line[j]);
	sequence[i]=0;
	for(i=0;i<npad;i++)
		strcat(sequence,"N");

	fclose(file);
	return sequence;
}

