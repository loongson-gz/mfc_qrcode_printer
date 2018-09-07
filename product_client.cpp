#include "StdAfx.h"
#include "product_client.h"
#include <sstream>


ProductClient::ProductClient(stSQLConf conf)
	: m_conf(conf)
	, m_pDbHelper(nullptr)
{
	printf("%s make", __FUNCTION__);

}

ProductClient::~ProductClient()
{
	if (m_pDbHelper)
	{
		delete m_pDbHelper;
	}
	printf("%s free", __FUNCTION__);

}

int ProductClient::Init()
{
	try {
		//m_pDbHelper = new DbHelper(m_conf.szUser, m_conf.szPwd, m_conf.szDbName, m_conf.szDnsName);
		m_pDbHelper = new DbHelper(m_conf.szDnsName);
		int ret = m_pDbHelper->ConnectToSvr();
		if (ret != 0)
		{
			return ret;
		}
	}
	catch (const std::exception& e)
	{
		printf("%s:%s", __FUNCTION__, e.what());
		return -1;
	}

	return 0;
}



int ProductClient::GetData(stProductInfo &data, std::string productCode)
{
	try {
		std::stringstream ss;
		ss << "select erp_code, kind from t_product_info where code="
		<< "'" << productCode << "'";

		std::string str = ss.str();
		int ret = m_pDbHelper->GetData(str.c_str(), data);
		if (ret != 0)
		{
			printf("%s:[%s] get data err.", __FUNCTION__, str.c_str());
			return ret;
		}
	}
	catch (const std::exception& e)
	{
		printf("%s:%s", __FUNCTION__, e.what());
		return -1;
	}
	
	return 0;
}


