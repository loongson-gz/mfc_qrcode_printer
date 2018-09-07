#ifndef __HUAXI_CLIENT_H__
#define __HUAXI_CLIENT_H__

#include "dbhelper.h"
////���ݿ�����
//typedef struct _stSQLConf
//{
//	char szIpAddr[20];		//IP��ַ
//	uint16_t uPort;			//�˿ں�
//	char szUser[256];		//�û���
//	char szPwd[256];		//����
//	char szDbName[256];		//���ݿ���
//	char szDnsName[256];	//���ݿ�����Դ����
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