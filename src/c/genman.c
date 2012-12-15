/*
NAME: genman
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: Generates man pages for other programs.
*/

#include <stdio.h>
#include "MySql_Defs.h"

int main()
{
	char query[1024],ProgName[128],TimeString[256],*tmp;
	int i;
	FILE *ManFile;
	MYSQL mysql;
	MYSQL_RES *ProgRes,*ArgRes,*DescRes;
	MYSQL_ROW *ProgRow,*ArgRow,*DescRow;

	mysql_apps_logon(&mysql);

	printf("Must be root user to run this program \(./Bin/GenMan/\) and in the DNAomics directory\n");

	sprintf(query,"select CONCAT(DAYOFMONTH(curdate()),\" \",MONTHNAME(curdate()),\" \",YEAR(curdate()))");
	fprintf(stderr,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	ProgRes=mysql_use_result(&mysql);
	ProgRow=mysql_fetch_row(ProgRes);
	sprintf(TimeString,"%s",ProgRow[0]);
	mysql_free_result(ProgRes);
	sprintf(query,"use Apps");
	fprintf(stderr,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
	sprintf(query,"show tables like '%_Calls'");
	fprintf(stderr,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");

	ProgRes=mysql_store_result(&mysql);
	while(ProgRow=mysql_fetch_row(ProgRes))
	{
		sprintf(ProgName,"%s",ProgRow[0]);
		ProgName[strlen(ProgName)-6]='\0';
		sprintf(query,"Bin/Man/%s.1",ProgName);
		if(!(ManFile=fopen(query,"w")))
			return fprintf("Could not write to '%s'\n",query);

		fprintf(ManFile,".TH man 1 \"%s\" \"1.0\" \"%s\"\n",TimeString,ProgName);
		fprintf(ManFile,".SH NAME\n");
		

		sprintf(query,"select Synopsis from Info where ProgramName='%s'\n",ProgName);
		fprintf(stderr,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		DescRes=mysql_store_result(&mysql);
		if(mysql_num_rows(DescRes))
		{
			DescRow=mysql_fetch_row(DescRes);
			fprintf(ManFile,"%s \\- %s\n",ProgName,DescRow[0]);
		}
		else
			fprintf(ManFile,"%s \\- NO DESCRIPTION GIVEN\n",ProgName);

		fprintf(ManFile,".SH SYNOPSIS\n");
		fprintf(ManFile,".B %s ",ProgName);
		sprintf(query,"describe %s_Calls",ProgName);
		fprintf(stderr,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		ArgRes=mysql_store_result(&mysql);
		ArgRow=mysql_fetch_row(ArgRes);
		while(ArgRow=mysql_fetch_row(ArgRes))
			if(strstr(ArgRow[0],"File"))
				fprintf(ManFile,"\\fI%s\\fR ",ArgRow[0]);
			else
				fprintf(ManFile,"\\fB%s\\fR ",ArgRow[0]);
		fprintf(ManFile,"\n");
		mysql_free_result(ArgRes);

		fprintf(ManFile,".P\n");
		fprintf(ManFile,".B %s\n",ProgName);
		fprintf(ManFile,"[\\fIOPTION\\fR]\n");
		fprintf(ManFile,".SH EXAMPLES\n");
		fprintf(ManFile,"%s ",ProgName);

		sprintf(query,"select * from %s_Logs limit 1",ProgName);
		fprintf(stderr,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		ArgRes=mysql_store_result(&mysql);
		if(mysql_num_rows(ArgRes))
		{
			ArgRow=mysql_fetch_row(ArgRes);
			for(i=2;i<mysql_num_fields(ArgRes);i++)
				fprintf(ManFile,"%s ",ArgRow[i]);
			fprintf(ManFile,"\n");
		}
		else
			fprintf(ManFile,"NO CORE EXAMPLE GIVEN\n");
		mysql_free_result(ArgRes);

		fprintf(ManFile,".P\n%s -l\n",ProgName);
		fprintf(ManFile,".P\n%s -c=C22_Default\n",ProgName);
		fprintf(ManFile,".P\n%s -d=C22_Default\n",ProgName);
		fprintf(ManFile,".SH DESCRIPTION\n");
		fprintf(ManFile,".B %s\n",ProgName);
		fprintf(ManFile,"thorough description goes here\n");
		fprintf(ManFile,".SH ARGUMENTS\n");

		sprintf(query,"describe %s_Calls",ProgName);
		fprintf(stderr,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		ArgRes=mysql_store_result(&mysql);
		ArgRow=mysql_fetch_row(ArgRes);
		while(ArgRow=mysql_fetch_row(ArgRes))
		{
			fprintf(ManFile,".TP\n\\fB%s\\fR\n",ArgRow[0]);
			sprintf(query,"select Description from ManPages where Argument='%s'\n",ArgRow[0]);
			fprintf(stderr,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
			DescRes=mysql_store_result(&mysql);
			if(mysql_num_rows(DescRes))
			{
				DescRow=mysql_fetch_row(DescRes);
				fprintf(ManFile,"%s\n",DescRow[0]);
			}
			else
				fprintf(ManFile,"NO DESCRIPTION GIVEN\n");
		}
		mysql_free_result(ArgRes);

		fprintf(ManFile,".TP\n\\fBCallID\\fR\n");
		fprintf(ManFile,"Run program by 'call id'\n");
		fprintf(ManFile,".SH OPTIONS\n");
		fprintf(ManFile,".TP\n\\fB\\-l\\fR\nList Call Logs in database. Every time this program is run, the timestamp and parameters used are stored in a log.\n");
		fprintf(ManFile,".TP\n\\fB\\-c\\fR[=\\fICallID\\fR]\nList Call IDs in database _OR_ run program by parameters of CallID.\n");
		fprintf(ManFile,".TP\n\\fB\\-d\\fR[=\\fICallID\\fR]\nDelete CallID (by name) from database. Note: If no CallID is given, a list of Call IDs in the database are displayed.\n");
		fprintf(ManFile,".SH FILES\n.P\n.I /usr/share/man/man1/%s.1.gz\n",ProgName);
		fprintf(ManFile,".SH SEE ALSO\n");
		fprintf(ManFile,".TP\n");

		sprintf(query,"select SeeAlso from Info where ProgramName='%s'\n",ProgName);
		fprintf(stderr,mysql_query(&mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(&mysql),"\n\n")):"");
		ArgRes=mysql_store_result(&mysql);
		if(mysql_num_rows(ArgRes))
		{
			ArgRow=mysql_fetch_row(ArgRes);
			tmp=ArgRow[0];
			fprintf(ManFile,ArgRow[0]);
		}
		mysql_free_result(ArgRes);
		fprintf(ManFile,"\n");

		fprintf(ManFile,"\n");
		fprintf(ManFile,".SH BUGS\n");
		fprintf(ManFile,"No known bugs at this time.\n");
		fprintf(ManFile,".SH AUTHOR\n.nf\n");
		fprintf(ManFile,"Sandor Maurice <sandor_maurice@hotmail.com> and P. Christoph Champ <christoph_champ@hotmail.com>.\n");
		fprintf(ManFile,".fi\n.SH HISTORY\n");
		fprintf(ManFile,"2004 \- Written for The DNAomics Project.\n");

		fclose(ManFile);
	}
	mysql_free_result(ProgRes);

	system("cp Bin/Man/* /usr/share/man/man1/");
	system("gzip -qf /usr/share/man/man1/*");
	system("mandb -q -p");

	return 0;
}

