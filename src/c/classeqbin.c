/*
NAME: classeqbin
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Renders plots for Gene Bins
    - argv 1  Fasta file from which to read
    - argv 2  ClasSeq file for gene positions
    - argv 3  Start position range
    - argv 4  Stop position range
    - argv 5  OutFile for storing results
    - argv 6  Plot numeric flag to display data (1: plot, 0: no plot)
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
	MYSQL_RES *Result;
	gnuplot_ctrl *g;
	char filename[100],query[4000],table[100],line[1000],CallID[256];
	int i,j,seqlength,total;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	file=fopen(argv[1],"r");
	if(!file)
		return fprintf(stderr,"Invalid file: '%s'\n",argv[1]);

	for(seqlength=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		seqlength+=strlen(line);

	fclose(file);

	seqlength=(atoi(argv[4])-atoi(argv[3]));

	printf("Sequence Length :  %d\n",seqlength);

	sprintf(query,argv[2]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}
	sprintf(table,"%s.%s","Genomes",query);

	sprintf(filename,"%s.dat",argv[5]);
	file=fopen(filename,"w");

	for(i=0;i<100;i++)
	{
		sprintf(query,"select * from %s where (start-%s) < %d and (stop-%s) >= %d",table,argv[3],(i+1)*(seqlength/100),argv[3],i*(seqlength/100));
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		Result=mysql_store_result(&mysql);
		fprintf(file,"%d %d\n",i,mysql_num_rows(Result));
		total+=mysql_num_rows(Result);
		mysql_free_result(Result);
	}

	mysql_close(&mysql);
	fclose(file);

	printf("Total :  %d\n",total);

	g=gnuplot_init();
	sprintf(query,"call \"Format/%s.format\"",argv[0]);
	gnuplot_cmd(g,query);

	if(atoi(argv[5]))
	{
		sprintf(query,"p \"%s\" u 1:2 w i",filename);
		gnuplot_cmd(g,query);
		printf("Hit Enter to continue\n");
		getchar();
	}
	else
	{
		if(!strcmp(argv[5]+strlen(argv[5])-3,".ps"))
		{
			sprintf(query,"set output '%s'",argv[5]);
			gnuplot_cmd(g,query);
			sprintf(query,"set term postscript landscape color");
			gnuplot_cmd(g,query);
			sprintf(query,"p \"%s\" u 1:2 w i",filename);
			gnuplot_cmd(g,query);
		}
		else if(!strcmp(argv[5]+strlen(argv[5])-4,".png"))
		{
			sprintf(query,"set output '%s'",argv[5]);
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
