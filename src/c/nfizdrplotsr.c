/*
NAME: nfizdrplotsr
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Matches Genes to NFIs & ZDRs with incrementing cutoffs, renders 3D difference plot
    - argv 1  First fasta file to read from
    - argv 2  First ClasSeq file to read from
    - argv 3  Second fasta file to read from
    - argv 4  Second ClasSeq file to read from
    - argv 5  Order for Nfi Zdr Tss, e.g. NZT
    - argv 6/7  Min/Max Nfi Zdr BP seperation distance
    - argv 8/9/10  Min/Max nfi cutoff range, increment value
    - argv 11/12/13  Min/Max zdr cutoff range, increment value
    - argv 14  Out file to store data
    - argv 15  Numeric plot flag
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MySql_Defs.h"
//#include <gnuplot/gnuplot.h>
#include "gnuplot_i.h"

int main(int argc,char **argv)
{
	MYSQL mysql;
	FILE *file;
	MYSQL_RES *Gene_Result,*NFI_Result,*ZDR_Result;
	MYSQL_ROW *Gene_Row,*NFI_Row,*ZDR_Row;
	MYSQL_ROW_OFFSET *NFI_Rollback,*ZDR_Rollback;
	gnuplot_ctrl *g;
	char Order[32],query[4000],NFI_Table1[100],NFI_Table2[100],ZDR_Table1[100],ZDR_Table2[100],Gene_Table1[100],Gene_Table2[100];
	int GenOrd,NfiOrd,ZdrOrd;
	long int i,MinDist,MaxDist,GeneCount1,GeneCount2,NfiZdrGenes1,NfiZdrGenes2,GenePos,NfiStart,NfiStop,NfiBst,ZdrStart,ZdrStop,ZdrBst,NFIPos,ZDRPos;
	double NfiMin,NfiMax,NfiStp,NfiCut,ZdrMin,ZdrMax,ZdrStp,ZdrCut,GenePerc1,GenePerc2,MinDiff,MaxDiff;
	double *NfiScores1,*NfiScores2,*ZdrScores1,*ZdrScores2;

	if(argc!=16)
		return printf("usage:  %s Fasta_File1 Gene_File1 Fasta_File2 Gene_File2 Order MinDist MaxDist NfiMin NfiMax NfiStep ZdrMax ZdrMin ZdrStep OutFile Plot\n",argv[0]);

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

	sprintf(Order,argv[5]);
	MinDist=atoi(argv[6]);
	MaxDist=atoi(argv[7]);
	NfiMin=atof(argv[8]);
	NfiMax=atof(argv[9]);
	NfiStp=atof(argv[10]);
	ZdrMax=atof(argv[11]);
	ZdrMin=atof(argv[12]);
	ZdrStp=atof(argv[13]);

	if(argc==15||strcmp(argv[15],"-r"))
	{
		if(!(strstr(Order,"T")&&strstr(Order,"N")&&strstr(Order,"Z")&&strlen(Order)==3))
			return printf("Improper Order format '%s'\n",Order);

		GenOrd=strstr(Order,"T")-Order;
		NfiOrd=strstr(Order,"N")-Order;
		ZdrOrd=strstr(Order,"Z")-Order;

		mysql_apps_logon(&mysql);

		fprintf(stderr,"Populating internal table 1\n");

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
			sprintf(query,Gene_Row[1]);
			switch(query[0])
			{
				case '+':       GenOrd=abs(GenOrd);
						NfiOrd=abs(NfiOrd);
						ZdrOrd=abs(ZdrOrd);
						break;
				case '-':       GenOrd=-abs(GenOrd);
						NfiOrd=-abs(NfiOrd);
						ZdrOrd=-abs(ZdrOrd);
						break;
				default:	return fprintf(stderr,"Invalid Gene Strand '%s\n'",Gene_Row[1]);
			}
			NfiStart=GenOrd<NfiOrd?(GenePos+1):(GenePos-1000);
			NfiStop=GenOrd<NfiOrd?(GenePos+1000):(GenePos-1);
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
			ZdrStart=NfiOrd<ZdrOrd?(NfiBst+MinDist):(NfiBst-MaxDist);
			ZdrStop=NfiOrd<ZdrOrd?(NfiBst+MaxDist):(NfiBst-MinDist);
			ZdrStart=(GenOrd<ZdrOrd&&GenePos>ZdrStart)?GenePos:ZdrStart;
			ZdrStart=(GenOrd>ZdrOrd&&GenePos<ZdrStart)?GenePos:ZdrStart;
			ZdrStop=(GenOrd<ZdrOrd&&GenePos>ZdrStop)?GenePos:ZdrStop;
			ZdrStop=(GenOrd>ZdrOrd&&GenePos<ZdrStop)?GenePos:ZdrStop;
			ZdrBst=GenePos+100000;
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
			printf("%3s %d %-16s %4d %4.0f %4d %4.2f %4d\n",Gene_Row[2],GenePos,Gene_Row[3],NfiBst-GenePos,NfiScores1[i],ZdrBst-GenePos,ZdrScores1[i],NfiBst-ZdrBst);
		}

		mysql_free_result(Gene_Result);
		mysql_free_result(NFI_Result);
		mysql_free_result(ZDR_Result);


		fprintf(stderr,"Populating internal table 2\n");

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
			sprintf(query,Gene_Row[1]);
			switch(query[0])
			{
				case '+':       GenOrd=abs(GenOrd);
						NfiOrd=abs(NfiOrd);
						ZdrOrd=abs(ZdrOrd);
						break;
				case '-':       GenOrd=-abs(GenOrd);
						NfiOrd=-abs(NfiOrd);
						ZdrOrd=-abs(ZdrOrd);
						break;
				default:	return fprintf(stderr,"Invalid Gene Strand '%s\n'",Gene_Row[1]);
			}
			NfiStart=GenOrd<NfiOrd?(GenePos+1):(GenePos-1000);
			NfiStop=GenOrd<NfiOrd?(GenePos+1000):(GenePos-1);
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
			ZdrStart=NfiOrd<ZdrOrd?(NfiBst+MinDist):(NfiBst-MaxDist);
			ZdrStop=NfiOrd<ZdrOrd?(NfiBst+MaxDist):(NfiBst-MinDist);
			ZdrStart=(GenOrd<ZdrOrd&&GenePos>ZdrStart)?GenePos:ZdrStart;
			ZdrStart=(GenOrd>ZdrOrd&&GenePos<ZdrStart)?GenePos:ZdrStart;
			ZdrStop=(GenOrd<ZdrOrd&&GenePos>ZdrStop)?GenePos:ZdrStop;
			ZdrStop=(GenOrd>ZdrOrd&&GenePos<ZdrStop)?GenePos:ZdrStop;
			ZdrBst=GenePos+100000;
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

		FILE *OutFile=fopen(argv[14],"w");
		if(!OutFile)
			return printf("Could not write to file '%s'\n",argv[14]);

		fprintf(stderr,"Generating results\n");

		MinDiff=100;
		MaxDiff=0;
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
				fprintf(OutFile,"%f %f %f %f %f\n",NfiCut,ZdrCut,GenePerc1-GenePerc2,GenePerc1,GenePerc2);
				fflush(OutFile);
				MinDiff=MinDiff>GenePerc1-GenePerc2?GenePerc1-GenePerc2:MinDiff;
				MaxDiff=MaxDiff<GenePerc1-GenePerc2?GenePerc1-GenePerc2:MaxDiff;
			}
		}

		fclose(OutFile);
	
	}

	if(argc>15)
	{
		sprintf(NFI_Table1,NFI_Table1+8);
		NFI_Table1[strlen(NFI_Table1)-8]='\0';
		sprintf(NFI_Table2,NFI_Table2+8);
		NFI_Table2[strlen(NFI_Table2)-8]='\0';
		g=gnuplot_init();
		sprintf(query,"set title \"%% Difference Between %s and %s of TSS w/(NFI + Z-DNA-12bps)\\n",NFI_Table1,NFI_Table2);
		for(i=0;i<argc;i++)
		{
			strcat(query,argv[i]);
			strcat(query," ");
		}
		strcat(query,"\"");
		gnuplot_cmd(g,query);
		sprintf(query,"set dgrid3d %.0f,%.0f,8",(ZdrMax-ZdrMin)/ZdrStp,(NfiMax-NfiMin)/NfiStp);
		gnuplot_cmd(g,query);
		sprintf(query,"set xrange [%f:%f]",NfiMin,NfiMax);
		sprintf(query,"set yrange [%f:%f]",ZdrMin,ZdrMax);
		gnuplot_cmd(g,query);
		if(!strcmp(argv[15],"-r"))
		{
			sprintf(query,"set zrange [*:*]");
			gnuplot_cmd(g,query);
			sprintf(query,"set cntrparam levels incremental -10,1,10");
			gnuplot_cmd(g,query);
		}
		else
		{
			sprintf(query,"set zrange [%f:%f]",MinDiff,MaxDiff);
			gnuplot_cmd(g,query);
			sprintf(query,"set cntrparam levels incremental %.0f,%d,%.0f",MinDiff,1+(int)((MaxDiff-MinDiff)/10),MaxDiff);
			gnuplot_cmd(g,query);
		}
		sprintf(query,"call 'Format/%s.format'",argv[0]);
		gnuplot_cmd(g,query);
		sprintf(query,"sp '%s' u 1:2:3 notitle",argv[14]);
		//sprintf(query,"sp '%s' w l lw 2",argv[14]);
		gnuplot_cmd(g,query);
		printf("Hit Enter to continue\n");
		getchar();
	}

	return 0;
}
