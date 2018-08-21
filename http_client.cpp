/*
*
* Copyright (c) 2018, winhi intelligent technology. All Rights Reserved.
*
*
*
*
*
*
*/
#include "StdAfx.h"
#include "http_client.h"
#include <iostream>
#include <curl/curl.h>

static size_t OnWriteData(char *buffer, size_t size, size_t nitems, void *outstream)
{
	std::string* str = dynamic_cast<std::string*>((std::string*)outstream);
	if (str == NULL || buffer == NULL)
	{
		return -1;
	}
	str->append(buffer, size * nitems);
	return nitems;
}

static int global_flags = 0;

HttpClient::HttpClient() {
	if (global_flags++ == 0) {
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}
}
HttpClient::HttpClient(const HttpClient&) {
	if (global_flags++ == 0) {
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}
}
HttpClient::~HttpClient() {
	if (--global_flags == 0) {
		curl_global_cleanup();
	}
}

int HttpClient::Post(const std::string &url, const std::string& post_str, std::string& response) {
	//
	int ret = 0;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::cout << curl_easy_strerror(CURLE_FAILED_INIT) << std::endl;
		return ~CURLE_FAILED_INIT + 1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_str.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_str.length());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ret = ~res + 1;
		std::cout << curl_easy_strerror(res) << std::endl;
	}
	curl_easy_cleanup(curl);
	return ret;
}

int HttpClient::Get(const std::string& url, std::string& response) {
	//
	int ret = 0;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::cout << curl_easy_strerror(CURLE_FAILED_INIT) << std::endl;
		return ~CURLE_FAILED_INIT + 1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ret = ~res + 1;
		std::cout << curl_easy_strerror(res) << std::endl;
	}
	curl_easy_cleanup(curl);
	return ret;
}

typedef struct {
	const char *data;
	const char *pos;
	const char *last;
}drp_upload_ctx;

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	drp_upload_ctx *ctx = (drp_upload_ctx *)(stream);
	if (ctx->pos >= ctx->last)
	{
		return 0;
	}

	if (size == 0 || (nmemb == 0) || ((size *nmemb) < 1))
	{
		return 0;
	}

	size_t len = ctx->last - ctx->pos;
	if (len > size * nmemb)
	{
		len = size * nmemb;
	}
	memcpy(ptr, ctx->pos, len);
	ctx->pos += len;
	return len;

}

int HttpClient::Put(const std::string& url, const std::string& put_str, std::string& response) {
	int ret = 0;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::cout << curl_easy_strerror(CURLE_FAILED_INIT) << std::endl;
		return ~CURLE_FAILED_INIT + 1;
	}
#if 1
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
#endif
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_PUT, 1L);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	drp_upload_ctx *ctx = NULL;
	if (!put_str.empty()) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
//		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, put_str.c_str());
//		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, put_str.length());
		ctx = (drp_upload_ctx *)malloc(sizeof(drp_upload_ctx));
		ctx->data = put_str.c_str();
		ctx->pos = put_str.c_str();
		ctx->last = ctx->pos+put_str.length();

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA,  ctx);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)(ctx->last-ctx->pos));
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ret = ~res + 1;
		std::cout << curl_easy_strerror(res) << std::endl;
	}
#if 1
	curl_slist_free_all(headers);
#endif
	free(ctx);
	curl_easy_cleanup(curl);
	return ret;
}

int HttpClient::Delete(const std::string& url, const std::string& del_str, std::string& response) {
	int ret = 0;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::cout << curl_easy_strerror(CURLE_FAILED_INIT) << std::endl;
		return ~CURLE_FAILED_INIT + 1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	if (!del_str.empty()) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, del_str.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, del_str.length());
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ret = ~res + 1;
		std::cout << curl_easy_strerror(res) << std::endl;
	}
	curl_easy_cleanup(curl);
	return ret;
}

int HttpClient::Posts(const std::string& url, const std::string& post_str, std::string& response, const char* ca) {
	int ret = 0;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::cout << curl_easy_strerror(CURLE_FAILED_INIT) << std::endl;
		return ~CURLE_FAILED_INIT + 1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	if (!post_str.empty()) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_str.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_str.length());
	}
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == ca)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, ca);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ret = ~res + 1;
		std::cout << curl_easy_strerror(res) << std::endl;
	}
	curl_easy_cleanup(curl);
	return ret;
}

int HttpClient::Gets(const std::string& url, std::string& response, const char* ca) {
	//
	int ret = 0;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::cout << curl_easy_strerror(CURLE_FAILED_INIT) << std::endl;
		return ~CURLE_FAILED_INIT + 1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == ca)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, ca);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ret = ~res + 1;
		std::cout << curl_easy_strerror(res) << std::endl;
	}
	curl_easy_cleanup(curl);
	return ret;
}


int HttpClient::Puts(const std::string& url, const std::string& put_str, std::string& response, const char* ca) {
	int ret = 0;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::cout << curl_easy_strerror(CURLE_FAILED_INIT) << std::endl;
		return ~CURLE_FAILED_INIT + 1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_PUT, 1L);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	if (!put_str.empty()) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, put_str.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, put_str.length());
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	if (NULL == ca)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, ca);
	}

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ret = ~res + 1;
		std::cout << curl_easy_strerror(res) << std::endl;
	}
	curl_easy_cleanup(curl);
	return ret;

}


int HttpClient::Deletes(const std::string& url, const std::string& del_str, std::string& response, const char* ca) {
	int ret = 0;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::cout << curl_easy_strerror(CURLE_FAILED_INIT) << std::endl;
		return ~CURLE_FAILED_INIT + 1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	if (!del_str.empty()) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, del_str.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, del_str.length());
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	if (NULL == ca)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, ca);
	}
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ret = ~res + 1;
		std::cout << curl_easy_strerror(res) << std::endl;
	}
	curl_easy_cleanup(curl);
	return ret;
}