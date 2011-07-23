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
	static void Init(); // ���μ��� ���۽� �ݵ�� ȣ��
	static void Free(); // ���μ��� ������ �ݵ�� ȣ��

	// URL ������ �ٿ�ε��Ͽ� ���� ��� ����
	static CString GetFile(LPCTSTR szURL, BOOL *pbAbort = NULL);
};
#endif