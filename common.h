#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <sstream>

//using namespace std;


//���ݿ�����
typedef struct _stSQLConf
{
	char szIpAddr[20];		//IP��ַ
	uint16_t uPort;			//�˿ں�
	char szUser[256];		//�û���
	char szPwd[256];		//����
	char szDbName[256];		//���ݿ���
	char szDnsName[256];	//���ݿ�����Դ����
}stSQLConf;

#endif