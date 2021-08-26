/*************************************************************************
	> File Name: cliMain.c
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月06日 星期五 11时01分24秒
 ************************************************************************/
#include "../include/smsClient.h"

/*
void initsmsOLClientCTL(struct smsOLClientCTL *pstClient);
void endsmsOLClientCTL(struct smsOLClientCTL *pstClient);
int connectServer(struct smsOLClientCTL *pstClient,const char *ip,unsigned short port);
void mainUILoop(struct smsOLClientCTL *pstClient);
int displayMainUI();
int functionUILoop(struct smsOLClientCTL *pstClient);
int displayFunctionUI();
*/
static void printOwnInfo(struct acceptStaffInfo *apt);

int main(int argc,char *argv[]){
	int ret = 0;
	unsigned short port = -1;
	struct smsOLClientCTL pstClient;

	if(argc < 3){
		PrintClientInfo("the arguements is too few\n");
		return -1;
	}

	sscanf(argv[2],"%hd",&port);
	if(port < 0 || port > 0xffff){
		PrintClientWarning("the port is invalid\n");
		return -1;
	}

	initsmsOLClientCTL(&pstClient);
	ret = connectServer(&pstClient,argv[1],port);
	if(0 == ret){
		mainUILoop(&pstClient);
	}else{
		PrintClientError("connectServer failed\n");
		endsmsOLClientCTL(&pstClient);
		return -1;
	}
	endsmsOLClientCTL(&pstClient);
	return 0;
}

/*初始化smsOLClientCTL*/
void initsmsOLClientCTL(struct smsOLClientCTL *pstClient)
{
	pstClient->sockfd = -1;
	memset(pstClient->clientStaffName,0,NAME_LENGTH);
	return;
}

/*关闭sockfd*/
void endsmsOLClientCTL(struct smsOLClientCTL *pstClient)
{
	close(pstClient->sockfd);
	pstClient->sockfd = -1;
	return;
}

/*向服务器发送连接请求*/
int connectServer(struct smsOLClientCTL *pstClient,const char *ip,unsigned short port)
{
	int ret = 0;
	struct sockaddr_in servaddr;

	pstClient->sockfd = socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_aton(ip,&servaddr.sin_addr);

	ret = connect(pstClient->sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	if(ret){
		PrintClientError("connect server failed\n");
		endsmsOLClientCTL(pstClient);
		return -1;
	}

	return 0;
}

/*Client主UI界面*/
void mainUILoop(struct smsOLClientCTL *pstClient)
{
	int op = -1;
	int ret = -1;
	int exitFlag = -1;

	while(1){
		op = displayMainUI();
		switch(op){
			case 1:
				ret = staffLogin(pstClient);
				if(0 == ret){
					functionUILoop(pstClient);
				}
				break;
			case 0:
				exitFlag = 1;
				break;
		}
		if(1 == exitFlag){
			break;
		}
	}
	return;
}

/*主UI界面显示*/
int displayMainUI()
{
	int cmd = -1;
	char buf[NORMAL_LENGTH] = "";
	do{
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 1.Login                  @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 0.Exit                   @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ Please input your choice @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		myGetString(buf,NORMAL_LENGTH);
		sscanf(buf,"%d",&cmd);
		if(cmd < 0 || cmd > 1){
			printf("Your input is invalid,please redo it!\n");
			continue;
		}else{
			break;
		}
	}while(1);

	return cmd;
}

int functionUILoop(struct smsOLClientCTL *pstClient)
{
	int ret = 0;
	int op = -1;
	int exitflag = -1;
	struct acceptStaffInfo *apt = NULL;
	
	while(1){
		op = displayFunctionUI();
		switch(op){
		case 1:
			modifyStaffPasswordReq(pstClient);
			break;
		case 2:
			apt = queryStaffOwnInfo(pstClient);
			if(NULL == apt){
				printf("queryStaffOwnInfo failed\n");
				break;
			}
			printOwnInfo(apt);
			break;
		case 0:
			ret = staffLogout(pstClient);
			if(ret != 0){
				printf("Logout failed!\n");
				break;
			}
			printf("Logout success!\n");
			exitflag = 1;
			break;
		}
		if(1 == exitflag){
			break;
		}
	}
	return 0;
}

/*functionUI界面显示*/
int displayFunctionUI()
{
	int cmd = -1;
	char buf[NORMAL_LENGTH] = "";
	do{
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 1.Modify password        @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 2.Query own infomation   @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 0.Exit                   @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ Please input your choice @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		myGetString(buf,NORMAL_LENGTH);
		sscanf(buf,"%d",&cmd);
		if(cmd < 0 || cmd > 2){
			printf("Your input is invalid,please redo it!\n");
			continue;
		}else{
			break;
		}
	}while(1);

	return cmd;
}

/*打印自身信息*/
void printOwnInfo(struct acceptStaffInfo *apt)
{
	printf("---------------------------------------------\n");
	printf("| %-20s| %-20s|\n","Name",apt->name);
	printf("---------------------------------------------\n");
	printf("| %-20s| %-20s|\n","Address",apt->address);
	printf("---------------------------------------------\n");
	printf("| %-20s| %-20s|\n","Level",apt->level);
	printf("---------------------------------------------\n");
	printf("| %-20s| %-20s|\n","Age",apt->age);
	printf("---------------------------------------------\n");
	printf("| %-20s| %-20s|\n","ID",apt->id);
	printf("---------------------------------------------\n");
	printf("| %-20s| %-20s|\n","Salary",apt->salary);
	printf("---------------------------------------------\n");
	printf("| %-20s| %-20s|\n","Phone",apt->phone);
	printf("---------------------------------------------\n");
	printf("| %-20s| %-20s|\n","Email",apt->email);
	printf("---------------------------------------------\n");
	
	return;
}

/*test*/
/*void mainLoop(int sockfd)
{
	int ret = 0;
	char buf[40] = "";

	while(1){
		printf("please input the string:\n");
		myGetString(buf,40);
		if(strcmp(buf,"quit") == 0){
			break;
		}

		ret = write(sockfd,buf,40);
		if(ret != 40){
			PrintClientError("send string failed,ret = %d\n",ret);
			break;
		}

		memset(buf,0,40);
		ret = read(sockfd,buf,40);
		if(ret != 40){
			PrintClientError("send string failed,ret = %d\n",ret);
			break;
		}
		printf("server send to me:%s\n",buf);
	}

	close(sockfd);
	sockfd = -1;
	return;
}*/

