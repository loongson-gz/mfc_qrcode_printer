#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <sstream>

//using namespace std;


//数据库配置
typedef struct _stSQLConf
{
	char szIpAddr[20];		//IP地址
	uint16_t uPort;			//端口号
	char szUser[256];		//用户名
	char szPwd[256];		//密码
	char szDbName[256];		//数据库名
	char szDnsName[256];	//数据库数据源名称
}stSQLConf;

#endif