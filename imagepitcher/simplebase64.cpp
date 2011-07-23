#include "stdafx.h"

#include "SimpleBase64.h"

static const char s_encode_table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z',	'0', '1', '2', '3', 
  '4', '5', '6', '7', '8', '9', '+', '/'
};

static const char s_decode_table[] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, 62, -1, -1, -1, 63, 52, 53,
  54, 55, 56, 57, 58, 59, 60, 61, -1, -1,
  -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,
   5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
  25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
  29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
  39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 51, -1, -1, -1, -1, -1, -1, -1
};

std::string CSimpleBase64::Encode(const std::string& str) {
  std::string text;
  char input[48]; 
  char output[64 + 2 + 1];
  int inlen = 0, outlen = 0;

  std::istringstream ss(str);

  while (TRUE)
  {
    ss.read(reinterpret_cast<char*>(input), 48);

    inlen = (int)ss.gcount();

    if (inlen > 0)
    {
      outlen = EncodeChunk(input, output, inlen);
      text.insert(text.end(), &output[0], &output[outlen]);
    }

    if (inlen < 48)
    {
      break;
    }
  }

  return text;
}

std::string CSimpleBase64::Decode( const std::string& encoded_text )
{
  std::string text;
  char output[256];
  char* input;
  const char* delimeter = "\r\n\t";
  int inlen, outlen;
  char* next_token = NULL;

  std::vector<char> buf(encoded_text.length()+1 , 0);
  strcpy_s(&buf[0], buf.size() , encoded_text.c_str());

  input = strtok_s( &buf[0], delimeter, &next_token);

  while (input != NULL)
  {
    inlen = strlen(input);

    if (inlen > 0)
    {
      outlen = DecodeChunk(input, output, inlen);
      text.insert(text.end(), &output[0], &output[outlen]);
    }

    input = strtok_s(NULL, delimeter, &next_token);
  }

  return text;
}

tstring CSimpleBase64::EncodeFromFile(std::ifstream& ifs)
{
  tstring text;
  BYTE input[48];
  TCHAR output[64 + 2 + 1];
  int inlen = 0, outlen = 0;

  ifs.seekg( 0, std::ios_base::beg );

  while (TRUE)
  {
    ifs.read(reinterpret_cast<char*>(input), 48);

    inlen = (int)ifs.gcount();

    if (inlen > 0)
    {
      outlen = EncodeChunk(input, output, inlen);
      output[outlen + 0] = '\r';
      output[outlen + 1] = '\n';
      output[outlen + 2] = '\0';
      text += output;
    }

    if (inlen < 48)
    {
      break;
    }
  }

  return text;
}

void CSimpleBase64::DecodeToFile(const tstring& text, std::ofstream& ofs)
{
  BYTE output[256];
  LPTSTR input;
  LPCTSTR delimeter = _T("\r\n\t");
  int inlen, outlen;
  TCHAR * next_token = NULL;

  ofs.seekp(0, std::ios::beg);

  std::vector<TCHAR> buf( text.length()+1 , 0);
  _tcscpy_s( &buf[0], buf.size() , text.c_str() );

  input = _tcstok_s( &buf[0], delimeter, &next_token);

  while (input != NULL)
  {
    inlen = _tcslen(input);

    if (inlen > 0)
    {
      outlen = DecodeChunk(input, output, inlen);
      ofs.write(reinterpret_cast<const char *>( output ), outlen);
    }

    input = _tcstok_s(NULL, delimeter, &next_token);
  }

  return;
}

int CSimpleBase64::EncodeChunk(char* src, char* dest, int inlen)
{
  BYTE tripple[3];
  char quad[4]; /* 6bits each */
  int nremain;
  int outlen;

  for (int i = 0, j = 0; i < inlen; i += 3, j += 4)
  {
    nremain = inlen - i;

    tripple[0] = (nremain > 0) ? src[i + 0] : 0;
    tripple[1] = (nremain > 1) ? src[i + 1] : 0;
    tripple[2] = (nremain > 2) ? src[i + 2] : 0;

    quad[0] = (tripple[0] >> 2) & 0x3f;
    quad[1] = ((tripple[0] << 4) | (tripple[1] >> 4)) & 0x3f;
    quad[2] = ((tripple[1] << 2) | (tripple[2] >> 6)) & 0x3f;
    quad[3] = (tripple[2] >> 0) & 0x3f;

    dest[j + 0] = s_encode_table[quad[0]];
    dest[j + 1] = s_encode_table[quad[1]];
    dest[j + 2] = (nremain < 2) ? '=' : s_encode_table[quad[2]];
    dest[j + 3] = (nremain < 3) ? '=' : s_encode_table[quad[3]];
  }

  outlen = (inlen + 2) / 3 * 4;

  return outlen;
}

int CSimpleBase64::EncodeChunk(LPBYTE src, LPTSTR dest, int inlen)
{
  BYTE tripple[3];
  char quad[4]; /* 6bits each */
  int nremain;
  int outlen;

  for (int i = 0, j = 0; i < inlen; i += 3, j += 4)
  {
    nremain = inlen - i;
      
    tripple[0] = (nremain > 0) ? src[i + 0] : 0;
    tripple[1] = (nremain > 1) ? src[i + 1] : 0;
    tripple[2] = (nremain > 2) ? src[i + 2] : 0;
    
    quad[0] = (tripple[0] >> 2) & 0x3f;
    quad[1] = ((tripple[0] << 4) | (tripple[1] >> 4)) & 0x3f;
    quad[2] = ((tripple[1] << 2) | (tripple[2] >> 6)) & 0x3f;
    quad[3] = (tripple[2] >> 0) & 0x3f;

    dest[j + 0] = s_encode_table[quad[0]];
    dest[j + 1] = s_encode_table[quad[1]];
    dest[j + 2] = (nremain < 2) ? '=' : s_encode_table[quad[2]];
    dest[j + 3] = (nremain < 3) ? '=' : s_encode_table[quad[3]];
  }

  outlen = (inlen + 2) / 3 * 4;

  return outlen;
}

int CSimpleBase64::DecodeChunk(LPCTSTR src, LPBYTE dest, int inlen)
{
  BYTE tripple[3];
  char quad[4]; /* 6bits each */
  int i, j;
  int outlen;

  for (i = 0, j = 0; i < inlen; i += 4, j += 3)
  {
    quad[0] = s_decode_table[src[i + 0]];
    quad[1] = s_decode_table[src[i + 1]];
    quad[2] = s_decode_table[src[i + 2]];
    quad[3] = s_decode_table[src[i + 3]];

    tripple[0] = (quad[0] << 2) | (quad[1] >> 4);
    tripple[1] = (quad[1] << 4) | (quad[2] >> 2);
    tripple[2] = (quad[2] << 6) | (quad[3] >> 0);

    dest[j + 0] = tripple[0];
    dest[j + 1] = tripple[1];
    dest[j + 2] = tripple[2];
  }

  outlen = (inlen + 3) / 4 * 3;

  if (src[inlen - 1] == '=') outlen--;
  if (src[inlen - 2] == '=') outlen--;
  
  return outlen;
}

int CSimpleBase64::DecodeChunk(char* src, char* dest, int inlen)
{
  BYTE tripple[3];
  char quad[4]; /* 6bits each */
  int i, j;
  int outlen;

  for (i = 0, j = 0; i < inlen; i += 4, j += 3)
  {
    quad[0] = s_decode_table[src[i + 0]];
    quad[1] = s_decode_table[src[i + 1]];
    quad[2] = s_decode_table[src[i + 2]];
    quad[3] = s_decode_table[src[i + 3]];

    tripple[0] = (quad[0] << 2) | (quad[1] >> 4);
    tripple[1] = (quad[1] << 4) | (quad[2] >> 2);
    tripple[2] = (quad[2] << 6) | (quad[3] >> 0);

    dest[j + 0] = tripple[0];
    dest[j + 1] = tripple[1];
    dest[j + 2] = tripple[2];
  }

  outlen = (inlen + 3) / 4 * 3;

  if (src[inlen - 1] == '=') outlen--;
  if (src[inlen - 2] == '=') outlen--;

  return outlen;
}