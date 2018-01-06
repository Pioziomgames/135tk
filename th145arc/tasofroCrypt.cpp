extern "C"
{
	#include "miracl.h"
}
#include "tasofroCrypt.h"
#if defined(WIN32) || defined(_WIN32)
# include <windows.h>
#endif
#include <string.h>

const unsigned char PATCHED_KEY_N[KEY_BYTESIZE] = {
	0xFF, 0x65, 0x72, 0x74, 0x61, 0x69, 0x52, 0x20, 0x2D, 0x2D, 0x20, 0x69, 0x6F, 0x6B, 0x6E, 0x61, 
	0x6C, 0x46, 0x20, 0x73, 0x73, 0x65, 0x6C, 0x42, 0x20, 0x64, 0x6F, 0x47, 0x20, 0x79, 0x61, 0x4D, 
	0x08, 0x8B, 0xF4, 0x75, 0x5D, 0x78, 0xB1, 0xC8, 0x93, 0x7F, 0x40, 0xEA, 0x34, 0xA5, 0x85, 0xC1, 
	0x1B, 0x8D, 0x63, 0x17, 0x75, 0x98, 0x2D, 0xA8, 0x17, 0x45, 0x31, 0x31, 0x51, 0x4F, 0x6E, 0x8D
};

const unsigned char PATCHED_KEY_d[KEY_BYTESIZE] = {
	0xC7, 0x6F, 0x87, 0xDE, 0x1B, 0xC7, 0x6B, 0xEB, 0x56, 0x7B, 0xDB, 0xC8, 0x99, 0xCA, 0x51, 0xCA, 
	0xE0, 0x0B, 0x2F, 0x02, 0x18, 0xC5, 0xC5, 0xB1, 0x29, 0xAC, 0x64, 0x3D, 0x2D, 0xBD, 0xBB, 0x5C, 
	0xDB, 0x67, 0x32, 0xB2, 0x46, 0x86, 0xEE, 0xB6, 0x65, 0x6A, 0x57, 0xF9, 0xDD, 0x4D, 0xEA, 0xFA, 
	0x85, 0xE6, 0x6C, 0x4A, 0xBD, 0x0F, 0x16, 0x82, 0xB8, 0x17, 0x6C, 0x08, 0x6E, 0x98, 0x88, 0x41
};

/*
const unsigned char PUBLIC_KEY_N[KEY_BYTESIZE] = {
	0xC7, 0x9A, 0x9E, 0x9B, 0xFB, 0xC2, 0x0C, 0xB0, 0xC3, 0xE7, 0xAE, 0x27, 0x49, 0x67, 0x62, 0x8A, 
	0x78, 0xBB, 0xD1, 0x2C, 0xB2, 0x4D, 0xF4, 0x87, 0xC7, 0x09, 0x35, 0xF7, 0x01, 0xF8, 0x2E, 0xE5, 
	0x49, 0x3B, 0x83, 0x6B, 0x84, 0x26, 0xAA, 0x42, 0x9A, 0xE1, 0xCC, 0xEE, 0x08, 0xA2, 0x15, 0x1C, 
	0x42, 0xE7, 0x48, 0xB1, 0x9C, 0xCE, 0x7A, 0xD9, 0x40, 0x1A, 0x4D, 0xD4, 0x36, 0x37, 0x5C, 0x89
}; // In Big Endian
*/

const unsigned char PUBLIC_KEY_N[KEY_BYTESIZE] = {
	0xC6, 0x43, 0xE0, 0x9D, 0x35, 0x5E, 0x98, 0x1D, 0xBE, 0x63, 0x6D, 0x3A, 0x5F, 0x84, 0x0F, 0x49,
	0xB8, 0xE8, 0x53, 0xF5, 0x42, 0x06, 0x37, 0x3B, 0x36, 0x25, 0xCB, 0x65, 0xCE, 0xDD, 0x68, 0x8C,
	0xF7, 0x5D, 0x72, 0x0A, 0xC0, 0x47, 0xBD, 0xFA, 0x3B, 0x10, 0x4C, 0xD2, 0x2C, 0xFE, 0x72, 0x03,
	0x10, 0x4D, 0xD8, 0x85, 0x15, 0x35, 0x55, 0xA3, 0x5A, 0xAF, 0xC3, 0x4A, 0x3B, 0xF3, 0xE2, 0x37
}; // In Big Endian

bool mipInitialized = false;
int keyInitialized = -1;
big RSA_N;
big RSA_d;
big RSA_e;

int InitRSAKeyPair(int type)
{
	if(!mipInitialized)
	{
		miracl* mip = mirsys(128,16);
		mip->IOBASE = 16;
		mipInitialized = true;
	}

	RSA_e = mirvar(0x10001);
	if(type == 0)
	{
		RSA_N = mirvar(0);
		bytes_to_big(KEY_BYTESIZE,(const char*)PUBLIC_KEY_N,RSA_N);
	}
	else
	{
		RSA_N = mirvar(0);
		bytes_to_big(KEY_BYTESIZE,(const char*)PATCHED_KEY_N,RSA_N);

		RSA_d = mirvar(0);
		bytes_to_big(KEY_BYTESIZE,(const char*)PATCHED_KEY_d,RSA_d);
	}
	keyInitialized = type;
	return 0;
}

int DecryptBlock(const unsigned char* src,unsigned char* dst)
{
	big sint = mirvar(0);
	bytes_to_big(KEY_BYTESIZE,(const char*)src,sint);

	big result = mirvar(0);
	powmod(sint,RSA_e,RSA_N,result);
	big_to_bytes(KEY_BYTESIZE,result,(char*)dst,TRUE);
	//std::reverse(dst,dst+KEY_BYTESIZE);
	return 0;
}

int EncryptBlock(const unsigned char* src,unsigned char* dst)
{
	memcpy(dst, src, 0x20);
	return 0;
}

const unsigned char PaddingBytes[32] = {0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00};

// auto padding :D
int Encrypt3264(const unsigned char* src,unsigned char* dst,size_t srcLen)
{
	if(srcLen > 0x20) return -1;
	unsigned char tmp[64] = {0};
	memcpy(tmp, src, srcLen);
	memset(tmp + srcLen, 0, 0x40 - srcLen);

	if(keyInitialized != 1) InitRSAKeyPair(1);
	EncryptBlock(tmp,dst);
	return 0;
}

// By truncating
int Decrypt6432(const unsigned char* src,unsigned char* dst,size_t dstLen)
{
	if(keyInitialized == -1) InitRSAKeyPair(0);
	if(dstLen > 0x20) return -1;
	if (memcmp(src + 0x20, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 0x20) == 0)
	{
		memcpy(dst, src, dstLen);
		return 0;
	}

	unsigned char tmp[64] = {0};
	DecryptBlock(src,tmp);
	if(memcmp(PaddingBytes,tmp,31) != 0)
	{
		InitRSAKeyPair(keyInitialized^1);
		DecryptBlock(src,tmp);
		if(memcmp(PaddingBytes,tmp,31) != 0) return -1;
	}
	memcpy(dst, tmp + 0x40 - dstLen, dstLen);
	return 0;
}
