#pragma once

#ifdef _COMPILING_DLL
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif

class CSimpleBase64
{
public:
  static std::string Encode(const std::string& text);
  static std::string Decode(const std::string& encoded_text);
  static tstring EncodeFromFile(std::ifstream& ifs);
  static void DecodeToFile(const tstring& text, std::ofstream & ofs);

protected:
  static int EncodeChunk(LPBYTE src, LPTSTR dest, int inlen);
  static int EncodeChunk(char* src, char* dest, int inlen);
  static int DecodeChunk(LPCTSTR src, LPBYTE dest, int inlen);
  static int DecodeChunk(char* src, char* dest, int inlen);
};

