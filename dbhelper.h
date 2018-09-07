#ifndef __DB_HELPER_H__
#define __DB_HELPER_H__



#include "common.h"

typedef struct stProductInfo_
{
	std::string strErpCode;
	std::string strKind;
}stProductInfo;

class DbHelper {
public:
	DbHelper(const char *usr, const char *pwd, const char *database, const char *dsn);
	DbHelper(const char *dsn);
	~DbHelper();

	int ConnectToSvr();
	int InsertData(const char *szSql);
	int GetData(const char *szSql, stProductInfo &res);

private:
	//Poco::Data::SessionPool *m_pool;
	//Poco::Data::Session *m_ses;

	std::string m_url;
	bool m_bConnect;
};

#endif
