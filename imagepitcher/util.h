#pragma once

#include <COMDEF.h>
#include <ShTypes.h>

class CUtil
{
public:
  static void DisplayCOMError(_com_error e);

  static std::string  GetLastErrorMessageA();
  static std::wstring GetLastErrorMessageW();
  static tstring GetLastErrorMessage(void);

  static tstring GetIEPath(void);

  // for Vista or higher
  static bool IsVistaOrHigher(void);
  static HRESULT IsElevated(BOOL* pElevated);
  static bool GetProcessElevation(TOKEN_ELEVATION_TYPE* pElevationType, BOOL* pIsAdmin);
  static bool ShellExecWithVerb(HWND hWnd, LPCTSTR lpVerb, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);
  static bool ShellExecWithElevation(HWND hWnd, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);
  static bool OpenUrlWithElevation(HWND hWnd, LPCTSTR lpUrl);
  ////

  static bool GetFileTime( const tstring& strPathName, FILETIME& ft );
  static tstring GetDesktopFolderPath();

  static int GetIconIndex(LPCTSTR lpszPath, BOOL bIsDir, BOOL bSelected = FALSE);
  static BOOL InitSystemImageLists(HWND hWnd, BOOL bIsTree);

  static BOOL FindDir(LPCTSTR lpDir);
  static BOOL MakePath(tstring strPath);
  //static void NormalizePath(tstring &strPath);

  static tstring GetMergeReference(std::list<tstring>& lstToken);

  static BOOL RegSetString(HKEY hKeyRoot, tstring csSubKey, tstring csItem, tstring csValue);
  static tstring RegGetString(HKEY hKeyRoot, tstring csSubKey, tstring csItem, tstring csDefault = _T(""));
  static DWORD RegDeleteSubKey(HKEY hStartKey, LPCTSTR pKeyName );

  static tstring GetTempFileNameSimple(LPCTSTR szExt);

  static void CopyMap(const std::map<tstring, tstring>& map1, std::map<tstring, tstring>& map2);

  static void DebugString(TCHAR *szMsg, ...);

  static std::list<tstring> GetFiles( LPCTSTR path );
  static BOOL RecursiveDeleteFile( LPCTSTR path );
  // 	static void CopyDirectory(tstring strOrigPath, tstring strDestPath, BOOL bRemoveOrigFile);

  static bool RenameFile( tstring const& strOldFilePathName, tstring const& strNewFilePathName );
  static void SplitPath( tstring const& strPath, tstring& strDrive, tstring& strDir, tstring& strFileName, tstring& strExt );

  // return old path
  static tstring ChangeDirectoryToModulePath();

  static tstring GetSizeUnitString(long long nFileSize);
  static tstring GetSizeWithUnitString(long long nFileSize, int nPrecision = 0);

  static tstring GetModulePath(void);
  static tstring GetModulePathName(void);	

  static unsigned long long GetTickCount(void);

  static tstring SelectDirectory(HWND parentWnd, tstring title, tstring displayPath);

  static INT64 GetFileSize(LPCTSTR szPathName);

  // int 값을 binary로 변환 length 만큼 채움 (ex: length가 8, n이 2면 00000010 이 된다)
  static void itob(int n, char arr[], int length);
  static std::string itob(int n);
  //static std::string itob(unsigned int n);
  // binary 형태의 배열을 int값으로 돌려줌.
  static int btoi(char arr[], int length);
  static int btoi(char* str);
  static ITEMIDLIST* IDListFromPath(LPCTSTR pathName);
  static BOOL OpenFolderAndSelectItems(LPCTSTR filePathName);
  static BOOL OpenFolderAndSelectItemsSimple(LPCTSTR filePathName);
  //static unsigned int btoi(char* str);

private:
  //static LPITEMIDLIST ConvertPathToLpItemIdList(const TCHAR* pszPath);
  //static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};
