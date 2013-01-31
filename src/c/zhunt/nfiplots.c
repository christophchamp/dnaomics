/*
NAME: nfiplots
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Matches Genes to NFIs with incrementing cutoffs, generates difference plot data
    - argv 1  First fasta file to read from
    - argv 2  First ClasSeq file to read from
    - argv 3  Second fasta file to read from
    - argv 4  Second ClasSeq file to read from
    - argv 5  BP window size around TSS
    - argv 6  Minimum cutoff
    - argv 7  Minimum cutoff increment step
    - argv 8  Out file to store data
*/

#include <stdio.h>
#include <string.h>
#include "MySql_Defs.h"

#define _TSS_ONLY_
//#define _PERCENTAGE_

int main(int argc,char *argv[])
{
	MYSQL mysql;
	FILE *file;
	MYSQL_RES *Gene_Result,*NFI_Result;
	MYSQL_ROW *Gene_Row,*NFI_Row;
	char query[4096],NFI_Table1[128],NFI_Table2[128],Gene_Table1[128],Gene_Table2[128],CallID[256];
	long int i,NfiGenes1,NfiGenes2,GeneStart,GeneStop,NFIPos,Genes1,Genes2,Nfis1,Nfis2,cutoff,step,window;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

#ifdef _PERCENTAGE_
	printf("Running as \"percentage\"\n");
#else
	printf("Running as \"number\"\n");
#endif

	sprintf(query,argv[1]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       sprintf(query,&query[i+1],100);
					i=0;
		}
	sprintf(NFI_Table1,"NFI_DNA.%s_nfihunt_classeq",query);
	sprintf(query,argv[2]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';   break;
			case '/':       sprintf(query,&query[i+1]);
					i=0;
		}
	sprintf(Gene_Table1,"Genomes.%s",query);
	sprintf(query,argv[3]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       sprintf(query,&query[i+1],100);
					i=0;
		}
	sprintf(NFI_Table2,"NFI_DNA.%s_nfihunt_classeq",query);
	sprintf(query,argv[4]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       sprintf(query,&query[i+1],100);
					i=0;
		}
	sprintf(Gene_Table2,"Genomes.%s",query);

	window=atoi(argv[5]);
	cutoff=atoi(argv[6]);
	step=atoi(argv[7]);

	FILE *OutFile;
	OutFile=fopen(argv[8],"w");
	if(!OutFile)
		fprintf(stderr,"Could not write to file '%s'\n",argv[8]);

	do
	{
		NfiGenes1=0;

#ifdef _TSS_ONLY_
		sprintf(query,"select start_site-%d, start_site+%d from %s order by start_site",window,window,Gene_Table1);
#else
		sprintf(query,"select start-%d,stop+%d from %s order by start",window,window,Gene_Table1);
#endif
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		Gene_Result=mysql_store_result(&mysql);
		Genes1=mysql_num_rows(Gene_Result);

		sprintf(query,"select position from %s where nfiscore >= %d order by position",NFI_Table1,cutoff);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		NFI_Result=mysql_store_result(&mysql);
		Nfis1=mysql_num_rows(NFI_Result);

		Gene_Row=mysql_fetch_row(Gene_Result);
		NFI_Row=mysql_fetch_row(NFI_Result);

#ifdef _PERCENTAGE_
		while(Gene_Row&&NFI_Row)
		{
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			NFIPos=atoi(NFI_Row[0]);
			if(NFIPos<GeneStart)
				NFI_Row=mysql_fetch_row(NFI_Result);
			else
			{
				NfiGenes1+=NFIPos<=GeneStop?1:0;
				Gene_Row=mysql_fetch_row(Gene_Result);
			}
		}
#else
		while(Gene_Row&&NFI_Row)
		{
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			NFIPos=atoi(NFI_Row[0]);
			if(NFIPos<GeneStop)
			{
				NfiGenes1+=NFIPos<GeneStart?0:1;
				NFI_Row=mysql_fetch_row(NFI_Result);
			}
			else
				Gene_Row=mysql_fetch_row(Gene_Result);
		}
#endif

		mysql_free_result(Gene_Result);
		mysql_free_result(NFI_Result);

		NfiGenes2=0;

#ifdef _TSS_ONLY_
		sprintf(query,"select start_site-%d,start_site+%d from %s order by start_site",window,window,Gene_Table2);
#else
		sprintf(query,"select start-%d,stop+%d from %s order by start",window,window,Gene_Table2);
#endif
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		Gene_Result=mysql_store_result(&mysql);
		Genes2=mysql_num_rows(Gene_Result);

		sprintf(query,"select position from %s where nfiscore >= %d order by position",NFI_Table2,cutoff);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		NFI_Result=mysql_store_result(&mysql);
		Nfis2=mysql_num_rows(NFI_Result);

		Gene_Row=mysql_fetch_row(Gene_Result);
		NFI_Row=mysql_fetch_row(NFI_Result);

#ifdef _PERCENTAGE_
		while(Gene_Row&&NFI_Row)
		{
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			NFIPos=atoi(NFI_Row[0]);
			if(NFIPos<GeneStart)
				NFI_Row=mysql_fetch_row(NFI_Result);
			else
			{
				NfiGenes2+=NFIPos<=GeneStop?1:0;
				Gene_Row=mysql_fetch_row(Gene_Result);
			}

		}
#else
		while(Gene_Row&&NFI_Row)
		{
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			NFIPos=atoi(NFI_Row[0]);
			if(NFIPos<GeneStop)
			{
				NfiGenes2+=NFIPos<GeneStart?0:1;
				NFI_Row=mysql_fetch_row(NFI_Result);
			}
			else
				Gene_Row=mysql_fetch_row(Gene_Result);
		}
#endif

		mysql_free_result(Gene_Result);
		mysql_free_result(NFI_Result);

#ifdef _PERCENTAGE_
		printf("%d %f %f %f\n",cutoff,(100.0*NfiGenes1)/Genes1,(100.0*NfiGenes2)/Genes2,(100.0*NfiGenes1)/Genes1-(100.0*NfiGenes2)/Genes2);
#else
		printf("%d %f %f %f\n",cutoff,(1.0*NfiGenes1)/Genes1,(1.0*NfiGenes2)/Genes2,(1.0*NfiGenes1)/Genes1-(1.0*NfiGenes2)/Genes2);
#endif
		if(OutFile)
		{
#ifdef _PERCENTAGE_
			fprintf(OutFile,"%d %f %f %f\n",cutoff,(100.0*NfiGenes1)/Genes1,(100.0*NfiGenes2)/Genes2,(100.0*NfiGenes1)/Genes1-(100.0*NfiGenes2)/Genes2);
#else
			fprintf(OutFile,"%d %f %f %f\n",cutoff,(1.0*NfiGenes1)/Genes1,(1.0*NfiGenes2)/Genes2,(1.0*NfiGenes1)/Genes1-(1.0*NfiGenes2)/Genes2);
#endif
			fflush(OutFile);	
		}

		cutoff+=step;

		// for log scale
//		step=step*100==cutoff?step*10:step;

	}
	while(cutoff<=100);	// max cutoff

	return 0;

}
