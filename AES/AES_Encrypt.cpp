#include <Windows.h>
#include "aes.h"

//#define KEY "e10adc3949ba59abbe56e057f20f883e"
//char str_key[] = { 0xe1,0x0a,0xdc,0x39,0x49,0xba,0x59,0xab,0xbe,0x56,0xe0,0x57,0xf2,0x0f,0x88,0x3e };
const char str_key[] = "*NYK7CXCKM3iScx!";

static char *Base64Encode(char *bin_data, int bin_length, char *base64)
{
	const char *base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int i, j;
	unsigned char current;

	for (i = 0, j = 0; i < bin_length; i += 3)
	{
		current = (bin_data[i] >> 2);
		current &= (unsigned char)0x3F;
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bin_data[i] << 4)) & ((unsigned char)0x30);
		if (i + 1 >= bin_length)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bin_data[i + 1] >> 4)) & ((unsigned char)0x0F);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bin_data[i + 1] << 2)) & ((unsigned char)0x3C);
		if (i + 2 >= bin_length)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bin_data[i + 2] >> 6)) & ((unsigned char)0x03);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)bin_data[i + 2]) & ((unsigned char)0x3F);
		base64[j++] = base64char[(int)current];
	}
	base64[j] = '\0';
	return base64;
}

static int Base64Decode(const char *base64, unsigned char *bindata)
{
	const char *base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int i, j;
	unsigned char k;
	unsigned char temp[4];
	for (i = 0, j = 0; base64[i] != '\0'; i += 4)
	{
		memset(temp, 0xFF, sizeof(temp));
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i])
				temp[0] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 1])
				temp[1] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 2])
				temp[2] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 3])
				temp[3] = k;
		}

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2)) & 0xFC)) |
			((unsigned char)((unsigned char)(temp[1] >> 4) & 0x03));
		if (base64[i + 2] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4)) & 0xF0)) |
			((unsigned char)((unsigned char)(temp[2] >> 2) & 0x0F));
		if (base64[i + 3] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6)) & 0xF0)) |
			((unsigned char)(temp[3] & 0x3F));
	}
	return j;
}


int AES_Base64_Encrypt(
	const char* input
	, int in_size
	, char*& output
	, int& out_size
	, const char* in_key)
{
	int result = -1;
	unsigned char *encrypt = NULL;
	char *base64 = NULL;
	//char *base64_base64 = NULL;
	do 
	{
		aes_context ctx;
		aes_set_key(&ctx, (unsigned char *)str_key, 128);

		int bits = 16;
		int mod = in_size % bits;
		int encrypt_size = mod == 0 ? in_size : in_size + bits - mod;
		encrypt = (unsigned char *)malloc(encrypt_size + 1);
		if (encrypt == NULL)
			break;
		memset(encrypt, 0, encrypt_size + 1);

		for (int i = 0; i < in_size; i += 16)
		{
			if (i + 16 <= in_size)
			{
				aes_encrypt(&ctx, (unsigned char *)input + i, encrypt + i);
			}
			else
			{
				/*默认是0填充*/
				unsigned char tmp[16] = {0};// pkcs5padding
				memcpy(tmp, input + i, in_size - i);
				aes_encrypt(&ctx, tmp, encrypt + i);
			}
		}
// 		output = (char*)malloc(encrypt_size+1);
// 		memset(output, 0, encrypt_size + 1);
// 		memcpy(output, encrypt, encrypt_size);
// 		out_size = encrypt_size;
// 		break;

		//base64两次，防止url里面出现+、/
		int base64_size = encrypt_size * 2 + 4;
		base64 = (char *)malloc(base64_size + 1);
		if (base64 == NULL)
			break;
		memset(base64, 0, base64_size + 1);
		Base64Encode((char *)encrypt, encrypt_size, base64);

		//int base64_base64_size = strlen(base64) * 2;
		//base64_base64 = (char *)malloc(base64_base64_size + 1);
		//if (base64_base64 == NULL)
		//	break;
		//memset(base64_base64, 0, base64_base64_size);
		//Base64Encode(base64, strlen(base64), base64_base64);

		output = base64;
		out_size = strlen(base64);

		result = 1;
	} while (FALSE);

	if (encrypt)
		free(encrypt);
	//if (base64)
	//	free(base64);

	return 1;
}

int AES_Ecb_Base64_Encrypt_pkcs5(const char* input
	, int in_size
	, char*& output
	, int& out_size
	, const char* in_key)
{
	int result = 0;
	unsigned char *encrypt = NULL;
	char *base64 = NULL;

	do
	{
		aes_context ctx;
		aes_set_key(&ctx, (unsigned char *)str_key, 128);

		int bits = 16;
		int mod = in_size % bits;
		int encrypt_size = mod == 0 ? in_size : in_size + bits - mod;
		encrypt = (unsigned char *)malloc(encrypt_size + 1);
		if (encrypt == NULL)
		{
			break;
		}
		memset(encrypt, 0, encrypt_size + 1);

		for (int i = 0; i < in_size; i += 16)
		{
			if (i + 16 <= in_size)
			{
				aes_encrypt(&ctx, (unsigned char *)input + i, encrypt + i);
			}
			else
			{
				/*pkcs5填充*/
				int need_padding_size = 16 - (in_size - i);
				unsigned char tmp[16] = { 0 };
				for (auto k = 0; k < 16; ++k)
				{
					tmp[k] = need_padding_size & 0x0F;
				}
				memcpy(tmp, input + i, in_size - i);
				aes_encrypt(&ctx, tmp, encrypt + i);
			}
		}

		//base64两次，防止url里面出现+、/
		int base64_size = encrypt_size * 2 + 4;
		base64 = (char *)malloc(base64_size + 1);
		if (base64 == NULL)
		{
			break;
		}
		memset(base64, 0, base64_size + 1);
		Base64Encode((char *)encrypt, encrypt_size, base64);

		output = base64;
		out_size = strlen(base64);
		result = out_size;
	} while (FALSE);

	if (encrypt)
	{
		free(encrypt);
	}
	return out_size;
}

int AES_Base64_Decrypt(
	const char* input
	, int in_size
	, unsigned char*& output
	, int& out_size
	, const char* in_key)
{
	int result = -1;
	unsigned char *encrypt = NULL;
	unsigned char *plain = NULL;
	do 
	{
		encrypt = (unsigned char *)malloc(in_size + 1);
		if (encrypt == NULL)
		{
			return -1;
		}
		memset(encrypt, 0, in_size + 1);
		int encrypt_size = Base64Decode(input, encrypt);

		int mod = encrypt_size % 16;
		int plain_size = mod == 0 ? encrypt_size : encrypt_size + 16 -mod;
		plain = (unsigned char *)malloc(plain_size + 1);
		if (plain == NULL)
		{
			return -1;
		}
		memset(plain, 0, plain_size + 1);

		aes_context ctx;
		aes_set_key(&ctx, (unsigned char *)in_key, 128);

		for (int i = 0; i < encrypt_size; i += 16)
		{
			if (encrypt_size - i < 16)
			{
				char tmp[16] = {0};
				memcpy(tmp, encrypt + i, encrypt_size - i);
				aes_decrypt(&ctx, (unsigned char*)tmp, (unsigned char*)plain + i);
			}
			else
			{
				aes_decrypt(&ctx, (unsigned char*)encrypt + i, (unsigned char*)plain + i);
			}		
		}

		output = plain;
		out_size = strlen((char *)output);

		result = 1;
	} while (FALSE);	

	if (encrypt)
	{
		free(encrypt);
	}

	return result;
}

int AES_Ecb_Base64_Decrypt_pkcs5(
	const char* input
	, int in_size
	, unsigned char*& output
	, int& out_size
	, const char* in_key)
{
	int result = 0;
	unsigned char *encrypt = NULL;
	unsigned char *plain = NULL;
	do
	{
		encrypt = (unsigned char *)malloc(in_size + 1);
		if (encrypt == NULL)
		{
			break;
		}
		memset(encrypt, 0, in_size + 1);
		int encrypt_size = Base64Decode(input, encrypt);

		int mod = encrypt_size % 16;
		int plain_size = mod == 0 ? encrypt_size : encrypt_size + 16 - mod;
		plain = (unsigned char *)malloc(plain_size + 1);
		if (plain == NULL)
		{
			break;
		}
		memset(plain, 0, plain_size + 1);

		aes_context ctx;
		aes_set_key(&ctx, (unsigned char *)in_key, 128);

		for (int i = 0; i < encrypt_size; i += 16)
		{
			if (encrypt_size - i < 16)
			{
				int need_padding_size = 16 - (encrypt_size - i);
				char tmp[16] = { 0 };
				for (auto k = 0; k < 16; ++k)
				{
					tmp[k] = need_padding_size & 0x0F;
				}
				memcpy(tmp, encrypt + i, encrypt_size - i);
				aes_decrypt(&ctx, (unsigned char*)tmp, (unsigned char*)plain + i);
			}
			else
			{
				aes_decrypt(&ctx, (unsigned char*)encrypt + i, (unsigned char*)plain + i);
			}
		}

		output = plain;
		out_size = strlen((char *)output);

		result = out_size;
	} while (FALSE);

	if (encrypt)
	{
		free(encrypt);
	}

	return result;
}