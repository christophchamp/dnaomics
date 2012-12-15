/*
NAME: genestats
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates statistics for ClasSeqs, ZDRs and NFIs
    - argv 1  Fasta file to read from
    - argv 2  ClasSeq file to read from
    - argv 3  BP Window around start/stop sites
    - argv 4  ZDR probability cutoff
    - argv 5  Nfi score non-random cutoff
    - argv 6  Nfi score functional cutoff
*/

#include <stdio.h>
#include <stdlib.h>
#include "MySql_Defs.h"
#include <string.h>

#include "nfiprobability.h"

int runQuery1(MYSQL mysql,char *query1,char *query2);
int runQuery2(MYSQL mysql,char *query1,char *query2);

int main(int argc,char *argv[])
{
	MYSQL mysql;
	FILE *file;
	MYSQL_RES *Result;
	MYSQL_ROW *Row;
	char query[4000],query1[4000],query2[4000],Fasta_File[100],CLQ_File[100],CLQ_Table[100],ZDR_Table[100],NFI_Table[100],line[1000],CallID[256];
	int i,j,A=0,C=0,G=0,T=0,O=0,CLQs,ZDRs,NFNFIs,FNFIs,CLQBP,window,minzdr,minnfi1,minnfi2;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_add_call_log(&mysql,CallID,argc,argv);

	printf("ClasSeqStats    Fasta	ClasSeq	WindowSize    MinZdr    MinNfi1    MinNfi2\n");
	for(i=0;i<argc;i++)
		printf("%s    ",argv[i]);
	printf("\n");

	file=fopen(argv[1],"r");
	if(!file)
		return fprintf(stderr,"Invalid file: '%s'",argv[1]);

	sprintf(Fasta_File,argv[1]);
	sprintf(CLQ_File,argv[2]);

	for(i=0;i<100;i++)
		switch(Fasta_File[i])
		{
			case '.':       Fasta_File[i]='_';       break;
			case '/':       strncpy(Fasta_File,&Fasta_File[i+1],100);
					i=0;
		}

	for(i=0;i<100;i++)
		switch(CLQ_File[i])
		{
			case '.':       CLQ_File[i]='_';       break;
			case '/':       strncpy(CLQ_File,&CLQ_File[i+1],100);
					i=0;
		}

	sprintf(CLQ_Table,"Genomes.%s",CLQ_File);
	sprintf(ZDR_Table,"Z_DNA.%s_%s",Fasta_File,"zhunt_r");
	sprintf(NFI_Table,"NFI_DNA.%s_%s",Fasta_File,"nfihunt");

	window=atoi(argv[3]);
	minzdr=atoi(argv[4]);
	minnfi1=atoi(argv[5]);
	minnfi2=atoi(argv[6]);

	//Count Base Pairs
	for(i=0,j=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
	{
		for(i=0;i<strlen(line);i++)
		{
			j++;
			switch(toupper(line[i]))
			{
				case 'A':       A++;    break;
				case 'C':       C++;    break;
				case 'G':       G++;    break;
				case 'T':       T++;    break;
				default:	O++;	j--;
			}
		}
	}
	fclose(file);
	printf("\nBase Pairs :  %d  (total: %d)\n\n\t\tA: %12d  %8.2f\%\n\t\tC: %12d  %8.2f\%\n\t\tG: %12d  %8.2f\%\n\t\tT: %12d  %8.2f\%\n\t\tN: %12d\n\n\n",j,j+O,A,100.0*A/j,C,100.0*C/j,G,100.0*G/j,T,100.0*T/j,O);

	printf("%24s%15s%18s%16s%16s%13s%17s\n\n","Total","BP","% Total BP","Average Size","ST. DEV.","MIN","MAX");

	//Obtain statistics on genes
	sprintf(query,"select count(*),sum(size),100*sum(size)/%d,avg(size),std(size),min(size),max(size) from %s",j,CLQ_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	Row=mysql_fetch_row(Result);
	printf("ClasSeqs : %12s %16s %12s%% %16s %16s %12s %16s\n",Row[0],Row[1],Row[2],Row[3],Row[4],Row[5],Row[6]);
	CLQs=atoi(Row[0]);
	CLQBP=atoi(Row[1]);
	mysql_free_result(Result);
  
	//Obtain statistics on zdrs
	sprintf(query,"select count(*),sum(length),100*sum(length)/%d,avg(length),std(length),min(length),max(length) from %s where probability>=%d",j,ZDR_Table,minzdr);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	Row=mysql_fetch_row(Result);
	printf("ZDRs     : %12s %16s %12s%% %16s %16s %12s %16s\n",Row[0],Row[1],Row[2],Row[3],Row[4],Row[5],Row[6]);
	ZDRs=atoi(Row[0]);
	mysql_free_result(Result);

	//Obtain statistics on nfis
	sprintf(query,"select count(*) from %s where nfiscore between %d and %d-1",NFI_Table,minnfi1,minnfi2);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	Row=mysql_fetch_row(Result);
	printf("NR NFIs  : %12s %16s %12s  %16s %16s %12s %16s\n",Row[0],"-","-","-","-","-","-");
	NFNFIs=atoi(Row[0]);
	mysql_free_result(Result);

	sprintf(query,"select count(*) from %s where nfiscore>=%d",NFI_Table,minnfi2);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	Row=mysql_fetch_row(Result);
	printf("F NFIs   : %12s %16s %12s  %16s %16s %12s %16s\n\n",Row[0],"-","-","-","-","-","-");
	FNFIs=atoi(Row[0]);
	mysql_free_result(Result);

	printf("Predicted ZDRs    : %12d\n",j/minzdr);
	printf("Predicted NR NFIs : %12.0f\n",(j/Probability(minnfi1))-(j/Probability(minnfi2)));
	printf("Predicted F NFIs  : %12.0f\n\n",j/Probability(minnfi2));

	printf("\nZDR Cutoff : probability >= %d\n\n",minzdr);

	CLQBP+=2*window*CLQs;

	//CLQ-ZDR match
	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	sprintf(query2,"select position from %s where probability>=%d",ZDR_Table,minzdr);
	i=runQuery1(mysql,query1,query2);
	printf("ClasSeqs with ZDRs : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("5' with ZDRs       : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("3' with ZDRs       : %12d %12.2f%%\n\n",i,(100.0*i)/CLQs);

	//ZDR-CLQ match
	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("ZDRs in ClasSeqs   : %12d %12.2f%% %12d %12.2f%% %12.0f\n",i,(100.0*i)/ZDRs,CLQBP/minzdr,(100.0*CLQBP)/j,(1.0*ZDRs*CLQBP)/j);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("ZDRs in 5'	   : %12d %12.2f%% %12d %12.2f%% %12.0f\n",i,(100.0*i)/ZDRs,(2*window*CLQs)/minzdr,(200.0*window*CLQs)/j,(2.0*ZDRs*window*CLQs)/j);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("ZDRs in 3'	   : %12d %12.2f%% %12d %12.2f%% %12.0f\n\n",i,(100.0*i)/ZDRs,(2*window*CLQs)/minzdr,(200.0*window*CLQs)/j,(2.0*ZDRs*window*CLQs)/j);


	printf("\nNFI Range : %d <= nfiscore < %d\n\n",minnfi1,minnfi2);

	//CLQ-NFI match

	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	sprintf(query2,"select position from %s where nfiscore between %d and %d-1",NFI_Table,minnfi1,minnfi2);
	i=runQuery1(mysql,query1,query2);
	printf("ClasSeqs with NFIs : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("5' with NFIs       : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("3' with NFIs       : %12d %12.2f%%\n\n",i,(100.0*i)/CLQs);

	//NFI-CLQ match

	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("NFIs in ClasSeqs   : %12d %12.2f%% %12.0f %12.2f%% %12.0f\n",i,(100.0*i)/NFNFIs,(CLQBP/Probability(minnfi1))-(CLQBP/Probability(minnfi2)),(100.0*CLQBP)/j,(1.0*NFNFIs*CLQBP)/j-(1.0*FNFIs*CLQBP)/j);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("NFIs in 5'	   : %12d %12.2f%% %12.0f %12.2f%% %12.0f\n",i,(100.0*i)/NFNFIs,((2*window*CLQs)/Probability(minnfi1))-((2*window*CLQs)/Probability(minnfi2)),(200.0*window*CLQs)/j,(2.0*NFNFIs*window*CLQs)/j-(1.0*FNFIs*window*CLQs)/j);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("NFIs in 3'	   : %12d %12.2f%% %12.0f %12.2f%% %12.0f\n\n",i,(100.0*i)/NFNFIs,((2*window*CLQs)/Probability(minnfi1))-((2*window*CLQs)/Probability(minnfi2)),(200.0*window*CLQs)/j,(2.0*NFNFIs*window*CLQs)/j-(1.0*FNFIs*window*CLQs)/j);


	printf("\nNFI Cutoff : nfiscore >= %d\n\n",minnfi2);

	//CLQ-NFI match

	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	sprintf(query2,"select position from %s where nfiscore>=%d",NFI_Table,minnfi2);
	i=runQuery1(mysql,query1,query2);
	printf("ClasSeqs with NFIs : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("5' with NFIs       : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("3' with NFIs       : %12d %12.2f%%\n\n",i,(100.0*i)/CLQs);

	//NFI-CLQ match

	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("NFIs in ClasSeqs   : %12d %12.2f%% %12.0f %12.2f%% %12.0f\n",i,(100.0*i)/FNFIs,CLQBP/Probability(minnfi2),(100.0*CLQBP)/j,(1.0*FNFIs*CLQBP)/j);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("NFIs in 5'	   : %12d %12.2f%% %12.0f %12.2f%% %12.0f\n",i,(100.0*i)/FNFIs,(2*window*CLQs)/Probability(minnfi2),(200.0*window*CLQs)/j,(2.0*FNFIs*window*CLQs)/j);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("NFIs in 3'	   : %12d %12.2f%% %12.0f %12.2f%% %12.0f\n\n",i,(100.0*i)/FNFIs,(2*window*CLQs)/Probability(minnfi2),(200.0*window*CLQs)/j,(2.0*FNFIs*window*CLQs)/j);

	return 0;
}

int runQuery1(MYSQL mysql,char *query1,char *query2)
{
	MYSQL_RES *Result1,*Result2;
	MYSQL_ROW *Row1,*Row2;
	int i=0,Start,Stop,Pos;

	printf(mysql_query(&mysql,query1)?strcat(strcat(query1,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result1=mysql_store_result(&mysql);

	printf(mysql_query(&mysql,query2)?strcat(strcat(query2,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result2=mysql_store_result(&mysql);

	Row1=mysql_fetch_row(Result1);
	Row2=mysql_fetch_row(Result2);

	while(Row1&&Row2)
	{
		Start=atoi(Row1[0]);
		Stop=atoi(Row1[1]);
		Pos=atoi(Row2[0]);
		if(Start>Pos)
			Row2=mysql_fetch_row(Result2);
		else
		{
			i+=Stop>Pos?1:0;
			Row1=mysql_fetch_row(Result1);
		}
	}

	mysql_free_result(Result1);
	mysql_free_result(Result2);

	return i;
}

int runQuery2(MYSQL mysql,char *query1,char *query2)
{
	MYSQL_RES *Result1,*Result2;
	MYSQL_ROW *Row1,*Row2;
	int i=0,Start,Stop,Pos;

	printf(mysql_query(&mysql,query1)?strcat(strcat(query1,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result1=mysql_store_result(&mysql);

	printf(mysql_query(&mysql,query2)?strcat(strcat(query2,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result2=mysql_store_result(&mysql);

	Row1=mysql_fetch_row(Result1);
	Row2=mysql_fetch_row(Result2);

	while(Row1&&Row2)
	{
		Start=atoi(Row1[0]);
		Stop=atoi(Row1[1]);
		Pos=atoi(Row2[0]);
		if(Pos>Stop)
			Row1=mysql_fetch_row(Result1);
		else
		{
			i+=Pos>Start?1:0;
			Row2=mysql_fetch_row(Result2);
		}
	}

	mysql_free_result(Result1);
	mysql_free_result(Result2);

	return i;
}
