/*************************************************************************
	> File Name: handleClient.c
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月08日 星期日 11时43分38秒
 ************************************************************************/
#include "../include/smsServer.h"

/*
void *handleClient(void *arg);
int handleStaffLogin(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu);
int handleModifyPassword(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu,char *username);
int handlePrintStaffInfo(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu);
int handleStaffLogout(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu);
*/

/*处理客户端请求*/
void *handleClient(void *arg)
{
	int ret = 0;
	struct smsOLServerCTL PstServer = *(struct smsOLServerCTL *)arg;
	struct smsOLServerCTL *pstServer = &PstServer;
	int isLogin = 0;
	char username[NAME_LENGTH] = "";
	struct smsOLPDU *pdu = NULL;

	pthread_detach(pthread_self());
	while(1){
		pdu = recvPDU(pstServer->datafd);
		if(NULL == pdu){
			break;
		}
		strncpy(username,pdu->buf,NAME_LENGTH);
		//未登录
		if(0 == isLogin){
			switch(pdu->type){
			case SMS_PDU_TYPE_LOGIN_REQ:
				ret = handleStaffLogin(pstServer,pdu);
				if(0 == ret){
					isLogin = 1;
					pdu = NULL;
				}
				break;
			default:
				destroysmsOLPDU(pdu);
				pdu = NULL;
				break;
			}
		}
		//已登录
		if(1 == isLogin){
			while(1){
				pdu = recvPDU(pstServer->datafd);
				if(NULL == pdu){
					break;
				}
				switch(pdu->type){
				case SMS_PDU_TYPE_MODIFY_PASSWD_REQ:
					handleModifyPassword(pstServer,pdu,username);
					break;
				case SMS_PDU_TYPE_QUERY_STAFFINFO_REQ:
					handlePrintStaffInfo(pstServer,pdu);
					break;
				case SMS_PDU_TYPE_LOGOUT_REQ:
					handleStaffLogout(pstServer,pdu);
					isLogin = 0;
					break;
				default:
					destroysmsOLPDU(pdu);
					pdu = NULL;
					break;
				}
				if(0 == isLogin){
					break;
				}
			}
		}
	}
	close(pstServer->datafd);
	pstServer->datafd = -1;
	return NULL;
}

/*处理员工登入*/
int handleStaffLogin(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu)
{
	int ret = 0;
	int value = 0;
	char username[NAME_LENGTH] = "";
	char password[PASSWD_LENGTH] = "";
	struct smsOLPDU *pret = NULL;

	strncpy(username,pdu->buf,NAME_LENGTH);
	strncpy(password,pdu->buf+NAME_LENGTH,PASSWD_LENGTH);

	destroysmsOLPDU(pdu);
	pdu = NULL;
	ret = checkLogin(pstServer->pdb,username,password);

	pret = createStaffLoginRSPPDU(ret);
	value = sendPDU(pstServer->datafd,pret);
	if(-1 == value){
		PrintServerError("sendPDU failed\n");
		return -1;
	}
	destroysmsOLPDU(pret);
	pret = NULL;
	return ret;
}

/*处理修改密码请求*/
int handleModifyPassword(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu,char *username)
{
	int ret = 0;
	char password[PASSWD_LENGTH] = "";
	struct smsOLPDU *pret = NULL;

	strncpy(password,pdu->buf,PASSWD_LENGTH);
	ret = modifyUserPassword(pstServer->pdb,username,password);
	if(ret != 0){
		PrintServerError("handleChangePassword failed\n");
		return -1;
	}

	destroysmsOLPDU(pdu);
	pdu = NULL;
	pret = createModifyPasswordRSPPDU(ret);
	sendPDU(pstServer->datafd,pret);
	destroysmsOLPDU(pret);
	pret = NULL;

	return 0;
}

/*处理打印员工自身信息*/
int handlePrintStaffInfo(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu)
{
	char staffname[NAME_LENGTH] = "";
	struct smsOLPDU *pret = NULL;
	struct acceptStaffInfo *apt = NULL;
	char staffInfo[TOTAL_LENGTH] = "";

	strncpy(staffname,pdu->buf,NAME_LENGTH);
	
	apt = printStaffInfoTableByName(pstServer->pdb,staffname);

	strcpy(staffInfo,apt->name);
	strcat(staffInfo," ");
	strcat(staffInfo,apt->address);
	strcat(staffInfo," ");
	strcat(staffInfo,apt->level);
	strcat(staffInfo," ");
	strcat(staffInfo,apt->age);
	strcat(staffInfo," ");
	strcat(staffInfo,apt->id);
	strcat(staffInfo," ");
	strcat(staffInfo,apt->salary);
	strcat(staffInfo," ");
	strcat(staffInfo,apt->phone);
	strcat(staffInfo," ");
	strcat(staffInfo,apt->email);
	strcat(staffInfo," ");
	strcat(staffInfo," ");

	free(apt);
	apt = NULL;
	destroysmsOLPDU(pdu);
	pdu = NULL;
	pret = createQueryStaffOwnInfoRSPPDU(staffInfo);
	sendPDU(pstServer->datafd,pret);
	destroysmsOLPDU(pret);
	pret = NULL;

	return 0;
}

/*处理员工退出*/
int handleStaffLogout(struct smsOLServerCTL *pstServer,struct smsOLPDU *pdu)
{
	int ret = 0;
	char username[NAME_LENGTH] = "";
	char password[PASSWD_LENGTH] = "";
	struct smsOLPDU *pret = NULL;

	strncpy(username,pdu->buf,NAME_LENGTH);
	strncpy(password,pdu->buf+NAME_LENGTH,PASSWD_LENGTH);

	ret = updateUserFlag(pstServer->pdb,username);
	if(ret != 0){
		PrintServerError("updateUserFlag failed\n");
		return -1;
	}

	destroysmsOLPDU(pdu);
	pdu = NULL;
	pret = createStaffLoginRSPPDU(ret);
	sendPDU(pstServer->datafd,pret);
	destroysmsOLPDU(pret);
	pret = NULL;

	return 0;
}

