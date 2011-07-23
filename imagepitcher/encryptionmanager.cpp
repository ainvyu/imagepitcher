#include "stdafx.h"

#define _COMPILING_DLL

#include <functional>

#include "EncryptionManager.h"
#include "FileCache.h"
#include "TextFile.h"
#include "StringUtil.h"

using namespace std;

static char szHeader[32]	= "Secure File v1.0";
static char szHeaderEnd[4]	= { 0x00, 0x00, 0x1A, 0x00 };

struct bitxor
{
	bitxor(const int _Val): val(_Val) { }

	template <typename T>
	inline T operator() (const T& x) const	{ return x^val; }

private:
	int val;
};

// void CEncryptionManager::Encode(tstring& szSrc)
// {
// 	transform(szSrc.begin(), szSrc.end(), szSrc.begin(), bitxor(ENCRYPTION_CODE));
// }
// 
// void CEncryptionManager::Decode(tstring& szSrc)
// {
// 	transform(szSrc.begin(), szSrc.end(), szSrc.begin(), bitxor(ENCRYPTION_CODE));
// }

void CEncryptionManager::Encode(LPTSTR szSrc, DWORD dwMemSize)
{
	_TCHAR ch;

	if (dwMemSize == NULL)
		dwMemSize = (DWORD)(_tcslen(szSrc) * sizeof(TCHAR));

	while (ch = *szSrc)
		*szSrc++ = ch ^ ENCRYPTION_CODE;

	*szSrc++ = NULL;
}

void CEncryptionManager::Decode(LPTSTR szSrc, DWORD dwMemSize)
{
	_TCHAR ch;

	if (dwMemSize == NULL)
		dwMemSize = (DWORD)(_tcslen(szSrc) * sizeof(TCHAR));

	while (dwMemSize-- > 0)
	{
		ch = *szSrc;
		*szSrc++ = ch ^ ENCRYPTION_CODE;
	}
}

BOOL CEncryptionManager::SaveFile(tstring strFullPath, tstring strData, BOOL bEncryptionMode, tstring strEncoding)
{
	const unsigned short szUnicode[2] = { 0xFEFF, 0x0000 };

	BOOL bUniCode = !(sizeof(TCHAR) % 2);
	CTextFile file;

	if (!file.Open(strFullPath.c_str(), ios_base::out | ios_base::binary))
	{
		return FALSE;
	}

	if (!bEncryptionMode)
	{
		int nFileEncoding = FILE_ENCODING_ANSI;

		if (CStringUtil::CompareNoCase(strEncoding, _T("UTF-8")) == 0)	
			nFileEncoding = FILE_ENCODING_UTF8;
		else if (CStringUtil::CompareNoCase(strEncoding, _T("UTF-16")) == 0)	
			nFileEncoding = FILE_ENCODING_UTF16;

		file.SetText(strData.c_str(), nFileEncoding);
	}
	else
	{
		file.Write(szHeader, 31);
		file.Write(szHeaderEnd, 3);

		if (bUniCode)
		{
			Encode((LPTSTR)szUnicode);
			file.Write(szUnicode, 2);
		}

		LPTSTR szData = const_cast<LPTSTR>(strData.c_str());
		Encode(szData);

		file.Write(szData, (UINT)(_tcslen(szData) * sizeof(TCHAR)));
	}

	file.Flush();
	file.Close();

	return TRUE;
}

BOOL CEncryptionManager::LoadFile(tstring strFullPath, tstring &strData, BOOL *pEncryptionMode)
{
	CTextFile file;
	IStream *pStream = NULL;

	if (!file.Open(strFullPath.c_str(), ios_base::in | ios_base::binary))
		return FALSE;

	DWORD dwFileSize = (DWORD)file.GetLength();
	HGLOBAL hGlobal = NULL;
	void *pBuf = NULL;

	char szFileHeader[32] = { 0, };
	file.Read(szFileHeader, 31);
	szFileHeader[31] = NULL;

	if (strcmp(szFileHeader, szHeader) == 0)
	{
		file.Read(szFileHeader, 3);

		hGlobal = GlobalAlloc(GPTR, dwFileSize - 34);

		if (!hGlobal)
		{
			file.Close();
			return FALSE;
		}

		pBuf = (void*)hGlobal;

		file.Read(pBuf, dwFileSize - 34);
		file.Close();

		Decode((LPTSTR)pBuf, (dwFileSize - 34) / sizeof(TCHAR));

		if (pEncryptionMode)	
		{
			*pEncryptionMode = TRUE;
		}

		strData = tstring((LPTSTR)pBuf);
	}
	else
	{
		file.SeekToBegin();
		strData = file.GetText();
		file.Close();

		if (pEncryptionMode)
		{
			*pEncryptionMode = FALSE;
		}
	}

	return TRUE;
}