/*************************************************************************
	> File Name: myGetString.c
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月07日 星期六 11时11分49秒
 ************************************************************************/
#include "../include/smsPublic.h"

/*从stdin输入字符串*/  
char *myGetString(char *pstrbuf,int size)
{
	int len = 0;

	fgets(pstrbuf,size,stdin);
	len = strlen(pstrbuf);
	if(pstrbuf[len - 1] == '\n'){
		pstrbuf[len - 1] = '\0';
	}else{
		while(getchar() != '\n'){
		}
	}

	return pstrbuf;
}

