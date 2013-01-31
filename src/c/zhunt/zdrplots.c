/*
	Matches Genes to ZDRs with incrementing cutoffs, generates difference plot data
	argv 1	  First fasta file to read from
	argv 2	  First ClasSeq file to read from
	argv 3	  Second fasta file to read from
	argv 4	  Second ClasSeq file to read from
	argv 5	  BP window size around TSS
	argv 6	  Minimum cutoff
	argv 7		Maximum cutoff
	argv 8	  Minimum cutoff increment step
	argv 9	  Out file to store data
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
	MYSQL_RES *Gene_Result,*ZDR_Result;
	MYSQL_ROW *Gene_Row,*ZDR_Row;
	char query[4096],ZDR_Table1[128],ZDR_Table2[128],Gene_Table1[128],Gene_Table2[128],CallID[256];
	long int i,Genes1,Genes2,ZDRs,GeneStart,GeneStop,ZDRPos,ZDRGenes1,ZDRGenes2,cutoff,MaxCutoff,step,window;

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

	window=atoi(argv[5]);
	cutoff=atoi(argv[6]);
	MaxCutoff=atoi(argv[7]);
	step=atoi(argv[8]);


	FILE *OutFile=fopen(argv[9],"w");
	if(!OutFile)
		fprintf(stderr,"Could not write to file '%s'\n",argv[9]);

	do
	{

		ZDRGenes1=0;

#ifdef _TSS_ONLY_
		sprintf(query,"select start_site-%d ,start_site+%d from %s order by start_site",window,window,Gene_Table1);
#else
		sprintf(query,"select start-%d ,stop+%d from %s order by start",window,window,Gene_Table1);
#endif
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		Gene_Result=mysql_store_result(&mysql);
		Genes1=mysql_num_rows(Gene_Result);

		sprintf(query,"select position from %s where probability >= %d",ZDR_Table1,cutoff);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		ZDR_Result=mysql_store_result(&mysql);

		Gene_Row=mysql_fetch_row(Gene_Result);
		ZDR_Row=mysql_fetch_row(ZDR_Result);

#ifdef _PERCENTAGE_
		while(Gene_Row&&ZDR_Row)
		{
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			ZDRPos=atoi(ZDR_Row[0]);
			if(GeneStart>ZDRPos)
				ZDR_Row=mysql_fetch_row(ZDR_Result);
			else
			{
				ZDRGenes1+=GeneStop>ZDRPos?1:0;
				Gene_Row=mysql_fetch_row(Gene_Result);
			}
		}
#else
		while(Gene_Row&&ZDR_Row)
		{
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			ZDRPos=atoi(ZDR_Row[0]);
			if(ZDRPos<GeneStop)
			{
				ZDRGenes1+=ZDRPos<GeneStart?0:1;
				ZDR_Row=mysql_fetch_row(ZDR_Result);
			}
			else
				Gene_Row=mysql_fetch_row(Gene_Result);
		}
#endif

		mysql_free_result(Gene_Result);
		mysql_free_result(ZDR_Result);

		ZDRGenes2=0;

#ifdef _TSS_ONLY_
		sprintf(query,"select start_site-%d,start_site+%d from %s order by start_site",window,window,Gene_Table2);
#else
		sprintf(query,"select start-%d,stop+%d from %s order by start",window,window,Gene_Table2);
#endif
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		Gene_Result=mysql_store_result(&mysql);
		Genes2=mysql_num_rows(Gene_Result);

		sprintf(query,"select position from %s where probability >= %d",ZDR_Table2,cutoff);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		ZDR_Result=mysql_store_result(&mysql);

		Gene_Row=mysql_fetch_row(Gene_Result);
		ZDR_Row=mysql_fetch_row(ZDR_Result);

#ifdef _PERCENTAGE_
		while(Gene_Row&&ZDR_Row)
		{
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			ZDRPos=atoi(ZDR_Row[0]);
			if(GeneStart>ZDRPos)
				ZDR_Row=mysql_fetch_row(ZDR_Result);
			else
			{
				ZDRGenes2+=GeneStop>ZDRPos?1:0;
				Gene_Row=mysql_fetch_row(Gene_Result);
			}
		}
#else
		while(Gene_Row&&ZDR_Row)
		{
			GeneStart=atoi(Gene_Row[0]);
			GeneStop=atoi(Gene_Row[1]);
			ZDRPos=atoi(ZDR_Row[0]);
			if(ZDRPos<GeneStop)
			{
				ZDRGenes2+=ZDRPos<GeneStart?0:1;
				ZDR_Row=mysql_fetch_row(ZDR_Result);
			}
			else
				Gene_Row=mysql_fetch_row(Gene_Result);
		}
#endif

		mysql_free_result(Gene_Result);
		mysql_free_result(ZDR_Result);

#ifdef _PERCENTAGE_
		printf("%d %f %f %f\n",cutoff,(100.0*ZDRGenes1)/Genes1,(100.0*ZDRGenes2)/Genes2,(100.0*(ZDRGenes1))/Genes1-(100.0*ZDRGenes2)/Genes2);
#else
		printf("%d %f %f %f\n",cutoff,(1.0*ZDRGenes1)/Genes1,(1.0*ZDRGenes2)/Genes2,(1.0*(ZDRGenes1))/Genes1-(1.0*ZDRGenes2)/Genes2);
#endif
		if(OutFile)
		{
#ifdef _PERCENTAGE_
			fprintf(OutFile,"%d %f %f %f\n",cutoff,(100.0*ZDRGenes1)/Genes1,(100.0*ZDRGenes2)/Genes2,(100.0*(ZDRGenes1))/Genes1-(100.0*ZDRGenes2)/Genes2);
#else
			fprintf(OutFile,"%d %f %f %f\n",cutoff,(1.0*ZDRGenes1)/Genes1,(1.0*ZDRGenes2)/Genes2,(1.0*(ZDRGenes1))/Genes1-(1.0*ZDRGenes2)/Genes2);
#endif
			fflush(OutFile);
		}

		cutoff+=step;

		step=step*10==cutoff?step*10:step;

	}
	while(cutoff<=MaxCutoff);

	return 0;

}
