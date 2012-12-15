/*
NAME: nfizdrplots
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Matches Genes to NFIs & ZDRs with incrementing cutoffs, renders 3D difference plots
    - argv 1  First fasta file to read from
    - argv 2  First ClasSeq file to read from
    - argv 3  Second fasta file to read from
    - argv 4  Second ClasSeq file to read from
    - argv 5  BP window size
    - argv 6/7/8  Min/Max nfi cutoff range, increment value
    - argv 9/10/11  Min/Max zdr cutoff range, increment value
    - argv 12  Out file to store data
    - argv 13  Numeric plot flag
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MySql_Defs.h"
//#include <gnuplot/gnuplot.h>
#include "gnuplot_i.h"

int main(int argc,char **argv)
{
	MYSQL mysql;
	FILE *file;
	MYSQL_RES *Gene_Result,*NFI_Result,*ZDR_Result;
	MYSQL_ROW *Gene_Row,*NFI_Row,*ZDR_Row;
	gnuplot_ctrl *g=gnuplot_init();
	char query[4096],NFI_Table1[128],NFI_Table2[128],ZDR_Table1[128],ZDR_Table2[128],Gene_Table1[128],Gene_Table2[128],CallID[256];
	long int i,GeneCount1,GeneCount2,NfiZdrGenes1,NfiZdrGenes2,GeneStart,GeneStop,NFIPos,ZDRPos,Window;
	double NfiMin,NfiMax,NfiStp,NfiCut,ZdrMin,ZdrMax,ZdrStp,ZdrCut,GenePerc1,GenePerc2;
	double *NfiScores1,*NfiScores2,*ZdrScores1,*ZdrScores2;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	sprintf(query,argv[1]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       sprintf(query,&query[i+1],100);
					i=0;
		}
	sprintf(NFI_Table1,"NFI_DNA.%s_nfihunt_classeq",query);
	sprintf(ZDR_Table1,"Z_DNA.%s_zhunt_classeq",query);
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
	sprintf(ZDR_Table2,"Z_DNA.%s_zhunt_classeq",query);
	sprintf(query,argv[4]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       sprintf(query,&query[i+1],100);
					i=0;
		}
	sprintf(Gene_Table2,"Genomes.%s",query);

	Window=atoi(argv[5]);
	NfiMin=atof(argv[6]);
	NfiMax=atof(argv[7]);
	NfiStp=atof(argv[8]);
	ZdrMax=atof(argv[9]);
	ZdrMin=atof(argv[10]);
	ZdrStp=atof(argv[11]);

	if(strcmp(argv[13],"-r"))
	{
		printf("Populating internal table 1\n");

		sprintf(query,"select start_site-%d, start_site+%d from %s order by start_site",Window,Window,Gene_Table1);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		Gene_Result=mysql_store_result(&mysql);
		GeneCount1=mysql_num_rows(Gene_Result);

		sprintf(query,"select position,nfiscore from %s where nfiscore>=%f order by position",NFI_Table1,NfiMin);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		NFI_Result=mysql_store_result(&mysql);

		sprintf(query,"select position,deltalinking from %s where deltalinking<=%f order by position",ZDR_Table1,ZdrMax);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		ZDR_Result=mysql_store_result(&mysql);

		NfiScores1=(double*)malloc(GeneCount1*sizeof(double));
		ZdrScores1=(double*)malloc(GeneCount1*sizeof(double));

		for(i=0;i<GeneCount1;i++)
		{
			printf("\r%.2f%%",(i*100.0)/GeneCount1);
			Gene_Row=mysql_fetch_row(Gene_Result);
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			NfiScores1[i]=-100;
			while(NFI_Row=mysql_fetch_row(NFI_Result),NFIPos=NFI_Row?atoi(NFI_Row[0]):GeneStop+1,NFIPos<=GeneStop)
				if(NFIPos>=GeneStart&&NfiScores1[i]<=atoi(NFI_Row[1]))
					NfiScores1[i]=atoi(NFI_Row[1]);
			ZdrScores1[i]=100;
			while(ZDR_Row=mysql_fetch_row(ZDR_Result),ZDRPos=ZDR_Row?atoi(ZDR_Row[0]):GeneStop+1,ZDRPos<=GeneStop)
				if(ZDRPos>=GeneStart&&ZdrScores1[i]>=atoi(ZDR_Row[1]))
					ZdrScores1[i]=atoi(ZDR_Row[1]);
		}

		mysql_free_result(Gene_Result);
		mysql_free_result(NFI_Result);
		mysql_free_result(ZDR_Result);


		printf("\rPopulating internal table 2\n");

		sprintf(query,"select start_site-%d, start_site+%d from %s order by start_site",Window,Window,Gene_Table2);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		Gene_Result=mysql_store_result(&mysql);
		GeneCount2=mysql_num_rows(Gene_Result);

		sprintf(query,"select position,nfiscore from %s where nfiscore>=%f order by position",NFI_Table2,NfiMin);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		NFI_Result=mysql_store_result(&mysql);

		sprintf(query,"select position,deltalinking from %s where deltalinking<=%f order by position",ZDR_Table2,ZdrMax);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		ZDR_Result=mysql_store_result(&mysql);

		NfiScores2=(double*)malloc(GeneCount2*sizeof(double));
		ZdrScores2=(double*)malloc(GeneCount2*sizeof(double));

		for(i=0;i<GeneCount2;i++)
		{
			printf("\r%.2f%%",(i*100.0)/GeneCount2);
			Gene_Row=mysql_fetch_row(Gene_Result);
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			NfiScores2[i]=-100;
			while(NFI_Row=mysql_fetch_row(NFI_Result),NFIPos=NFI_Row?atoi(NFI_Row[0]):GeneStop+1,NFIPos<=GeneStop)
				if(NFIPos>=GeneStart&&NfiScores2[i]<=atoi(NFI_Row[1]))
					NfiScores2[i]=atoi(NFI_Row[1]);
			ZdrScores2[i]=100;
			while(ZDR_Row=mysql_fetch_row(ZDR_Result),ZDRPos=ZDR_Row?atoi(ZDR_Row[0]):GeneStop+1,ZDRPos<=GeneStop)
				if(ZDRPos>=GeneStart&&ZdrScores2[i]>=atoi(ZDR_Row[1]))
					ZdrScores2[i]=atoi(ZDR_Row[1]);
		}

		mysql_free_result(Gene_Result);
		mysql_free_result(NFI_Result);
		mysql_free_result(ZDR_Result);

		mysql_close(&mysql);

		FILE *OutFile=fopen(argv[12],"w");
		if(!OutFile)
			return printf("Could not write to file '%s'\n",argv[12]);

		printf("\rGenerating results\n");

		for(ZdrCut=ZdrMax;ZdrCut>=ZdrMin;ZdrCut-=ZdrStp)
		{
			for(NfiCut=NfiMin;NfiCut<=NfiMax;NfiCut+=NfiStp)
			{
				NfiZdrGenes1=0;
				for(i=0;i<GeneCount1;i++)
					if(NfiScores1[i]>=NfiCut&&ZdrScores1[i]<=ZdrCut)
						NfiZdrGenes1++;
				NfiZdrGenes2=0;
				for(i=0;i<GeneCount2;i++)
					if(NfiScores2[i]>=NfiCut&&ZdrScores2[i]<=ZdrCut)
						NfiZdrGenes2++;
				GenePerc1=(100.0*NfiZdrGenes1)/GeneCount1;
				GenePerc2=(100.0*NfiZdrGenes2)/GeneCount2;
				fprintf(OutFile,"%f %f %f %f %f\n",NfiCut,ZdrCut,GenePerc1-GenePerc2,GenePerc1,GenePerc2);
				fflush(OutFile);
			}
		}

		fclose(OutFile);
	}

	if(atoi(argv[13])||!strcmp(argv[13],"-r"))
	{
		g=gnuplot_init();
		sprintf(query,"set dgrid3d %.0f,%.0f,1",(ZdrMax-ZdrMin)/ZdrStp,(NfiMax-NfiMin)/NfiStp);
		gnuplot_cmd(g,query);
		sprintf(query,"call 'Format/%s.format'",argv[0]);
		gnuplot_cmd(g,query);
		sprintf(query,"sp '%s' u 1:2:3",argv[12]);
		//sprintf(query,"sp '%s' u 1:2:3 w l lw 2",argv[12]);
		gnuplot_cmd(g,query);
		printf("Hit Enter to continue\n");
		getchar();
	}

	return 0;
}
