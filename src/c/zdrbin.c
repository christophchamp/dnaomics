
/*
	Renders plots for ZDR Bins
	argv 1/2	Zdr Min/Max Range
	argv 3	  	Fasta file to read from
	argv 4/5	Position Start/Stop Range
	argv 6	  	OutPut data file
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
	char filename[128],query[4096],table[128],line[1024],CallID[256];
	int i,j,seqlength,bin,pos,total;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	file=fopen(argv[3],"r");
	if(!file)
		return fprintf(stderr,"Invalid file: '%s'\n",argv[3]);

	for(seqlength=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		seqlength+=strlen(line);
	fclose(file);

	if(seqlength<atoi(argv[5]))
	{
		fprintf(stderr,"Bounds exceed fasta sequence, assuming %d\n",seqlength);
		sprintf(argv[5],"%d",seqlength);
	}
	fprintf(stdout,"Sequence Length :  %d\n",seqlength=atoi(argv[5])-atoi(argv[4]));

	sprintf(query,argv[3]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}
	sprintf(table,"%s.%s_zhunt_r","Z_DNA",query);

	sprintf(filename,"%s.dat",argv[6]);
	file=fopen(filename,"w");

	sprintf(line,"(position between %s and %s) and",argv[4],argv[5]);
	sprintf(query,"select position-%s from %s where %s (probability between %s and %s)",argv[4],table,line,argv[1],argv[2]);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);

	for(i=0;i<100;i++,bin=pos<(i+1)*(seqlength/100)?1:0)
	{
		for(Row=mysql_fetch_row(Result);pos<(i+1)*(seqlength/100)&&Row;Row=mysql_fetch_row(Result),bin++)
			pos=atoi(Row[0]);
		fprintf(file,"%d %d\n",i,bin);
		total+=bin;
	}

	mysql_free_result(Result);
	mysql_close(&mysql);
	fclose(file);

	printf("Total :  %d\n",total);

	g=gnuplot_init();
	sprintf(query,"call \"Format/%s.format\"",argv[0]);
	gnuplot_cmd(g,query);

	if(atoi(argv[6]))
	{
		sprintf(query,"p \"%s\" u 1:2 w i ls 1",filename);
		gnuplot_cmd(g,query);
		printf("Hit Enter to continue\n");
		getchar();
	}
	else
	{
		if(!strcmp(argv[6]+strlen(argv[6])-3,".ps"))
		{
			sprintf(query,"set output '%s'",argv[6]);
			gnuplot_cmd(g,query);
			sprintf(query,"set term postscript landscape color");
			gnuplot_cmd(g,query);
			sprintf(query,"p \"%s\" u 1:2 w i ls 1",filename);
			gnuplot_cmd(g,query);
		}
		else if(!strcmp(argv[6]+strlen(argv[6])-4,".png"))
		{
			sprintf(query,"set output '%s'",argv[6]);
			gnuplot_cmd(g,query);
			sprintf(query,"set term png");
			gnuplot_cmd(g,query);
			sprintf(query,"p \"%s\" u 1:2 w i ls 1",filename);
			gnuplot_cmd(g,query);
		}
		else
			return fprintf(stderr,"Invalid output file type, must be .ps or .png\n");
	}

	printf("Done\n");
	return 0;
}
