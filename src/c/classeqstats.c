/*
NAME: classeqstats
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates various statistics on ClasSeqs

	Variable argument count

	Usage :  ClasSeqStats -<option> [<arguments>] [-<option> [<arguments>]] ...
	Possible options are :
	        -C <ClasSeq>                    (specify ClasSeq)
	        -F <Fasta>                      (specify Fasta)
	        -c                              (ClasSeq analysis)
	        -f                              (Fasta analysis)
	        -g <window>                     (GC content analysis)
	        -h                              (show this help message)
	        -n <window> <min> <max>         (NFI content analysis)
	        -z <window> <min> <max>         (ZDR content analysis)
*/

#include <stdio.h>
#include <string.h>
#include "MySql_Defs.h"

void Error(char *arg);
char *FileToTable(char *file);

char *WriteClasSeq(char *ClasSeq);
char *WriteFasta(char *Fasta);
void ClasSeqStats(char *ClasSeq,char *Fasta);
void FastaStats(char *Fasta);
void GCStats(char *ClasSeq,char *Fasta,char *Window);
void Help();
void NfiStats(char *ClasSeq,char *Fasta,char *Max,char *Min,char *Window);
void ZdrStats(char *ClasSeq,char *Fasta,char *Max,char *Min,char *Window);

int runQuery1(MYSQL mysql,char *query1,char *query2);
int runQuery2(MYSQL mysql,char *query1,char *query2);

int main(int argc,char **argv)
{
	char ClasSeq[100],Fasta[100],err[100];
	int i;

	if(argc==1)
	{
		printf("Usage :  ClasSeqStats -<option> [<arguments>] [-<option> [<arguments>]] ...\n");
		printf("For help  $> ClasSeqStats -h\n");
		return 1;
	}

	sprintf(ClasSeq,"");
	sprintf(Fasta,"");
	sprintf(err,"");

	for(i=1;i<argc;i++)
	{
		if(argv[i][0]=='-')
			switch(argv[i][1])
			{
				case 'C':	sprintf(ClasSeq,WriteClasSeq(argv[++i]));		break;
				case 'F':	sprintf(Fasta,WriteFasta(argv[++i]));			break;
				case 'c':	ClasSeqStats(ClasSeq,Fasta);				break;
				case 'f':	FastaStats(Fasta);					break;
				case 'h':	Help();							break;
				case 'g':	GCStats(ClasSeq,Fasta,argv[++i]);			break;
				case 'n':	NfiStats(ClasSeq,Fasta,argv[++i],argv[++i],argv[++i]);	break;
				case 'z':	ZdrStats(ClasSeq,Fasta,argv[++i],argv[++i],argv[++i]);	break;
				default :	Error(strcat(strcat(err,"Invalid option '"),argv[i]));
			}
		else
			Error(strcat(strcat(err,"Option expected in place of '"),argv[i]));
	}
	return 0;
}

void Error(char *argv)
{
	fprintf(stderr,"\n%s'\n",argv);
	fprintf(stderr,"For help  $> ClasSeqStats -h\n\n");
	exit(1);
}

char *FileToTable(char *file)
{
	int i;
	for(i=0;i<100;i++)
		switch(file[i])
		{
			case '.':	file[i]='_';				break;
			case '/':	strncpy(file,&file[i+1],100);	i=0;	break;
			case '\0':	i=99;
		}
	return file;
}

char *WriteClasSeq(char *ClasSeq)
{
	char err[100];
	sprintf(err,"");
	if(!ClasSeq)
		Error("ClasSeq argument expected to follow '-C");
	if(ClasSeq[0]=='-')
		Error(strcat(strcat(err,"ClasSeq expected in place of '"),ClasSeq));
	return ClasSeq;
}

char *WriteFasta(char *Fasta)
{
	char err[100];
	sprintf(err,"");
	if(!Fasta)
		Error("Fasta argument expected to follow '-F");
	if(Fasta[0]=='-')
		Error(strcat(strcat(err,"Fasta expected in place of '"),Fasta));
}

void ClasSeqStats(char *ClasSeq,char *Fasta)
{
	char CLQ_Table[100],line[1000],query[4000],err[100];
	int i,bps;
	FILE *file;
	MYSQL mysql;
	MYSQL_RES *Result;
	MYSQL_ROW *Row;

	sprintf(err,"");
	if(!strlen(ClasSeq))
		Error("No ClasSeq specified, must call '-C <ClasSeq>' prior to '-c");
	if(!strlen(Fasta))
		Error("No Fasta specified, must call '-F <Fasta>' prior to '-c");
	if(!(file=fopen(Fasta,"r")))
		Error(strcat(strcat(err,"Invalid file : '"),Fasta));

	sprintf(CLQ_Table,"Genomes.%s",FileToTable(ClasSeq));

	for(fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		for(i=0;i<strlen(line);i++)
			bps+=line[i]=='N'?0:1;
	fclose(file);

	mysql_apps_logon(&mysql);

        printf("\n%24s%14s%18s%16s%16s%13s%17s\n","Total","BPs","% Total BPs","Average Size","Std. Dev.","MIN","MAX");

        sprintf(query,"select count(*),sum(size),100*sum(size)/%d,avg(size),std(size),min(size),max(size) from %s",bps,CLQ_Table);
        printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
        Result=mysql_store_result(&mysql);
        Row=mysql_fetch_row(Result);
        printf("ClasSeq :  %12s %16s %12s%% %16s %16s %12s %16s\n\n",Row[0],Row[1],Row[2],Row[3],Row[4],Row[5],Row[6]);
        mysql_free_result(Result);
}

void FastaStats(char *Fasta)
{
	char line[1000],err[100];
	int i,A=0,C=0,G=0,T=0,O=0,bps=0;
	FILE *file;

	sprintf(err,"");
	if(!strlen(Fasta))
		Error("No Fasta specified, must call '-F <Fasta>' prior to '-f");
	if(!(file=fopen(Fasta,"r")))
		Error(strcat(strcat(err,"Invalid file : '"),Fasta));
	
	for(fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		for(i=0;i<strlen(line);i++)
		{
			bps++;
			switch(toupper(line[i]))
			{
				case 'A':       A++;    break;
				case 'C':       C++;    break;
				case 'G':       G++;    break;
				case 'T':       T++;    break;
				default:        O++;    bps--;
			}
		}
	fclose(file);

	printf("\nBase Pairs :  %d  (total: %d)\n\n",bps,bps+O);
	printf("\t\tA :  %12d  %8.2f\%\n",A,100.0*A/bps);
	printf("\t\tC :  %12d  %8.2f\%\n",C,100.0*C/bps);
	printf("\t\tG :  %12d  %8.2f\%\n",G,100.0*G/bps);
	printf("\t\tT :  %12d  %8.2f\%\n",T,100.0*T/bps);
	printf("\t\tN :  %12d\n\n",O);
}

void GCStats(char *ClasSeq,char *Fasta,char *Window)
{
	char line[1000],*sequence,CLQ_Table[100],query[4000],err[100];
	int i,j,window,start,stop,GC,GpC,bps,N;
	FILE *file;
	MYSQL mysql;
	MYSQL_RES *Result;
	MYSQL_ROW *Row;

	sprintf(err,"");
	if(!strlen(ClasSeq))
		Error("No ClasSeq specified, must call '-C <ClasSeq>' prior to '-g");
	if(!strlen(Fasta))
		Error("No Fasta specified, must call '-F <Fasta>' prior to '-g");
	if(!(file=fopen(Fasta,"r")))
		Error(strcat(strcat(err,"Invalid file : '"),Fasta));
	if(!Window)
		Error("Window argument expected to follow '-g");
	if(Window[0]=='-')
		Error(strcat(strcat(err,"Window argument expected to follow '-g' in place of '"),Window));

	mysql_apps_logon(&mysql);

	window=atoi(Window);
	sprintf(CLQ_Table,"Genomes.%s",FileToTable(ClasSeq));

	for(bps=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		bps+=strlen(line);
	sequence=(char *)malloc(bps);
	rewind(file);
	for(i=0,fscanf(file,"%s",line);!feof(file);fscanf(file,"%s",line))
		for(j=0;j<strlen(line);j++)
			sequence[i++]=toupper(line[j]);

	GC=GpC=N=0;
	for(i=0;i<bps;i++)
	{
		sequence[i]=='N'?N++:j++;
		sequence[i]=='C'?GC++,sequence[i+1]=='G'?GpC++:j++:j++;
		sequence[i]=='G'?GC++,sequence[i+1]=='C'?GpC++:j--:j--;
	}

	printf("\nWindow = %d\n\n",window);

	printf("Total\t\tGC Content = %5.2f%%\t GpC Content = %5.2f%%\n",(100.0*GC)/(bps-N),(100.0*GpC)/(bps-N));

	sprintf(query,"select start-%d,stop+%d from %s\n",window,window,CLQ_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	GC=GpC=bps=0;
	while(Row=mysql_fetch_row(Result))
	{
		start=atoi(Row[0]);
		stop=atoi(Row[1]);
		for(i=start;i<=stop;i++)
		{
			bps++;
			sequence[i]=='C'?GC++,sequence[i+1]=='G'?GpC++:j++:j++;
			sequence[i]=='G'?GC++,sequence[i+1]=='C'?GpC++:j--:j--;
		}
	}
	mysql_free_result(Result);
	printf("ClasSeq\t\tGC Content = %5.2f%%\t GpC Content = %5.2f%%\n",(100.0*GC)/bps,(100.0*GpC)/bps);

	sprintf(query,"select start_site-%d,start_site+%d from %s\n",window,window,CLQ_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	GC=GpC=bps=0;
	while(Row=mysql_fetch_row(Result))
	{
		start=atoi(Row[0]);
		stop=atoi(Row[1]);
		for(i=start;i<=stop;i++)
		{
			bps++;
			sequence[i]=='C'?GC++,sequence[i+1]=='G'?GpC++:j++:j++;
			sequence[i]=='G'?GC++,sequence[i+1]=='C'?GpC++:j--:j--;
		}
	}
	mysql_free_result(Result);
	printf("Start Site\tGC Content = %5.2f%%\t GpC Content = %5.2f%%\n",(100.0*GC)/bps,(100.0*GpC)/bps);

        sprintf(query,"select stop_site-%d,stop_site+%d from %s\n",window,window,CLQ_Table);
        printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
        Result=mysql_store_result(&mysql);
        GC=GpC=bps=0;
        while(Row=mysql_fetch_row(Result))
        {
                start=atoi(Row[0]);
                stop=atoi(Row[1]);
                for(i=start;i<=stop;i++)
                {
                        bps++;
                        sequence[i]=='C'?GC++,sequence[i+1]=='G'?GpC++:j++:j++;
                        sequence[i]=='G'?GC++,sequence[i+1]=='C'?GpC++:j--:j--;
                }
        }
        mysql_free_result(Result);
        printf("Stop Site\tGC Content = %5.2f%%\t GpC Content = %5.2f%%\n\n",(100.0*GC)/bps,(100.0*GpC)/bps);
}

void Help()
{
	printf("Usage :  ClasSeqStats -<option> [<arguments>] [-<option> [<arguments>]] ...\n");
	printf("Possible options are :\n");
	printf("	-C <ClasSeq>			(specify ClasSeq)\n");
	printf("	-F <Fasta>			(specify Fasta)\n");
	printf("	-c				(ClasSeq analysis)\n");
	printf("	-f				(Fasta analysis)\n");
	printf("	-g <window>			(GC content analysis)\n");
	printf("	-h				(show this help message)\n");
	printf("	-n <window> <min> <max>		(NFI content analysis)\n");
	printf("	-z <window> <min> <max>		(ZDR content analysis)\n");
}

void NfiStats(char *ClasSeq,char *Fasta,char *Max,char *Min,char *Window)
{
	char CLQ_Table[100],NFI_Table[100],query1[4000],query2[4000],err[100];
	int i,window,min,max,CLQs,NFIs;
        MYSQL mysql;
        MYSQL_RES *Result;
        MYSQL_ROW *Row;

	sprintf(err,"");
	if(!strlen(ClasSeq))
		Error("No ClasSeq specified, must call '-C <ClasSeq>' prior to '-n");
	if(!strlen(Fasta))
		Error("No Fasta specified, must call '-F <Fasta>' prior to '-n");
	if(!Window)
		Error("Window argument expected to follow '-n");
	if(Window[0]=='-')
		Error(strcat(strcat(err,"Window argument expected to follow '-n' in place of '"),Window));
	if(!Min)
		Error("Min argument expected to follow '-n");
	if(Min[0]=='-')
		Error(strcat(strcat(err,"Min argument expected to follow '-n' in place of '"),Min));
	if(!Max)
		Error("Max argument expected to follow '-n");
	if(Max[0]=='-')
		Error(strcat(strcat(err,"Max argument expected to follow '-n' in place of '"),Max));

	sprintf(CLQ_Table,"Genomes.%s",FileToTable(ClasSeq));
	sprintf(NFI_Table,"NFI_DNA.%s_nfihunt",FileToTable(Fasta));

	mysql_apps_logon(&mysql);

	sprintf(query1,"select gene_num from %s",CLQ_Table);
        printf(mysql_query(&mysql,query1)?strcat(strcat(query1,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	CLQs=mysql_num_rows(Result);
	mysql_free_result(Result);

	sprintf(query2,"select max(nfiscore) from %s",NFI_Table);
        printf(mysql_query(&mysql,query2)?strcat(strcat(query2,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	Row=mysql_fetch_row(Result);
	mysql_free_result(Result);

	window=atoi(Window);
	min=atoi(Min);
	max=atoi(toupper(Max[0])=='I'?Row[0]:Max);

	sprintf(query2,"select position from %s where nfiscore between %d and %d order by position",NFI_Table,min,max);
        printf(mysql_query(&mysql,query2)?strcat(strcat(query2,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	NFIs=mysql_num_rows(Result);
	Row=mysql_fetch_row(Result);
	mysql_free_result(Result);

	//CLQ-NFI match

	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	sprintf(query2,"select position from %s where nfiscore between %d and %d order by position",NFI_Table,min,max);
	i=runQuery1(mysql,query1,query2);
	printf("\nClasSeqs with NFIs    : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("Start Sites with NFIs : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("Stop Sites with NFIs  : %12d %12.2f%%\n\n",i,(100.0*i)/CLQs);

	//NFI-CLQ match

	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("NFIs in ClasSeqs      : %12d %12.2f%%\n",i,(100.0*i)/NFIs);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("NFIs in Start Sites   : %12d %12.2f%%\n",i,(100.0*i)/NFIs);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("NFIs in Stop Sites    : %12d %12.2f%%\n\n",i,(100.0*i)/NFIs);
}

void ZdrStats(char *ClasSeq,char *Fasta,char *Max,char *Min,char *Window)
{
	char CLQ_Table[100],ZDR_Table[100],query1[4000],query2[4000],err[100];
	int i,window,min,max,CLQs,ZDRs;
        MYSQL mysql;
        MYSQL_RES *Result;
        MYSQL_ROW *Row;

	sprintf(err,"");
	if(!strlen(ClasSeq))
		Error("No ClasSeq specified, must call '-C <ClasSeq>' prior to '-z");
	if(!strlen(Fasta))
		Error("No Fasta specified, must call '-F <Fasta>' prior to '-z");
	if(!Window)
		Error("Window argument expected to follow '-z");
	if(Window[0]=='-')
		Error(strcat(strcat(err,"Window argument expected to follow '-z' in place of '"),Window));
	if(!Min)
		Error("Min argument expected to follow '-z");
	if(Min[0]=='-')
		Error(strcat(strcat(err,"Min argument expected to follow '-z' in place of '"),Min));
	if(!Max)
		Error("Max argument expected to follow '-z");
	if(Max[0]=='-')
		Error(strcat(strcat(err,"Max argument expected to follow '-z' in place of '"),Max));

	sprintf(CLQ_Table,"Genomes.%s",FileToTable(ClasSeq));
	sprintf(ZDR_Table,"Z_DNA.%s_zhunt_r",FileToTable(Fasta));

	mysql_apps_logon(&mysql);

	sprintf(query1,"select gene_num from %s",CLQ_Table);
        printf(mysql_query(&mysql,query1)?strcat(strcat(query1,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	CLQs=mysql_num_rows(Result);
	mysql_free_result(Result);

	sprintf(query2,"select max(probability) from %s",ZDR_Table);
        printf(mysql_query(&mysql,query2)?strcat(strcat(query2,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	Row=mysql_fetch_row(Result);
	mysql_free_result(Result);

	window=atoi(Window);
	min=atoi(Min);
	max=atoi(toupper(Max[0])=='I'?Row[0]:Max);

	sprintf(query2,"select position from %s where probability between %d and %d",ZDR_Table,min,max);
        printf(mysql_query(&mysql,query2)?strcat(strcat(query2,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Result=mysql_store_result(&mysql);
	ZDRs=mysql_num_rows(Result);
	Row=mysql_fetch_row(Result);
	mysql_free_result(Result);

	//CLQ-ZDR match

	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	sprintf(query2,"select position from %s where probability between %d and %d",ZDR_Table,min,max);
	i=runQuery1(mysql,query1,query2);
	printf("\nClasSeqs with ZDRs    : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("Start Sites with ZDRs : %12d %12.2f%%\n",i,(100.0*i)/CLQs);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery1(mysql,query1,query2);
	printf("Stop Sites with ZDRs  : %12d %12.2f%%\n\n",i,(100.0*i)/CLQs);

	//ZDR-CLQ match

	sprintf(query1,"select start-%d,stop+%d from %s",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("ZDRs in ClasSeqs      : %12d %12.2f%%\n",i,(100.0*i)/ZDRs);

	sprintf(query1,"select start_site-%d,start_site+%d from %s order by start_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("ZDRs in Start Site    : %12d %12.2f%%\n",i,(100.0*i)/ZDRs);

	sprintf(query1,"select stop_site-%d,stop_site+%d from %s order by stop_site",window,window,CLQ_Table);
	i=runQuery2(mysql,query1,query2);
	printf("ZDRs in Stop Site     : %12d %12.2f%%\n\n",i,(100.0*i)/ZDRs);
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
