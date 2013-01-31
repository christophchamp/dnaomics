/*
NAME: findseq
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Searches for DNA sequence in fasta file
    - argv 1  Fasta file in which to search
    - argv 2  sequence to search for, e.g. ATYBNRCT
    - argv 3  repeat, how many consecutive repeats to search for.  1 if not specified.

Source of IUPAC nucleotide ambiguity codes: Cornish-Bowden (1985) Nucl. Acids Res. 13: 3021-3030.
*/

#include <stdio.h>
#include <string.h>

const char A=0x01;
const char T=0x02;
const char G=0x04;
const char C=0x08;

#define M (A|C)
#define R (A|G)
#define W (A|T)
#define S (C|G)
#define Y (C|T)
#define K (G|T)
#define V (A|C|G)
#define H (A|C|T)
#define D (A|G|T)
#define B (C|G|T)
#define X (A|T|G|C)
#define N (A|T|G|C)

char Encode(char bp)
{
	bp=toupper(bp);
	switch(bp)
	{
		case 'A':	return A;
		case 'T':	return T;
		case 'G':	return G;
		case 'C':	return C;
		case 'M':	return M;
		case 'R':	return R;
		case 'W':	return W;
		case 'S':	return S;
		case 'Y':	return Y;
		case 'K':	return K;
		case 'V':	return V;
		case 'H':	return H;
		case 'D':	return D;
		case 'B':	return B;
		case 'X':	return X;
		case 'N':	return N;
	}
	return 0;
}

char Decode(char bp)
{
	if(bp==A)	return 'A';
	if(bp==T)	return 'T';
	if(bp==G)	return 'G';
	if(bp==C)	return 'C';
	if(bp==M)	return 'M';
	if(bp==R)	return 'R';
	if(bp==W)	return 'W';
	if(bp==S)	return 'S';
	if(bp==Y)	return 'Y';
	if(bp==K)	return 'K';
	if(bp==V)	return 'V';
	if(bp==H)	return 'H';
	if(bp==D)	return 'D';
	if(bp==B)	return 'B';
	if(bp==N)	return 'N';
	
	return 0;
}

double ProbBP(char bp)
{
	return ((bp&A?1.0:0.0)+(bp&T?1.0:0.0)+(bp&G?1.0:0.0)+(bp&C?1.0:0.0))/4.0;
}

int main(int argc,char *argv[])
{
	FILE *file;
	char filename[100],line[1000],*sequence,*fasta,*subsequence;
	int i,j,k,seqlength,fastalength,repeat,matchcount;

	if(argc!=3&&argc!=4)
		return fprintf(stderr,"usage: %s fasta sequence [repeat]\n",argv[0]); 

	seqlength=strlen(argv[2]);
	sequence=(char*)malloc(seqlength+1);
	sprintf(sequence,tolower(argv[2]));

	for(i=0;i<seqlength;i++)
		sequence[i]=Encode(sequence[i]);

	if(argc==4)
		repeat=atoi(argv[3]);
	else
		repeat=1;

	subsequence=(char*)malloc(seqlength+1);

	if(!(file=fopen(argv[1],"r")))
		return fprintf(stderr,"Invalid file: '%s'\n",argv[1]);

	for(i=0,fastalength=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		fastalength+=strlen(line);

	fasta=(char*)malloc(fastalength+1);
	rewind(file);

	for(i=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		for(j=0;j<strlen(line);j++)
			fasta[i++]=tolower(line[j]);

	fasta[fastalength]=0;
	fclose(file);

	for(i=0;i<fastalength;i++)
		fasta[i]=Encode(fasta[i]);

	for(i=0,matchcount=0;i<fastalength-strlen(sequence)+1;i++)
	{
		for(j=0;j<repeat;j++)
			for(k=0;k<strlen(sequence);k++)
				if((~sequence[k])&fasta[i+(j*seqlength)+k])
					goto MatchFail;

		matchcount++;
		strncpy(subsequence,fasta+i,repeat*seqlength);
		subsequence[repeat*seqlength]=0;
		i+=repeat*seqlength-1;

		MatchFail:
	}

	double seqprob=1;

	for(i=0;i<repeat;i++)
		for(j=0;j<seqlength;j++)
			seqprob*=ProbBP(sequence[j]);

	double expected=seqprob*fastalength;

	for(i=0;i<seqlength;i++)
		sequence[i]=Decode(sequence[i]);

	printf("\n\n\tSequence :\t\td(%s)%d\n",sequence,repeat);
	printf("\tSequence Probability :\t%e\n",seqprob);
	printf("\tExpected :\t\t%f\n",expected);
	printf("\tFound :\t\t\t%d\n",matchcount);
	double significance=matchcount>expected?(100*(matchcount/expected)-100):(100*(expected/matchcount)-100);
	printf("\tSignificance :\t\t%f%%\n\n",significance*(matchcount>expected?1:-1));

	return 0;
}
