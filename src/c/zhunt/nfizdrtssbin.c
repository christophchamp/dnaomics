/*
NAME: nfizdrtssbin
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Matches NFIs to ZDRs around tss regions, plots scatter graph and averages
    - argv 1  Fasta file to read from
    - argv 2  ClasSeq file to read from
    - argv 3  BP window size around tss
    - argv 4  Min nfiscore
    - argv 5  Max nfiscore
    - argv 6  Minimum ZDR probability
    - argv 7  Output file name to store results
    - argv 8  Numeric plot flag
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
	MYSQL_ROW *Row;
	gnuplot_ctrl *g;
	char Gene_Table[128],NFI_Table[128],ZDR_Table[128],query[4096],filename[128],CallID[256];
	int i,j,window,minnfi,maxnfi,minzdr,Genes,NFIs,ZDRs,GenePos,NFIPos,ZDRPos,x,y,xs,ys,total;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	window=atoi(argv[3]);
	minnfi=atoi(argv[4]);
	maxnfi=atoi(argv[5]);
	minzdr=atoi(argv[6]);

	sprintf(query,argv[1]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';   break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}
	sprintf(NFI_Table,"%s.%s_%s","NFI_DNA",query,"nfihunt");
	sprintf(ZDR_Table,"%s.%s_%s","Z_DNA",query,"zhuntopt_r");

	sprintf(query,argv[2]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}
	sprintf(Gene_Table,"%s.%s","Genomes",query);

	file=fopen(argv[7],"w");

	sprintf(query,"select %s.position,%s.position,%s.start_site,%s.strand from %s,%s,%s where (nfiscore between %d and %d) and (%s.probability>%d) and (%s.position between %s.start_site-%d and %s.start_site+%d) and (%s.position between %s.start_site-%d and %s.start_site+%d)",NFI_Table,ZDR_Table,Gene_Table,Gene_Table,NFI_Table,ZDR_Table,Gene_Table,minnfi,maxnfi,ZDR_Table,minzdr,NFI_Table,Gene_Table,window,Gene_Table,window,ZDR_Table,Gene_Table,window,Gene_Table,window);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);

	while(Row=mysql_fetch_row(Result))
	{
		x=(Row[3][0]=='+')?(atoi(Row[0])-atoi(Row[2])):(atoi(Row[2])-atoi(Row[0]));
		y=(Row[3][0]=='+')?(atoi(Row[1])-atoi(Row[2])):(atoi(Row[2])-atoi(Row[1]));
		fprintf(file,"%d %d\n",x,y);
		xs+=x;
		ys+=y;
		total++;
	}

	mysql_free_result(Result);
	fclose(file);

	if(atoi(argv[8]))
	{
		g=gnuplot_init();
		sprintf(query,"call \"Format/%s.format\"",argv[0]);
		gnuplot_cmd(g,query);
		sprintf(query,"set arrow from %d,%d to %d,%d nohead lt -1 lw 2",-window,0,window,0);
		gnuplot_cmd(g,query);
		sprintf(query,"set arrow from %d,%d to %d,%d nohead lt -1 lw 2",0,-window,0,window);
		gnuplot_cmd(g,query);
		sprintf(query,"set arrow from %d,%d to %d,%d nohead lt 2 lw 1.6",-window,ys/total,window,ys/total);
		gnuplot_cmd(g,query);
		sprintf(query,"set arrow from %d,%d to %d,%d nohead lt 2 lw 1.6",xs/total,-window,xs/total,window);
		gnuplot_cmd(g,query);
		sprintf(query,"p \"%s\" u 1:2 w p",argv[7]);
		gnuplot_cmd(g,query);

		printf("Hit Enter to continue\n");
		getchar();
	}

	printf("Done\n");

	return 0;
}
