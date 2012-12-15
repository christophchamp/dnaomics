/*
NAME: allseq
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates all possible DNA sequences length read from command line
    - argv 1  BpCount number of base pairs, permutations = 4^BpCount
    - argv 2  GapStart (internal group of N's)
    - argv 3  GapStop (internal group of N's)
*/

#include <stdio.h>

int main(int argc,char **argv)
{
	int BP=0;

	if(argc!=2&&argc!=4)
		return fprintf(stderr,"usage: %s BpCount [GapStart GapStop]\n",argv[0]);

	BP=atoi(argv[1]);

	char C[BP+1];
	int i,j,total;

	for(i=0;i<=BP;i++)
		C[i]=0;

	i=0;
	total=0;
	while(!C[BP])
	{
		for(i=0;i<BP;i++)
		{
			if(argc>2&&i>=atoi(argv[2])&&i<=atoi(argv[3]))
				printf("N");
			else
				switch (C[i])
				{
					case 0: printf("A"); break;
					case 1: printf("C"); break;
					case 2: printf("G"); break;
					case 3: printf("T"); break;
				}
		}
		printf("\n");
		for(i=0;C[i]==3&&i<=BP;i++)
		{
			C[i]=0;
			if(argc>2&&i==atoi(argv[2])-1)
				i+=(atoi(argv[3])-atoi(argv[2])+1);
				
		}
		C[i]++;
	}

	return 0;
}
