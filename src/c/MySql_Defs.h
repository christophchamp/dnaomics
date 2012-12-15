/*
NAME: MySql_Defs.h
AUTHORS:
    - Sandor Maurice <sandormaurice@gmail.com>
    - P. Christoph Champ <christoph.champ@gmail.com>
    (C) Copyright 2002. All rights reserved.
DATE: 2002-05-24
LAST UPDATE: 2008-01-14
DESCRIPTION: MySQL definitions header file
*/

#ifndef _MYSQL_DEFS_H_
#define _MYSQL_DEFS_H_

#include <mysql/mysql.h>

void mysql_apps_logon(MYSQL *mysql);
void mysql_su_logon(MYSQL *mysql,char *user);
void mysql_parse_input(MYSQL *mysql,char *CallID,int *argc,char **argv);
void mysql_add_call_log(MYSQL *mysql,char *CallID,int argc,char **argv);

#endif

