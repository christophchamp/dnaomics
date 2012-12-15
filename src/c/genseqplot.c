/*
NAME: genseqplot
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Plots gene sequence data with bar graphs, Outputs as PostScript file
    - argv 1  ClasSeq file to read from
    - argv 2  Sequence constraints, e.g. "where length(sequence)=1"
    - argv 3  Out file to store results
    - argv 4  Numeric plot flag
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MySql_Defs.h"
//#include <gnuplot/gnuplot.h>
#include "gnuplot_i.h"

int main(int argc,char **argv)
{
	char CLQ_Table[256],query[1024],constraints[1024],CallID[256];
	int i;
	FILE *OutFile;
	MYSQL mysql;
	MYSQL_RES *CLQSEQ_Res;
	MYSQL_ROW *CLQSEQ_Row;
	gnuplot_ctrl *g;

	mysql_apps_logon(&mysql);

	if(argc!=5)
		return fprintf(stderr,"usage: %s ClasSeq \"Sequence Constraints\" OutFileName.ps Plot\n",argv[0]);

	sprintf(query,argv[1]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';   break;
			case '/':       sprintf(query,&query[i+1]);
					i=0;
		}
	sprintf(CLQ_Table,"FindSeq.%s_FindAllSeq",query);

	sprintf(constraints,argv[2]);

	if(!(OutFile=fopen("GenSeqPlot.tmp","w")))
		return fprintf(stderr,"Could not open plot file\n");

	sprintf(query,"select sequence,repeat,ingen_found/ingen_exp,outgen_found/outgen_exp,(ingen_found/ingen_exp)/(outgen_found/outgen_exp) from %s %s order by sequence",CLQ_Table,constraints);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	CLQSEQ_Res=mysql_store_result(&mysql);

	g=gnuplot_init();

	sprintf(query,"set terminal postscript color");
	gnuplot_cmd(g,query);
	sprintf(query,"set output '%s'",argv[3]);
	gnuplot_cmd(g,query);
	sprintf(query,"set title \"Gene Sequence Plot '%s'\\n%s\"",argv[1],constraints);
	gnuplot_cmd(g,query);
	sprintf(query,"set xrange [-2:%d]",mysql_num_rows(CLQSEQ_Res)+1);
	gnuplot_cmd(g,query);
	sprintf(query,"set yrange [-4:4]");
	gnuplot_cmd(g,query);
	sprintf(query,"set xtics 0");
	gnuplot_cmd(g,query);
	sprintf(query,"set grid");
	gnuplot_cmd(g,query);

	for(CLQSEQ_Row=mysql_fetch_row(CLQSEQ_Res),i=0;CLQSEQ_Row;CLQSEQ_Row=mysql_fetch_row(CLQSEQ_Res),i++)
	{
		fprintf(OutFile,"%d\t\td(%s)%s\t\t%-16s\t\t-%-16s\t\t%-16s\n",i,CLQSEQ_Row[0],CLQSEQ_Row[1],CLQSEQ_Row[2],CLQSEQ_Row[3],CLQSEQ_Row[4]);
		sprintf(query,"set label \"%.2f d(%s)%s %.2f\" at %d,0 rotate font \"Ariel,10\"",atof(CLQSEQ_Row[2]),CLQSEQ_Row[0],CLQSEQ_Row[1],atof(CLQSEQ_Row[3]),i);
		gnuplot_cmd(g,query);
	}
	fclose(OutFile);
	mysql_free_result(CLQSEQ_Res);

	sprintf(query,"p 'GenSeqPlot.tmp' u 1:3 ti \"Inside Genes\" with boxes, 'GenSeqPlot.tmp' u 1:4 ti \"Outside Genes\" w boxes");
	gnuplot_cmd(g,query);

	return 0;
}
