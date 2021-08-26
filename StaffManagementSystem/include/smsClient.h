/*************************************************************************
	> File Name: smsClient.h
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月08日 星期日 22时19分56秒
 ************************************************************************/
#ifndef SMS_CLIENT_H
#define SMS_CLIENT_H

#include "./smsPublic.h"

struct smsOLClientCTL
{
	int sockfd;
	char clientStaffName[NAME_LENGTH];
};

/*handleServer*/
int staffLogin(struct smsOLClientCTL *pstClient);
char *inpurNewPassword();
int modifyStaffPasswordReq(struct smsOLClientCTL *pstClient);
struct acceptStaffInfo *queryStaffOwnInfo(struct smsOLClientCTL *pstClient);
int staffLogout(struct smsOLClientCTL *pstClient);

/*cliMain*/
void initsmsOLClientCTL(struct smsOLClientCTL *pstClient);
void endsmsOLClientCTL(struct smsOLClientCTL *pstClient);
int connectServer(struct smsOLClientCTL *pstClient,const char *ip,unsigned short port);
void mainUILoop(struct smsOLClientCTL *pstClient);
int displayMainUI();
int functionUILoop(struct smsOLClientCTL *pstClient);
int displayFunctionUI();

#endif
