/*************************************************************************
	> File Name: smsDebug.h
	> Author: Hippyll
	> Mail: hippyll@foxmail.com 
	> Created Time: 2021年08月06日 星期五 11时10分16秒
 ************************************************************************/
#ifndef SMS_DEBUG_H
#define SMS_DEBUG_H

/*
 * Client dedicated "printf"
*/
#define SMS_CLIENT_DEBUG_ERROR
#define SMS_CLIENT_DEBUG_WARNING
#define SMS_CLIENT_DEBUG_INFO

#ifdef SMS_CLIENT_DEBUG_ERROR
#define PrintClientError printf("<Client Error> at %s:%s:%d--",__FILE__,__func__,__LINE__);printf
#else
#define PrintClientError(...)
#endif

#ifdef SMS_CLIENT_DEBUG_WARNING
#define PrintClientWarning printf("<Client Warning> at %s:%s:%d--",__FILE__,__func__,__LINE__);printf
#else
#define PrintClientWarning(...)
#endif

#ifdef SMS_CLIENT_DEBUG_INFO
#define PrintClientInfo printf("<Client Info> at %s:%s:%d--",__FILE__,__func__,__LINE__);printf
#else
#define PrintClientInfo(...)
#endif

/*
 *Server dedicated "printf"
 */
#define SMS_SERVER_DEBUG_ERROR
#define SMS_SERVER_DEBUG_WARNING
#define SMS_SERVER_DEBUG_INFO

#ifdef SMS_SERVER_DEBUG_ERROR
#define PrintServerError printf("<Server Error> at %s:%s:%d--",__FILE__,__func__,__LINE__);printf
#else
#define PrintServerError(...)
#endif

#ifdef SMS_SERVER_DEBUG_WARNING
#define PrintServerWarning printf("<Server Warning> at %s:%s:%d--",__FILE__,__func__,__LINE__);printf
#else
#define PrintServerWarning(...)
#endif

#ifdef SMS_SERVER_DEBUG_INFO
#define PrintServerInfo printf("<Server Info> at %s:%s:%d--",__FILE__,__func__,__LINE__);printf
#else
#define PrintServerInfo(...)
#endif

/*
 *Public "printf"
 */
#define SMS_PUBLIC_DEBUG_ERROR
#define SMS_PUBLIC_DEBUG_WARNING
#define SMS_PUBLIC_DEBUG_INFO

#ifdef SMS_PUBLIC_DEBUG_ERROR
#define PrintPublicError printf("<Public Error> at %s:%s:%d--",__FILE__,__func__,__LINE__);printf
#else
#define PrintPublicError(...)
#endif

#ifdef SMS_PUBLIC_DEBUG_WARNING
#define PrintPublicWarning printf("<Public Warning> at %s:%s:%d--",__FILE__,__func__,__LINE__);printf
#else
#define PrintPublicWarning(...)
#endif

#ifdef SMS_PUBLIC_DEBUG_INFO
#define PrintPublicInfo printf("<Public Info> at %s:%s:%d--",__FILE__,__func__,__LINE__);printf
#else
#define PrintPublicInfo(...)
#endif


#endif

