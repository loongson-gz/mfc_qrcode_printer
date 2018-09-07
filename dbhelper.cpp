#include "StdAfx.h"
#include "dbhelper.h"

#include "Poco/Data/Session.h"
#include "Poco/Data/SessionPool.h"
#include "Poco/Data/ODBC/ConnectionHandle.h"
#include "Poco/Data/ODBC/Connector.h"

#pragma comment(lib, "PocoDataODBC.lib")
#pragma comment(lib, "PocoData.lib")
#pragma comment(lib, "PocoFoundation.lib")

using namespace Poco::Data::Keywords;

Poco::Data::SessionPool *m_pool = nullptr;
Poco::Data::Session *m_ses = nullptr;

//DbHelper::DbHelper()
//{
//}

DbHelper::DbHelper(const char * usr, const char * pwd, const char * database, const char * dsn)
	:  m_bConnect(false)

{
	m_url.append("UID=");
	m_url.append(usr);
	m_url.append(";PWD=");
	m_url.append(pwd);
	m_url.append(";DATABASE=");
	m_url.append(database);
	m_url.append(";dsn=");
	m_url.append(dsn);

	printf("%s", m_url.c_str());
	Poco::Data::ODBC::Connector::registerConnector();
}

DbHelper::DbHelper(const char * dsn)
{
	m_url.append(";dsn=");
	m_url.append(dsn);
	Poco::Data::ODBC::Connector::registerConnector();
}

DbHelper::~DbHelper()
{
	Poco::Data::ODBC::Connector::unregisterConnector();
	printf("%s, %s free ............", __FUNCTION__, m_url.c_str());
}

int DbHelper::ConnectToSvr()
{
	//m_pool = new Poco::Data::SessionPool("ODBC", "UID=jmuser;PWD=qwer1234;DATABASE=wjlwmsst;dsn=wjl_local_wms");
//	m_pool = new Poco::Data::SessionPool("ODBC", "UID=jmuser;PWD=qwer1234;DATABASE=wjlwmsst;dsn=oracle_wjl_mes");
	try
	{
		m_pool = new Poco::Data::SessionPool("ODBC", m_url);
		//m_pool = new Poco::Data::SessionPool("ODBC", "dsn=dsn_mysql");

		m_ses = new Poco::Data::Session(m_pool->get());
	}
	catch (const std::exception& e)
	{
		printf("%s:%d [%s] %s", __FUNCTION__, __LINE__, m_url.c_str(), e.what());
		m_bConnect = false;
		return -1;
	}

	m_bConnect = true;
	return 0;
}

int DbHelper::InsertData(const char *szSql)
{
	if (!m_bConnect || !m_ses)
	{
		return -1;
	}

	try
	{
		Poco::Data::Statement sql(*m_ses);
		sql << szSql;

		//sql << "INSERT INTO TB_DEVICECJJL  VALUES (RAWTOHEX(sys_guid()), NULL, NULL, NULL, sysdate, NULL, NULL, NULL, NULL, NULL, NULL, '新建', NULL, NULL, '1号工位', '1', NULL, NULL, NULL, NULL, NULL, NULL, NULL);";
		sql.execute();
	}
	catch (std::exception& e)
	{
		printf("%s:%d %s", __FUNCTION__, __LINE__, e.what());
		return -1;
	}
	
	return 0;
}

int DbHelper::GetData(const char * szSql, stProductInfo &res)
{
	if (!m_bConnect || !m_ses)
	{
		return -1;
	}
	try
	{
		Poco::Data::Statement sql(*m_ses);
		sql << szSql, into(res.strErpCode), into(res.strKind), range(0, 1);
		sql.execute();
	}
	catch (const std::exception& e)
	{
		printf("%s:%d %s", __FUNCTION__, __LINE__, e.what());
		return -1;
	}

	return 0;
}

