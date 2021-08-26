/*************************************************************************
	> File Name: operationFunctions.c
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月06日 星期五 21时49分56秒
 ************************************************************************/
#include "../include/smsServer.h"

/*
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
*/
static int inputStaffInfo(struct staffInfo *staff);

/*向user表中添加新记录*/
int insertNewUser(sqlite3 *pdb,char *username,char *password)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;

	/*比较姓名*/
	sprintf(sqlstr,"SELECT * FROM user WHERE name LIKE '%s'",username);
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		sqlite3_close(pdb);
		pdb = NULL;
		return -1;
	}else if(strcmp(*(ppret+3),username) == 0){
		/*该用户已存在*/
//		printf("the username %s is exist!\n",username);
		return 0;
	}else{
		/*向user表中添加信息*/
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"INSERT INTO user(Name,Password,Flag) VALUES('%s','%s',0)",username,password);
		ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("exec %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return -1;
		}
	}
	return 0;
}

/*向staffInfo表中添加新记录*/
int addNewStaffInfo(sqlite3 *pdb)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char *perr = NULL;
	struct staffInfo *staff = NULL;
	staff = (struct staffInfo *)malloc(sizeof(struct staffInfo));
	memset(staff,0,sizeof(struct staffInfo));

	/*向staffInfo表中插入数据*/
	inputStaffInfo(staff);
	memset(sqlstr,0,1024);
	sprintf(sqlstr,"INSERT INTO staffInfo(Name,Address,Age,level,ID,Salary,Phone,Email) VALUES('%s','%s','%d','%d','%d','%f','%s','%s')",
			staff->name,staff->address,staff->age,staff->level,staff->id,staff->salary,staff->phone,staff->email);
	ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("exec %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	
	/*同时在user表中添加相关信息*/
	memset(sqlstr,0,1024);
	sprintf(sqlstr,"INSERT INTO user(Name,Password,Flag) VALUES('%s','%s',0)",staff->name,STAFF_INITIAL_PASSWD);
	ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("exec %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	return 0;
}

/*通过名字删除成员信息*/
int deleteStaffByName(sqlite3 *pdb,char *staffname)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;

	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM user");
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	sscanf(*(ppret+1),"%d",&ret);
	if(0 == ret){
		printf("The user Name:%s has no record in Tables!\n",staffname);
		return -1;
	}else{
		/*删除user表中的信息*/
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"DELETE FROM user WHERE name='%s'",staffname);
		ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("exec %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return -1;
		}

		/*删除staffInfo表中的信息*/
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"DELETE FROM staffInfo WHERE name='%s'",staffname);
		ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("exec %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return -1;
		}
	}
	return 0;
}

/*更改user表的登入密码*/
int modifyUserPassword(sqlite3 *pdb,char *username,char *password)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char operation[OPERATION_LENGTH] = "Modify password";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;

	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM user WHERE name='%s'",username);
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	sscanf(*(ppret+1),"%d",&ret);
	if(0 == ret){
		printf("The user Name:%s has no record in Tables!\n",username);
		return -1;
	}else{
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"UPDATE user SET Password='%s' WHERE Name='%s'",password,username);
		ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("exec %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return -1;
		}
		ret = insertHistoryRecord(pdb,username,operation);
		if(ret != 0){
			PrintServerError("Insert history records failed\n");
			return -1;
		}
	}
	return 0;
}

/*更改staffInfo表中的信息*/
int modifyStaffInfo(sqlite3 *pdb,char *staffname)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;
	char column[NORMAL_LENGTH] = "";
	int i = 0;

	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM StaffInfo WHERE Name='%s'",staffname);
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	sscanf(*(ppret+1),"%d",&ret);
	if(0 == ret){
		printf("The user Name:%s has no record in the Tables!\n",staffname);
		return -1;
	}else{
		printf("please input the column which want to modify:");
		myGetString(column,NORMAL_LENGTH);
		/*
		 *判断选择的column是字符串型还是整型或浮点型
		 */
		if(strncasecmp(column,"name",(int)sizeof("name")) == 0 ||strncasecmp(column,"address",(int)sizeof("address")) == 0 || 
				strncasecmp(column,"phone",(int)sizeof("phone")) == 0 ||strncasecmp(column,"email",(int)sizeof("email")) == 0){
			char value[NORMAL_LENGTH] = "";	//字符串型数据，用char[]接收
			printf("please input the value which want to modify:");
			myGetString(value,NORMAL_LENGTH);

			memset(sqlstr,0,1024);
			sprintf(sqlstr,"UPDATE staffInfo SET %s='%s' WHERE Name='%s'",column,value,staffname);
			ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
			if(ret != SQLITE_OK){
				PrintServerError("exec %s failed:%s\n",sqlstr,perr);
				sqlite3_free(perr);
				perr = NULL;
				return -1;
			}
		}else if(strncasecmp(column,"age",(int)sizeof("age")) == 0 ||strncasecmp(column,"level",(int)sizeof("level")) == 0 || 
				strncasecmp(column,"id",(int)sizeof("id")) == 0){
			int val = 0;	//整型数据，用int接收
			printf("please input the value which want to modify:");
			scanf("%d",&val);
			getchar();

			memset(sqlstr,0,1024);
			sprintf(sqlstr,"UPDATE staffInfo SET %s=%d WHERE Name='%s'",column,val,staffname);
			ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
			if(ret != SQLITE_OK){
				PrintServerError("exec %s failed:%s\n",sqlstr,perr);
				sqlite3_free(perr);
				perr = NULL;
				return -1;
			}
		}else if(strncasecmp(column,"salary",(int)sizeof("salary")) == 0){
			float val = 0.0;	//浮点型数据，用float接收
			printf("please input the value which want to modify:");
			scanf("%f",&val);
			getchar();

			memset(sqlstr,0,1024);
			sprintf(sqlstr,"UPDATE staffInfo SET %s=%f WHERE Name='%s'",column,val,staffname);
			ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
			if(ret != SQLITE_OK){
				PrintServerError("exec %s failed:%s\n",sqlstr,perr);
				sqlite3_free(perr);
				perr = NULL;
				return -1;
			}
		}else{
			printf("the 'name'or'column'or'value' you input is not exist!\n");
			return -1;
		}
	}
	return 0;
}

/*查询注册信息--user*/
int printUserTable(sqlite3 *pdb)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;
	int i = 0;

	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM user");
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	sscanf(*(ppret+1),"%d",&ret);
	if(0 == ret){
		printf("NO record in User Table!\n");
		return -1;
	}else{
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"SELECT * FROM user");
		ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return -1;
		}
		printf("%-20s %-20s %-10s\n",*ppret,*(ppret+1),*(ppret+2));
		printf("-----------------    -----------------    -------    \n");
		for(i=1;i<=rows;i++){
			printf("%-20s %-20s %-10s\n",*(ppret + i * cols),*(ppret + i * cols + 1),*(ppret + i * cols + 2));
		}
	}
	sqlite3_free_table(ppret);
	ppret = NULL;
	return 0;
}

/*查询员工信息--StaffInfo*/
int printStaffInfoTable(sqlite3 *pdb)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;
	int i = 0;

	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM staffInfo");
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	sscanf(*(ppret+1),"%d",&ret);
	if(0 == ret){
		printf("Has no record in StaffInfo Table!\n");
		return -1;
	}else{
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"SELECT * FROM staffInfo");
		ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return -1;
		}
		printf("%-20s %-20s %-10s %-10s %-10s %-20s %-20s %-20s\n",
				*ppret,*(ppret+1),*(ppret+2),*(ppret+3),*(ppret+4),*(ppret+5),*(ppret+6),*(ppret+7));
		printf("-----------------    -----------------    -------    -------    -------    -----------------    -----------------    -----------------\n");
		for(i=1;i<=rows;i++){
			printf("%-20s %-20s %-10s %-10s %-10s %-20s %-20s %-20s\n",
					*(ppret + i * cols),*(ppret + i * cols + 1),
					*(ppret + i * cols + 2),*(ppret + i * cols + 3),
					*(ppret + i * cols + 4),*(ppret + i * cols + 5),
					*(ppret + i * cols + 6),*(ppret + i * cols + 7));
		}
	}
	sqlite3_free_table(ppret);
	ppret = NULL;
	return 0;
}

/*通过名字查询员工信息--StaffInfo*/
struct acceptStaffInfo *printStaffInfoTableByName(sqlite3 *pdb,char *staffname)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;
	struct acceptStaffInfo *apt;
	apt = (struct acceptStaffInfo *)malloc(sizeof(struct acceptStaffInfo));
	if(NULL == apt){
		PrintServerError("apt malloc failed\n");
		return NULL;
	}

	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM staffInfo");
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return NULL;
	}
	sscanf(*(ppret+1),"%d",&ret);
	if(0 == ret){
		printf("Has no record in StaffInfo Table!\n");
		return NULL;
	}else{
		memset(apt,0,sizeof(struct acceptStaffInfo));
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"SELECT * FROM staffInfo WHERE Name='%s'",staffname);
		ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return NULL;
		}
		strncpy(apt->name,*(ppret + cols + 0),strlen(*(ppret + cols + 0)));
		strncpy(apt->address,*(ppret + cols + 1),strlen(*(ppret + cols + 1)));
		strncpy(apt->level,*(ppret + cols + 2),strlen(*(ppret + cols + 2)));
		strncpy(apt->age,*(ppret + cols + 3),strlen(*(ppret + cols + 3)));
		strncpy(apt->id,*(ppret + cols + 4),strlen(*(ppret + cols + 4)));
		strncpy(apt->salary,*(ppret + cols + 5),strlen(*(ppret + cols + 5)));
		strncpy(apt->phone,*(ppret + cols + 6),strlen(*(ppret + cols + 6)));
		strncpy(apt->email,*(ppret + cols + 7),strlen(*(ppret + cols + 7)));
	}
	sqlite3_free_table(ppret);
	ppret = NULL;
	return apt;
}

/*查询staff历史记录 --history*/
int printHistoryTableByName(sqlite3 *pdb,char *staffname)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;
	int i = 0;

	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM history WHERE name='%s'",staffname);
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	sscanf(*(ppret+1),"%d",&ret);
	if(0 == ret){
		printf("The user Name:%s has no history record!\n",staffname);
		return -1;
	}else{
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"SELECT * FROM history WHERE name='%s'",staffname);
		ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return -1;
		}
		printf("%-20s %-20s %-20s\n",*ppret,*(ppret+1),*(ppret+2));
		printf("-----------------    -----------------    -----------------    \n");
		for(i=1;i<=rows;i++){
			printf("%-20s %-20s %-20s\n",*(ppret + i * cols),*(ppret + i * cols + 1),*(ppret + i * cols + 2));
		}
	}
	sqlite3_free_table(ppret);
	ppret = NULL;
	return 0;
}

/*给staffInfo结构体赋值*/
static int inputStaffInfo(struct staffInfo *staff)
{
	printf("please input the name:");
	myGetString(staff->name,NAME_LENGTH);
	printf("please input the address:");
	myGetString(staff->address,ADDRESS_LENGTH);
	printf("please input the level:");
	scanf("%d",&(staff->level));
	printf("please input the age:");
	scanf("%d",&(staff->age));
	printf("please input the id:");
	scanf("%d",&(staff->id));
	printf("please input the salary:");
	scanf("%f",&(staff->salary));
	getchar();
	printf("please input the phone:");
	myGetString(staff->phone,PHONE_LENGTH);
	printf("please input the email:");
	myGetString(staff->email,EMAIL_LENGTH);

	return 0;
}

/*插入历史记录*/
int insertHistoryRecord(sqlite3 *pdb,char *staffname,char *operation)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char *perr = NULL;

	/*向history表中添加历史记录*/
	memset(sqlstr,0,1024);
	sprintf(sqlstr,"INSERT INTO history(Name,Operation,Datatime) VALUES('%s','%s',datetime('now','localtime'))",staffname,operation);
	ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("exec %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	return 0;
}

/*确认登入*/
int checkLogin(sqlite3 *pdb,char *username,char *password)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;
	int flag = 0;

	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM user WHERE name='%s'",username);
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return 4;
	}
	sscanf(*(ppret+1),"%d",&ret);
	if(0 == ret){
//		printf("The user Name:%s is not exist!\n",username);
		return 3;
	}else{
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"SELECT * FROM user WHERE name like '%s'",username);
		ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return 4;
		}else{
			if(strncmp(*(ppret+cols),username,NAME_LENGTH) == 0){
				if(strncmp(*(ppret+cols+1),password,PASSWD_LENGTH) != 0){
//					PrintServerError("Password Error!!!\n");
					sqlite3_free(perr);
					perr = NULL;
					return 2;
				}else{
					sscanf(*(ppret+cols+2),"%d",&flag);
					if(0 == flag){
						updateUserFlag(pdb,username);
						return 0;
					}else{
//						printf("%s has been login!!!\n",username);
						sqlite3_free(perr);
						perr = NULL;
						return 1;
					}
				}
			}else{
//				PrintServerError("The user Name:%s is not exist\n",username);
				sqlite3_free(perr);
				perr = NULL;
				return 3;
			}
		}
	}
	return 0;
}

/*更新Flag状态*/
int updateUserFlag(sqlite3 *pdb,char *username)
{
	int ret = 0;
	char sqlstr[1024] = "";
	char operationIn[OPERATION_LENGTH] = "Login";
	char operationOut[OPERATION_LENGTH] = "Logout";
	char **ppret = NULL;
	int rows = 0,cols = 0;
	char *perr = NULL;
	int flag = 0;

	memset(sqlstr,0,1024);
	sprintf(sqlstr,"SELECT count(*) FROM user WHERE name='%s'",username);
	ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
	if(ret != SQLITE_OK){
		PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
		sqlite3_free(perr);
		perr = NULL;
		return -1;
	}
	sscanf(*(ppret+1),"%d",&ret);
	if(0 == ret){
		printf("The user Name:%s is not exist!\n",username);
		return -1;
	}else{
		memset(sqlstr,0,1024);
		sprintf(sqlstr,"SELECT * FROM user WHERE name like '%s'",username);
		ret = my_sqlite3_get_table(pdb,sqlstr,&ppret,&rows,&cols,&perr);
		if(ret != SQLITE_OK){
			PrintServerError("my_sqlite3_get_table %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
		}
		sscanf(*(ppret+cols+2),"%d",&flag);
		flag = (flag+1)%2;
		if(1 == flag){
			ret = insertHistoryRecord(pdb,username,operationIn);
			if(ret != 0){
				PrintServerError("Insert history records failed\n");
				return -1;
			}
		}else if(0 == flag){
			ret = insertHistoryRecord(pdb,username,operationOut);
			if(ret != 0){
				PrintServerError("Insert history records failed\n");
				return -1;
			}
		}
		sprintf(sqlstr,"UPDATE user SET flag=%d WHERE name = '%s';",flag,username);
		ret = my_sqlite3_exec(pdb,sqlstr,NULL,NULL,&perr);
		if(ret != SQLITE_OK){/*更新失败*/
			PrintServerError("exec %s failed:%s\n",sqlstr,perr);
			sqlite3_free(perr);
			perr = NULL;
			return -1;
		}else{
			//		printf("update flag success\n");
			sqlite3_free(perr);
			perr = NULL;
			return 0;
		}
	}
}

/*test*/
/*
int main()
{
	sqlite3 *psql = NULL;
	int ret = 0;
	int i = 0;
	psql = createSqlite3Engine("../sqlite3/sms.db");
	if(NULL == psql){
		return 1;
	}
	ret = insertNewUser(psql,ROOT,ROOT_PASSWD);
	if(ret != 0){
		PrintServerError("insertNewUser failed\n");
		return -1;
	}
	for(i=0;i<3;i++){
		printf("========== insert No.%d ==========\n",i+1);
		ret = addNewStaffInfo(psql);
		if(ret != 0){
			PrintServerError("addNewStaffInfo failed\n");
			return -1;
		}
	}
	printf("========== insert end ==========\n");
	printf("========== print start ==========\n");	
	printUserTable(psql);
	printStaffInfoTable(psql);
	printf("========== print end ==========\n");
	printf("========== delete start ==========\n");
	ret = deleteStaffByName(psql);
	if(ret != 0){
		PrintServerError("deleteStaffByName failed\n");
		return -1;
	}
	printf("========== delete end ==========\n");
	printf("========== print start ==========\n");	
	printUserTable(psql);
	printStaffInfoTable(psql);
	printf("========== print end ==========\n");
	printf("========== modify start ==========\n");	
	for(i=0;i<2;i++){
		ret = modifyStaffInfo(psql);
		if(ret != 0){
			PrintServerError("modifyStaffInfo failed\n");
			return -1;
		}
	}
	printf("========== modify end ==========\n");	
	printf("========== print start ==========\n");	
	printUserTable(psql);
	printStaffInfoTable(psql);
	printf("========== print end ==========\n");
	sqlite3_close(psql);
	psql = NULL;
	return 0;
}
*/


