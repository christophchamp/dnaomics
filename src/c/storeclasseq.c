
#include <stdio.h>
#include "MySql_Defs.h"

int main(int argc,char **argv)
{
	char line[1024],columns[32][1024],*tmp1,*tmp2,Store,query[4096],CLQ_Table[256];
	int i,column;
	int GeneNum=1,GeneName,GeneID,Strand,Start,Stop,GeneDesc;
	FILE *InFile;
	MYSQL mysql;

	if(argc<2)
		return fprintf(stderr,"usage: %s ClasSeqFileName [Delimiter]\n",argv[0]);

	if(!(InFile=fopen(argv[1],"r")))
		return fprintf(stderr,"Could not open file '%s'\n",argv[1]);

	mysql_su_logon(&mysql,"Genomes_Write");

	fgets(line,1020,InFile);
	line[strlen(line)-1]='\0';

	for(column=0,tmp1=tmp2=line;tmp2<line+strlen(line);tmp2++)
		if(*tmp2=='\t'||(argc==3&&*tmp2==argv[2][0]))
		{
			strncpy(columns[column],tmp1,tmp2-tmp1);
			columns[column][tmp2-tmp1]='\0';
			tmp1=tmp2+1;
			column++;
		}
	strncpy(columns[column],tmp1,tmp2-tmp1);
	columns[column][tmp2-tmp1]='\0';
	column++;

	for(i=0;i<column;i++)
		fprintf(stdout,"%d:\t%s\n",i+1,columns[i]);

	fprintf(stdout,"\nGene Name Column (1-%d required): ",column);
	fscanf(stdin,"%d",&GeneName);
	fprintf(stdout,"Gene ID Column (1-%d required): ",column);
	fscanf(stdin,"%d",&GeneID);
	fprintf(stdout,"Gene Strand Column (1-%d required): ",column);
	fscanf(stdin,"%d",&Strand);
	fprintf(stdout,"Gene Start Column (1-%d required): ",column);
	fscanf(stdin,"%d",&Start);
	fprintf(stdout,"Gene Stop Column (1-%d required): ",column);
	fscanf(stdin,"%d",&Stop);
	fprintf(stdout,"Gene Description Column (0-%d optional): ",column);
	fscanf(stdin,"%d",&GeneDesc);

	fprintf(stdout,"\nGene Name: %s\n",columns[--GeneName]);
	fprintf(stdout,"Gene ID: %s\n",columns[--GeneID]);
	fprintf(stdout,"Strand: %s\n",columns[--Strand]);
	fprintf(stdout,"Start: %s\n",columns[--Start]);
	fprintf(stdout,"Stop: %s\n",columns[--Stop]);
	fprintf(stdout,"Description: %s\n\n",GeneDesc?columns[GeneDesc-1]:"");

	fprintf(stdout,"Store ClasSeq(y/n)?");
	for(Store=' ';tolower(Store)!='y'&&tolower(Store)!='n';fscanf(stdin,"%c",&Store));
	if(tolower(Store)=='n')
		return 0;

	sprintf(query,"%s",argv[1]);
	for(i=0;i<256;i++)
		switch(query[i])
		{
			case '.':       query[i]='_';       break;
			case '/':       strncpy(query,&query[i+1],100);
					i=0;
		}

	sprintf(CLQ_Table,"Genomes.%s",query);

	sprintf(query,"drop table if exists %s",CLQ_Table);
	fprintf(stdout,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	sprintf(query,"create table %s(gene_num int,strand char,gene_name blob,gene_id blob,gene_desc blob,start int,stop int,size int,start_site int,stop_site int,nfi_pos int,nfi_score int,zdr_pos int,zdr_prob int,start_site_gc double,stop_site_gc double)");
	fprintf(stdout,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");

	sprintf(query,"insert into %s values(%d,'%s','%s','%s','%s',%s,%s,%s-%s,if(strand='+',start,stop),if(strand='+',stop,start),NULL,NULL,NULL,NULL,NULL,NULL)",CLQ_Table,GeneNum++,columns[Strand],columns[GeneName],columns[GeneID],GeneDesc?columns[GeneDesc-1]:"NULL",columns[Start],columns[Stop],columns[Stop],columns[Start]);
	fprintf(stdout,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");

	while(!feof(InFile))
	{
		fgets(line,1020,InFile);
		line[strlen(line)-1]='\0';

		for(column=0,tmp1=tmp2=line;tmp2<line+strlen(line);tmp2++)
			if(*tmp2=='\t'||(argc==3&&*tmp2==argv[2][0]))
			{
				strncpy(columns[column],tmp1,tmp2-tmp1);
				columns[column][tmp2-tmp1]='\0';
				tmp1=tmp2+1;
				column++;
			}
		strncpy(columns[column],tmp1,tmp2-tmp1);
		columns[column][tmp2-tmp1]='\0';
		column++;

		sprintf(query,"insert into %s values(%d,'%s','%s','%s','%s',%s,%s,%s-%s,if(strand='+',start,stop),if(strand='+',stop,start),NULL,NULL,NULL,NULL,NULL,NULL)",CLQ_Table,GeneNum++,columns[Strand],columns[GeneName],columns[GeneID],GeneDesc?columns[GeneDesc-1]:"NULL",columns[Start],columns[Stop],columns[Stop],columns[Start]);
		fprintf(stdout,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	}

	return 0;
}

