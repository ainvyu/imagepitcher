#pragma once

#define	ENCRYPTION_CODE	0x98

#ifdef _COMPILING_DLL
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif

class CEncryptionManager  
{
public:		
	static BOOL LoadFile(tstring strFullPath, tstring &strData, BOOL *pEncryptionMode);
	static BOOL SaveFile(tstring strFullPath, tstring strData, BOOL bEncryptionMode, tstring strEncoding = _T(""));

protected:
// 	static void Encode(tstring& szSrc);
// 	static void Decode(tstring& szSrc);
	static void Encode(LPTSTR szSrc, DWORD dwMemSize = NULL);
	static void Decode(LPTSTR szSrc, DWORD dwMemSize = NULL);
};