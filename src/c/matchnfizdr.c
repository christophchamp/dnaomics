/*
NAME: matchnfizdr
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Matches NFIs and ZDRs to ClasSeq start_site, prints matching results in tables
    - argv 1  Fasta file to read from
    - argv 2  ClasSeq file to read from
    - argv 3  BP window around start_site
    - argv 4  ZDR probability cutoff
    - argv 5  NFI score non-random cutoff
    - argv 6  NFI score functional cutoff
*/

#include <stdio.h>
#include <string.h>
#include "MySql_Defs.h"
//#include <gnuplot/gnuplot.h>
#include "gnuplot_i.h"

int main(int argc,char **argv)
{
	MYSQL mysql;
	MYSQL_RES *Gene_Result,*NFI_Result,*ZDR_Result;
	MYSQL_ROW *Gene_Row,*NFI_Row,*ZDR_Row;
	char Gene_Table[128],NFI_Table[128],ZDR_Table[128],query[4096],CallID[256];
	int i,j,nfi_pos,nfi_score,window,fzdr,rnfi,fnfi;

	mysql_apps_logon(&mysql);
	mysql_parse_input(&mysql,CallID,&argc,argv);
	mysql_close(&mysql);
	mysql_su_logon(&mysql,"Genomes_Write");
	mysql_add_call_log(&mysql,CallID,argc,argv);

	sprintf(query,argv[1]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';   break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}
	sprintf(NFI_Table,"%s.%s_%s","NFI_DNA",query,"nfihunt_classeq");
	sprintf(ZDR_Table,"%s.%s_%s","Z_DNA",query,"zhunt_classeq");

	sprintf(query,argv[2]);
	for(i=0;i<100;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}
	sprintf(Gene_Table,"%s.%s","Genomes",query);

	window=atoi(argv[3]);
	fzdr=atoi(argv[4]);
	rnfi=atoi(argv[5]);
	fnfi=atoi(argv[6]);

#if 0
	sprintf(query,"update %s set nfi_pos = NULL",Gene_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	sprintf(query,"update %s set nfi_score = NULL",Gene_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	sprintf(query,"update %s set zdr_pos = NULL",Gene_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	sprintf(query,"update %s set zdr_prob = NULL",Gene_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");

	sprintf(query,"select start_site,gene_num,gene_id from %s order by start_site",Gene_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Gene_Result=mysql_store_result(&mysql);

	while(Gene_Row=mysql_fetch_row(Gene_Result))
	{
#if 0		// Uncomment for cell genes
		sprintf(query,"0");
		sprintf(ZDR_Table,strlen(Gene_Row[1])==1?strcat(query,Gene_Row[1]):Gene_Row[1]);
		sprintf(NFI_Table,"NFI_DNA.cell_%s_%s_fa_nfihunt_classeq",ZDR_Table,Gene_Row[2]);
#endif		// #############

		sprintf(query,"select position,nfiscore,abs(position-%s) as pos from %s where position between %s-%d and %s+%d order by nfiscore desc,pos",Gene_Row[0],NFI_Table,Gene_Row[0],window,Gene_Row[0],window);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		NFI_Result=mysql_store_result(&mysql);
		if(NFI_Row=mysql_fetch_row(NFI_Result))
		{
			sprintf(query,"update %s set nfi_pos = %s where gene_num = %s",Gene_Table,NFI_Row[0],Gene_Row[1]);
			printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
			sprintf(query,"update %s set nfi_score = %s where gene_num = %s",Gene_Table,NFI_Row[1],Gene_Row[1]);
			printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		}
		mysql_free_result(NFI_Result);
	}

	mysql_free_result(Gene_Result);


	sprintf(query,"select nfi_pos,gene_num,gene_id from %s order by nfi_pos",Gene_Table);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	Gene_Result=mysql_store_result(&mysql);

	while(Gene_Row=mysql_fetch_row(Gene_Result))
	{
#if 0		// Uncomment for cell genes
		sprintf(query,"0");
		sprintf(NFI_Table,strlen(Gene_Row[1])==1?strcat(query,Gene_Row[1]):Gene_Row[1]);
		sprintf(ZDR_Table,"Z_DNA.cell_%s_%s_fa_zhunt_classeq",NFI_Table,Gene_Row[2]);
#endif		// #############

		sprintf(query,"select position,probability,abs(position-%s) as pos from %s where probability >= %d and position between %s-%d and %s+%d order by pos",Gene_Row[0],ZDR_Table,fzdr,Gene_Row[0],window,Gene_Row[0],window);
		printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		ZDR_Result=mysql_store_result(&mysql);
		if(ZDR_Row=mysql_fetch_row(ZDR_Result))
		{
			sprintf(query,"update %s set zdr_pos = %s where gene_num = %s",Gene_Table,ZDR_Row[0],Gene_Row[1]);
			printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
			sprintf(query,"update %s set zdr_prob = %s where gene_num = %s",Gene_Table,ZDR_Row[1],Gene_Row[1]);
			printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		}
		else
		{
			sprintf(query,"update %s set nfi_pos = NULL where gene_num = %s",Gene_Table,Gene_Row[1]);
			printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
			sprintf(query,"update %s set nfi_score = NULL where gene_num = %s",Gene_Table,Gene_Row[1]);
			printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		}
		mysql_free_result(ZDR_Result);
	}

	mysql_free_result(Gene_Result);
#endif

#if 0
	printf("\n\n\t\t\t     Non-Random ( %d - %d )\n\n",rnfi,fnfi);
	sprintf(query,"select * from %s where (nfi_score between %d and %d-1)",Gene_Table,rnfi,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\t\t\t\t     Total :  %d\n\n",mysql_num_rows(mysql_store_result(&mysql)));
	sprintf(query,"select * from %s where (nfi_score between %d and %d-1) and (if(strand='+',zdr_pos<=nfi_pos,zdr_pos>nfi_pos))",Gene_Table,rnfi,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\t\t  NFI ZDR upstrm :  %d\t",mysql_num_rows(mysql_store_result(&mysql)));
	sprintf(query,"select * from %s where (nfi_score between %d and %d-1) and (if(strand='+',zdr_pos>nfi_pos,zdr_pos<=nfi_pos))",Gene_Table,rnfi,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("|  NFI ZDR downstrm :  %d\n",mysql_num_rows(mysql_store_result(&mysql)));
	printf("\t\t________________________|________________________\n\t\t\t\t\t|\n");
	sprintf(query,"select * from %s where (nfi_score between %d and %d-1) and (if(strand='+',zdr_pos<=nfi_pos,zdr_pos>nfi_pos)) and (abs(zdr_pos-nfi_pos)<150)",Gene_Table,rnfi,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\t<=150\t\t\t%d\t",mysql_num_rows(mysql_store_result(&mysql)));
	sprintf(query,"select * from %s where (nfi_score between %d and %d-1) and (if(strand='+',zdr_pos>nfi_pos,zdr_pos<=nfi_pos)) and (abs(zdr_pos-nfi_pos)<150)",Gene_Table,rnfi,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("|\t%d\n",mysql_num_rows(mysql_store_result(&mysql)));
	sprintf(query,"select * from %s where (nfi_score between %d and %d-1) and (if(strand='+',zdr_pos<=nfi_pos,zdr_pos>nfi_pos)) and (abs(zdr_pos-nfi_pos) between 150 and 300)",Gene_Table,rnfi,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\t150-300\t\t\t%d\t",mysql_num_rows(mysql_store_result(&mysql)));
	sprintf(query,"select * from %s where (nfi_score between %d and %d) and (if(strand='+',zdr_pos>nfi_pos,zdr_pos<=nfi_pos)) and (abs(zdr_pos-nfi_pos) between 150 and 300)",Gene_Table,rnfi,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("|\t%d\n",mysql_num_rows(mysql_store_result(&mysql)));
	sprintf(query,"select * from %s where (nfi_score between %d and %d) and (if(strand='+',zdr_pos<=nfi_pos,zdr_pos>nfi_pos)) and (abs(zdr_pos-nfi_pos)>300)",Gene_Table,rnfi,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\t>300\t\t\t%d\t",mysql_num_rows(mysql_store_result(&mysql)));
	sprintf(query,"select * from %s where (nfi_score between %d and %d) and (if(strand='+',zdr_pos>nfi_pos,zdr_pos<=nfi_pos)) and (abs(zdr_pos-nfi_pos)>300)",Gene_Table,rnfi,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("|\t%d\n\n\n",mysql_num_rows(mysql_store_result(&mysql)));
#endif

	printf("\n\n\t\t\t     Functional ( >= %d )\n\n",fnfi);
//	sprintf(query,"select * from %s where (nfi_score >= %d)",Gene_Table,fnfi);
//	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
//	printf("\t\t\t\t     Total :  %d\n\n",mysql_num_rows(mysql_store_result(&mysql)));
//	sprintf(query,"select * from %s where (nfi_score >= %d) and (if(strand='+',zdr_pos<=nfi_pos,zdr_pos>nfi_pos))",Gene_Table,fnfi);
//	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
//	printf("\t\t  NFI ZDR upstrm :  %d\t",mysql_num_rows(mysql_store_result(&mysql)));
//	sprintf(query,"select * from %s where (nfi_score >= %d) and (if(strand='+',zdr_pos>nfi_pos,zdr_pos<=nfi_pos))",Gene_Table,fnfi);
//	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
//	printf("|  NFI ZDR downstrm :  %d\n",mysql_num_rows(mysql_store_result(&mysql)));
//	printf("\t\t________________________|________________________\n\t\t\t\t\t|\n");

	sprintf(query,"select gene_name,gene_id,gene_desc,if(strand='+',nfi_pos-start_site,start_site-nfi_pos),if(strand='+',zdr_pos-start_site,start_site-zdr_pos) from %s where (nfi_score >= %d) and (if(strand='+',zdr_pos<=nfi_pos,zdr_pos>nfi_pos)) and (abs(zdr_pos-nfi_pos)<150) order by gene_name",Gene_Table,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("NFI ZDR upstrm <=150\t\t%d\n\n",mysql_num_rows(Gene_Result=mysql_store_result(&mysql)));
	while(Gene_Row=mysql_fetch_row(Gene_Result),Gene_Row)
		fprintf(stdout,"%s\t%s\t\t%s\t\t%s\n",Gene_Row[3],Gene_Row[4],Gene_Row[0],Gene_Row[1]);
	fprintf(stdout,"\n\n");
	mysql_free_result(Gene_Result);

	sprintf(query,"select gene_name,gene_id,gene_desc,if(strand='+',nfi_pos-start_site,start_site-nfi_pos),if(strand='+',zdr_pos-start_site,start_site-zdr_pos) from %s where (nfi_score >= %d) and (if(strand='+',zdr_pos>nfi_pos,zdr_pos<=nfi_pos)) and (abs(zdr_pos-nfi_pos)<150) order by gene_name",Gene_Table,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\nNFI ZDR downstrm <=150\t\t%d\n\n",mysql_num_rows(Gene_Result=mysql_store_result(&mysql)));
	while(Gene_Row=mysql_fetch_row(Gene_Result),Gene_Row)
		fprintf(stdout,"%s\t%s\t\t%s\t\t%s\n",Gene_Row[3],Gene_Row[4],Gene_Row[0],Gene_Row[1]);
	fprintf(stdout,"\n");
	mysql_free_result(Gene_Result);

	sprintf(query,"select gene_name,gene_id,gene_desc,if(strand='+',nfi_pos-start_site,start_site-nfi_pos),if(strand='+',zdr_pos-start_site,start_site-zdr_pos) from %s where (nfi_score >= %d) and (if(strand='+',zdr_pos<=nfi_pos,zdr_pos>nfi_pos)) and (abs(zdr_pos-nfi_pos) between 150 and 300) order by gene_name",Gene_Table,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\nNFI ZDR upstrm 150-300\t\t%d\n\n",mysql_num_rows(Gene_Result=mysql_store_result(&mysql)));
	while(Gene_Row=mysql_fetch_row(Gene_Result),Gene_Row)
		fprintf(stdout,"%s\t%s\t\t%s\t\t%s\n",Gene_Row[3],Gene_Row[4],Gene_Row[0],Gene_Row[1]);
	fprintf(stdout,"\n");
	mysql_free_result(Gene_Result);

	sprintf(query,"select gene_name,gene_id,gene_desc,if(strand='+',nfi_pos-start_site,start_site-nfi_pos),if(strand='+',zdr_pos-start_site,start_site-zdr_pos) from %s where (nfi_score >= %d) and (if(strand='+',zdr_pos>nfi_pos,zdr_pos<=nfi_pos)) and (abs(zdr_pos-nfi_pos) between 150 and 300) order by gene_name",Gene_Table,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\nNFI ZDR downstrm 150-300\t%d\n\n",mysql_num_rows(Gene_Result=mysql_store_result(&mysql)));
	while(Gene_Row=mysql_fetch_row(Gene_Result),Gene_Row)
		fprintf(stdout,"%s\t%s\t\t%s\t\t%s\n",Gene_Row[3],Gene_Row[4],Gene_Row[0],Gene_Row[1]);
	fprintf(stdout,"\n");
	mysql_free_result(Gene_Result);

	sprintf(query,"select gene_name,gene_id,gene_desc,if(strand='+',nfi_pos-start_site,start_site-nfi_pos),if(strand='+',zdr_pos-start_site,start_site-zdr_pos) from %s where (nfi_score >= %d) and (if(strand='+',zdr_pos<=nfi_pos,zdr_pos>nfi_pos)) and (abs(zdr_pos-nfi_pos)>300) order by gene_name",Gene_Table,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\nNFI ZDR upstrm >300\t\t%d\n\n",mysql_num_rows(Gene_Result=mysql_store_result(&mysql)));
	while(Gene_Row=mysql_fetch_row(Gene_Result),Gene_Row)
		fprintf(stdout,"%s\t%s\t\t%s\t\t%s\n",Gene_Row[3],Gene_Row[4],Gene_Row[0],Gene_Row[1]);
	fprintf(stdout,"\n");
	mysql_free_result(Gene_Result);

	sprintf(query,"select gene_name,gene_id,gene_desc,if(strand='+',nfi_pos-start_site,start_site-nfi_pos),if(strand='+',zdr_pos-start_site,start_site-zdr_pos) from %s where (nfi_score >= %d) and (if(strand='+',zdr_pos>nfi_pos,zdr_pos<=nfi_pos)) and (abs(zdr_pos-nfi_pos)>300) order by gene_name",Gene_Table,fnfi);
	printf(mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	printf("\nNFI ZDR downstrm >300\t\t%d\n\n",mysql_num_rows(Gene_Result=mysql_store_result(&mysql)));
	while(Gene_Row=mysql_fetch_row(Gene_Result),Gene_Row)
		fprintf(stdout,"%s\t%s\t\t%s\t\t%s\n",Gene_Row[3],Gene_Row[4],Gene_Row[0],Gene_Row[1]);
	fprintf(stdout,"\n");
	mysql_free_result(Gene_Result);

	mysql_close(&mysql);

	return 0;
}
