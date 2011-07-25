#pragma once

#ifdef _COMPILING_DLL
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif

class CRegisterMenu
{
public:
	static bool Register(const tstring& strAppName, tstring strExt, tstring strMenu, tstring strMenuDisplay, tstring strCommand, tstring strDesc);
	static bool UnRegister(tstring strAppName, tstring strExt, tstring strMenu);

private:
	static bool RegisterMenu(const tstring& strSubKey, const tstring& strValue_);
	static bool UnRegisterMenu(const tstring& strSubKey);

	static bool RegisterExtKey(const tstring& strExt, const tstring& strMenu, const tstring& strMenuDisplay, const tstring& strCommmand);
	static bool UnRegisterExtKey(const tstring& strExt, const tstring& strMenu);
};
