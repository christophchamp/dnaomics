/*
NAME: gpctssbin
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Matches GpC Content Percentage to ClasSeq start_site or stop_site
    - argv 1  Should be 'start_site' or 'stop_site' to define which site to analyze
    - argv 2  Window size to analyze
    - argv 3  BinSize, base pair count in each bin
    - argv 4  Fasta file from which to read
    - argv 5  ClasSeq file for start_site/stop_site data
    - argv 6  ClasSeq constraints, e.g. "with start_site_gc >= 50"
    - argv 7  Plot numeric flag to display data (1: plot, 0: no plot)
*/

#include <stdio.h>
#include <string.h>
#include "MySql_Defs.h"
//#include <gnuplot/gnuplot.h>
#include "gnuplot_i.h"

int main(int argc,char *argv[])
{
	FILE *file;
	MYSQL mysql;
	MYSQL_RES *Gene_Result;
	MYSQL_ROW *Gene_Row;
	gnuplot_ctrl *g=gnuplot_init();
	char Gene_Table[100],query[4000],filename[100],line[1000],*sequence,subsequence[100000],site[10],CallID[256];
	int window,binsize,bins;
	int i,j,Genes,GenePos,seqlength,bin[20000];

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	sprintf(site,argv[1]);
	window=atoi(argv[2]);
	binsize=atoi(argv[3]);
	bins=2*window/binsize+1;

	if(bins>20000)
	{
		printf("Too many bins, Window / BinSize > 20000\n");
		return 1;
	}

	file=fopen(argv[4],"r");
	if(!file)
		return fprintf(stderr,"Invalid file: '%s'\n",argv[4]);

	for(j=0,seqlength=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		seqlength+=strlen(line);

	sequence=(char *)malloc(seqlength);
	rewind(file);

	for(i=0,j=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		for(j=0;j<strlen(line);j++)
			sequence[i++]=tolower(line[j]);

	fclose(file);

	sprintf(query,argv[5]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}
	sprintf(Gene_Table,"%s.%s","Genomes",query);

	printf("Obaining Gene Data Elements\n");
	sprintf(query,"select %s,strand from %s order by %s",site,Gene_Table,site);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Gene_Result=mysql_store_result(&mysql);
	Genes=mysql_num_rows(Gene_Result);

	printf("Matching Elements\n");
	while(Gene_Row=mysql_fetch_row(Gene_Result))
	{
		GenePos=atoi(Gene_Row[0]);
		strncpy(subsequence,sequence+GenePos-window,2*window);
		for(i=0;i<2*window;i++)
			if((subsequence[i]=='c'&&subsequence[i+1]=='g')||(subsequence[i]=='g'&&subsequence[i+1]=='c'))
				bin[strcmp(Gene_Row[1],"+")?(2*window-i)/binsize:i/binsize]++;
	}

	mysql_free_result(Gene_Result);
	mysql_close(&mysql);

	bin[0]=bin[1];
	bin[bins-1]=bin[bins-2];

	sprintf(filename,"%s.dat",argv[6]);
	file=fopen(filename,"w");
	for(i=0;i<bins;i++)
		fprintf(file,"%d %f\n",i*binsize-window,(100.0*bin[i])/(Genes*binsize));
	fclose(file);

	g=gnuplot_init();
	sprintf(query,"call \"Format/%s.format\"",argv[0]);
	gnuplot_cmd(g,query);

	if(atoi(argv[6]))
	{
		sprintf(query,"p \"%s\" u 1:2 w l",filename);
		gnuplot_cmd(g,query);
		printf("Hit Enter to continue\n");
		getchar();
	}
	else
	{
		if(!strcmp(argv[6]+strlen(argv[6])-3,".ps"))
		{
			sprintf(query,"set output '%s'",argv[7]);
			gnuplot_cmd(g,query);
			sprintf(query,"set term postscript landscape color");
			gnuplot_cmd(g,query);
			sprintf(query,"p \"%s\" u 1:2 w l",filename);
			gnuplot_cmd(g,query);
		}
		else if(!strcmp(argv[6]+strlen(argv[6])-4,".png"))
		{
			sprintf(query,"set output '%s'",argv[7]);
			gnuplot_cmd(g,query);
			sprintf(query,"set term png");
			gnuplot_cmd(g,query);
			sprintf(query,"p \"%s\" u 1:2 w l",filename);
			gnuplot_cmd(g,query);
		}
		else
			return fprintf(stderr,"Invalid output file type, must be .ps or .png\n");
	}


	printf("Done\n");
	return 0;
}
