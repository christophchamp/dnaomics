/*
NAME: gctssbinpng
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates 2D start_site GC Content Percentage color map
    - argv 1  fasta file from which to read
    - argv 2  classeq file for start_site data
    - argv 3  BinSize, base pair count in each bin
    - argv 4  Window size to analyze
    - argv 5  Min range for colors
    - argv 6  Max range for colors
    - argv 7  Outfile.png, PNG file to write results
    - argv 8  Plot numeric flag to display data (1: plot, 0: no plot)
*/

#include <stdio.h>
#include <math.h>
#include "MySql_Defs.h"
#include <gd.h>
#include <gdfonts.h>
#include <gdfontg.h>

double RangeConvert(double MinSrc,double MaxSrc,double Value,double MinDst,double MaxDst)
{
	return Value<MinSrc?MinDst:Value>MaxSrc?MaxDst:((Value-MinSrc)/(MaxSrc-MinSrc))*(MaxDst-MinDst)+MinDst;
}

int main(int argc,char **argv)
{
	char query[1024],CLQ_Table[128],*sequence,*subsequence,CallID[256];
	int i,j,SeqLength,Genes,GenePos,GCs,BinSize,Window,MinRange,MaxRange,Palette[250];
	FILE *file;
	MYSQL mysql;
	MYSQL_RES *CLQ_Res;
	MYSQL_ROW *CLQ_Row;
	gdImagePtr GdImage;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	if(!(file=fopen(argv[1],"r")))
		return fprintf(stderr,"Invalid file: '%s'\n",argv[1]);

	for(SeqLength=0,fscanf(file,"%s",query);!feof(file);fscanf(file,"%s",query))
		SeqLength+=strlen(query);

	sequence=(char*)malloc(SeqLength);
	rewind(file);

	for(i=0,j=0,fscanf(file,"%s",query);!feof(file);fscanf(file,"%s",query))
		for(j=0;j<strlen(query);j++)
			sequence[i++]=toupper(query[j]);

	fclose(file);

	sprintf(query,argv[2]);
	for(i=0;i<128;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       strncpy(query,query+i+1,128);
					i=0;
		}
	sprintf(CLQ_Table,"%s.%s","Genomes",query);

	BinSize=atoi(argv[3]);
	Window=atoi(argv[4]);
	MinRange=atoi(argv[5]);
	MaxRange=atoi(argv[6]);

	subsequence=(char*)malloc(2*Window+1);

	sprintf(query,"select start_site,strand from %s\n",CLQ_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	CLQ_Res=mysql_store_result(&mysql);
	Genes=mysql_num_rows(CLQ_Res);

	GdImage=gdImageCreate(2*Window/BinSize,Genes);

	Palette[0]=gdImageColorAllocate(GdImage,255,255,128);
	Palette[249]=gdImageColorAllocate(GdImage,128,255,255);
	for(i=1;i<249;i++)
		Palette[i]=gdImageColorAllocate(GdImage,
			(unsigned char)(RangeConvert(1,248*.8,248/2-i,0,255)),
			(unsigned char)(RangeConvert(1,248,248/2-abs(i-248/2),0,255)),
			(unsigned char)(RangeConvert(1,248*.8,i-248/2,0,255))
		);

	Genes=-1;
	while(Genes++,CLQ_Row=mysql_fetch_row(CLQ_Res))
	{
		GenePos=atoi(CLQ_Row[0]);
		if(!strcmp(CLQ_Row[1],"+"))
			for(i=0;i<2*Window;i++)
				subsequence[i]=sequence[GenePos-Window+i];
		else
			for(i=0;i<2*Window;i++)
				subsequence[i]=sequence[GenePos+Window-i];

		for(i=0;i<2*Window/BinSize;i++)
		{
			for(j=GCs=0;j<BinSize;j++)
				GCs+=subsequence[i*BinSize+j]=='G'||subsequence[i*BinSize+j]=='C'?1:0;
			gdImageSetPixel(GdImage,i,Genes,Palette[(int)(RangeConvert(MinRange,MaxRange,(100.0*GCs)/BinSize,0,249))]);
		}
	}
	mysql_free_result(CLQ_Res);

	if(!(file=fopen(argv[7],"wb")))
		return fprintf(stderr,"Could not write to file '%s'\n",argv[7]);

	gdImagePng(GdImage,file);
	fclose(file);

	return 0;
}

