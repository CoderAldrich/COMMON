#pragma once

/*in_key 长度必须为16字节*/
int AES_Base64_Encrypt(
	const char* input
	, int in_size
	, char*& output
	, int& out_size
	, const char* in_key);

/*in_key 长度必须为16字节*/
int AES_Base64_Decrypt(
	const char* input
	, int in_size
	, unsigned char*& output
	, int& out_size
	, const char* in_key);

/*in_key 长度必须为16字节*/
int AES_Ecb_Base64_Encrypt_pkcs5(const char* input
	, int in_size
	, char*& output
	, int& out_size
	, const char* in_key);

int AES_Ecb_Base64_Decrypt_pkcs5(
	const char* input
	, int in_size
	, unsigned char*& output
	, int& out_size
	, const char* in_key);