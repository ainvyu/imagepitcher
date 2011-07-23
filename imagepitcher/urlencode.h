#pragma once

#ifdef _COMPILING_DLL
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif

class CUrlEncode
{
public:
  static std::string URLEncode(const std::string& strSrc,
                               bool bWebBrowserFriendly = false,
                               bool bUpperCase = true);
  static std::string URLEncode(const char* strSrc, 
                               bool bWebBrowserFriendly = false,
                               bool bUpperCase = true);
  static std::string URLDecode(const char* strSrc);
  static char x2c(char hex_up, char hex_low);
};

