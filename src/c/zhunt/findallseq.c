
/*
	Searches for all possible (size constrained) DNA sequences in fasta file
	argv 1		fasta file in which to search
	argv 2		maxbp, maximum sequence length
	argv 3		maxrep, maximum consecutive repeats
	argv 4		cutoff, minimum significance defined as over/under (+/-) representation ratio
	argv 5		min_found, minimum pattern matches for sequence

*
* Source of IUPAC nucleotide ambiguity codes: Cornish-Bowden (1985) Nucl. Acids Res. 13: 3021-3030.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MySql_Defs.h"
#include <math.h>

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
	MYSQL mysql;
	FILE *file;
	char filename[100],line[1000],query[4000],table[100],*sequence,*fasta,*subsequence;
	unsigned LastPos,Pos[10000];
	int i,j,k,cutoff,minfound,seqlength,maxseqlength,fastalength,offset,repeat,maxrepeat,matchcount;
	double seqprob,expected,significance,maxsig,avg,std;

	if(argc!=6)
		return fprintf(stderr,"usage: %s fasta maxbp maxrep cutoff min_found\n",argv[0]);

	maxseqlength=atoi(argv[2]);
	sequence=(char*)malloc(maxseqlength+1);

	for(i=0;i<maxseqlength;i++)
		sequence[i]=0;

	maxrepeat=atoi(argv[3]);
	cutoff=atoi(argv[4]);
	minfound=atoi(argv[5]);

	subsequence=(char*)malloc(seqlength+1);

	if(!(file=fopen(argv[1],"r")))
		fprintf(stderr,"Invalid file: '%s'\n",argv[1]);

	for(i=0,fastalength=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		fastalength+=strlen(line);

	fasta=(char*)malloc(fastalength+1);
	rewind(file);

	for(i=offset=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		for(j=0;j<strlen(line);j++)
			offset+=(fasta[i++]=toupper(line[j]))=='N'?1:0;

	fasta[fastalength]=0;
	fclose(file);

	for(i=0;i<fastalength;i++)
		fasta[i]=Encode(fasta[i]);

	for(i=0;i<maxseqlength;i++)
		sequence[i]=0;

	mysql_apps_logon(&mysql);

	sprintf(query,"%s",argv[1]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '-':
			case '.':	query[i]='_';
					break;
			case '/':	strncpy(query,query+i+1,100);
					i=0;
					break;
		}
	sprintf(table,"%s.%s_%s","FindSeq",query,"FindAllSeq");

	sprintf(query,"drop table if exists %s",table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	sprintf(query,"create table %s (sequence char(%d),repeat int,expected double,found int,significance double,PosAvg double,PosStd double)",table,maxseqlength+1);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");

	for(seqlength=1;seqlength<=maxseqlength;seqlength++)
	{
		while(1)
		{
			for(i=0;i<seqlength;i++)
				subsequence[i]=Decode(sequence[i]);
			subsequence[i]=0;

			for(i=0;i<maxseqlength;i++)
				if(sequence[i]<N)
				{
					sequence[i]++;
					break;
				}
				else
					sequence[i]=1;

			if(i==seqlength)
				break;

			if(sequence[0]==N||sequence[seqlength-1]==N)
				goto NextSequence;

			for(i=0;i<seqlength;i++)
				subsequence[i]=Decode(sequence[i]);
			subsequence[seqlength]=0;

//			fprintf(stdout,"Testing d[%s]\r",subsequence);

			for(i=1;i<seqlength;i++)
				if(seqlength%i==0)
				{
					for(j=1;j<seqlength/i;j++)
						if(memcmp(sequence,sequence+i*j,i))
							break;
					if(j==seqlength/i)
						goto NextSequence;
				}

			maxsig=0;
			matchcount=minfound;
			for(repeat=1;repeat<=maxrepeat&&matchcount>=minfound;repeat++)
			{
				for(i=0,matchcount=0,LastPos=0;i<fastalength-seqlength;i++)
				{
					if(fasta[i]==N)
						LastPos=i;
					for(j=0;j<repeat;j++)
						for(k=0;k<seqlength;k++)
							if((~sequence[k])&fasta[i+(j*seqlength)+k])
								goto MatchFail;

					Pos[matchcount<10000?matchcount:9999]=i-LastPos;
					matchcount++;
					strncpy(subsequence,fasta+i,repeat*seqlength);
					subsequence[repeat*seqlength]=0;
#if 0		// Uncomment to advance by seqlength when match found
					i+=repeat*seqlength-1;
#endif
#if 0		// Uncomment to advance to next tss when match found
					while(i<fastalength-strlen(sequence)+1&&fasta[i]!=N)
						i++;
#endif

					MatchFail:
				}

				seqprob=1;

				for(i=0;i<repeat;i++)
					for(j=0;j<seqlength;j++)
						seqprob*=ProbBP(sequence[j]);

				expected=seqprob*(fastalength-offset);

//				significance=matchcount>expected?(100*(matchcount/expected)-100):(100*(expected/matchcount)-100);
				significance=matchcount>expected?(matchcount/expected):(expected/matchcount);
				significance*=matchcount>expected?1:-1;
			       	for(i=0;i<seqlength;i++)
				       	subsequence[i]=Decode(sequence[i]);
			       	subsequence[seqlength]=0;
//				printf("\rd(%s)%d%30f%20d%20f%%	",subsequence,repeat,expected,matchcount,significance);
				if(abs(significance)>maxsig&&matchcount>=minfound)
				{
					maxsig=abs(significance);
					for(i=0,avg=0;i<matchcount&&i<10000;i++)
						avg+=Pos[i];
					avg/=matchcount;
					for(i=0,std=0;i<matchcount&&i<10000;i++)
						std+=abs(avg-Pos[i]);
					std/=matchcount;
					sprintf(query,"insert into %s values('%s',%d,%f,%d,%f,%f,%f)",table,subsequence,repeat,expected,matchcount,significance,avg,std);
				}
			}
			if(maxsig>=cutoff)
				printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
			NextSequence:
		}
	}

	mysql_close(&mysql);

	return 0;
}
