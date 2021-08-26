/*************************************************************************
	> File Name: createSql.c
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月06日 星期五 11时50分57秒
 ************************************************************************/
#include "../include/smsServer.h"

/*
int my_sqlite3_exec(sqlite3 *pdb,const char *sqlstr,int myCallback(void *para,int ccnt,char **cval,char **cname),void *arg,char ** errmsg);
int my_sqlite3_get_table(sqlite3 *pdb,const char *zSql,char ***pazResult,int *pRow,int *pColumn,char ** errmsg);
sqlite3 *createSqlite3Engine(const char *filename);
int myCallback(void *para,int ccnt,char **cval,char **cname);
*/

/*重新封装sqlite3_exec*/
int my_sqlite3_exec(sqlite3 *pdb,const char *sqlstr,int myCallback(void *para,int ccnt,char **cval,char **cname),void *arg,char ** errmsg)
{
	int ret = 0;

	do{
		ret = sqlite3_exec(pdb,sqlstr,myCallback,arg,errmsg);
		if(ret == SQLITE_BUSY || ret == SQLITE_LOCKED){
			usleep(30 * 1000);
			continue;
		}else{
			break;
		}
	}while(1);

	return ret;
}

/*重新封装sqlite3_get_table*/
int my_sqlite3_get_table(sqlite3 *pdb,const char *zSql,char ***pazResult,int *pRow,int *pColumn,char ** errmsg)
{
	int ret = 0;

	do{
		ret = sqlite3_get_table(pdb,zSql,pazResult,pRow,pColumn,errmsg);
		if(ret == SQLITE_BUSY || ret == SQLITE_LOCKED){
			usleep(30 * 1000);
			continue;
		}else{
			break;
		}
	}while(1);
	
	return ret;
}

/*创建并打开数据库*/
sqlite3 *createSqlite3Engine(const char *filename)
{
	int ret = 0;
	sqlite3 *psql = NULL;
	char sqlstr[1024] = "";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;
	int flag = 0;

	/*创建并打开指定数据库*/
	ret = sqlite3_open(filename,&psql);
	if(ret != SQLITE_OK){
		PrintPublicError("open %s db failed:%s\n",filename,sqlite3_errmsg(psql));
		return NULL;
	}

	/*判断user表是否存在*/
	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM sqlite_master WHERE name = '%s'","user");
	ret = my_sqlite3_get_table(psql,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintPublicError("sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
//		sqlite3_close(psql);
//		psql = NULL;
		return NULL;
	}

	sscanf(*(ppret+1),"%d",&flag);
	sqlite3_free_table(ppret);
	ppret = NULL;

	/*user表不存在就创建表*/
	if(0 == flag){
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"CREATE TABLE user(Name TEXT(%d) PRIMARY KEY NOT NULL,Password TEXT(%d),Flag INTEGER)",NAME_LENGTH,PASSWD_LENGTH);
		ret = my_sqlite3_exec(psql,sqlstr,NULL,NULL,&perr);
		if(ret != SQLITE_OK){
			PrintPublicError("sqlite3_exec %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
//			sqlite3_close(psql);
//			psql = NULL;
			return NULL;
		}
	}

	/*判断staffInfo表是否存在*/
	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM sqlite_master WHERE name = '%s'","staffInfo");
	ret = my_sqlite3_get_table(psql,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintPublicError("sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
//		sqlite3_close(psql);
//		psql = NULL;
		return NULL;
	}

	sscanf(*(ppret+1),"%d",&flag);
	sqlite3_free_table(ppret);
	ppret = NULL;

	/*staffInfo表不存在就创建表*/
	if(0 == flag){
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"CREATE TABLE staffInfo(Name TEXT(%d) PRIMARY KEY NOT NULL,Address TEXT(%d),Level INTEGER,Age INTEGER,ID INTEGER,Salary REAL,Phone TEXT(%d),Email TEXT(%d))",
				NAME_LENGTH,ADDRESS_LENGTH,PHONE_LENGTH,EMAIL_LENGTH);
		ret = my_sqlite3_exec(psql,sqlstr,NULL,NULL,&perr);
		if(ret != SQLITE_OK){
			PrintPublicError("sqlite3_exec %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
//			sqlite3_close(psql);
//			psql = NULL;
			return NULL;
		}
	}

	/*判断history表是否存在*/
	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM sqlite_master WHERE name = '%s'","history");
	ret = my_sqlite3_get_table(psql,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintPublicError("sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		sqlite3_close(psql);
		psql = NULL;
		return NULL;
	}

	sscanf(*(ppret+1),"%d",&flag);
	sqlite3_free_table(ppret);
	ppret = NULL;

	/*history表不存在就创建表*/
	if(0 == flag){
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"CREATE TABLE history(Name TEXT(%d),Operation TEXT(%d),Datatime TEXT(%d))",NAME_LENGTH,OPERATION_LENGTH,DATATIME_LENGTH);
		ret = my_sqlite3_exec(psql,sqlstr,NULL,NULL,&perr);
		if(ret != SQLITE_OK){
			PrintPublicError("sqlite3_exec %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
//			sqlite3_close(psql);
//			psql = NULL;
			return NULL;
		}
	}

	return psql;
}

int myCallback(void *para,int ccnt,char **cval,char **cname)
{
	int i = 0;
	static int flag = 1;

	if(flag){
		for(i=0;i<ccnt;i++){
			printf("%s ",*(cname + i));
		}
		printf("\n");
		flag = 0;
	}

	for(i=0;i<ccnt;i++){
		printf("%s ",*(cval + i));
	}
	printf("\n");

	return 0;
}

/*test create sqlite3 tables*/
/*
int main(){
	sqlite3 *psql = NULL;
	psql = createSqlite3Engine("../../sqlite3/sms.db");
	if(NULL == psql){
		return 1;
	}
	sqlite3_close(psql);
	psql = NULL;

	return 0;
}
*/
