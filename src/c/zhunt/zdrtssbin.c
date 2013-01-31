
/*
	Matches ZDRs to ClasSeq start_site or stop_site and renders correlation plots
	argv 1		Site to analyze
	argv 2		BP window size
	argv 3		Bin size
	argv 4		Fasta file to read from
	argv 5		Fasta Zdr constraints, e.g. " where probability >= 65 "
	argv 6		ClasSeq file to read from
	argv 7		ClasSeq constraints, e.g. " where start_site_gc >= 50 "
	argv 8		Out file to store data
	argv 9		Numeric plot flag, (0: No-Plot, 1: Plot)
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
	MYSQL_RES *Gene_Result,*ZDR_Result;
	MYSQL_ROW *Gene_Row,*ZDR_Row,*Call_Row;
	MYSQL_ROW_OFFSET ZDR_RollBack;
	gnuplot_ctrl *g=gnuplot_init();
	char CLQ_Table[128],CLQ_Constraints[1024],ZDR_Table[128],ZDR_Constraints[1024],query[1024],filename[128],site[32],CallID[256];
	int window,binsize,bins;
	int i,j,Genes,ZDRs,GenePos,ZDRPos,bin[20000];

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
	sprintf(ZDR_Table,"%s.%s_%s","Z_DNA",query,"zhunt_classeq");
	sprintf(ZDR_Constraints,argv[5]);
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

	bins=2*window/binsize+1;

	if(bins>20000)
	{
		printf("Too many bins, Window / BinSize > 20000\n");
		return 1;
	}

	printf("Obaining Gene Data Elements: SELECT %s,strand FROM %s %s ORDER BY %s\n",site,CLQ_Table,CLQ_Constraints,site);
	sprintf(query,"SELECT %s,strand FROM %s %s ORDER BY %s",site,CLQ_Table,CLQ_Constraints,site);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Gene_Result=mysql_store_result(&mysql);
	Genes=mysql_num_rows(Gene_Result);

	printf("Obtaining ZDR Data Elements: SELECT position FROM %s %s ORDER BY position\n",ZDR_Table,ZDR_Constraints);
	sprintf(query,"SELECT position FROM %s %s ORDER BY position",ZDR_Table,ZDR_Constraints);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	ZDR_Result=mysql_store_result(&mysql);
	ZDRs=mysql_num_rows(ZDR_Result);

	Gene_Row=mysql_fetch_row(Gene_Result);	
	ZDR_Row=mysql_fetch_row(ZDR_Result);
	GenePos=atoi(Gene_Row[0]);
	ZDRPos=atoi(ZDR_Row[0]);
	ZDR_RollBack=mysql_row_tell(ZDR_Result);

	printf("Matching Elements\n");
	while(Gene_Row&&ZDR_Row)
	{
		if(GenePos+window>ZDRPos)
		{
			if(GenePos-window<ZDRPos)
				bin[strcmp(Gene_Row[1],"+")?(GenePos-ZDRPos+window)/binsize:(ZDRPos-GenePos+window)/binsize]++;
			else
				ZDR_RollBack=mysql_row_tell(ZDR_Result);
			ZDR_Row=mysql_fetch_row(ZDR_Result);
			if(ZDR_Row)	ZDRPos=atoi(ZDR_Row[0]);
		}
		else
		{
			Gene_Row=mysql_fetch_row(Gene_Result);
			if(Gene_Row)
			{
				GenePos=atoi(Gene_Row[0]);
				if(GenePos-window<ZDRPos)
				{
					mysql_row_seek(ZDR_Result,ZDR_RollBack);
					ZDR_Row=mysql_fetch_row(ZDR_Result);
					if(ZDR_Row)	ZDRPos=atoi(ZDR_Row[0]);
				}
			}
		}
	}

	mysql_free_result(Gene_Result);
	mysql_free_result(ZDR_Result);
	mysql_close(&mysql);

	bin[bins-1]=bin[bins-2];

	sprintf(filename,"%s.dat",argv[8]);
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
