/*************************************************************************
	> File Name: smsServer.h
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月08日 星期日 22时20分08秒
 ************************************************************************/
#ifndef SMS_SERVER_H
#define SMS_SERVER_H

#include "./smsPublic.h"

struct smsOLServerCTL
{
	int datafd;
	sqlite3 *pdb;
};

/*createSqlTables*/
int my_sqlite3_exec(sqlite3 *pdb,const char *sqlstr,int my_callback(void *para,int ccnt,char **cval,char **cname),void *arg,char ** errmsg);
int my_sqlite3_get_table(sqlite3 *pdb,const char *zSql,char ***pazResult,int *pRow,int *pColumn,char ** errmsg);
sqlite3 *createSqlite3Engine(const char *filename);
int myCallback(void *para,int ccnt,char **cval,char **cname); 

/*handleClient*/
void *handleClient(void *arg);
int handleStaffLogin(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu);
int handleModifyPassword(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu,char *username);
int handlePrintStaffInfo(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu);
int handleStaffLogout(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu);

/*operationFunctions*/
int insertNewUser(sqlite3 *pdb,char *username,char *password);
int addNewStaffInfo(sqlite3 *pdb);
int deleteStaffByName(sqlite3 *pdb,char *staffname);
int modifyUserPassword(sqlite3 *pdb,char *username,char *password);
int modifyStaffInfo(sqlite3 *pdb,char *staffname);
int printUserTable(sqlite3 *pdb);
int printStaffInfoTable(sqlite3 *pdb);
struct acceptStaffInfo *printStaffInfoTableByName(sqlite3 *pdb,char *staffname);
int printHistoryTableByName(sqlite3 *pdb,char *staffname);
int insertHistoryRecord(sqlite3 *pdb,char *staffname,char *operation);
int checkLogin(sqlite3 *pdb,char *username,char *password);
int updateUserFlag(sqlite3 *pdb,char *username);

/*serMain*/
int createServerSocket(const char *ip,unsigned short port);
int mainLoop(int servfd,sqlite3 *pdb);
void *serverMainUILoop(void *arg);
int displayServerMainUI();
int checkRootLogin(sqlite3 *pdb);
void serverFunctionLoop(sqlite3 *pdb);
int displayServerFunctionUI();

#endif

