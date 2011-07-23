#pragma once

#ifdef _COMPILING_DLL
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif

//#pragma warning(disable: 4251)

#ifdef _LOAD_MFC_
class CFileCache  
{
public:
	static void Init(); // 프로세스 시작시 반드시 호출
	static void Free(); // 프로세스 끝날때 반드시 호출

	// URL 파일을 다운로드하여 로컬 경로 리턴
	static CString GetFile(LPCTSTR szURL, BOOL *pbAbort = NULL);
};
#endif