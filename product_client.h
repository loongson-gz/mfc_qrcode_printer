#ifndef __HUAXI_CLIENT_H__
#define __HUAXI_CLIENT_H__

#include "dbhelper.h"
////数据库配置
//typedef struct _stSQLConf
//{
//	char szIpAddr[20];		//IP地址
//	uint16_t uPort;			//端口号
//	char szUser[256];		//用户名
//	char szPwd[256];		//密码
//	char szDbName[256];		//数据库名
//	char szDnsName[256];	//数据库数据源名称
//}stSQLConf;


class ProductClient {
public:
	ProductClient(stSQLConf conf);
	~ProductClient();

	int Init();
	int GetProductInfo(std::string productCode, stProductInfo &data);
	int GetBarcodeRule(TBarcodeRuleLst &ruleLst);
private:
	DbHelper *m_pDbHelper;
	stSQLConf m_conf;
};

#endif