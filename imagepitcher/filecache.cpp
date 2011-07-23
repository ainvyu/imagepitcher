#include "stdafx.h"

#define _COMPILING_DLL
#ifdef _LOAD_MFC_
#include "FileCache.h"
#include "DownloadCallback.h"
#include "AfxINet.h"

static CString				csTempPath;
static CMapStringToString	mapFile;	

void CFileCache::Init()
{
	TCHAR szPath[MAX_PATH];
	::GetTempPath(MAX_PATH, szPath);
	
	csTempPath = szPath;
}

void CFileCache::Free()
{
	POSITION pos = mapFile.GetStartPosition();
	while (pos)
	{
		CString csURL;
		CString csLocal;

		mapFile.GetNextAssoc(pos, csURL, csLocal);
		
		if (!csLocal.IsEmpty())
		{
			::DeleteFile(csLocal);
		}
	}

	mapFile.RemoveAll();
}

CString CFileCache::GetFile(LPCTSTR szURL, BOOL *pbAbort)
{
	ASSERT(!csTempPath.IsEmpty());

	DWORD	dwServiceType = 0;
	CString strServer;
	CString strObject;
	INTERNET_PORT nPort;
 
	AfxParseURL(szURL, dwServiceType, strServer, strObject, nPort);

	if(dwServiceType == AFX_INET_SERVICE_UNK ||
	   dwServiceType == AFX_INET_SERVICE_FILE) return szURL;

	CString csLocal;

	CDownloadCallback callback(pbAbort);
	TCHAR szLocal[MAX_PATH] = {0,};

	::GetTempFileName(csTempPath, _T("tmp"), 0, szLocal); 

	HRESULT hr = ::URLDownloadToFile(NULL, szURL, szLocal, 0, &callback);
	if (FAILED(hr))		return _T("");

	csLocal = szLocal;

	mapFile.SetAt(szURL, csLocal);

	return csLocal;
}
#endif // _LOAD_MFC_