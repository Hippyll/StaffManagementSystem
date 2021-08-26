/*************************************************************************
	> File Name: handleServer.c
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月08日 星期日 16时20分53秒
 ************************************************************************/
#include "../include/smsClient.h"

/*
int staffLogin(struct smsOLClientCTL *pstClient);
int modifyStaffPasswordReq(struct smsOLClientCTL *pstClient);
char *inpurNewPassword();
struct acceptStaffInfo *queryStaffOwnInfo(struct smsOLClientCTL *pstClient);
int staffLogout(struct smsOLClientCTL *pstClient);
*/

/*staff登入*/
int staffLogin(struct smsOLClientCTL *pstClient)
{
	int ret = 0;
	char password[PASSWD_LENGTH] = "";
	struct smsOLPDU *preq = NULL;
	struct smsOLPDU *prsp = NULL;

	printf("Please input your name:");
	myGetString(pstClient->clientStaffName,NAME_LENGTH);
	printf("Please input your password:");
	myGetString(password,PASSWD_LENGTH);

	preq = createStaffLoginREQPDU(pstClient->clientStaffName,password);
	if(NULL == preq){
		PrintClientError("createStaffLoginREQPDU failed\n");
		return -1;
	}
	ret = sendPDU(pstClient->sockfd,preq);
	if(ret != 0){
		PrintClientError("sendPDU failed\n");
		return -1;
	}
	destroysmsOLPDU(preq);

	prsp = recvPDU(pstClient->sockfd);
	if(NULL == prsp){
		PrintClientError("recvPDU failed\n");
		return -1;
	}
	if(prsp->type == SMS_PDU_TYPE_LOGIN_RSP){
		switch(*(int *)prsp->buf){
			case 0:
				printf("Login success!\n");
				break;
			case 1:
				printf("Repeat login!\n");
				return -1;
			case 2:
				printf("Password error!\n");
				return -2;
			case 3:
				printf("Invalid username!\n");
				return -3;
			default:
				break;
		}
	}
	destroysmsOLPDU(prsp);
	return 0;
}

/*输入新密码*/
char *inpurNewPassword()
{
	int ret = 0;
	static char password1[PASSWD_LENGTH] = "";
	char password2[PASSWD_LENGTH] = "";
	char password3[PASSWD_LENGTH] = "";

reset_password:
	printf("Please input the new password:");
	myGetString(password1,PASSWD_LENGTH);
	printf("Please input the new password again:");
	myGetString(password2,PASSWD_LENGTH);
	ret = strncmp(password1,password2,PASSWD_LENGTH);
	if(ret != 0){
		printf("The passwords you typed do not match,please try again!\n");
		goto reset_password;
	}
	printf("Please input the new password for the third time:");
	myGetString(password3,PASSWD_LENGTH);
	ret = strncmp(password2,password3,PASSWD_LENGTH);
	if(ret != 0){
		printf("The passwords you typed do not match,please try again!\n");
		goto reset_password;
	}

	return password1;
}

/*修改密码*/
int modifyStaffPasswordReq(struct smsOLClientCTL *pstClient)
{
	int ret = 0;
	char password[PASSWD_LENGTH] = "";
	struct smsOLPDU *preq = NULL;
	struct smsOLPDU *prsp = NULL;

	strncpy(password,inpurNewPassword(),PASSWD_LENGTH);
	preq = createModifyPasswordREQPDU(pstClient->clientStaffName,password);
	if(NULL == preq){
		PrintClientError("createModifyPasswordREQPDU failed\n");
		return -1;
	}
	ret = sendPDU(pstClient->sockfd,preq);
	if(ret != 0){
		PrintClientError("sendPDU failed\n");
		return -1;
	}
	destroysmsOLPDU(preq);

	prsp = recvPDU(pstClient->sockfd);
	if(NULL == prsp){
		PrintClientError("recvPDU failed\n");
		return -1;
	}
	if(prsp->type == SMS_PDU_TYPE_MODIFY_PASSWD_RSP){
		switch(*(int *)prsp->buf){
			case 0:
				printf("Modify password success!\n");
				break;
			case -1:
				printf("Modify password failed!\n");
				break;
			default:
				break;
		}
	}
	destroysmsOLPDU(prsp);
	return 0;
}

/*查看自身信息*/
struct acceptStaffInfo *queryStaffOwnInfo(struct smsOLClientCTL *pstClient)
{
	int ret = 0;
	struct smsOLPDU *preq = NULL;
	struct smsOLPDU *prsp = NULL;
	struct acceptStaffInfo *apt = NULL;
	apt = (struct acceptStaffInfo *)malloc(sizeof(struct acceptStaffInfo));
	if(NULL == apt){
		PrintClientError("apt malloc failed\n");
		return NULL;
	}
	memset(apt,0,sizeof(struct acceptStaffInfo));

	preq = createQueryStaffOwnInfoREQPDU(pstClient->clientStaffName);
	if(NULL == preq){
		PrintClientError("createQueryStaffOwnInfoREQPDU failed\n");
		return NULL;
	}
	ret = sendPDU(pstClient->sockfd,preq);
	if(ret != 0){
		PrintClientError("sendPDU failed\n");
		return NULL;
	}
	destroysmsOLPDU(preq);

	prsp = recvPDU(pstClient->sockfd);
	if(NULL == prsp){
		PrintClientError("recvPDU failed\n");
		return NULL;
	}
	if(prsp->type == SMS_PDU_TYPE_QUERY_STAFFINFO_RSP){
		char *strtmp = NULL;
		strtmp = strtok(prsp->buf," ");
		strcpy(apt->name,strtmp);
		strtmp = strtok(NULL," ");
		strcpy(apt->address,strtmp);
		strtmp = strtok(NULL," ");
		strcpy(apt->level,strtmp);
		strtmp = strtok(NULL," ");
		strcpy(apt->age,strtmp);
		strtmp = strtok(NULL," ");
		strcpy(apt->id,strtmp);
		strtmp = strtok(NULL," ");
		strcpy(apt->salary,strtmp);
		strtmp = strtok(NULL," ");
		strcpy(apt->phone,strtmp);
		strtmp = strtok(NULL," ");
		strcpy(apt->email,strtmp);
	}else{
		PrintClientError("Query history records failed\n");
		return NULL;
	}
	destroysmsOLPDU(prsp);
	return apt;
}

/*staff登出*/
int staffLogout(struct smsOLClientCTL *pstClient)
{
	int ret = 0;
	struct smsOLPDU *preq = NULL;
	struct smsOLPDU *prsp = NULL;

	preq = createStaffLogoutREQPDU(pstClient->clientStaffName);
	if(NULL == preq){
		PrintClientError("createStaffLogoutREQPDU failed\n");
		return -1;
	}
	ret = sendPDU(pstClient->sockfd,preq);
	if(ret != 0){
		PrintClientError("sendPDU failed\n");
		return -1;
	}
	destroysmsOLPDU(preq);

	prsp = recvPDU(pstClient->sockfd);
	if(NULL == prsp){
		PrintClientError("recvPDU failed\n");
		return -1;
	}
	if(prsp->type == SMS_PDU_TYPE_LOGOUT_RSP){
		switch(*(int *)prsp->buf){
			case 0:
//				printf("Logout success!\n");
				break;
			case -1:
//				printf("Logout failed!\n");
				destroysmsOLPDU(prsp);
				return -1;
			default:
				break;
		}
	}
	destroysmsOLPDU(prsp);
	return 0;
}

