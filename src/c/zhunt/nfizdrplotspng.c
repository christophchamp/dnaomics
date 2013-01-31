/*
NAME: nfizdrplotspng
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Matches Genes to NFIs & ZDRs with incrementing cutoffs, renders color map difference plots
    - argv 1  First fasta file to read from
    - argv 2  First ClasSeq file to read from
    - argv 3  Second fasta file to read from
    - argv 4  Second ClasSeq file to read from
    - argv 5/6  Min/Max nfi cutoff range
    - argv 7/8  Min/Max zdr cutoff range
    - argv 9  Max percentage range
    - argv 10  Max colors in palette
    - argv 11  Out file to store data
    - argv 12  Numeric plot flag
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MySql_Defs.h"
#include <gd.h>
#include <gdfonts.h>
#include <gdfontg.h>

#define _ZdrTss_

int main(int argc,char **argv)
{
	FILE *PngOutFile;
	gdImagePtr GdImage;
	MYSQL mysql;
	MYSQL_RES *Gene_Result,*NFI_Result,*ZDR_Result;
	MYSQL_ROW *Gene_Row,*NFI_Row,*ZDR_Row;
	MYSQL_ROW_OFFSET *NFI_Rollback,*ZDR_Rollback;
	char query[4096],NFI_Table1[128],NFI_Table2[128],ZDR_Table1[128],ZDR_Table2[128],Gene_Table1[128],Gene_Table2[128],CallID[256];
	long int i,Colors,GeneCount1,GeneCount2,NfiZdrGenes1,NfiZdrGenes2,GenePos,NFIPos,NfiStart,NfiStop,NfiBst,ZDRPos,ZdrStart,ZdrStop,ZdrBst;
	int *Palette;
	double NfiMin,NfiMax,NfiStp,NfiCut,ZdrMin,ZdrMax,ZdrStp,ZdrCut,GenePerc1,GenePerc2,GeneDiff,MinDiff,MaxDiff,MaxRange;
	double *NfiScores1,*NfiScores2,*ZdrScores1,*ZdrScores2;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	sprintf(query,argv[1]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       sprintf(query,&query[i+1],100);
					i=0;
		}
	sprintf(NFI_Table1,"NFI_DNA.%s_nfihunt_classeq",query);
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
	sprintf(NFI_Table2,"NFI_DNA.%s_nfihunt_classeq",query);
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

	NfiMin=atof(argv[5]);
	NfiMax=atof(argv[6]);
	NfiStp=(NfiMax-NfiMin)/700;
	ZdrMax=atof(argv[7]);
	ZdrMin=atof(argv[8]);
	ZdrStp=(ZdrMax-ZdrMin)/600;
	MaxRange=atof(argv[9]);
	Colors=atoi(argv[10]);
	Palette=(int*)malloc(Colors*sizeof(int));

	if(strcmp(argv[11]+strlen(argv[11])-4,".png"))
		return fprintf(stderr,"OutFile must be of type '.png'\n");

	fprintf(stdout,"Populating internal table 1\n");
	fflush(stdout);

	sprintf(query,"select start_site,strand,gene_num,gene_name from %s order by start_site",Gene_Table1);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Gene_Result=mysql_store_result(&mysql);
	GeneCount1=mysql_num_rows(Gene_Result);

	sprintf(query,"select position,nfiscore from %s where nfiscore>=%f order by position",NFI_Table1,NfiMin);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	NFI_Result=mysql_store_result(&mysql);

	sprintf(query,"select position,deltalinking from %s where deltalinking<=%f order by position",ZDR_Table1,ZdrMax);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	ZDR_Result=mysql_store_result(&mysql);

	NfiScores1=(double*)malloc(GeneCount1*sizeof(double));
	ZdrScores1=(double*)malloc(GeneCount1*sizeof(double));

	NFI_Rollback=mysql_row_tell(NFI_Result);
	ZDR_Rollback=mysql_row_tell(ZDR_Result);

	for(i=0;i<GeneCount1;i++)
	{
		Gene_Row=mysql_fetch_row(Gene_Result);
		if(atoi(Gene_Row[0])-GenePos<=2000)
		{
			mysql_row_seek(NFI_Result,NFI_Rollback);
			mysql_row_seek(ZDR_Result,ZDR_Rollback);
		}
		GenePos=atoi(Gene_Row[0]);
		NfiStart=GenePos-1000;
		NfiStop=GenePos+1000;
		NfiBst=GenePos+100000;
		NfiScores1[i]=-100;
		while(NFI_Row=mysql_fetch_row(NFI_Result),NFIPos=NFI_Row?atoi(NFI_Row[0]):NfiStop+1,NFIPos<=NfiStop)
			if(NFIPos>=NfiStart&&NfiScores1[i]<=atoi(NFI_Row[1]))
			{
				if(NfiScores1[i]<atoi(NFI_Row[1])||abs(atoi(NFI_Row[0])-GenePos)<abs(NfiBst-GenePos))
				{
					NfiBst=atoi(NFI_Row[0]);
					NfiScores1[i]=atoi(NFI_Row[1]);
				}
			}
#ifdef _ZdrTss_
		ZdrStart=GenePos-1000;
		ZdrStop=GenePos+1000;
		ZdrBst=GenePos+100000;
#else
		ZdrStart=NfiBst-1000;
		ZdrStop=NfiBst+1000;
		ZdrBst=NfiBst+100000;
#endif
		ZdrScores1[i]=100;
		while(ZDR_Row=mysql_fetch_row(ZDR_Result),ZDRPos=ZDR_Row?atoi(ZDR_Row[0]):ZdrStop+1,ZDRPos<=ZdrStop)
			if(ZDRPos>=ZdrStart&&ZdrScores1[i]>=atoi(ZDR_Row[1]))
			{
				if(ZdrScores1[i]>atoi(ZDR_Row[1])||abs(atoi(ZDR_Row[0])-NfiBst)<abs(ZdrBst-NfiBst))
				{
					ZdrBst=atoi(ZDR_Row[0]);
					ZdrScores1[i]=atof(ZDR_Row[1]);
				}
			}
	}

	mysql_free_result(Gene_Result);
	mysql_free_result(NFI_Result);
	mysql_free_result(ZDR_Result);


	fprintf(stdout,"Populating internal table 2\n");

	sprintf(query,"select start_site,strand from %s order by start_site",Gene_Table2);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Gene_Result=mysql_store_result(&mysql);
	GeneCount2=mysql_num_rows(Gene_Result);

	sprintf(query,"select position,nfiscore from %s where nfiscore>=%f order by position",NFI_Table2,NfiMin);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	NFI_Result=mysql_store_result(&mysql);

	sprintf(query,"select position,deltalinking from %s where deltalinking<=%f order by position",ZDR_Table2,ZdrMax);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	ZDR_Result=mysql_store_result(&mysql);

	NfiScores2=(double*)malloc(GeneCount2*sizeof(double));
	ZdrScores2=(double*)malloc(GeneCount2*sizeof(double));

	NFI_Rollback=mysql_row_tell(NFI_Result);
	ZDR_Rollback=mysql_row_tell(ZDR_Result);

	for(i=0;i<GeneCount2;i++)
	{
		Gene_Row=mysql_fetch_row(Gene_Result);
		if(atoi(Gene_Row[0])-GenePos<=2000)
		{
			mysql_row_seek(NFI_Result,NFI_Rollback);
			mysql_row_seek(ZDR_Result,ZDR_Rollback);
		}
		GenePos=atoi(Gene_Row[0]);
		NfiStart=GenePos-1000;
		NfiStop=GenePos+1000;
		NfiBst=GenePos+100000;
		NfiScores2[i]=-100;
		while(NFI_Row=mysql_fetch_row(NFI_Result),NFIPos=NFI_Row?atoi(NFI_Row[0]):NfiStop+1,NFIPos<=NfiStop)
			if(NFIPos>=NfiStart&&NfiScores2[i]<=atoi(NFI_Row[1]))
			{
				if(NfiScores2[i]<atoi(NFI_Row[1])||abs(atoi(NFI_Row[0])-GenePos)<abs(NfiBst-GenePos))
				{
					NfiBst=atoi(NFI_Row[0]);
					NfiScores2[i]=atoi(NFI_Row[1]);
				}
			}
#ifdef _ZdrTss_
		ZdrStart=GenePos-1000;
		ZdrStop=GenePos+1000;
		ZdrBst=GenePos+100000;
#else
		ZdrStart=NfiBst-1000;
		ZdrStop=NfiBst+1000;
		ZdrBst=NfiBst+100000;
#endif
		ZdrScores2[i]=100;
		while(ZDR_Row=mysql_fetch_row(ZDR_Result),ZDRPos=ZDR_Row?atoi(ZDR_Row[0]):ZdrStop+1,ZDRPos<=ZdrStop)
			if(ZDRPos>=ZdrStart&&ZdrScores2[i]>=atoi(ZDR_Row[1]))
			{
				if(ZdrScores2[i]>atoi(ZDR_Row[1])||abs(atoi(ZDR_Row[0])-NfiBst)<abs(ZdrBst-NfiBst))
				{
					ZdrBst=atoi(ZDR_Row[0]);
					ZdrScores2[i]=atof(ZDR_Row[1]);
				}
			}
	}

	mysql_free_result(Gene_Result);
	mysql_free_result(NFI_Result);
	mysql_free_result(ZDR_Result);

	mysql_close(&mysql);


	if(!(PngOutFile=fopen(argv[11],"wb")))
		return fprintf(stderr,"Could not write to png file '%s'\n",argv[11]);

	int SubImageX=120,SubImageY=140,SubImageW=(int)((ZdrMax-ZdrMin)/ZdrStp),SubImageH=(int)((NfiMax-NfiMin)/NfiStp);
	GdImage=gdImageCreate(360+SubImageW+1,240+SubImageH+1);

	int White=gdImageColorAllocate(GdImage,255,255,255),Black=gdImageColorAllocate(GdImage,0,0,0);
	gdImageFill(GdImage,0,0,White);
	Palette[0]=gdImageColorAllocate(GdImage,255,255,128);
	Palette[Colors-1]=gdImageColorAllocate(GdImage,128,255,255);

	for(i=1;i<Colors-1;i++)
	{
		if(i<Colors/2)
			Palette[i]=gdImageColorAllocate(GdImage,(unsigned char)(100+155*(Colors-i*2.0)/Colors),(unsigned char)(255*(i*2.0-Colors)/Colors),0);
		else
			Palette[i]=gdImageColorAllocate(GdImage,0,(unsigned char)(255*(Colors-(i-Colors/2)*2.0)/Colors),(unsigned char)(155*((i-Colors/2)*2.0-Colors)/Colors));
	}

	fprintf(stdout,"Generating results\n");

	MinDiff=100;
	MaxDiff=-100;
	for(ZdrCut=ZdrMax;ZdrCut>=ZdrMin;ZdrCut-=ZdrStp)
	{
		for(NfiCut=NfiMin;NfiCut<=NfiMax;NfiCut+=NfiStp)
		{
			NfiZdrGenes1=0;
			for(i=0;i<GeneCount1;i++)
				if(NfiScores1[i]>=NfiCut&&ZdrScores1[i]<=ZdrCut)
					NfiZdrGenes1++;
			NfiZdrGenes2=0;
			for(i=0;i<GeneCount2;i++)
				if(NfiScores2[i]>=NfiCut&&ZdrScores2[i]<=ZdrCut)
					NfiZdrGenes2++;
			GenePerc1=(100.0*NfiZdrGenes1)/GeneCount1;
			GenePerc2=(100.0*NfiZdrGenes2)/GeneCount2;
			GeneDiff=GenePerc1-GenePerc2;
			// Write to PNG
			gdImageSetPixel(GdImage,SubImageX+(int)(SubImageW*(ZdrCut-ZdrMin)/(ZdrMax-ZdrMin)),SubImageY+(int)(SubImageH*(NfiCut-NfiMin)/(NfiMax-NfiMin)),Palette[GeneDiff<-MaxRange?0:GeneDiff>MaxRange?Colors-1:(int)((Colors/2)*(MaxRange+GeneDiff)/MaxRange)]);
			fflush(PngOutFile);
			MinDiff=MinDiff>GeneDiff?GeneDiff:MinDiff;
			MaxDiff=MaxDiff<GeneDiff?GeneDiff:MaxDiff;
		}
	}

	gdImageString(GdImage,gdFontGiant,SubImageX+SubImageW/3,SubImageY/2,argv[11],Black);

	gdImageString(GdImage,gdFontGiant,SubImageX+SubImageW/3,SubImageY+SubImageH+60,"Z-DNA Delta Linking Cutoff",Black);
	for(ZdrCut=ZdrMax;ZdrCut>=ZdrMin-.0001;ZdrCut-=ZdrStp*100)
	{
		gdImageLine(GdImage,SubImageX+(int)(SubImageW*(ZdrCut-ZdrMin)/(ZdrMax-ZdrMin)),SubImageY,SubImageX+(int)(SubImageW*(ZdrCut-ZdrMin)/(ZdrMax-ZdrMin)),SubImageY+SubImageH,Black);
		sprintf(query,"%.2f",ZdrCut);
		gdImageStringUp(GdImage,gdFontSmall,SubImageX+(int)(SubImageW*(ZdrCut-ZdrMin)/(ZdrMax-ZdrMin))-8,SubImageY+SubImageH+40,query,Black);
	}

	gdImageStringUp(GdImage,gdFontGiant,30,SubImageY+1.2*SubImageH/2,"NFI Score Cutoff",Black);
	for(NfiCut=NfiMin;NfiCut<=NfiMax+.0001;NfiCut+=NfiStp*100)
	{
		gdImageLine(GdImage,SubImageX,SubImageY+(int)(SubImageH*(NfiCut-NfiMin)/(NfiMax-NfiMin)),SubImageX+SubImageW,SubImageY+(int)(SubImageH*(NfiCut-NfiMin)/(NfiMax-NfiMin)),Black);
		sprintf(query,"%.2f",NfiCut);
		gdImageString(GdImage,gdFontSmall,SubImageX-40,SubImageY+(int)(SubImageH*(NfiCut-NfiMin)/(NfiMax-NfiMin))-8,query,Black);
	}

	gdImageFilledRectangle(GdImage,SubImageX+SubImageW+80,SubImageY-10,SubImageX+SubImageW+140,SubImageY+30,Palette[Colors-1]);
	sprintf(query,">  %.2f%%",MaxRange);
	gdImageString(GdImage,gdFontSmall,SubImageX+SubImageW+160,SubImageY,query,Black);
	gdImageFilledRectangle(GdImage,SubImageX+SubImageW+80,SubImageY+SubImageH-10,SubImageX+SubImageW+140,SubImageY+SubImageH+30,Palette[0]);
	sprintf(query,"< -%.2f%%",MaxRange);
	gdImageString(GdImage,gdFontSmall,SubImageX+SubImageW+160,SubImageY+SubImageH,query,Black);

	for(i=0;i<9;i++)
	{
		gdImageFilledRectangle(GdImage,SubImageX+SubImageW+80,SubImageY+(i*SubImageH)/10+60,SubImageX+SubImageW+140,SubImageY+(i*SubImageH)/10+100,Palette[((9-i)*Colors)/10]);
		sprintf(query,"%.2f%%",(MaxRange*((8-i)-4.0))/4);
		gdImageString(GdImage,gdFontSmall,SubImageX+SubImageW+160,SubImageY+(i*SubImageH)/10+70,query,Black);
	}

	gdImagePng(GdImage,PngOutFile);
	fclose(PngOutFile);

	if(atoi(argv[12]))
	{
		sprintf(query,"xv %s",argv[11]);
		system(query);
	}

	return 0;
}

