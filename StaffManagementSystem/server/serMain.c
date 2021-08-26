/*************************************************************************
	> File Name: serMain.c
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月06日 星期五 15时48分59秒
 ************************************************************************/
#include "../include/smsServer.h"

/*
int createServerSocket(const char *ip,unsigned short port);
int mainLoop(int servfd,sqlite3 *pdb);
void *serverMainUILoop(void *arg);
int displayServerMainUI();
int checkRootLogin(sqlite3 *pdb);
void serverFunctionLoop(sqlite3 *pdb);
int displayServerFunctionUI();
*/

int main(int argc, const char *argv[])
{
	int ret = 0;
	int servfd = -1;
	short port = -1;
	sqlite3 *pdb = NULL;
	pthread_t maintid;
	
	if(argc < 3){
		PrintServerInfo("the arguements is too few\n");
		return -1;
	}

	sscanf(argv[2],"%hd",&port);
	if(port < 0 || port > 0xffff){
		PrintServerWarning("the port is invalid\n");
		return -1;
	}

	servfd = createServerSocket(argv[1],port);
	if(servfd < 0){
		PrintServerError("createServerSocket failed\n");
		return -1;
	}

	pdb = createSqlite3Engine("../sqlite3/sms.db");
	if(pdb == NULL){
		PrintServerError("createSqlite3Engine failed\n");
		return -1;
	}
	insertNewUser(pdb,ROOT,ROOT_PASSWD);
	
	signal(SIGCHLD,SIG_IGN);

	ret = pthread_create(&maintid,NULL,serverMainUILoop,(void *)pdb);
	if(ret != 0){
		PrintServerError("pthread_create serverMainUILoop failed\n");
		return -1;
	}

	mainLoop(servfd,pdb);

	close(servfd);
	servfd = -1;

	sqlite3_close(pdb);
	pdb = NULL;
	return 0;
}

/*监听客户端链接请求*/
int createServerSocket(const char *ip,unsigned short port)
{
	int servfd = -1;
	struct sockaddr_in servaddr;
	int ret = 0;

	servfd = socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_aton(ip,&servaddr.sin_addr);

	ret = bind(servfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	ret += listen(servfd,9);
	if(ret){
		PrintServerError("bind or listen failed\n");
		close(servfd);
		servfd = -1;
	}

	return servfd;
}

/*与客户端建立链接并接收数据*/
int mainLoop(int servfd,sqlite3 *pdb)
{
	int ret = 0;
	int datafd = -1;
	pthread_t tid;
//	pid_t pid;
	struct smsOLServerCTL PstServer = {0};

	while(1){
		/*accept*/
		datafd = accept(servfd,NULL,NULL);
		if(datafd < 0){
			if(errno == EINTR){
				continue;
			}else{
				PrintServerError("accept error\n");
				break;
			}
		}
		PstServer.datafd = datafd;
		PstServer.pdb = pdb;

/*		pid = fork();
		if(pid < 0){
			close(datafd);
			datafd = -1;
			printf("can not supply server for client\n");
			continue;
		}
		if(pid > 0){
			close(datafd);
			datafd = -1;
		}else{
			close(servfd);
			servfd = -1;
			handleClient((void *)&PstServer);
			exit(0);
		}*/
		ret = pthread_create(&tid,NULL,handleClient,(void *)&PstServer);
		if(ret != 0){
			close(datafd);
			datafd = -1;
			PrintServerError("pthread_create failed\n");
			continue;
		}
	}
	return 0;
}

/*Server主UI界面*/
void *serverMainUILoop(void *arg)
{
	sqlite3 *pdb = (sqlite3 *)arg;
	int op = -1;
	int ret = -1;
	int exitFlag = -1;

	ret = pthread_detach(pthread_self());
	if(ret != 0){
		PrintServerError("pthread_detach failed\n");
		return NULL;
	}
	while(1){
		op = displayServerMainUI();
		switch(op){
			case 1:
				ret = checkRootLogin(pdb);
				if(0 == ret){
					serverFunctionLoop(pdb);
				}else{
					break;
				}
				break;
			case 0:
				printf("Server can not exit!!\n");
				break;
		}
		if(1 == exitFlag){
			break;
		}
	}
	return NULL;
}

/*主UI界面显示*/
int displayServerMainUI()
{
	int cmd = -1;
	char buf[NORMAL_LENGTH] = "";
	do{
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@@ 1.Login                  @@@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@@ 0.Exit(reserved)         @@@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@@ Please input your choice @@@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
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


/*Root登入*/
int checkRootLogin(sqlite3 *pdb)
{
	char rootname[NAME_LENGTH] = "";
	char password[PASSWD_LENGTH] = "";

	printf("please input the name:");
	myGetString(rootname,NAME_LENGTH);
	printf("please input the password:");
	myGetString(password,PASSWD_LENGTH);

	if(strncmp(rootname,ROOT,NAME_LENGTH) == 0){
		if(strncmp(password,ROOT_PASSWD,PASSWD_LENGTH) != 0){
			printf("Password Error!\n");
			return -1;
		}else{
			printf("Login success!\n");
			updateUserFlag(pdb,rootname);
			return 0;
		}
	}else{
		printf("Limited authority!\n");
	}
	return -1;
}

/*Server functionUI界面*/
void serverFunctionLoop(sqlite3 *pdb)
{
	int op = -1;
	int ret = -1;
	int exitFlag = -1;
	char deletename[NAME_LENGTH] = "";
	char modifyname[NAME_LENGTH] = "";
	char historyname[NAME_LENGTH] = "";

	if(NULL == pdb){
		PrintServerError("sqlite3 error\n");
		return;
	}
	while(1){
		op = displayServerFunctionUI();
		switch(op){
			case 1:
				ret = printStaffInfoTable(pdb);
				if(ret != 0){
//					PrintServerError("Query staff Information failed!\n");
					break;
				}
				break;
			case 2:
				ret = addNewStaffInfo(pdb);
				if(ret != 0){
//					PrintServerError("Add new staff's information failed\n");
					break;
				}
				printf("Add new staff's information success!\n");
				break;
			case 3:
				printf("please input the staffname which want to delete:");
				myGetString(deletename,NAME_LENGTH);
				ret = deleteStaffByName(pdb,deletename);
				if(ret != 0){
//					PrintServerError("Delete staff's information failed!\n");
					break;
				}
				printf("Delete %s's information success!\n",deletename);
				break;
			case 4:
				printf("please input the staffname which want to modify:");
				myGetString(modifyname,NAME_LENGTH);
				ret = modifyStaffInfo(pdb,modifyname);
				if(ret != 0){
//					PrintServerError("Modify staff's information failed!\n");
					break;
				}
				printf("Modify %s's information success!\n",modifyname);
				break;
			case 5:
				printf("please input the staffname which want to query:");
				myGetString(historyname,NAME_LENGTH);
				ret = printHistoryTableByName(pdb,historyname);
				if(ret != 0){
//					PrintServerError("Query history records failed!\n");
					break;
				}
				break;
			case 0:
				updateUserFlag(pdb,ROOT);
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
int displayServerFunctionUI()
{
	int cmd = -1;
	char buf[NORMAL_LENGTH] = "";
	do{
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 1.Query staff Information  @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 2.Add new staff            @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 3.Delete a staff           @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 4.Modify staff Information @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 5.Query historical Records @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ 0.Exit                     @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@ Please input your choice   @@@@@@@@@@@@\n");
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		myGetString(buf,NORMAL_LENGTH);
		sscanf(buf,"%d",&cmd);
		if(cmd < 0 || cmd > 5){
			printf("Your input is invalid,please redo it!\n");
			continue;
		}else{
			break;
		}
	}while(1);

	return cmd;
}




/*test connect with client*/
/*
void *handleClient(void *arg)
{
	long datafd = (long)(arg);
	char buf[40] = "";
	int ret = 0;

	pthread_(pthread_self());

	while(1){
		ret = read(datafd,buf,40);
		if(ret != 40){
			PrintServerError("recv string failed,ret = %d\n",ret);
			break;
		}
		printf("client send to me:%s\n",buf);

		ret = write(datafd,buf,40);
		if(ret != 40){
			PrintServerError("send string failed,ret = %d\n",ret);
			break;
		}
	}
	close(datafd);
	datafd = -1;
	return NULL;
}
*/
