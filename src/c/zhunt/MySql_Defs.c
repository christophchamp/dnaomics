/*
NAME: MySql_Defs
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: MySQL definitions for use in other programs
*/

#include <stdio.h>
#include </usr/include/curses.h>
#include "MySql_Defs.h"

void mysql_apps_logon(MYSQL *mysql)
{
	mysql_init(mysql);
	if(fprintf(stderr,"%s",!mysql_real_connect(mysql,"localhost","apps","triple3",NULL,0,NULL,0)?strcat(mysql_error(mysql),"\n"):""))
		exit(1);
}

void mysql_su_logon(MYSQL *mysql,char *user)
{
	WINDOW *mywindow;
	char password[256];
	int len=0;

	mywindow=initscr();
	noecho();
	fprintf(stderr,"Enter %s password: ",user);
	while(password[len-1]!='\n')
	do
		password[len++]=(char)getch();
	while(password[len-1]!='\n');
	endwin();
	password[len-1]='\0';
	fprintf(stderr,"\n");
	mysql_init(mysql);
	if(fprintf(stderr,"%s",!mysql_real_connect(mysql,"localhost",user,password,NULL,0,NULL,0)?strcat(mysql_error(mysql),"\n"):""))
		exit(1);

}

void mysql_parse_input(MYSQL *mysql,char *CallID,int *argc,char **argv)
{
	char query[1024];
	int i;
	MYSQL_RES *Result,*DescRes;
	MYSQL_ROW *Row,*DescRow;

	if(*argc>1&&argv[1][0]=='-')
	{
		switch(argv[1][1])
		{
			case 'c':	if(strlen(argv[1]+2))
					{
						sprintf(CallID,"%s",argv[1]+3);
						sprintf(query,"select * from Apps.%s_Calls where CallID='%s'",argv[0],CallID);
						fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
						Result=mysql_store_result(mysql);
						if(mysql_num_rows(Result))
						{
							Row=mysql_fetch_row(Result);
							*argc=mysql_num_fields(Result);
							for(i=1;i<mysql_num_fields(Result);i++)
							{
								argv[i]=(char*)malloc(strlen(Row[i])+1);
								sprintf(argv[i],"%s",Row[i]);
								fprintf(stdout,"%s\t",Row[i]);
							}
							fprintf(stdout,"\n");
						}
						else
						{
							fprintf(stderr,"No such Call ID: '%s'\n",argv[1]+3);
							exit(1);
						}
					}
					else
					{
						sprintf(query,"select * from Apps.%s_Calls",argv[0]);
						fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
						Result=mysql_store_result(mysql);
						if(mysql_num_rows(Result))
							while(fprintf(stdout,"\n"),Row=mysql_fetch_row(Result),Row)
								for(i=0;i<mysql_num_fields(Result);i++)
									fprintf(stdout,"%s\t%s",Row[i],i==0?"=>\t":"");
						else
							fprintf(stderr,"No Calls in database\n");
						exit(0);
					}
					break;
			case 'l':	sprintf(query,"select * from Apps.%s_Logs",argv[0]);
					fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
					Result=mysql_store_result(mysql);
					if(mysql_num_rows(Result))
					{
						while(fprintf(stdout,"\n"),Row=mysql_fetch_row(Result),Row)
							for(i=0;i<mysql_num_fields(Result);i++)
								fprintf(stdout,"%s\t%s",Row[i],i==1?"=>\t":"");
						fprintf(stdout,"\n");
					}
					else
						fprintf(stderr,"No Logs in database\n");
					exit(0);
					break;
			case 'd':	if(strlen(argv[1]+2))
					{
						sprintf(query,"delete from Apps.%s_Calls where CallID='%s'",argv[0],argv[1]+3);
						fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
					}
					else
					{
						fprintf(stderr,"usage: %s -d=CallID\nNo CallID provided, nothing deleted\n",argv[0]);
						sprintf(query,"select * from Apps.%s_Calls\n",argv[0]);
						fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
						Result=mysql_store_result(mysql);
						if(mysql_num_rows(Result))
						{
							while(fprintf(stdout,"\n"),Row=mysql_fetch_row(Result),Row)
								for(i=0;i<mysql_num_fields(Result);i++)
									fprintf(stdout,"%s\t",Row[i]);
							fprintf(stdout,"\n");
						}
						else
							fprintf(stderr,"No Calls in database\n");
					}
					exit(0);
					break;
			default:	fprintf(stderr,"Unrecognized option %s\n\t-c\tList Calls\n\t-l\tList Logs\n\t-d\tDelete CallID\n",argv[1]);
					break;
		}
	}
	else
	{
		sprintf(CallID,"");
		sprintf(query,"describe Apps.%s_Calls",argv[0]);
		fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
		Result=mysql_store_result(mysql);
		if(*argc!=mysql_num_rows(Result))
		{
			sprintf(query,"select Synopsis from Apps.Info where ProgramName='%s'",argv[0]);
			fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
			DescRes=mysql_store_result(mysql);
			if(mysql_num_rows(DescRes))
			{
				DescRow=mysql_fetch_row(DescRes);
				fprintf(stderr,"%s - %s\n",argv[0],DescRow[0]);
			}
			fprintf(stderr,"\nusage  : %s",argv[0]);
			Row=mysql_fetch_row(Result);
			while(Row=mysql_fetch_row(Result))
				fprintf(stderr," %s",Row[0]);
			fprintf(stderr,"\n");
			sprintf(query,"select * from Apps.%s_Logs",argv[0]);
			fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
			Result=mysql_store_result(mysql);
			if(mysql_num_rows(Result))
			{
				Row=mysql_fetch_row(Result);
				fprintf(stderr,"example: %s",argv[0]);
				for(i=2;i<mysql_num_fields(Result);i++)
					fprintf(stderr," %s",Row[i]);
				fprintf(stderr,"\n");
			}
			fprintf(stderr,"\nType 'man %s' for more information.\n",argv[0]);
			exit(1);
		}
	}
}

void mysql_add_call_log(MYSQL *mysql,char *CallID,int argc,char **argv)
{
	char query[1024];
	int i;
	MYSQL_RES *Res;
	MYSQL_ROW *Row;

	if(!strlen(CallID))
	{
		fprintf(stderr,"Enter New Call ID: ");
		for(i=0;CallID[i-1]!='\n';i++)
			CallID[i]=getchar();
		CallID[i-1]='\0';
	}
	sprintf(query,"select CONCAT(curtime(),\" \",curdate())");
	fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
	Res=mysql_store_result(mysql);
	Row=mysql_fetch_row(Res);
	fprintf(stderr,"Generating Log at %s\n",Row[0]);
	if(strlen(CallID))
	{
		sprintf(query,"delete from Apps.%s_Calls where CallID='%s'",argv[0],CallID);
		fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
		sprintf(query,"insert into Apps.%s_Calls values('%s','%s",argv[0],CallID,argv[1]);
		for(i=2;i<argc;i++)
			strcat(strcat(query,"','"),argv[i]);
		strcat(query,"')");
		fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
		sprintf(query,"insert into Apps.%s_Logs values(CONCAT(curtime(),\" \",curdate()),'%s",argv[0],CallID);
	}
	else
		sprintf(query,"insert into Apps.%s_Logs values(CONCAT(curtime(),\" \",curdate()),'%s",argv[0],"");
	for(i=1;i<argc;i++)
		strcat(strcat(query,"','"),argv[i]);
	strcat(query,"')");
	fprintf(stderr,mysql_query(mysql,query)?strcat(strcat(query,"\n"),strcat(mysql_error(mysql),"\n\n")):"");
}

