/*
*
* Copyright (c) 2018,  All Rights Reserved.
*
*
*
*
*
*
*/
#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_
#include <string>


class HttpClient {
public:
	HttpClient();
	HttpClient(const HttpClient&);
	~HttpClient();

	int Post(const std::string &url, const std::string& post_str, std::string& response);

	int Get(const std::string& url, std::string& response);

	int Put(const std::string& url, const std::string& put_str, std::string& response);

	int Delete(const std::string& url, const std::string& del_str, std::string& response);

	int Posts(const std::string& url, const std::string& post_str, std::string& response, const char* ca = NULL);

	int Gets(const std::string& url, std::string& response, const char* ca = NULL);

	int Puts(const std::string& url, const std::string& put_str, std::string& response, const char* ca = NULL);

	int Deletes(const std::string& url, const std::string& del_str, std::string& response, const char* ca = NULL);

};



#endif
