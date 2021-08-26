/*************************************************************************
	> File Name: smsPublic.h
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月06日 星期五 11时02分35秒
 ************************************************************************/
#ifndef SMS_PUBLIC_H
#define SMS_PUBLIC_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "./smsDebug.h"

#define NAME_LENGTH 20
#define PASSWD_LENGTH 20
#define ADDRESS_LENGTH 20
#define PHONE_LENGTH 12
#define EMAIL_LENGTH 20
#define DATATIME_LENGTH 20
#define NORMAL_LENGTH 20
#define OPERATION_LENGTH 20
#define TOTAL_LENGTH 160
#define SMS_PDU_HEADER_LENGTH (sizeof(int)*2)

#define ROOT "admin"
#define ROOT_PASSWD "admin666"
#define STAFF_INITIAL_PASSWD "123456"

/*员工信息结构体*/
struct staffInfo{
	char name[NAME_LENGTH];
	char address[ADDRESS_LENGTH];
	int age;
	int level;
	int id;
	float salary;
	char phone[PHONE_LENGTH];
	char email[EMAIL_LENGTH];
};

struct acceptStaffInfo{
	char name[NORMAL_LENGTH];
	char address[NORMAL_LENGTH];
	char age[NORMAL_LENGTH];
	char level[NORMAL_LENGTH];
	char id[NORMAL_LENGTH];
	char salary[NORMAL_LENGTH];
	char phone[NORMAL_LENGTH];
	char email[NORMAL_LENGTH];
};

/*PDU数据对应的枚举常量*/
enum ENUM_SMS_PDU_TYPE
{
	SMS_PDU_TYPE_LOGIN_REQ = 1,
	SMS_PDU_TYPE_LOGIN_RSP,
	SMS_PDU_TYPE_MODIFY_PASSWD_REQ,
	SMS_PDU_TYPE_MODIFY_PASSWD_RSP,
	SMS_PDU_TYPE_QUERY_STAFFINFO_REQ,
	SMS_PDU_TYPE_QUERY_STAFFINFO_RSP,
	SMS_PDU_TYPE_LOGOUT_REQ,
	SMS_PDU_TYPE_LOGOUT_RSP,
};

/*员工信息管理系统协议结构体*/
struct smsOLPDU
{
	int len;
	int type;
	char buf[1];
};

/*myGetString*/
char *myGetString(char *pstrbuf,int size);

/*smsProtocol*/
struct smsOLPDU *createStaffLoginREQPDU(char *name,char *password);
struct smsOLPDU *createStaffLoginRSPPDU(int value);
struct smsOLPDU *createModifyPasswordREQPDU(char *name,char *password);
struct smsOLPDU *createModifyPasswordRSPPDU(int value);
struct smsOLPDU *createQueryStaffOwnInfoREQPDU(char *staffname);
struct smsOLPDU *createQueryStaffOwnInfoRSPPDU(char *staffInfo);
struct smsOLPDU *createStaffLogoutREQPDU(char *staffname);
struct smsOLPDU *createStaffLogoutRSPPDU(int value); 
struct smsOLPDU *recvPDU(int fd);
int sendPDU(int fd,struct smsOLPDU *pdu);
int destroysmsOLPDU(struct smsOLPDU *pdu);

#endif
