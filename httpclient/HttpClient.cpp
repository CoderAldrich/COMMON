#include "HttpClient.h"
#include <curl/curl.h>



#ifdef _DEBUG
#pragma comment(lib, "libcurld.lib")
#else
#pragma comment(lib, "libcurl.lib")
#endif


CHttpClient::CHttpClient(void) : m_debug(false)
{

}

CHttpClient::~CHttpClient(void)
{

}

static int OnDebug(CURL *, curl_infotype itype, char* pData, size_t size, void *)
{
	if(itype == CURLINFO_TEXT)  
	{  
		//printf("[TEXT]%s\n", pData);  
	}  
	else if(itype == CURLINFO_HEADER_IN)  
	{  
		printf("[HEADER_IN]%s\n", pData);  
	}  
	else if(itype == CURLINFO_HEADER_OUT)  
	{  
		printf("[HEADER_OUT]%s\n", pData);  
	}  
	else if(itype == CURLINFO_DATA_IN)  
	{  
		printf("[DATA_IN]%s\n", pData);  
	}  
	else if(itype == CURLINFO_DATA_OUT)  
	{  
		printf("[DATA_OUT]%s\n", pData);  
	}  
	return 0;  
}

static int OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string *str =dynamic_cast<string *>((string *) lpVoid);  
	if(NULL == str || NULL == buffer)  
	{  
		return -1;  
	}  

	char* pData = (char *)buffer;  
	str->append(pData,size* nmemb);  
	return nmemb;  
}

int CHttpClient::Post(const string &url, const string &post, string &response)
{
	CURLcode res;
	CURL *curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	if (m_debug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Get(const string &url, string &response)
{
	CURLcode res;
	CURL *curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	if (m_debug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		printf("curl_easy_perform() failed:%s\n",curl_easy_strerror(res));
	}
	return res;
}

int CHttpClient::Posts(const string &url, const string &post, string &response, const char* pCaPath /* = NULL */)
{
	CURLcode res;  
	CURL *curl = curl_easy_init();  
	if(NULL == curl)  
	{  
		return CURLE_FAILED_INIT;  
	}  
	if(m_debug)  
	{  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);  
	}  

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  
	curl_easy_setopt(curl, CURLOPT_POST, 1);  
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());  
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);  
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);  
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  

	if(NULL == pCaPath)  
	{  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);  
	}
	else
	{  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);  
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);  
	}  
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);  
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);  
	res = curl_easy_perform(curl);  
	curl_easy_perform(curl);  
	return res;  
}

int CHttpClient::Gets(const string &url, const string &post, string &response, const char* pCaPath /* = NULL */)
{
	CURLcode res;  
	CURL* curl = curl_easy_init();  
	if(NULL == curl)  
	{  
		return CURLE_FAILED_INIT;  
	}  
	if(m_debug)  
	{  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);  
	}  
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);  
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);  
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
	if(NULL == pCaPath)  
	{  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);  
	}
	else
	{  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);  
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);  
	}  
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);  
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);  
	res = curl_easy_perform(curl);  
	curl_easy_cleanup(curl);  
	return res;  
}

void CHttpClient::SetDebug(bool debug)
{
	m_debug = debug;
}
