/*************************************************************************
	> File Name: smsProtocal.c
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月07日 星期六 22时12分19秒
 ************************************************************************/
#include "../include/smsPublic.h"

/*struct smsOLPDU *createStaffLoginREQPDU(char *name,char *password);
struct smsOLPDU *createStaffLoginRSPPDU(int value);
struct smsOLPDU *createModifyPasswordREQPDU(char *name,char *password);
struct smsOLPDU *createModifyPasswordRSPPDU(int value);
struct smsOLPDU *createQueryStaffOwnInfoREQPDU(char *staffname);
struct smsOLPDU *createQueryStaffOwnInfoRSPPDU(char *staffInfo);
struct smsOLPDU *createStaffLogoutREQPDU(char *staffname);
struct smsOLPDU *createStaffLogoutRSPPDU(int value);
struct smsOLPDU *recvPDU(int fd);
int sendPDU(int fd,struct smsOLPDU *pdu);
int destroysmsOLPDU(struct smsOLPDU *pdu);*/

/*发送登入请求*/
struct smsOLPDU *createStaffLoginREQPDU(char *name,char *password)
{
	int len = 0;
	struct smsOLPDU *pdu = NULL;
	
	len = SMS_PDU_HEADER_LENGTH + NAME_LENGTH + PASSWD_LENGTH;
	pdu = (struct smsOLPDU *)malloc(len);
	if(NULL == pdu){
		PrintPublicError("malloc failed\n");
		return NULL;
	}

	memset(pdu,0,len);
	pdu->len = len;
	pdu->type = SMS_PDU_TYPE_LOGIN_REQ;
	strncpy(pdu->buf,name,NAME_LENGTH);
	strncat(pdu->buf+NAME_LENGTH,password,PASSWD_LENGTH);

	return pdu;
}

/*回复登入请求*/
struct smsOLPDU *createStaffLoginRSPPDU(int value)
{
	int len = 0;
	struct smsOLPDU *pdu = NULL;
	
	len = SMS_PDU_HEADER_LENGTH + sizeof(int);
	pdu = (struct smsOLPDU *)malloc(len);
	if(NULL == pdu){
		PrintPublicError("malloc failed\n");
		return NULL;
	}

	memset(pdu,0,len);
	pdu->len = len;
	pdu->type = SMS_PDU_TYPE_LOGIN_RSP;
	*(int *)pdu->buf = value;

	return pdu;
}

/*发送更改密码请求*/
struct smsOLPDU *createModifyPasswordREQPDU(char *name,char *password)
{
	int len = 0;
	struct smsOLPDU *pdu = NULL;
	
	len = SMS_PDU_HEADER_LENGTH + PASSWD_LENGTH;
	pdu = (struct smsOLPDU *)malloc(len);
	if(NULL == pdu){
		PrintPublicError("malloc failed\n");
		return NULL;
	}

	memset(pdu,0,len);
	pdu->len = len;
	pdu->type = SMS_PDU_TYPE_MODIFY_PASSWD_REQ;
	strncpy(pdu->buf,password,PASSWD_LENGTH);

	return pdu;
}

/*回复更改密码请求*/
struct smsOLPDU *createModifyPasswordRSPPDU(int value)
{
	int len = 0;
	struct smsOLPDU *pdu = NULL;
	
	len = SMS_PDU_HEADER_LENGTH + sizeof(int);
	pdu = (struct smsOLPDU *)malloc(len);
	if(NULL == pdu){
		PrintPublicError("malloc failed\n");
		return NULL;
	}

	memset(pdu,0,len);
	pdu->len = len;
	pdu->type = SMS_PDU_TYPE_MODIFY_PASSWD_RSP;
	*(int *)pdu->buf = value;

	return pdu;
}

/*发送查询员工自身信息请求*/
struct smsOLPDU *createQueryStaffOwnInfoREQPDU(char *staffname)
{
	int len = 0;
	struct smsOLPDU *pdu = NULL;

	len = SMS_PDU_HEADER_LENGTH + NAME_LENGTH;
	pdu = (struct smsOLPDU *)malloc(len);
	if(NULL == pdu){
		PrintPublicError("malloc failed\n");
		return NULL;
	}

	memset(pdu,0,len);
	pdu->len = len;
	pdu->type = SMS_PDU_TYPE_QUERY_STAFFINFO_REQ;
	strncpy(pdu->buf,staffname,NAME_LENGTH);

	return pdu;
}

/*回复查看员工自身信息*/
struct smsOLPDU *createQueryStaffOwnInfoRSPPDU(char *staffInfo)
{
	int len = 0;
	struct smsOLPDU *pdu = NULL;
	
	len = SMS_PDU_HEADER_LENGTH + strlen(staffInfo) + 1;
	pdu = (struct smsOLPDU *)malloc(len);
	if(NULL == pdu){
		PrintPublicError("malloc failed\n");
		return NULL;
	}

	memset(pdu,0,len);
	pdu->len = len;
	pdu->type = SMS_PDU_TYPE_QUERY_STAFFINFO_RSP;
	strncpy(pdu->buf,staffInfo,strlen(staffInfo)+1);

	return pdu;
}

/*staff发送退出通知*/
struct smsOLPDU *createStaffLogoutREQPDU(char *staffname)
{
	int len = 0;
	struct smsOLPDU *pdu = NULL;

	len = SMS_PDU_HEADER_LENGTH + NAME_LENGTH;
	pdu = (struct smsOLPDU *)malloc(len);
	if(NULL == pdu){
		PrintPublicError("malloc failed\n");
		return NULL;
	}

	memset(pdu,0,len);
	pdu->len = len;
	pdu->type = SMS_PDU_TYPE_LOGOUT_REQ;
	strncpy(pdu->buf,staffname,NAME_LENGTH);

	return pdu;
}

/*回复staff退出*/
struct smsOLPDU *createStaffLogoutRSPPDU(int value)
{
	int len = 0;
	struct smsOLPDU *pdu = NULL;
	
	len = SMS_PDU_HEADER_LENGTH + sizeof(int);
	pdu = (struct smsOLPDU *)malloc(len);
	if(NULL == pdu){
		PrintPublicError("malloc failed\n");
		return NULL;
	}

	memset(pdu,0,len);
	pdu->len = len;
	pdu->type = SMS_PDU_TYPE_LOGOUT_RSP;
	*(int *)pdu->buf = value;

	return pdu;
}

/*接收数据*/
struct smsOLPDU *recvPDU(int fd)
{
	int ret = 0;
	int len = 0;
	struct smsOLPDU *pdu = NULL;

	if(fd < 0){
		PrintPublicError("Input param is invalid");
		return NULL;
	}
//	PrintPublicError("===========recvPDU1==============\n");

	ret = read(fd,&len,sizeof(int));
	if(ret !=sizeof(int) || len <= 0){
//		PrintPublicError("recv len failed\n");
		return NULL;
	}
//	PrintPublicError("===========recvPDU2==============\n");

	pdu = (struct smsOLPDU *)malloc(len);
	if(NULL == pdu){
		PrintPublicError("malloc failed\n");
		return NULL;
	}

	memset(pdu,0,len);
	pdu->len = len;
	ret = read(fd,&pdu->type,len-sizeof(int));
	if(ret != len-sizeof(int)){
		PrintPublicError("recvive failed\n");
		free(pdu);
		pdu = NULL;
		return NULL;
	}

	return pdu;
}

/*发送数据*/
int sendPDU(int fd,struct smsOLPDU *pdu)
{
	int ret = 0;
	
	ret = write(fd,pdu,pdu->len);
	if(ret != pdu->len){
		PrintPublicError("send data failed\n");
		return -1;
	}
	return 0;
}

/*释放struct smsOLPDU *pdu*/
int destroysmsOLPDU(struct smsOLPDU *pdu)
{
	if(pdu != NULL){
		free(pdu);
		pdu = NULL;
	}
	return 0;
}
