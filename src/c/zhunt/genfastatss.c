/*
NAME: genfastatss
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates TSS regions of FASTA file
    - argv 1  Fasta file to read from
    - argv 2  ClasSeq file to read from
    - argv 3  BP window around start site
    - argv 4  Number of N's to pad between start sites
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MySql_Defs.h"

char Compliment(char bp)
{
	if(bp=='A')	return 'T';
	if(bp=='T')	return 'A';
	if(bp=='G')	return 'C';
	if(bp=='C')	return 'G';
	return 'N';
}

int main(int argc,char *argv[])
{
	MYSQL mysql;
	MYSQL_RES *CLQ_Res;
	MYSQL_ROW *CLQ_Row;
	FILE *file;
	char filename[100],line[1000],query[4000],CLQ_Table[100],*fasta,*tsssequence;
	int i,j,k,seqlength,fastalength,pad;

	if(argc!=5)
		return fprintf(stderr,"usage: %s Fasta ClasSeq Window N-Padding\n",argv[0]);

	seqlength=2*atoi(argv[3]);
	tsssequence=(char*)malloc(seqlength+1);
	tsssequence[seqlength]='\0';

	pad=atoi(argv[4]);

	if(!(file=fopen(argv[1],"r")))
		return fprintf(stderr,"Invalid file: '%s'\n",argv[1]);

	for(i=0,fastalength=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		fastalength+=strlen(line);

	fasta=(char*)malloc(fastalength+1);
	rewind(file);

	for(i=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		for(j=0;j<strlen(line);j++)
			fasta[i++]=toupper(line[j]);

	fasta[fastalength]=0;
	fclose(file);

	mysql_apps_logon(&mysql);

	sprintf(query,argv[2]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';
					break;
			case '/':       strncpy(query,query+i+1,100);
					i=0;
					break;
		}
	sprintf(CLQ_Table,"%s.%s","Genomes",query);

	sprintf(query,"select start_site,strand from %s order by start_site",CLQ_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	CLQ_Res=mysql_use_result(&mysql);

	j=0;
	for(i=0;i<pad;i++,j%100==0?fprintf(stdout,"%c",'\n'):j==j)
		fprintf(stdout,"N",j++);

	while(CLQ_Row=mysql_fetch_row(CLQ_Res))
	{
		if(!strcmp(CLQ_Row[1],"+"))
			for(i=0;i<seqlength;i++,j%100==0?fprintf(stdout,"%c",'\n'):j==j)
				fprintf(stdout,"%c",fasta[atoi(CLQ_Row[0])+i-seqlength/2],j++);
		else
			for(i=0;i<seqlength;i++,j%100==0?fprintf(stdout,"%c",'\n'):j==j)
				fprintf(stdout,"%c",Compliment(fasta[atoi(CLQ_Row[0])-i+seqlength/2]),j++);
		for(i=0;i<pad;i++,j%100==0?fprintf(stdout,"%c",'\n'):j==j)
			fprintf(stdout,"N",j++);
	}
	fprintf(stdout,"\n");
	mysql_free_result(CLQ_Res);

	return 0;
}
