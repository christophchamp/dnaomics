/*
NAME: gcbin
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Renders plots for GC Content Percentage Bins
    - argv 1  BpCount number of base pairs, permutations = 4^BpCount
    - argv 1		Fasta file from which to read
    - argv 2 Start/Stop position range
    - argv 4		Outfile in which to store data
    - argv 5		Plot numeric flag to display data (1: plot, 0: no plot)
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
	gnuplot_ctrl *g;
	char filename[100],query[4000],table[100],line[1000],*sequence,CallID[256];
	int i,j,seqlength,total;
	unsigned long GC_Content;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	file=fopen(argv[1],"r");
	if(!file)
		return fprintf(stderr,"Invalid file: '%s'\n",argv[1]);

	for(j=0,seqlength=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		seqlength+=strlen(line);

	sequence=(char *)malloc(seqlength);
	rewind(file);

	for(i=0,j=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		for(j=0;j<strlen(line);j++)
			sequence[i++]=tolower(line[j]);
	fclose(file);

	if(seqlength<atoi(argv[3]))
	{
		fprintf(stderr,"Bounds exceed fasta sequence, assuming %d\n",seqlength);
		sprintf(argv[3],"%d",seqlength);
	}
	fprintf(stdout,"Sequence Length :  %d\n",seqlength=atoi(argv[3])-atoi(argv[2]));

	sprintf(filename,"%s.dat",argv[4]);
	file=fopen(filename,"w");

	for(i=0,GC_Content=0;i<100;i++,GC_Content=0)
	{
		for(j=(i*(seqlength/100))+(argc>4?atoi(argv[2]):0);j<((i+1)*(seqlength/100))+(argc>4?atoi(argv[2]):0);j++)
			if(sequence[j]=='c'||sequence[j]=='g')
				GC_Content++;
		GC_Content=(10000*GC_Content)/seqlength;
		fprintf(file,"%d %d\n",i,GC_Content);
		total+=GC_Content;
	}

	fclose(file);
	printf("Total :  %d\n",total);

	g=gnuplot_init();
	sprintf(query,"call \"Format/%s.format\"",argv[0]);
	gnuplot_cmd(g,query);

	if(atoi(argv[4]))
	{
		sprintf(query,"p \"%s\" u 1:2 w i",filename);
		gnuplot_cmd(g,query);
		printf("Hit Enter to continue\n");
		getchar();
	}
	else
	{
		if(!strcmp(argv[4]+strlen(argv[4])-3,".ps"))
		{
			sprintf(query,"set output '%s'",argv[4]);
			gnuplot_cmd(g,query);
			sprintf(query,"set term postscript landscape color");
			gnuplot_cmd(g,query);
			sprintf(query,"p \"%s\" u 1:2 w i",filename);
			gnuplot_cmd(g,query);
		}
		else if(!strcmp(argv[4]+strlen(argv[4])-4,".png"))
		{
			sprintf(query,"set output '%s'",argv[4]);
			gnuplot_cmd(g,query);
			sprintf(query,"set term png");
			gnuplot_cmd(g,query);
			sprintf(query,"p \"%s\" u 1:2 w i",filename);
			gnuplot_cmd(g,query);
		}
		else
			return fprintf(stderr,"Invalid output file type, must be .ps or .png\n");
	}

	printf("Done\n");
	return 0;
}
