#include <array>
#include <vector>
#include <string.h>
#include "Rsa.hpp"

bool Rsa::miraclInitialized = false;

std::vector<std::array<unsigned char, Rsa::KEY_BYTESIZE> > KEYS_N = {
  // Touhou 13.5
  {
    0xC7, 0x9A, 0x9E, 0x9B, 0xFB, 0xC2, 0x0C, 0xB0, 0xC3, 0xE7, 0xAE, 0x27, 0x49, 0x67, 0x62, 0x8A, 
    0x78, 0xBB, 0xD1, 0x2C, 0xB2, 0x4D, 0xF4, 0x87, 0xC7, 0x09, 0x35, 0xF7, 0x01, 0xF8, 0x2E, 0xE5, 
    0x49, 0x3B, 0x83, 0x6B, 0x84, 0x26, 0xAA, 0x42, 0x9A, 0xE1, 0xCC, 0xEE, 0x08, 0xA2, 0x15, 0x1C, 
    0x42, 0xE7, 0x48, 0xB1, 0x9C, 0xCE, 0x7A, 0xD9, 0x40, 0x1A, 0x4D, 0xD4, 0x36, 0x37, 0x5C, 0x89
  },
  // Touhou 13.5 - English patch
  {
    0xFF, 0x65, 0x72, 0x74, 0x61, 0x69, 0x52, 0x20, 0x2D, 0x2D, 0x20, 0x69, 0x6F, 0x6B, 0x6E, 0x61, 
    0x6C, 0x46, 0x20, 0x73, 0x73, 0x65, 0x6C, 0x42, 0x20, 0x64, 0x6F, 0x47, 0x20, 0x79, 0x61, 0x4D, 
    0x08, 0x8B, 0xF4, 0x75, 0x5D, 0x78, 0xB1, 0xC8, 0x93, 0x7F, 0x40, 0xEA, 0x34, 0xA5, 0x85, 0xC1, 
    0x1B, 0x8D, 0x63, 0x17, 0x75, 0x98, 0x2D, 0xA8, 0x17, 0x45, 0x31, 0x31, 0x51, 0x4F, 0x6E, 0x8D
  },
  // Touhou 14.5
  {
    0xC6, 0x43, 0xE0, 0x9D, 0x35, 0x5E, 0x98, 0x1D, 0xBE, 0x63, 0x6D, 0x3A, 0x5F, 0x84, 0x0F, 0x49,
    0xB8, 0xE8, 0x53, 0xF5, 0x42, 0x06, 0x37, 0x3B, 0x36, 0x25, 0xCB, 0x65, 0xCE, 0xDD, 0x68, 0x8C,
    0xF7, 0x5D, 0x72, 0x0A, 0xC0, 0x47, 0xBD, 0xFA, 0x3B, 0x10, 0x4C, 0xD2, 0x2C, 0xFE, 0x72, 0x03,
    0x10, 0x4D, 0xD8, 0x85, 0x15, 0x35, 0x55, 0xA3, 0x5A, 0xAF, 0xC3, 0x4A, 0x3B, 0xF3, 0xE2, 0x37
  },
  // Touhou 15.5
  {
    0xC4, 0x4D, 0x6A, 0x2F, 0x05, 0x78, 0x2C, 0x0F, 0xD7, 0x5C, 0x82, 0x97, 0x17, 0x60, 0x91, 0xDD,
    0x6F, 0x83, 0x61, 0x81, 0xD1, 0x4E, 0x06, 0x9B, 0x94, 0x37, 0xD2, 0x98, 0x4D, 0xE4, 0x7B, 0xBF,
    0x42, 0x60, 0xA7, 0x8F, 0x88, 0xD6, 0xFD, 0xFE, 0xE1, 0xF5, 0x6A, 0x0B, 0x29, 0xCF, 0x0B, 0xED,
    0x66, 0xF0, 0xAC, 0x4E, 0xD7, 0xEF, 0x96, 0x06, 0x8B, 0xFA, 0x8E, 0x33, 0x48, 0xA3, 0x02, 0x7D
  },
  // MarisaLand Legacy
  {
    0xC1, 0x34, 0x2E, 0x25, 0x96, 0x26, 0xCB, 0x38, 0xB0, 0x39, 0xD1, 0x2F, 0xA2, 0xE4, 0x7A, 0x3B,
    0xDD, 0x7A, 0x7D, 0x1E, 0xF1, 0x2E, 0x7C, 0xFD, 0x75, 0x6D, 0x1F, 0x63, 0x5F, 0x87, 0x9F, 0x20,
    0x02, 0xCE, 0x69, 0x38, 0xCB, 0xAD, 0x01, 0x00, 0x92, 0xCC, 0x1C, 0x93, 0x75, 0x17, 0x35, 0xE2,
    0x69, 0xDF, 0x2B, 0xC5, 0xCE, 0xDC, 0xD6, 0xDD, 0xF7, 0x44, 0x15, 0x19, 0xA3, 0x81, 0x2E, 0x95
  },
}; // In Big Endian

/*
 * Private key used for older versions of the Th135 static patch.
 * Matches the public key above under Touhou 13.5 - English Patch.
const unsigned char KEY_d[KEY_BYTESIZE] = {
  0xC7, 0x6F, 0x87, 0xDE, 0x1B, 0xC7, 0x6B, 0xEB, 0x56, 0x7B, 0xDB, 0xC8, 0x99, 0xCA, 0x51, 0xCA, 
  0xE0, 0x0B, 0x2F, 0x02, 0x18, 0xC5, 0xC5, 0xB1, 0x29, 0xAC, 0x64, 0x3D, 0x2D, 0xBD, 0xBB, 0x5C, 
  0xDB, 0x67, 0x32, 0xB2, 0x46, 0x86, 0xEE, 0xB6, 0x65, 0x6A, 0x57, 0xF9, 0xDD, 0x4D, 0xEA, 0xFA, 
  0x85, 0xE6, 0x6C, 0x4A, 0xBD, 0x0F, 0x16, 0x82, 0xB8, 0x17, 0x6C, 0x08, 0x6E, 0x98, 0x88, 0x41
};
*/
// Touhou 13.5 original private key, computed by Riatre
const unsigned char KEY_d[Rsa::KEY_BYTESIZE] = {
  0x34, 0x78, 0x84, 0xF1, 0x64, 0x41, 0x22, 0xAC, 0xE5, 0x12, 0xE6, 0x49, 0x15, 0x96, 0xC3, 0xE4,
  0xBA, 0xD0, 0x44, 0xB0, 0x87, 0x3E, 0xCE, 0xE5, 0x52, 0x81, 0x2D, 0x5A, 0x7D, 0x7E, 0x0C, 0x75,
  0x6A, 0x96, 0x7C, 0xE7, 0x5F, 0xDF, 0x7A, 0x21, 0x86, 0x40, 0x5B, 0x10, 0x43, 0xFD, 0x47, 0xDA,
  0x7B, 0xA7, 0xA4, 0xAC, 0x89, 0x20, 0xA6, 0x93, 0x91, 0x1C, 0x63, 0x5A, 0x83, 0x8E, 0x08, 0x01
};

void Rsa::initMiracl()
{
  miracl* mip = mirsys(128,16);
  mip->IOBASE = 16;
  Rsa::miraclInitialized = true;
}

void Rsa::freeMiracl()
{
  mirexit();
  Rsa::miraclInitialized = false;
}

Rsa::Rsa(File& file)
  : file(file)
{
  if (!Rsa::miraclInitialized)
    Rsa::initMiracl();

  RSA_e = mirvar(0x10001);
  RSA_d = mirvar(0);
  bytes_to_big(KEY_BYTESIZE, (const char*)KEY_d, RSA_d);

  // For encryption, take the Touhou 13.5 public key (TODO: add support for other keys).
  // For decryption, initRsaPublicKey will be called later.
  RSA_N = mirvar(0);
  bytes_to_big(KEY_BYTESIZE, (const char*)KEYS_N[0].data(), RSA_N);
}

Rsa::~Rsa()
{
  mirkill(RSA_e);
  mirkill(RSA_d);
  mirkill(RSA_N);
}

bool Rsa::initRsaPublicKey(const unsigned char *crypted_sample)
{
  // Guess the correct key for decryption
  for (auto& it : KEYS_N)
    {
      mirkill(RSA_N);
      RSA_N = mirvar(0);
      bytes_to_big(KEY_BYTESIZE, (const char*)it.data(), RSA_N);

      unsigned char tmp[64] = {0};
      this->DecryptBlock(crypted_sample, tmp);
      if (this->checkPadding(tmp))
	{
	  this->publicKeyInitialized = true;
	  return true;
	}
    }
  return false;
}

/*
** This comment may be wrong, but that's how I understand things.
**
** All crypted data has a padding of the form 0x00 0x01 [a lot of 0xFF] 0x00.
** th135 uses a fixed-width padding: the whole padding takes 32 bytes,
** so we have 29 0xFF in the middle. The payload is at data+32.
** th145 uses a variable-width padding: the payload is at the end,
** the encryption code adds enough 0xFF to push the payload to the end.
** The payload is at data+(64-data_size).
** By using our payload pattern above, we should be able to support both formats.
*/
bool Rsa::skipPadding(const unsigned char *data, size_t& i)
{
  if (data[0] != 0 || data[1] != 1)
    return false;
  i = 2;
  while (i < 63 && data[i] == 0xFF)
    i++;
  if (i < 31)
    return false;
  if (data[i] != 0)
    return false;

  i++;
  return true;
}

bool Rsa::checkPadding(const unsigned char *data)
{
  size_t i;
  return skipPadding(data, i);
}

bool Rsa::skipPaddingAndCopy(const unsigned char *src, unsigned char *dst, size_t size)
{
  size_t i;
  if (!skipPadding(src, i))
    return false;

  if (64 - i < size)
    return false;

  memcpy(dst, src + i, size);
  return true;
}

void Rsa::DecryptBlock(const unsigned char *src, unsigned char *dst)
{
  big sint = mirvar(0);
  bytes_to_big(KEY_BYTESIZE, (const char*)src, sint);

  big result = mirvar(0);
  powmod(sint, RSA_e, RSA_N, result);
  big_to_bytes(KEY_BYTESIZE, result, (char*)dst, TRUE);

  mirkill(sint);
  mirkill(result);
}

bool Rsa::Decrypt6432(const unsigned char* src, unsigned char* dst, size_t dstLen)
{
  if (dstLen > 0x20)
    return false;

  // Th145 English patch - no encryption
  if (memcmp(src + 0x20, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 0x20) == 0)
    {
      memcpy(dst, src, dstLen);
      return true;
    }

  // Anything else
  if (!publicKeyInitialized) {
    if (this->initRsaPublicKey(src) == false)
      return false;
  }

  unsigned char tmp[64] = {0};
  DecryptBlock(src, tmp);
  return this->skipPaddingAndCopy(tmp, dst, dstLen);
}

bool Rsa::read(void *buffer, size_t size)
{
  unsigned char tmp[64];
  if (this->file.read(tmp, 64) != 64)
    return false;

  return this->Decrypt6432(tmp, (unsigned char*)buffer, size);
}

File& Rsa::getFile()
{
  return this->file;
}
