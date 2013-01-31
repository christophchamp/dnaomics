/*
NAME: nfitssbin
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Matches NFIs to ClasSeq start_site or stop_site and renders correlation plots
    - argv 1  Site to analyze
    - argv 2  BP window size
    - argv 3  Bin size
    - argv 4  Fasta file to read from
    - argv 5  Fasta Nfi constraints, e.g. " where nfiscore >= 65 "
    - argv 6  ClasSeq file to read from
    - argv 7  ClasSeq constraints, e.g. " where start_site_gc >= 50 "
    - argv 8  Out file to store data
    - argv 9  Numeric plot flag, (0: No-Plot, 1: Plot)
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
	MYSQL_RES *Gene_Result,*NFI_Result;
	MYSQL_ROW *Gene_Row,*NFI_Row,*Call_Row;
	MYSQL_ROW_OFFSET NFI_RollBack;
	gnuplot_ctrl *g;
	char CLQ_Table[100],CLQ_Constraints[1024],NFI_Table[100],NFI_Constraints[1024],query[1024],filename[100],site[10],CallID[256];
	int window,binsize,bins;
	int i,j,Genes,NFIs,GenePos,NFIPos,bin[20000];

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	sprintf(site,argv[1]);
	window=atoi(argv[2]);
	binsize=atoi(argv[3]);
	sprintf(query,argv[4]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';   break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}
	sprintf(NFI_Table,"%s.%s_%s","NFI_DNA",query,"nfihunt_classeq");
	sprintf(NFI_Constraints,argv[5]);
	sprintf(query,argv[6]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}
	sprintf(CLQ_Table,"%s.%s","Genomes",query);
	sprintf(CLQ_Constraints,argv[7]);
	sprintf(filename,"%s.dat",argv[8]);

	bins=2*window/binsize+1;

	if(bins>20000)
		return fprintf(stderr,"Too many bins, Window / BinSize > 20000\n");

	printf("Obaining Gene Data Elements: SELECT %s,strand FROM %s %s ORDER BY %s\n",site,CLQ_Table,CLQ_Constraints,site);
	sprintf(query,"SELECT %s,strand FROM %s %s ORDER BY %s",site,CLQ_Table,CLQ_Constraints,site);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Gene_Result=mysql_store_result(&mysql);
	Genes=mysql_num_rows(Gene_Result);

	printf("Obtaining NFI Data Elements: SELECT position FROM %s %s ORDER BY position\n",NFI_Table,NFI_Constraints);
	sprintf(query,"SELECT position FROM %s %s ORDER BY position",NFI_Table,NFI_Constraints);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	NFI_Result=mysql_store_result(&mysql);
	NFIs=mysql_num_rows(NFI_Result);

	Gene_Row=mysql_fetch_row(Gene_Result);	
	NFI_Row=mysql_fetch_row(NFI_Result);
	GenePos=atoi(Gene_Row[0]);
	NFIPos=atoi(NFI_Row[0]);
	NFI_RollBack=mysql_row_tell(NFI_Result);

	printf("Matching Elements\n");
	while(Gene_Row&&NFI_Row)
	{
		if(GenePos+window>NFIPos)
		{
			if(GenePos-window<NFIPos)
				bin[strcmp(Gene_Row[1],"+")?(GenePos-NFIPos+window)/binsize:(NFIPos-GenePos+window)/binsize]++;
			else
				NFI_RollBack=mysql_row_tell(NFI_Result);
			NFI_Row=mysql_fetch_row(NFI_Result);
			if(NFI_Row)	NFIPos=atoi(NFI_Row[0]);
		}
		else
		{
			Gene_Row=mysql_fetch_row(Gene_Result);
			if(Gene_Row)
			{
				GenePos=atoi(Gene_Row[0]);
				if(GenePos-window<NFIPos)
				{
					mysql_row_seek(NFI_Result,NFI_RollBack);
					NFI_Row=mysql_fetch_row(NFI_Result);
					if(NFI_Row)	NFIPos=atoi(NFI_Row[0]);
				}
			}
		}
	}

	mysql_free_result(Gene_Result);
	mysql_free_result(NFI_Result);
	mysql_close(&mysql);

	bin[bins-1]=bin[bins-2];

	file=fopen(filename,"w");
	for(i=0;i<bins;i++)
		fprintf(file,"%d %d\n",i*binsize-window,bin[i]);
	fclose(file);

        g=gnuplot_init();
        sprintf(query,"call \"Format/%s.format\"",argv[0]);
        gnuplot_cmd(g,query);

        if(atoi(argv[8]))
        {
                sprintf(query,"p \"%s\" u 1:2 w l ls 1",filename);
                gnuplot_cmd(g,query);
                printf("Hit Enter to continue\n");
                getchar();
        }
        else
        {
                if(!strcmp(argv[8]+strlen(argv[8])-3,".ps"))
                {
                        sprintf(query,"set output '%s'",argv[8]);
                        gnuplot_cmd(g,query);
                        sprintf(query,"set term postscript landscape color");
                        gnuplot_cmd(g,query);
                        sprintf(query,"p \"%s\" u 1:2 w l ls 1",filename);
                        gnuplot_cmd(g,query);
                }
                else if(!strcmp(argv[8]+strlen(argv[8])-4,".png"))
                {
                        sprintf(query,"set output '%s'",argv[8]);
                        gnuplot_cmd(g,query);
                        sprintf(query,"set term png");
                        gnuplot_cmd(g,query);
                        sprintf(query,"p \"%s\" u 1:2 w l ls 1",filename);
                        gnuplot_cmd(g,query);
                }
                else
                        return fprintf(stderr,"Invalid output file type, must be .ps or .png\n");
        }

	printf("Done\n");
	return 0;
}
