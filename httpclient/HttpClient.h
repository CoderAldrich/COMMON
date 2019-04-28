#pragma once

#include <Windows.h>
#include <string>

using std::string;

class CHttpClient
{
public:
	CHttpClient(void);
	~CHttpClient(void);
	int Post(const string &url, const string &post, string &response);
	int Get(const string &url, string &response);
	int Posts(const string &url, const string &post, string &response, const char* pCaPath = NULL);
	int Gets(const string &url, const string &post, string &response, const char* pCaPath = NULL);
	void SetDebug(bool debug);
protected:	
private:
	bool m_debug;
};